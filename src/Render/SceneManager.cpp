#include "SceneManager.h"
#include "System/Win32App.h"
#include "Resources/Resources.h"

#include "Render/ArticulatedObject.h"

#include <random>

namespace Render
{

SceneManager::SceneManager()
: m_gpuInstance(D3DInstance::GetInstance())
, m_frame(0)
, m_wireframe(false)
, m_enableGI(true)
, m_frontBuffer(0)
, m_scene(m_world)
, m_dirLightScene(m_world)
, m_dirLightActive(false)
, m_width(1280)
, m_height(720)
, m_aspectRatio((float)m_width / (float)m_height)
//, m_gbuffer({ FORMAT_R8G8B8A8_UNORM, FORMAT_R8G8B8A8_UNORM, FORMAT_R16G16B16A16_FLOAT }, FORMAT_UNKNOWN)
, m_colorBuffer(FORMAT_R8G8B8A8_UNORM)
, m_normalsBuffer(FORMAT_R8G8B8A8_UNORM)
, m_flatNormalsBuffer{ColorBuffer(FORMAT_R8G8B8A8_UNORM), ColorBuffer(FORMAT_R8G8B8A8_UNORM)}
, m_paramsBuffer(FORMAT_R16G16B16A16_FLOAT)
, m_hdrBuffer(FORMAT_R16G16B16A16_FLOAT, true, true)
, m_omniLightIndices(64)
, m_spotLightIndices(64)
, m_lightGrid(uint8_t(BufferRead | BufferWrite))
{
}

SceneManager& SceneManager::GetInstance()
{
    static SceneManager instance;

    return instance;
}

void SceneManager::init(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    m_aspectRatio = (float)width/(float)height;

    RenderingPipeline::Init();
    initResources();

    m_shadowBuffer.reset(DirShadowSize, DirShadowSize, ShadowLods);

    for (size_t i = 0; i < ShadowMapNum; i++) m_cubemapBuffers[i].reset(ShadowCubeSize);
    for (size_t i = 0; i < ShadowCacheSize; i++) m_cubemapCache[i].reset(ShadowCubeSize);

    for (size_t i = 0; i < ShadowMapNum; i++) m_shadowBuffers[i].reset(ShadowSize, ShadowSize);
    for (size_t i = 0; i < ShadowCacheSize; i++) m_shadowCache[i].reset(ShadowSize, ShadowSize);

    resize(width, height);

    generateHemisphereSamples();
    m_blueNoiseMask = ResourceManager::GetImage("blue_noise_2d_64x64.dds");

    { // This probably should be constant in the shader

        constexpr float znear = 0.1f;
        constexpr float zfar = 500.0f;

        m_shadowConstantBuffer = mat4::Projection(90, 1.0, znear, zfar);
    }
}

void SceneManager::resize(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    m_xtiles = (m_width + LightGridTileSize - 1) / LightGridTileSize;
    m_ytiles = (m_height + LightGridTileSize - 1) / LightGridTileSize;

    m_aspectRatio = (float)width / (float)height;

    RenderingPipeline::ResizeViewport(width, height);

    uint32_t gridsz = m_xtiles * m_ytiles;
    m_lightGrid.resize(gridsz * TileListSize);

    m_depthBuffer[0].reset(width, height);
    m_depthBuffer[1].reset(width, height);

    m_gbuffer[0].reset();
    m_gbuffer[1].reset();

    m_background.reset(width, height);

    m_hdrBuffer.reset(width, height);
    m_colorBuffer.reset(width, height);
    m_normalsBuffer.reset(width, height);
    m_flatNormalsBuffer[0].reset(width, height);
    m_flatNormalsBuffer[1].reset(width, height);
    m_paramsBuffer.reset(width, height);

    if (m_gpuInstance.rtxSupport())
    {
        m_sppBuffer[0].reset(width, height);
        m_sppBuffer[1].reset(width, height);
        m_ambientBuffer[0].reset(width, height);
        m_ambientBuffer[1].reset(width, height);
    }

    m_gbuffer[0].attachColorBuffer(m_colorBuffer);
    m_gbuffer[0].attachColorBuffer(m_normalsBuffer);
    m_gbuffer[0].attachColorBuffer(m_flatNormalsBuffer[0]);
    m_gbuffer[0].attachColorBuffer(m_paramsBuffer);
    m_gbuffer[0].attachDepthBuffer(m_depthBuffer[0]);

    m_gbuffer[1].attachColorBuffer(m_colorBuffer);
    m_gbuffer[1].attachColorBuffer(m_normalsBuffer);
    m_gbuffer[1].attachColorBuffer(m_flatNormalsBuffer[1]);
    m_gbuffer[1].attachColorBuffer(m_paramsBuffer);
    m_gbuffer[1].attachDepthBuffer(m_depthBuffer[1]);

    constexpr float znear = 0.1f;
    constexpr float zfar = 5000.0f;

    m_projMat = mat4::Projection(75, m_aspectRatio, znear, zfar);

    m_fovx = 1.0 / m_projMat[0][0];
    m_fovy = 1.0 / m_projMat[1][1];

    m_sceneConstantBuffer->fovx = m_fovx;
    m_sceneConstantBuffer->fovy = m_fovy;

    m_lightingConstantBuffer->xtiles = m_xtiles;
}

void SceneManager::reset()
{
    m_lights.destroy();
    m_objects.clear();
    m_skeletalData.clear();
    m_fogVolumes.destroy();
    m_world.reset();

    m_raytraceScene.clear();
    m_raytraceGeometry.clear();

    m_dirLightActive = false;
    m_giActive = false;

    m_raytraceContantBuffer->enable_dir_light = false;

    m_frame = 0;
}

void SceneManager::enableGI(bool enable)
{ 
    m_enableGI = enable;
    m_lightingConstantBuffer->ambient_buffer = (m_giActive && m_enableGI) ? 1 : 0;
}

void SceneManager::generateHemisphereSamples()
{
    std::mt19937 uniformGenerator;
    uniformGenerator.seed(1729);

    std::uniform_real_distribution<float> radiusDistribution(0, 0.9);
    std::uniform_real_distribution<float> angleDistribution(0, math::pi * 2.0);

    static constexpr size_t sampleNum = 640;

    std::vector<vec3> samples;
    samples.reserve(sampleNum);

    for (int i = 0; i < 1024; i++)
    {
        float len = radiusDistribution(uniformGenerator);
        float ang = angleDistribution(uniformGenerator);

        float x = cos(ang) * len;
        float y = sin(ang) * len;
        float z = sqrt(1.0 - len * len);

        samples.emplace_back(x, y, z);
    }

    // Blue noise generation
    /*for (size_t i = 0; i < 640; i++)
    {
        vec3 candidate = {};

        size_t candidateNum = samples.size() * 2;
        float bestDist = 0;

        for (size_t c = 0; c < candidateNum; c++)
        {
            float len = radiusDistribution(uniformGenerator);
            float ang = angleDistribution(uniformGenerator);

            float x = cos(ang) * len;
            float y = sin(ang) * len;
            float z = sqrt(1.0 - len * len);
        
            float minDist = std::numeric_limits<float>::infinity();

            for (size_t k = 0; k < samples.size(); k++)
            {
                float dist = acosf(samples[k] * vec3(x, y, z));
                if (dist < minDist) minDist = dist;
            }

            if (minDist > bestDist)
            {
                bestDist = minDist;
                candidate = vec3(x, y, z);
            }
        }

        samples.push_back(candidate);
    }*/

    m_hemisphereSamples.setData(samples.data(), samples.size());
}

void SceneManager::setSkybox(const std::string& name)
{
    std::string fname = "Skybox\\" + name;

    std::string img_name[6];

    img_name[0] = fname + "_ft.dds";
    img_name[1] = fname + "_bk.dds";
    img_name[2] = fname + "_rt.dds";
    img_name[3] = fname + "_lf.dds";
    img_name[4] = fname + "_up.dds";
    img_name[5] = fname + "_dn.dds";

    for (int f = 0; f < 6; f++) m_skyboxFace[f] = ResourceManager::GetImage(img_name[f]);
}

void SceneManager::setDirectionalLight(const vec3& dir, const vec3& color)
{
    m_dirLightActive = true;
    m_dirLight.setup(dir, color * 4.0f);

    m_lightingConstantBuffer->light_dir = -m_dirLight.direction();
    m_lightingConstantBuffer->light_flux = m_dirLight.color();

    m_raytraceContantBuffer->enable_dir_light = true;
    m_raytraceContantBuffer->light_color = color * 0.2f;
}

void SceneManager::setGi(const vec3& color)
{
    if (m_gpuInstance.rtxSupport())
    {
        m_giActive = true;
        m_giColor = color;

        m_raytraceContantBuffer->sky_color = color * 0.2f;
    }
}

void SceneManager::addLight(Light* light)
{
    m_lights.append(light);
}

void SceneManager::removeLight(Light* light)
{
    m_lights.remove(light);
}

void SceneManager::addObject(DisplayObject* object)
{
    m_objects.append(object);
}

void SceneManager::removeObject(DisplayObject* object)
{
    object->clearReferences();
    m_objects.remove(object);
}

void SceneManager::addFogVolume(FogVolume* volume)
{
    m_fogVolumes.append(volume);
}

void SceneManager::removeFogVolume(FogVolume* volume)
{
    volume->clearReferences();
    m_fogVolumes.remove(volume);
}

void SceneManager::initStaticGeometry()
{
    RaytraceId id = m_raytraceScene.createGeometry();
    m_raytraceScene.addInstance(id, {});
}

void SceneManager::addStaticGeometry(const D3D12_VERTEX_BUFFER_VIEW* vertexBuffer, const std::vector<GeometryData>& geometryData)
{
    m_raytraceScene.addStaticGeometry(vertexBuffer, geometryData);
}

void SceneManager::addStaticGeometry(const VertexBuffer& vertexBuffer, const vec3& pos)
{
    m_raytraceScene.addStaticGeometry(vertexBuffer, pos);
}

void SceneManager::addStaticGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer, const vec3& pos)
{
    m_raytraceScene.addStaticGeometry(vertexBuffer, indexBuffer, pos);
}

