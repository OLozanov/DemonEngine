#pragma once

#include "math/math3d.h"

#include "Resources/Image.h"

#include "Render/Render.h"

#include "Render/SceneConstantBuffer.h"

#include "Render/World.h"
#include "Render/Scene.h"

#include "Render/Camera.h"

#include "Render/Light.h"
#include "Render/DirectionalLight.h"
#include "Render/DisplayObject.h"
#include "Render/FogVolume.h"

#include <map>

namespace Render
{

using Microsoft::WRL::ComPtr;

class SkeletalData;

class SceneManager
{
    struct AmbientParams
    {
        vec4 color;
        vec4 background;
    };

    struct LightGridParams
    {
        uint32_t xtiles;
        float xtile_sz;
        float ytile_sz;
        uint32_t omni_num;
        uint32_t spot_num;
    };

    struct LightingParams
    {
        vec4 topleft;
        vec4 xdir;
        vec4 ydir;
        vec3 eyepos;

        uint32_t xtiles;

        vec3 light_dir;
        uint32_t enable_dir_light;
        vec3 light_flux;
        float pad;
        vec3 ambient_color;
        uint32_t ambient_buffer;
    };

    struct RaytraceConstantBuffer
    {
        vec3 topleft;
        float pad1;
        vec3 xdir;
        float pad2;
        vec3 ydir;
        float pad3;
        vec3 eyepos;
        float pat4;
        vec3 prev_topleft;
        float pad5;
        vec3 prev_xdir;
        float pad6;
        vec3 prev_ydir;
        float pad7;
        vec3 prev_eyepos;
        uint32_t frame;
        vec3 sky_color;
        float pad8;
        vec3 light_color;
        uint32_t enable_dir_light;
        mat4 reprojectionMat;
    };

public:
    static SceneManager& GetInstance();

    Camera& getCamera() { return m_camera; }
    World& getWorld() { return m_world; }

    void init(uint32_t width, uint32_t height);
    void resize(uint32_t width, uint32_t height);

    void reset();

    void enableGI(bool enable);

    void setWireframeMode(bool wire) { m_wireframe = wire; }
    void setSkybox(const std::string& name);
    void setDirectionalLight(const vec3& dir, const vec3& color);
    void setGi(const vec3& color);

    void addLight(Light* light);
    void removeLight(Light* light);

    void addObject(DisplayObject* object);
    void removeObject(DisplayObject* object);

    void addFogVolume(FogVolume* volume);
    void removeFogVolume(FogVolume* volume);

    void initStaticGeometry();
    void addStaticGeometry(const D3D12_VERTEX_BUFFER_VIEW* vertexBuffer, const std::vector<GeometryData>& geometryData);
    void addStaticGeometry(const VertexBuffer& vertexBuffer, const vec3& pos);
    void addStaticGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer, const vec3& pos);
    void addStaticGeometry(const VertexBuffer& vertexBuffer, const mat4& mat);

    RaytraceId addGeometry(const Model* model);
    RaytraceId addGeometryInstance(RaytraceId id, const mat4& mat);
    void setGeometryTransform(RaytraceId instance, const mat4& mat);

    void addOverlay(DisplayObject* object) { m_overlayObjects.append(object); }
    void removeOverlay(DisplayObject* object) { m_overlayObjects.remove(object); }
    void clearOverlay() { m_overlayObjects.clear(); }

    void addSprite(const SpriteData& spriteData);
    void clearSprites() { m_spriteBuffer.clear(); }

    void traceObject(DisplayObject* object) { m_world.addObject(object); }
    void moveObject(DisplayObject* object) { m_world.moveObject(object); }

    void registerSkeletalObject(SkeletalData* data);
    void unregisterSkeletalObject(SkeletalData* data);

    void updateWorld();

    void display();

private:

    SceneManager();
    SceneManager(SceneManager& sm) = delete;
    SceneManager& operator =(SceneManager& sm) = delete;

    void generateHemisphereSamples();

    void resetLightsFrameNum();
    void resetObjectFrameNum();
    void resetFogVolumesFrameNum();
    void resetFrameNum();
    void incrementFrameNum();

    void initResources();

    void processSkeletalObjects();

    void allocateShadowBuffers(std::vector<Light*>& lights, Light* frameLights[], Light* cachedLights[]);
    void calculateVisibility();
    void setupView();

    void drawShadow(CommandList& commandList, Light& light, CubemapBuffer& buffer);
    void drawDirectionalShadow(CommandList& commandList, const Scene& scene);

    void drawOverlay();
    void geometryPass();
    void emissivePass();
    void shadowPass();
    void lightPassSimple();
    void lightPass();
    void skyPass();
    void raytracePass(RaytraceContext& rc);
    void denoisePass(ComputeContext& context);
    void giPass();
    void ambientPass();
    void spritePass();
    void fogPass();

    void toneMapping();
    void clearScreen();

    void drawBBox(const BBox& bbox);
    void wireframePass();

private:

    GpuInstance& m_gpuInstance;

    uint64_t m_frame;

    bool m_wireframe;

    bool m_enableGI;

    int m_frontBuffer;

    uint32_t m_width;
    uint32_t m_height;
    float m_aspectRatio;

    uint32_t m_xtiles;
    uint32_t m_ytiles;

    float m_fovx;
    float m_fovy;

    World m_world;
    Scene m_scene;
    Scene m_dirLightScene;

    Camera m_camera;

    LinkedList<Light> m_lights;
    LinkedList<DisplayObject> m_objects;
    LinkedList<SkeletalData> m_skeletalData;
    LinkedList<FogVolume> m_fogVolumes;

    LinkedList<DisplayObject> m_overlayObjects;
    DisplayList m_overlay;

    RaytraceScene m_raytraceScene;
    std::map<const Model*, RaytraceId> m_raytraceGeometry;

    bool m_dirLightActive;
    DirectionalLight m_dirLight;

    bool m_giActive;
    vec3 m_giColor;

    std::vector<Light*> m_omniLights;
    std::vector<Light*> m_spotLights;

    mat4 m_projMat;
    mat4 m_reprojectionMat;

    BarrierBlock m_barriers;

    ConstantBuffer<SceneConstantBuffer> m_sceneConstantBuffer;
    ConstantBuffer<SceneConstantBuffer> m_backgroundConstantBuffer;
    ConstantBuffer<mat4> m_shadowConstantBuffer;
    ConstantBuffer<LightingParams> m_lightingConstantBuffer;
    ConstantBuffer<RaytraceConstantBuffer> m_raytraceContantBuffer;

    CommandList m_commandList;
    CommandList m_geometryCommandList;
    CommandList m_shadowCommandList;

    DepthBuffer m_depthBuffer[2] = { DepthBuffer(DXGI_FORMAT_D24_UNORM_S8_UINT), DepthBuffer(DXGI_FORMAT_D24_UNORM_S8_UINT) };
    ColorBuffer m_colorBuffer;
    ColorBuffer m_normalsBuffer;
    ColorBuffer m_flatNormalsBuffer[2];
    ColorBuffer m_paramsBuffer;
    ColorBuffer m_hdrBuffer;

    RenderBuffer m_gbuffer[2];
    DepthBuffer m_shadowBuffer;

    static constexpr size_t ShadowMapNum = 8;
    static constexpr size_t ShadowCacheSize = 8;

    Light* m_frameLights[ShadowMapNum];
    Light* m_cachedLights[ShadowCacheSize];

    Light* m_frameSpotLights[ShadowMapNum];
    Light* m_cachedSpotLights[ShadowCacheSize];
    
    CubemapBuffer m_cubemapBuffers[ShadowMapNum];
    CubemapBuffer m_cubemapCache[ShadowCacheSize];

    DepthBuffer m_shadowBuffers[ShadowMapNum];
    DepthBuffer m_shadowCache[ShadowCacheSize];

    StreamBuffer<OmniLightData> m_omniLightData;
    StreamBuffer<SpotLightData> m_spotLightData;
    
    StreamBuffer<uint32_t> m_omniLightIndices;
    StreamBuffer<uint32_t> m_spotLightIndices;

    Buffer<uint32_t> m_lightGrid;

    // RTX buffers
    Bitmap m_sppBuffer[2] = { Bitmap(DXGI_FORMAT_R8_UINT, true, true), Bitmap(DXGI_FORMAT_R8_UINT, true, true) };
    Bitmap m_ambientBuffer[2] = { Bitmap(DXGI_FORMAT_R16G16B16A16_FLOAT, true, true), Bitmap(DXGI_FORMAT_R16G16B16A16_FLOAT, true, true) };

    Bitmap m_background { DXGI_FORMAT_R16G16B16A16_FLOAT };

    Buffer<vec3> m_hemisphereSamples;
    ResourcePtr<Image> m_blueNoiseMask;

    ResourcePtr<Image> m_skyboxFace[6];
    VertexBuffer m_skyboxBuffer;

    VertexBuffer m_volumeVertexBuffer;
    IndexBuffer m_volumeIndexBuffer;

    PushBuffer<SpriteData> m_spriteBuffer;

    // Debug
    PushBuffer<vec3> m_vertexBuffer;

    //
    vec3 m_topleft;
    vec3 m_xdir;
    vec3 m_ydir;
    vec3 m_eyepos;

    bool m_emissiveOverlay;

    static constexpr uint32_t DirShadowSize = 2048;
    static constexpr uint32_t ShadowLods = 3;
    static constexpr uint32_t ShadowCubeSize = 1024;
    static constexpr uint32_t ShadowSize = 1024;

    static constexpr size_t DynamicOmniLights = 16;
    static constexpr size_t DynamicSpotLights = 1;

    static constexpr uint32_t LightGridTileSize = 16;
    static constexpr uint32_t TileListSize = 128;
};

} //namespace Render