void SceneManager::addStaticGeometry(const VertexBuffer& vertexBuffer, const mat4& mat)
{
    m_raytraceScene.addStaticGeometry(vertexBuffer, mat);
}

RaytraceId SceneManager::addGeometry(const Model* model)
{
    auto it = m_raytraceGeometry.find(model);

    if (it != m_raytraceGeometry.end()) return it->second;

    RaytraceId geometryId = m_raytraceScene.createGeometry(model->vertexBuffer(), model->indexBuffer());
    m_raytraceGeometry[model] = geometryId;

    return geometryId;
}

RaytraceId SceneManager::addGeometryInstance(RaytraceId id, const mat4& mat)
{
    return m_raytraceScene.addInstance(id, mat);
}

void SceneManager::setGeometryTransform(RaytraceId instance, const mat4& mat)
{
    if (!m_giActive) return;

    m_raytraceScene.setInstanceTransform(instance, mat);
}

void SceneManager::addSprite(const SpriteData& spriteData)
{
    m_spriteBuffer.push(spriteData);
}

void SceneManager::registerSkeletalObject(SkeletalData* data)
{
    m_skeletalData.append(data);
}

void SceneManager::unregisterSkeletalObject(SkeletalData* data)
{
    m_skeletalData.remove(data);
}

void SceneManager::resetLightsFrameNum()
{
    for (auto light : m_lights) light->setFrame(0);
}

void SceneManager::resetFogVolumesFrameNum()
{
    for (auto volume : m_fogVolumes) volume->setFrame(0);
}

void SceneManager::resetObjectFrameNum()
{
    for (auto obj : m_objects) obj->setFrame(0);
}

void SceneManager::resetFrameNum()
{
    m_world.resetLeafFrameNum();
    resetObjectFrameNum();
    resetLightsFrameNum();
    resetFogVolumesFrameNum();

    m_frame = 0;
}

void SceneManager::incrementFrameNum()
{
    if (m_frame == -1) resetFrameNum();
    else m_frame++;
}

void SceneManager::updateWorld()
{
    m_frame = 0;

    m_world.resetLeafFrameNum();
    resetObjectFrameNum();
    resetLightsFrameNum();

    for (DisplayObject* object : m_objects) m_world.addObject(object);
    for (FogVolume* volume : m_fogVolumes) m_world.addFogVolume(volume);

    size_t omniCount = 0;
    size_t spotCount = 0;

    for (auto light : m_lights)
    {
        incrementFrameNum();
        light->calculateVisibility(m_frame);

        if (light->type() == LightType::Omni)
            light->setId(omniCount++);
        else
            light->setId(spotCount++);
    }

    m_omniLightData.resize(omniCount + DynamicOmniLights);
    m_spotLightData.resize(spotCount + DynamicSpotLights);

    for (auto light : m_lights)
    {
        if (light->type() == LightType::Omni)
            m_omniLightData[light->id()] = *static_cast<const OmniLightData*>(light->data());
        else
            m_spotLightData[light->id()] = *static_cast<const SpotLightData*>(light->data());
    }

    for (size_t i = 0; i < ShadowMapNum; i++) m_frameLights[i] = nullptr;
    for (size_t i = 0; i < ShadowCacheSize; i++) m_cachedLights[i] = nullptr;

    for (size_t i = 0; i < ShadowMapNum; i++) m_frameSpotLights[i] = nullptr;
    for (size_t i = 0; i < ShadowCacheSize; i++) m_cachedSpotLights[i] = nullptr;

    incrementFrameNum();
    m_dirLightScene.globalLightVisibility(m_dirLight.direction(), m_frame);

    if (m_giActive) m_raytraceScene.build();
    m_lightingConstantBuffer->ambient_buffer = (m_giActive && m_enableGI) ? 1 : 0;
}

void SceneManager::initResources()
{
    struct SimpleVertex
    {
        vec3 pos;
        vec2 tcoord;
    };

    static SimpleVertex skyboxVerts[] =
    {
        {{1.0, 1.0, 1.0 }, {1.0, 0.0}}, {{1.0, -1.0, 1.0}, {1.0, 1.0}}, {{-1.0, 1.0, 1.0 }, {0.0, 0.0}}, {{-1.0, -1.0, 1.0}, {0.0, 1.0}},
        {{1.0, -1.0, -1.0}, {0.0, 1.0}}, {{ 1.0, 1.0, -1.0 }, {0.0, 0.0}}, {{-1.0, -1.0, -1.0}, {1.0, 1.0}}, {{ -1.0, 1.0, -1.0 }, {1.0, 0.0}},
        {{-1.0,  1.0, 1.0 }, {1.0, 0.0}}, {{-1.0, -1.0, 1.0}, {1.0, 1.0}}, {{-1.0,  1.0, -1.0 }, {0.0, 0.0}}, {{-1.0, -1.0, -1.0}, {0.0, 1.0}},
        {{1.0, -1.0, 1.0}, {0.0, 1.0}}, {{1.0, 1.0, 1.0 }, {0.0, 0.0}}, {{1.0, -1.0, -1.0}, {1.0, 1.0}}, {{1.0, 1.0, -1.0 }, {1.0, 0.0}},
        {{1.0, 1.0, -1.0}, {0.0, 0.0}}, {{1.0, 1.0, 1.0 }, {1.0, 0.0}}, {{-1.0, 1.0, -1.0}, {0.0, 1.0}}, {{-1.0, 1.0, 1.0 }, {1.0, 1.0}},
        {{1.0, -1.0, 1.0}, {1.0, 1.0}}, {{1.0, -1.0, -1.0}, {0.0, 1.0}}, {{-1.0, -1.0, 1.0 }, {1.0, 0.0}}, {{-1.0, -1.0, -1.0}, {0.0, 0.0}}
    };

    m_skyboxBuffer.setData(skyboxVerts, sizeof(skyboxVerts) / sizeof(SimpleVertex));

    static const vec3 volumeVertices[] = { {1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
                                           {1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f} };

    static const uint16_t volumeIndices[] = { 1,    // Front-top-left
                                              0,    // Front-top-right
                                              5,    // Front-bottom-left
                                              4,    // Front-bottom-right
                                              6,    // Back-bottom-right
                                              0,    // Front-top-right
                                              2,    // Back-top-right
                                              1,    // Front-top-left
                                              3,    // Back-top-left
                                              5,    // Front-bottom-left
                                              7,    // Back-bottom-left
                                              6,    // Back-bottom-right
                                              3,    // Back-top-left
                                              2     // Back-top-right
                                            };

    m_volumeVertexBuffer.setData(volumeVertices, _countof(volumeVertices));
    m_volumeIndexBuffer.setData(volumeIndices, _countof(volumeIndices));

#ifdef _DEBUG
    static const uint16_t bboxIndices[] = { 0, 1, 
                                            1, 3,
                                            3, 2,
                                            2, 0,
                                            4, 5,
                                            5, 7,
                                            7, 6,
                                            6, 4,
                                            1, 5,
                                            3, 7,
                                            2, 6,
                                            0, 4};

    m_bboxIndexBuffer.setData(bboxIndices, _countof(bboxIndices));
#endif
}

void SceneManager::processSkeletalObjects()
{
    if (m_skeletalData.empty()) return;

    ComputeContext computeContext = RenderingPipeline::StartCompute(RenderingPipeline::cm_skin);

    for (SkeletalData* data : m_skeletalData)
    {
        if (!data->isUpdateRequired()) continue;

        VertexBuffer& outputBuffer = data->outputBuffer();

        computeContext.barrier(Barrier(outputBuffer, STATE_VERTEX_BUFFER, STATE_WRITE));

        computeContext.bind(0, data->inputBuffer());
        computeContext.bind(1, data->boneIdBuffer());
        computeContext.bind(2, data->boneBuffer());
        computeContext.bind(3, outputBuffer.writeHandle());

        computeContext.dispatch(data->vertexNum());

        computeContext.barrier(Barrier(outputBuffer, STATE_WRITE, STATE_VERTEX_BUFFER));

        data->setUpdated();
    }

    computeContext.finish();
}

void SceneManager::drawOverlay()
{
    m_emissiveOverlay = false;

    mat4 overlayMat = m_camera.viewMat().inverse();

    for (DisplayObject* object : m_overlayObjects)
    {
        const std::vector<DisplayBlock>& displayData = object->displayData();

        for (const DisplayBlock& block : displayData)
        {
            if (block.hide) continue;

            mat4 objMat = overlayMat * (*block.mat);
            
            switch (block.type)
            {
            case DisplayBlock::display_regular:
                m_geometryCommandList.draw(block, objMat);
            break;
            case DisplayBlock::display_emissive:
                m_emissiveOverlay = true;
            break;
            }
        }
    }
}

void SceneManager::geometryPass()
{
    //Prepare gbuffer
    m_geometryCommandList.start(RenderingPipeline::rm_gbuffer_overlay);
    m_geometryCommandList.bindConstantBuffer(0, m_sceneConstantBuffer);
    m_geometryCommandList.setDefaultViewport();

    m_barriers.transit(m_colorBuffer, STATE_RENDER);
    m_barriers.transit(m_normalsBuffer, STATE_RENDER);
    m_barriers.transit(m_flatNormalsBuffer[m_frontBuffer], STATE_RENDER);
    m_barriers.transit(m_paramsBuffer, STATE_RENDER);
    m_barriers.transit(m_depthBuffer[m_frontBuffer], STATE_DEPTH_WRITE);
    m_geometryCommandList.barrier(m_barriers);

    m_geometryCommandList.bindFrameBuffer(m_gbuffer[m_frontBuffer]);

    m_geometryCommandList.clearDepthStencil(m_gbuffer[m_frontBuffer]);
    m_geometryCommandList.setStencilValue(100);

    m_geometryCommandList.setTopology(topology_trianglelist);

    drawOverlay();

    m_geometryCommandList.setRenderMode(RenderingPipeline::rm_gbuffer);
    m_geometryCommandList.draw(m_scene.displayList());

    if (!m_scene.instancedList().empty())
    {
        m_geometryCommandList.setRenderMode(RenderingPipeline::rm_gbuffer_instanced);
        m_geometryCommandList.draw(m_scene.instancedList());
    }

    if (!m_scene.displayListLayered().empty())
    {
        m_geometryCommandList.setRenderMode(RenderingPipeline::rm_gbuffer_layered);
        m_geometryCommandList.drawLayered(m_scene.displayListLayered());
    }
   
    m_geometryCommandList.finish();
    m_gpuInstance.execute(m_geometryCommandList);
}

void SceneManager::emissivePass()
{
    if (!m_emissiveOverlay && m_scene.displayListTransparent().empty() && m_scene.displayListEmissive().empty()) return;

    m_commandList.setRenderMode(RenderingPipeline::rm_emissive);
    m_commandList.bindConstantBuffer(0, m_sceneConstantBuffer);
    m_commandList.setDefaultViewport();

    m_commandList.barrier(m_depthBuffer[m_frontBuffer], STATE_DEPTH_WRITE);
    m_commandList.bindFrameBuffer(m_hdrBuffer, m_depthBuffer[m_frontBuffer]);
    m_commandList.setStencilValue(100);

    m_commandList.setTopology(topology_trianglelist);

    if (!m_scene.displayListEmissive().empty()) m_commandList.drawSimple(m_scene.displayListEmissive());

    mat4 overlayMat = m_camera.viewMat().inverse();

    for (DisplayObject* object : m_overlayObjects)
    {
        const std::vector<DisplayBlock>& displayData = object->displayData();

        for (const DisplayBlock& block : displayData)
        {
            if (block.hide) continue;

            mat4 objMat = overlayMat * (*block.mat);

            if (block.type == DisplayBlock::display_emissive)
            {
                m_commandList.drawSimple(block, objMat);
            }
        }
    }

    if (!m_scene.displayListTransparent().empty())
    {
        m_commandList.barrier({ Barrier(m_hdrBuffer, STATE_RENDER, STATE_COPY_SOURCE),
                                Barrier(m_background, STATE_PIXEL_SHADER_READ, STATE_COPY_DEST) });

        m_commandList.copy(m_background, m_hdrBuffer);

        m_commandList.barrier({ Barrier(m_background, STATE_COPY_DEST, STATE_PIXEL_SHADER_READ),
                                Barrier(m_hdrBuffer, STATE_COPY_SOURCE, STATE_RENDER) });

        m_commandList.setRenderMode(RenderingPipeline::rm_transparent);
        m_commandList.setConstant(2, std::make_pair<uint32_t, uint32_t>(m_width - 1, m_height - 1));
        m_commandList.bind(5, m_background);
        m_commandList.drawRefract(m_scene.displayListTransparent());
    }
}

void SceneManager::drawShadow(CommandList& commandList, Light& light, CubemapBuffer& buffer)
{
    incrementFrameNum();
    light.updateVisibility(m_frame);

    commandList.setViewport(ShadowCubeSize, ShadowCubeSize);

    commandList.barrier(buffer, STATE_DEPTH_WRITE);
    commandList.bindFrameBuffer(buffer);
    commandList.clearDepth(buffer, 1.0f);

    commandList.setTopology(topology_trianglelist);

    commandList.drawDepth(light.scene().displayList());

    commandList.barrier(buffer, STATE_PIXEL_SHADER_READ);

    commandList.setDefaultViewport();
}

void SceneManager::drawDirectionalShadow(CommandList& commandList, const Scene& scene)
{
    commandList.setViewport(DirShadowSize, DirShadowSize);

    commandList.bindConstantBuffer(0, m_dirLight.shadowMatrices());

    commandList.barrier(m_shadowBuffer, STATE_DEPTH_WRITE);
    commandList.bindDepthBuffer(m_shadowBuffer);
    commandList.clearDepth(m_shadowBuffer, 1.0f);

    commandList.setTopology(topology_trianglelist);

    commandList.drawDepth(m_dirLightScene.displayList());

    commandList.setDefaultViewport();
}

void SceneManager::shadowPass()
{
    RenderingPipeline::StartRender(m_shadowCommandList, RenderingPipeline::rm_shadow_cube);
    m_shadowCommandList.bindConstantBuffer(0, m_shadowConstantBuffer);
    m_shadowCommandList.setViewport(ShadowCubeSize, ShadowCubeSize);
    m_shadowCommandList.setTopology(topology_trianglelist);

    for (Light* light : m_omniLights)
    {
        if (light->shadowType() == LightShadow::None) continue;
        if (light->shadowType() == LightShadow::Static && !light->needStaticUpdate()) continue;

        m_shadowCommandList.setConstant(2, light->pos());

        size_t shadowIdx = light->shadowIndex();
        size_t cacheIdx = light->cacheIndex();

        if (light->needStaticUpdate())
        {
            incrementFrameNum();
            light->calculateVisibility(m_frame);

            m_shadowCommandList.barrier(m_cubemapCache[cacheIdx], STATE_DEPTH_WRITE);
            m_shadowCommandList.bindDepthBuffer(m_cubemapCache[cacheIdx]);
            m_shadowCommandList.clearDepth(m_cubemapCache[cacheIdx], 1.0f);
            m_shadowCommandList.drawDepth(light->scene().displayList());

            light->setStaticUpdateFlag(false);
        }

        if (light->shadowType() == LightShadow::Dynamic)
        {
            m_shadowCommandList.barrier(m_cubemapBuffers[shadowIdx], STATE_COPY_DEST);
            m_shadowCommandList.barrier(m_cubemapCache[cacheIdx], STATE_COPY_SOURCE);

            m_shadowCommandList.copy(m_cubemapBuffers[shadowIdx],
                                     m_cubemapCache[cacheIdx]);

            incrementFrameNum();
            light->updateVisibility(m_frame);
            m_shadowCommandList.barrier(m_cubemapBuffers[shadowIdx], STATE_DEPTH_WRITE);
            m_shadowCommandList.bindDepthBuffer(m_cubemapBuffers[shadowIdx]);
            m_shadowCommandList.drawDepth(light->scene().displayList());
        }
    }

    if (!m_spotLights.empty())
    {
        m_shadowCommandList.setRenderMode(RenderingPipeline::rm_shadow);
        m_shadowCommandList.setViewport(ShadowSize, ShadowSize);
    }

    for (Light* light : m_spotLights)
    {
        if (light->shadowType() == LightShadow::None) continue;
        if (light->shadowType() == LightShadow::Static && !light->needStaticUpdate()) continue;

        size_t shadowIdx = light->shadowIndex();
        size_t cacheIdx = light->cacheIndex();

        if (light->needStaticUpdate())
        {
            incrementFrameNum();
            light->calculateVisibility(m_frame);

            m_shadowCommandList.barrier(m_shadowCache[cacheIdx], STATE_DEPTH_WRITE);
            m_shadowCommandList.bindDepthBuffer(m_shadowCache[cacheIdx]);
            m_shadowCommandList.clearDepth(m_shadowCache[cacheIdx], 1.0f);
            m_shadowCommandList.setConstant(0, light->shadowMat());
            m_shadowCommandList.drawDepth(light->scene().displayList());

            light->setStaticUpdateFlag(false);
        }

        if (light->shadowType() == LightShadow::Dynamic)
        {
            m_shadowCommandList.barrier(m_shadowBuffers[shadowIdx], STATE_COPY_DEST);
            m_shadowCommandList.barrier(m_shadowCache[cacheIdx], STATE_COPY_SOURCE);

            m_shadowCommandList.copy(m_shadowBuffers[shadowIdx], m_shadowCache[cacheIdx]);

            incrementFrameNum();
            light->updateVisibility(m_frame);
            m_shadowCommandList.barrier(m_shadowBuffers[shadowIdx], STATE_DEPTH_WRITE);
            m_shadowCommandList.bindDepthBuffer(m_shadowBuffers[shadowIdx]);
            m_shadowCommandList.setConstant(0, light->shadowMat());
            m_shadowCommandList.drawDepth(light->scene().displayList());
        }
    }

    if (m_dirLightActive && m_scene.isGlobalLit())
    {
        m_shadowCommandList.setRenderMode(RenderingPipeline::rm_shadow_cascaded);
        drawDirectionalShadow(m_shadowCommandList, m_dirLightScene);
    }

    m_shadowCommandList.finish();
    m_gpuInstance.execute(m_shadowCommandList);
}

void SceneManager::lightPassSimple()
{
    RenderingPipeline::StartRender(m_commandList, RenderingPipeline::rm_omnilight);
    m_commandList.bindConstantBuffer(0, m_sceneConstantBuffer);
    m_commandList.setDefaultViewport();

    m_barriers.transit(m_colorBuffer, STATE_PIXEL_SHADER_READ);
    m_barriers.transit(m_normalsBuffer, STATE_PIXEL_SHADER_READ);
    m_barriers.transit(m_paramsBuffer, STATE_PIXEL_SHADER_READ);
    m_barriers.transit(m_depthBuffer[m_frontBuffer], STATE_PIXEL_SHADER_READ);
    m_barriers.transit(m_hdrBuffer, STATE_PIXEL_SHADER_READ, STATE_RENDER);
    
    for (Light* light : m_omniLights)
    {
        if (light->shadowType() == LightShadow::None) continue;

        if (light->shadowType() == LightShadow::Dynamic)
        {
            m_barriers.transit(m_cubemapBuffers[light->shadowIndex()], STATE_PIXEL_SHADER_READ);
            m_omniLightData[light->id()].shadowIdx = light->shadowIndex();
        }
        else
        {
            m_barriers.transit(m_cubemapCache[light->cacheIndex()], STATE_PIXEL_SHADER_READ);
            m_omniLightData[light->id()].shadowIdx = ShadowMapNum + light->cacheIndex();
        }
    }
    
    m_commandList.barrier(m_barriers);

    m_commandList.bindFrameBuffer(m_hdrBuffer);
    m_commandList.clearBuffer(m_hdrBuffer, { 0.0f, 0.0f, 0.0f, 1.0f });

    m_commandList.setRenderMode(RenderingPipeline::rm_omnilight);
    m_commandList.bindConstantBuffer(0, m_sceneConstantBuffer);
    m_commandList.bind(2, m_omniLightData);
    m_commandList.bind(3, m_colorBuffer);
    m_commandList.bind(4, m_normalsBuffer);
    m_commandList.bind(5, m_paramsBuffer);
    m_commandList.bind(6, m_depthBuffer[m_frontBuffer]);

    m_commandList.bind(7, m_cubemapBuffers[0]);

    for (Light* light : m_omniLights)
    {
        m_commandList.setConstant(1, light->id());
        m_commandList.setTopology(topology_trianglestrip);
        m_commandList.draw(4);
    }

    if (!m_spotLights.empty())
    {
        m_commandList.setRenderMode(RenderingPipeline::rm_spotlight);
        m_commandList.setTopology(topology_trianglestrip);

        for (Light* light : m_spotLights)
        {
            if (light->shadowType() == LightShadow::None) continue;

            if (light->shadowType() == LightShadow::Dynamic)
            {
                m_barriers.transit(m_shadowBuffers[light->shadowIndex()], STATE_PIXEL_SHADER_READ);
                m_spotLightData[light->id()].shadowIdx = light->shadowIndex();
            }
            else
            {
                m_barriers.transit(m_shadowCache[light->cacheIndex()], STATE_PIXEL_SHADER_READ);
                m_spotLightData[light->id()].shadowIdx = ShadowMapNum + light->cacheIndex();
            }
        }

        m_commandList.barrier(m_barriers);

        m_commandList.bind(2, m_spotLightData);
        m_commandList.bind(7, m_shadowBuffers[0]);
        //m_commandList.bind(3, m_colorBuffer);
        //m_commandList.bind(4, m_normalsBuffer);
        //m_commandList.bind(5, m_paramsBuffer);
        //m_commandList.bind(6, m_depthBuffer[m_frontBuffer]);

        for (Light* light : m_spotLights)
        {
            m_commandList.setConstant(1, light->id());
            m_commandList.draw(4);
        }
    }

    if (m_dirLightActive && m_scene.isGlobalLit())
    {
        m_commandList.barrier(m_shadowBuffer, STATE_PIXEL_SHADER_READ);
        m_commandList.setRenderMode(RenderingPipeline::rm_dirlight);
        m_commandList.bindConstantBuffer(0, m_sceneConstantBuffer);

        m_commandList.bindFrameBuffer(m_hdrBuffer);

        m_commandList.bind(3, m_colorBuffer);
        m_commandList.bind(4, m_normalsBuffer);
        m_commandList.bind(5, m_paramsBuffer);
        m_commandList.bind(6, m_depthBuffer[m_frontBuffer]);
        m_commandList.bind(7, m_shadowBuffer);

        m_commandList.setConstant(1, m_dirLight);
        m_commandList.bindConstantBuffer(2, m_dirLight.shadowMatrices());
        m_commandList.setTopology(topology_trianglestrip);
        m_commandList.draw(4);
    }
}

void SceneManager::lightPass()
{
    m_lightingConstantBuffer->ambient_color = { 0.05f, 0.05f, 0.05f };

    ComputeContext context = m_commandList;

    LightGridParams params = { m_xtiles, m_width / LightGridTileSize, m_height / LightGridTileSize, m_omniLights.size(), m_spotLights.size() };

    context.barrier(m_lightGrid, STATE_WRITE);

    context.setComputeMode(RenderingPipeline::cm_lightgrid);

    context.bindConstantBuffer(0, m_sceneConstantBuffer);
    context.setConstant(1, params);
    context.bind(2, m_omniLightData);
    context.bind(3, m_spotLightData);
    context.bind(4, m_omniLightIndices);
    context.bind(5, m_spotLightIndices);
    context.bind(6, m_lightGrid.writeHandle());

    context.dispatch(m_xtiles, m_ytiles, 1);

    for (Light* light : m_omniLights)
    {
        if (light->shadowType() == LightShadow::None) continue;

        if (light->shadowType() == LightShadow::Dynamic)
        {
            m_barriers.transit(m_cubemapBuffers[light->shadowIndex()], STATE_READ);
            m_omniLightData[light->id()].shadowIdx = light->shadowIndex();
        }
        else
        {
            m_barriers.transit(m_cubemapCache[light->cacheIndex()], STATE_READ);
            m_omniLightData[light->id()].shadowIdx = ShadowMapNum + light->cacheIndex();
        }
    }

    for (Light* light : m_spotLights)
    {
        if (light->shadowType() == LightShadow::None) continue;

        if (light->shadowType() == LightShadow::Dynamic)
        {
            m_barriers.transit(m_shadowBuffers[light->shadowIndex()], STATE_READ);
            m_spotLightData[light->id()].shadowIdx = light->shadowIndex();
        }
        else
        {
            m_barriers.transit(m_shadowCache[light->cacheIndex()], STATE_READ);
            m_spotLightData[light->id()].shadowIdx = ShadowMapNum + light->cacheIndex();
        }
    }

    if (m_dirLightActive && m_scene.isGlobalLit())
        m_barriers.transit(m_shadowBuffer, STATE_READ);

    m_barriers.transit(m_lightGrid, STATE_SHADER_READ);
    m_barriers.transit(m_hdrBuffer, STATE_PIXEL_SHADER_READ, STATE_WRITE),
    m_barriers.transit(m_colorBuffer, STATE_SHADER_READ),
    m_barriers.transit(m_normalsBuffer, STATE_SHADER_READ),
    m_barriers.transit(m_paramsBuffer, STATE_SHADER_READ),
    m_barriers.transit(m_depthBuffer[m_frontBuffer], STATE_SHADER_READ);

    if ((m_giActive && m_enableGI)) m_barriers.transit(m_ambientBuffer[1], STATE_WRITE, STATE_SHADER_READ);

    context.barrier(m_barriers);

    context.setComputeMode(RenderingPipeline::cm_lighting);

    context.bindConstantBuffer(0, m_lightingConstantBuffer);
    context.bindConstantBuffer(1, m_dirLight.shadowMatrices());

    context.bind(2, m_hdrBuffer.writeHandle());

    context.bind(3, m_lightGrid);
    context.bind(4, m_omniLightData);
    context.bind(5, m_spotLightData);

    context.bind(6, m_colorBuffer);
    context.bind(7, m_normalsBuffer);
    context.bind(8, m_paramsBuffer);
    context.bind(9, m_depthBuffer[m_frontBuffer]);

    context.bind(10, m_ambientBuffer[1]);

    context.bind(11, m_shadowBuffer);
    context.bind(12, m_cubemapBuffers[0]);
    context.bind(13, m_shadowBuffers[0]);

    context.dispatch(m_xtiles, m_ytiles, 1);

    context.barrier(Barrier(m_hdrBuffer, STATE_WRITE, STATE_RENDER));
}

void SceneManager::skyPass()
{
    if (!m_scene.isSkyVisible()) return;

    m_commandList.setRenderMode(RenderingPipeline::rm_sky);
    m_commandList.bindConstantBuffer(0, m_backgroundConstantBuffer);
    m_commandList.setDefaultViewport();

    m_commandList.barrier(m_depthBuffer[m_frontBuffer], STATE_PIXEL_SHADER_READ);

    m_commandList.bindFrameBuffer(m_hdrBuffer);

    m_commandList.setTopology(topology_trianglestrip);
    m_commandList.bindVertexBuffer(m_skyboxBuffer);

    m_commandList.bind(4, m_depthBuffer[m_frontBuffer]);

    for (int i = 0; i < 6; i++)
    {
        m_commandList.bind(3, m_skyboxFace[i]->handle);
        m_commandList.draw(4, 4*i);
    }
}

void SceneManager::raytracePass(RaytraceContext& rc)
{
    int backBuffer = (m_frontBuffer + 1) & 1;

    rc.updateScene(m_raytraceScene);

    m_barriers.transit(m_ambientBuffer[1], STATE_SHADER_READ, STATE_WRITE);
    m_barriers.transit(m_sppBuffer[m_frontBuffer], STATE_SHADER_READ, STATE_WRITE);
    m_barriers.transit(m_depthBuffer[m_frontBuffer], STATE_SHADER_READ);
    m_barriers.transit(m_flatNormalsBuffer[m_frontBuffer], STATE_SHADER_READ);
    rc.barrier(m_barriers);

    rc.bind(0, m_raytraceScene);

    rc.bindConstantBuffer(1, m_raytraceContantBuffer);
    rc.bindConstantBuffer(2, m_dirLight.shadowMatrices());

    rc.bind(3, m_ambientBuffer[1].writeHandle());
    rc.bind(4, m_sppBuffer[m_frontBuffer].writeHandle());
    rc.bind(5, m_depthBuffer[m_frontBuffer]);
    rc.bind(6, m_ambientBuffer[0]);
    rc.bind(7, m_sppBuffer[backBuffer]);
    rc.bind(8, m_depthBuffer[backBuffer]);
    rc.bind(9, m_flatNormalsBuffer[m_frontBuffer]);
    rc.bind(10, m_flatNormalsBuffer[backBuffer]);
    rc.bind(11, m_blueNoiseMask->handle);
    rc.bind(12, m_shadowBuffer);
    rc.bind(13, m_hemisphereSamples);
    rc.dispatchRays(m_width, m_height);

    rc.barrier({ Barrier(m_ambientBuffer[1], STATE_WRITE, STATE_SHADER_READ),
                 Barrier(m_sppBuffer[m_frontBuffer], STATE_WRITE, STATE_SHADER_READ) });
}

void SceneManager::denoisePass(ComputeContext& context)
{
    //ComputeContext context = RenderingPipeline::StartCompute(RenderingPipeline::cm_denoise_spatial);

    context.setComputeMode(RenderingPipeline::cm_denoise_spatial);
    context.barrier(Barrier(m_ambientBuffer[0], STATE_SHADER_READ, STATE_WRITE));

    uint32_t dim[2] = { m_width, m_height };

    context.setConstant(0, dim);
    context.bind(1, m_ambientBuffer[0].writeHandle());
    context.bind(2, m_ambientBuffer[1]);
    context.bind(3, m_depthBuffer[m_frontBuffer]);
    context.dispatch(m_width, m_height, 1);

    context.barrier({ Barrier(m_ambientBuffer[0], STATE_WRITE, STATE_SHADER_READ),
                      Barrier(m_ambientBuffer[1], STATE_SHADER_READ, STATE_WRITE) });

    context.setComputeMode(RenderingPipeline::cm_disocclusion_blur);

    context.setConstant(0, dim);
    context.bind(1, m_ambientBuffer[1].writeHandle());
    context.bind(2, m_ambientBuffer[0]);
    context.bind(3, m_depthBuffer[m_frontBuffer]);
    context.bind(4, m_sppBuffer[m_frontBuffer]);
    context.dispatch(m_width, m_height, 1);

    //context.barrier(Barrier(m_ambientBuffer[1], STATE_WRITE, STATE_READ));
}

void SceneManager::giPass()
{
    RaytraceContext rc = RenderingPipeline::StartRaytracing();
    ComputeContext context = rc;

    raytracePass(rc);
    denoisePass(context);

    context.finish();
}

void SceneManager::ambientPass()
{
    AmbientParams ambientParams = { vec3{ 0.05f, 0.05f, 0.05f },
                                    vec3{ 0.0f, 0.0f, 0.0f} };

    AmbientParams giParams = { vec3{ 2.0f, 2.0f, 2.0f },
                               vec3{ 0.005f, 0.005f, 0.005f} };

    ComputeContext context = m_commandList;
    
    context.setComputeMode(RenderingPipeline::cm_ambient);
    context.barrier(Barrier(m_hdrBuffer, STATE_RENDER, STATE_WRITE));

    context.setConstant(0, m_giActive ? giParams : ambientParams);
    context.bind(1, m_hdrBuffer.writeHandle());
    context.bind(2, (m_giActive && m_enableGI) ? m_ambientBuffer[1] : 1);
    context.bind(3, m_colorBuffer);
    context.bind(4, m_paramsBuffer);
    context.dispatch(m_xtiles, m_ytiles, 1);

    context.barrier(Barrier(m_hdrBuffer, STATE_WRITE, STATE_RENDER));
}

void SceneManager::spritePass()
{
    if (m_spriteBuffer.empty() && m_scene.displayListSprites().empty()) return;

    m_commandList.setRenderMode(RenderingPipeline::rm_sprite);
    m_commandList.bindConstantBuffer(0, m_sceneConstantBuffer);
    m_commandList.setDefaultViewport();

    m_commandList.barrier(m_depthBuffer[m_frontBuffer], STATE_DEPTH_WRITE);
    m_commandList.bindFrameBuffer(m_hdrBuffer, m_depthBuffer[m_frontBuffer]);

    m_commandList.setTopology(topology_trianglestrip);
    m_commandList.bind(1, 0);

    if (!m_spriteBuffer.empty())
    {
        m_commandList.bindVertexBuffer(m_spriteBuffer);
        m_commandList.drawInstanced(m_spriteBuffer.size(), 4, 0);
    }

    const DisplayList& displayList = m_scene.displayListSprites();

    for (const DisplayBlock* block : displayList)
    {
        m_commandList.bindVertexBuffer(block->vertexData);
        m_commandList.drawInstanced(block->displayData[0]->vertexnum, 4, 0);
    }
}

void SceneManager::fogPass()
{
    if (m_fogVolumes.empty()) return;

    m_commandList.setRenderMode(RenderingPipeline::rm_fog);
    m_commandList.bindConstantBuffer(0, m_sceneConstantBuffer);
    m_commandList.bindConstantBuffer(1, m_lightingConstantBuffer);
    m_commandList.setDefaultViewport();

    m_barriers.transit(m_depthBuffer[m_frontBuffer], STATE_PIXEL_SHADER_READ);    
    m_barriers.transit(m_lightGrid, STATE_PIXEL_SHADER_READ);
    m_commandList.barrier(m_barriers);

    m_commandList.bind(3, m_depthBuffer[m_frontBuffer]);
    m_commandList.bind(4, m_lightGrid);
    m_commandList.bind(5, m_omniLightData);
    m_commandList.bind(6, m_spotLightData);
    m_commandList.bind(7, m_shadowBuffer);
    m_commandList.bind(8, m_cubemapBuffers[0]);
    m_commandList.bind(9, m_shadowBuffers[0]);

    m_commandList.bindFrameBuffer(m_hdrBuffer);

    m_commandList.setTopology(topology_trianglestrip);
    m_commandList.bindVertexBuffer(m_volumeVertexBuffer);
    m_commandList.bindIndexBuffer(m_volumeIndexBuffer);

    for (const FogVolume* volume : m_scene.fogVolumes())
    {
        m_commandList.setConstant(2, volume->data());
        m_commandList.drawIndexed(14);
    }
}

void SceneManager::toneMapping()
{
    FrameBuffer& frameBuffer = Win32App::GetFrameBuffer();

    m_commandList.setRenderMode(RenderingPipeline::rm_tone_mapping);
    m_commandList.setDefaultViewport();

    m_commandList.barrier(Barrier(m_hdrBuffer, STATE_RENDER, STATE_PIXEL_SHADER_READ));
    m_commandList.barrier(frameBuffer.writeBarriers());
    m_commandList.bindFrameBuffer(frameBuffer);

    m_commandList.bind(0, m_hdrBuffer);
    m_commandList.setTopology(topology_trianglestrip);
    m_commandList.draw(4);

    m_commandList.barrier(frameBuffer.readBarriers());
}

void SceneManager::clearScreen()
{
    FrameBuffer& frameBuffer = Win32App::GetFrameBuffer();

    RenderingPipeline::StartRender(m_commandList, RenderingPipeline::rm_simple);
    m_commandList.setDefaultViewport();
    m_commandList.clearBuffer(frameBuffer, { 0.0, 0.0, 0.0, 1.0 });
    m_commandList.finish();
    m_gpuInstance.execute(m_commandList);
}

void SceneManager::drawBBox(const BBox& bbox)
{
    vec3 verts[8] = { {bbox.min},
                      {bbox.max.x, bbox.min.y, bbox.min.z},
                      {bbox.max.x, bbox.min.y, bbox.max.z},
                      {bbox.min.x, bbox.min.y, bbox.max.z},

                      {bbox.min.x, bbox.max.y, bbox.min.z},
                      {bbox.max.x, bbox.max.y, bbox.min.z},
                      {bbox.max},
                      {bbox.min.x, bbox.max.y, bbox.max.z} };

    m_vertexBuffer.push(verts[0]);
    m_vertexBuffer.push(verts[1]);

    m_vertexBuffer.push(verts[1]);
    m_vertexBuffer.push(verts[2]);

    m_vertexBuffer.push(verts[2]);
    m_vertexBuffer.push(verts[3]);

    m_vertexBuffer.push(verts[3]);
    m_vertexBuffer.push(verts[0]);

    //
    m_vertexBuffer.push(verts[4]);
    m_vertexBuffer.push(verts[5]);

    m_vertexBuffer.push(verts[5]);
    m_vertexBuffer.push(verts[6]);

    m_vertexBuffer.push(verts[6]);
    m_vertexBuffer.push(verts[7]);

    m_vertexBuffer.push(verts[7]);
    m_vertexBuffer.push(verts[4]);

    //
    m_vertexBuffer.push(verts[0]);
    m_vertexBuffer.push(verts[4]);

    m_vertexBuffer.push(verts[1]);
    m_vertexBuffer.push(verts[5]);

    m_vertexBuffer.push(verts[2]);
    m_vertexBuffer.push(verts[6]);

    m_vertexBuffer.push(verts[3]);
    m_vertexBuffer.push(verts[7]);
}

void SceneManager::wireframePass()
{
    if (m_world.empty()) return;

    FrameBuffer& frameBuffer = Win32App::GetFrameBuffer();

    RenderingPipeline::StartRender(m_commandList, RenderingPipeline::rm_color_wire);
    m_commandList.bindConstantBuffer(0, m_sceneConstantBuffer);
    m_commandList.setDefaultViewport();

    m_commandList.barrier(frameBuffer.writeBarriers());
    m_commandList.bindFrameBuffer(frameBuffer);

    m_commandList.clearBuffer(frameBuffer, { 0.0f, 0.6f, 0.8f, 1.0f });

    const vec4 GeometryColor = vec4(1.0, 1.0, 1.0, 1.0);
    m_commandList.setConstant(2, GeometryColor);

    m_commandList.setTopology(topology_trianglelist);
    m_commandList.drawColor(m_scene.displayList());

    // Draw portals
    static const mat4 PortalMat = {};
    static const vec4 PortalColor = { 0.0f, 1.0f, 0.0f, 1.0f };
    static const vec4 BBoxColor = { 1.0f, 0.0f, 0.0f, 1.0f };

    m_commandList.setTopology(topology_linestrip);
    m_commandList.setConstant(1, PortalMat);
    m_commandList.setConstant(2, PortalColor);
    m_vertexBuffer.clear();

    size_t offset = 0;

    for (const Portal& portal : m_world.portals())
    {
        const std::vector<vec3>& verts = (portal.bid == -1) ? portal.verts : portal.vbuff[portal.bid];

        if (verts.size() > 2)
        {
            for (const vec3& vert : verts) m_vertexBuffer.push(vert);
            m_vertexBuffer.push(verts[0]);

            offset += verts.size() + 1;
        }
    }

    //for (const Leaf& leaf : m_world.leafs()) drawBBox(leaf.bbox);

    m_commandList.bindVertexBuffer(m_vertexBuffer);

    offset = 0;

    for (const Portal& portal : m_world.portals())
    {
        const std::vector<vec3>& verts = (portal.bid == -1) ? portal.verts : portal.vbuff[portal.bid];

        if (verts.size() > 2)
        {
            m_commandList.draw(verts.size() + 1, offset);
            offset += verts.size() + 1;
        }
    }

    m_commandList.setTopology(topology_linelist);
    m_commandList.setConstant(2, BBoxColor);
    m_commandList.drawColor(m_scene.displayListDebug());

    //m_commandList.setTopology(RenderingPipeline::topology_linelist);
    //commandList->SetGraphicsRoot32BitConstants(2, 4, &BBoxColor, 0);
    //commandList->DrawInstanced(m_world.leafs().size()*24, 1, offset, 0);

    m_commandList.barrier(frameBuffer.readBarriers());

    m_commandList.finish();
    m_gpuInstance.execute(m_commandList);
}

void SceneManager::allocateShadowBuffers(std::vector<Light*>& lights, Light* frameLights[], Light* cachedLights[])
{
    for (Light* light : lights)
    {
        if (light->shadowType() == LightShadow::None) continue;

        size_t shadowIdx = light->shadowIndex();
        size_t cacheIdx = light->cacheIndex();

        if (cacheIdx == Light::InvalidShadowIndex)
        {
            size_t candidate = Light::InvalidShadowIndex;

            for (size_t i = 0; i < ShadowCacheSize; i++)
            {
                if (cachedLights[i] == nullptr)
                {
                    cachedLights[i] = light;
                    light->setCacheIndex(i);
                    light->setStaticUpdateFlag(true);

                    candidate = Light::InvalidShadowIndex;

                    break;
                }

                if (cachedLights[i]->frame() != m_frame)
                {
                    if (candidate == Light::InvalidShadowIndex || cachedLights[i]->frame() < cachedLights[candidate]->frame())
                    {
                        candidate = i;
                    }
                }
            }

            if (candidate != Light::InvalidShadowIndex)
            {
                cachedLights[candidate]->setCacheIndex(Light::InvalidShadowIndex);
                cachedLights[candidate] = light;
                light->setCacheIndex(candidate);
                light->setStaticUpdateFlag(true);
            }
        }

        if (light->shadowType() == LightShadow::Dynamic && shadowIdx == Light::InvalidShadowIndex)
        {
            for (size_t i = 0; i < ShadowMapNum; i++)
            {
                if (frameLights[i] == nullptr || frameLights[i]->frame() != m_frame)
                {
                    if (frameLights[i]) frameLights[i]->setShadowIndex(Light::InvalidShadowIndex);
                    
                    frameLights[i] = light;
                    light->setShadowIndex(i);

                    break;
                }
            }
        }
    }
}

void SceneManager::calculateVisibility()
{
    incrementFrameNum();

    vec3 frustum[4];

    static const vec2 quadVertices[] =
    {
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },
        { 1.0f, -1.0f },
        { -1.0f, -1.0f }
    };

    const vec3& pos = m_camera.pos();
    const vec3& dir = m_camera.direction();
    float d = dir * pos;

    for (int i = 0; i < 4; i++)
    {
        frustum[i] = m_camera.basis() * vec3(quadVertices[i].x * m_fovx, quadVertices[i].y * m_fovy, 1.0) + m_camera.pos();
    }

    m_scene.calculateVisibility(m_camera.pos(), vec4(dir, -d), frustum, m_frame);
    m_scene.enumLights(m_omniLights, m_spotLights, m_frame);

    if (m_omniLightIndices.size() < m_omniLights.size()) m_omniLightIndices.resize(m_omniLights.size() * 2);
    if (m_spotLightIndices.size() < m_spotLights.size()) m_spotLightIndices.resize(m_spotLights.size() * 2);

    for (size_t i = 0; i < m_omniLights.size(); i++) m_omniLightIndices[i] = m_omniLights[i]->id();
    for (size_t i = 0; i < m_spotLights.size(); i++) m_spotLightIndices[i] = m_spotLights[i]->id();

    allocateShadowBuffers(m_omniLights, m_frameLights, m_cachedLights);
    allocateShadowBuffers(m_spotLights, m_frameSpotLights, m_cachedSpotLights);

    if (!m_wireframe)
    {
        incrementFrameNum();
        m_dirLightScene.globalLightVisibility(m_dirLight.direction(), m_frame);
    }

    bool dirLight = m_dirLightActive && m_scene.isGlobalLit();
    m_lightingConstantBuffer->enable_dir_light = dirLight;
    m_raytraceContantBuffer->enable_dir_light = dirLight;
}

void SceneManager::setupView()
{
    mat4 worldMat = m_camera.basis();
    mat4 projView = m_projMat * m_camera.viewMat();

    m_raytraceContantBuffer->prev_eyepos = m_eyepos;

    m_eyepos = m_camera.pos();

    m_sceneConstantBuffer->projViewMat = projView;
    m_sceneConstantBuffer->worldMat = worldMat;
    m_sceneConstantBuffer->eyepos = m_eyepos;

    m_backgroundConstantBuffer->projViewMat = m_projMat * m_camera.rotMat();

    m_raytraceContantBuffer->eyepos = m_eyepos;

    m_raytraceContantBuffer->prev_topleft = m_topleft;
    m_raytraceContantBuffer->prev_xdir = m_xdir;
    m_raytraceContantBuffer->prev_ydir = m_ydir;

    m_topleft = worldMat * vec3(-m_fovx, m_fovy, 1.0f);
    m_xdir = worldMat[0] * m_fovx * (2.0 / m_width);
    m_ydir = -(worldMat[1] * m_fovy * (2.0 / m_height));

    m_raytraceContantBuffer->topleft = m_topleft;
    m_raytraceContantBuffer->xdir = m_xdir;
    m_raytraceContantBuffer->ydir = m_ydir;
    m_raytraceContantBuffer->frame++;
    m_raytraceContantBuffer->reprojectionMat = m_reprojectionMat;

    m_reprojectionMat = projView;

    m_lightingConstantBuffer->topleft.xyz = m_topleft;
    m_lightingConstantBuffer->xdir.xyz = m_xdir;
    m_lightingConstantBuffer->ydir.xyz = m_ydir;
    m_lightingConstantBuffer->eyepos = m_eyepos;
}

void SceneManager::display()
{
    if (m_world.empty())
    {
        clearScreen();
        return;
    }
   
    setupView();

    if (m_dirLightActive)
    {
        m_dirLight.setPosition(m_camera.pos());
    }

    calculateVisibility();
    processSkeletalObjects();

    bool dirLightActive = m_dirLightActive && m_scene.isGlobalLit();

    if (m_wireframe) wireframePass();
    else
    {
        geometryPass();

        if (!m_omniLights.empty() || !m_spotLights.empty() || dirLightActive) shadowPass();
        if (m_giActive && m_enableGI) giPass();

        if (false)
        {
            lightPassSimple();
            ambientPass();
        }
        else
        {
            m_commandList.start();
            lightPass();
        }

        skyPass();
        emissivePass();
        spritePass();
        fogPass();

        toneMapping();

        m_commandList.finish();
        m_gpuInstance.execute(m_commandList);
    }

    //m_d3dInstance.execute(m_commandList.Get());

    m_frontBuffer = (m_frontBuffer + 1) & 1;
}

} //namespace render