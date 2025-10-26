#include "RaytraceScene.h"

#include "Render/D3D/D3DInstance.h"
#include "System/ErrorMsg.h"
#include "math/math3d.h"

#include "Render/D3D/RenderingPipeline.h"

namespace Render
{

RaytraceGeometry::RaytraceGeometry(const D3D12_VERTEX_BUFFER_VIEW* vertexBuffer, const std::vector<GeometryData>& geometryData)
{
    addGeometry(vertexBuffer, geometryData);
}

RaytraceGeometry::RaytraceGeometry(const VertexBuffer& vertexBuffer)
{
    addGeometry(vertexBuffer);
}

RaytraceGeometry::RaytraceGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer)
{
    addGeometry(vertexBuffer, indexBuffer);
}

void RaytraceGeometry::addGeometry(const D3D12_VERTEX_BUFFER_VIEW* vertexBuffer, const std::vector<GeometryData>& geometryData)
{
    for (const GeometryData& data : geometryData)
    {
        D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
        geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
        geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
        geometryDesc.Triangles.IndexBuffer = 0;
        geometryDesc.Triangles.IndexCount = 0;
        geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_UNKNOWN;
        geometryDesc.Triangles.Transform3x4 = 0;
        geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        geometryDesc.Triangles.VertexCount = data.count;
        geometryDesc.Triangles.VertexBuffer.StartAddress = vertexBuffer->BufferLocation + data.offset * vertexBuffer->StrideInBytes;
        geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexBuffer->StrideInBytes;

        m_geometryDesc.push_back(geometryDesc);
    }
}

void RaytraceGeometry::addGeometry(const VertexBuffer& vertexBuffer)
{
    const D3D12_VERTEX_BUFFER_VIEW* vertexView = vertexBuffer;

    D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
    geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
    geometryDesc.Triangles.IndexBuffer = 0;
    geometryDesc.Triangles.IndexCount = 0;
    geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_UNKNOWN;
    geometryDesc.Triangles.Transform3x4 = 0;
    geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    geometryDesc.Triangles.VertexCount = vertexBuffer.size();
    geometryDesc.Triangles.VertexBuffer.StartAddress = vertexView->BufferLocation;
    geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexView->StrideInBytes;

    m_geometryDesc.push_back(geometryDesc);
}

void RaytraceGeometry::addGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer)
{
    const D3D12_VERTEX_BUFFER_VIEW* vertexView = vertexBuffer;
    const D3D12_INDEX_BUFFER_VIEW* indexView = indexBuffer;

    D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
    geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
    geometryDesc.Triangles.IndexBuffer = indexView->BufferLocation;
    geometryDesc.Triangles.IndexCount = indexBuffer.size();
    geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
    geometryDesc.Triangles.Transform3x4 = 0;
    geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    geometryDesc.Triangles.VertexCount = vertexBuffer.size();
    geometryDesc.Triangles.VertexBuffer.StartAddress = vertexView->BufferLocation;
    geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexView->StrideInBytes;

    m_geometryDesc.push_back(geometryDesc);
}

void RaytraceGeometry::addGeometry(const VertexBuffer& vertexBuffer, const vec3& pos)
{
    const D3D12_VERTEX_BUFFER_VIEW* view = vertexBuffer;

    D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
    geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
    geometryDesc.Triangles.IndexBuffer = 0;
    geometryDesc.Triangles.IndexCount = 0;
    geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_UNKNOWN;
    geometryDesc.Triangles.Transform3x4 = 0;
    geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    geometryDesc.Triangles.VertexCount = vertexBuffer.size();
    geometryDesc.Triangles.VertexBuffer.StartAddress = view->BufferLocation;
    geometryDesc.Triangles.VertexBuffer.StrideInBytes = view->StrideInBytes;

    m_transformOffsets.emplace_back(m_geometryDesc.size(), m_transformData.size() * sizeof(vec4));

    m_transformData.emplace_back(1.0f, 0.0f, 0.0f, pos.x);
    m_transformData.emplace_back(0.0f, 1.0f, 0.0f, pos.y);
    m_transformData.emplace_back(0.0f, 0.0f, 1.0f, pos.z);

    m_geometryDesc.push_back(geometryDesc);
}

void RaytraceGeometry::addGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer, const vec3& pos)
{
    const D3D12_VERTEX_BUFFER_VIEW* vertexView = vertexBuffer;
    const D3D12_INDEX_BUFFER_VIEW* indexView = indexBuffer;

    D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
    geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
    geometryDesc.Triangles.IndexBuffer = indexView->BufferLocation;
    geometryDesc.Triangles.IndexCount = indexBuffer.size();
    geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
    geometryDesc.Triangles.Transform3x4 = 0;
    geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    geometryDesc.Triangles.VertexCount = vertexBuffer.size();
    geometryDesc.Triangles.VertexBuffer.StartAddress = vertexView->BufferLocation;
    geometryDesc.Triangles.VertexBuffer.StrideInBytes = vertexView->StrideInBytes;

    m_transformOffsets.emplace_back(m_geometryDesc.size(), m_transformData.size() * sizeof(vec4));

    m_transformData.emplace_back(1.0f, 0.0f, 0.0f, pos.x);
    m_transformData.emplace_back(0.0f, 1.0f, 0.0f, pos.y);
    m_transformData.emplace_back(0.0f, 0.0f, 1.0f, pos.z);

    m_geometryDesc.push_back(geometryDesc);
}

void RaytraceGeometry::addGeometry(const VertexBuffer& vertexBuffer, const mat4& mat)
{
    const D3D12_VERTEX_BUFFER_VIEW* view = vertexBuffer;

    D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
    geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
    geometryDesc.Triangles.IndexBuffer = 0;
    geometryDesc.Triangles.IndexCount = 0;
    geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_UNKNOWN;
    geometryDesc.Triangles.Transform3x4 = 0;
    geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    geometryDesc.Triangles.VertexCount = vertexBuffer.size();
    geometryDesc.Triangles.VertexBuffer.StartAddress = view->BufferLocation;
    geometryDesc.Triangles.VertexBuffer.StrideInBytes = view->StrideInBytes;

    m_transformOffsets.emplace_back(m_geometryDesc.size(), m_transformData.size() * sizeof(vec4));

    m_transformData.emplace_back(mat[0][0], mat[1][0], mat[2][0], mat[3][0]);
    m_transformData.emplace_back(mat[0][1], mat[1][1], mat[2][1], mat[3][1]);
    m_transformData.emplace_back(mat[0][2], mat[1][2], mat[2][2], mat[3][2]);

    m_geometryDesc.push_back(geometryDesc);
}

void RaytraceGeometry::setScratchBuffer(D3D12_GPU_VIRTUAL_ADDRESS buffer)
{
    m_buildDesc.ScratchAccelerationStructureData = buffer;
}

void RaytraceGeometry::buildTransformInfo(ID3D12Device5* device)
{
    if (m_transformOffsets.empty()) return;

    size_t size = m_transformData.size() * sizeof(vec4);

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_transformInfo)));

    void* data;

    CD3DX12_RANGE readRange(0, 0);
    m_transformInfo->Map(0, &readRange, reinterpret_cast<void**>(&data));

    memcpy(data, m_transformData.data(), size);

    m_transformInfo->Unmap(0, nullptr);

    for (size_t i = 0; i < m_transformOffsets.size(); i++)
    {
        const auto& offset = m_transformOffsets[i];
        m_geometryDesc[offset.first].Triangles.Transform3x4 = m_transformInfo->GetGPUVirtualAddress() + offset.second;
    }
}

void RaytraceGeometry::init(ID3D12Device5* dxrDevice)
{
    m_buildDesc = {};
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& bottomLevelInputs = m_buildDesc.Inputs;
    bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    bottomLevelInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
    bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
    bottomLevelInputs.NumDescs = m_geometryDesc.size();
    bottomLevelInputs.pGeometryDescs = m_geometryDesc.data();

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
    dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &prebuildInfo);

    m_scratchDataSize = prebuildInfo.ScratchDataSizeInBytes;

    dxrDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                                       D3D12_HEAP_FLAG_NONE,
                                       &CD3DX12_RESOURCE_DESC::Buffer(prebuildInfo.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
                                       D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
                                       nullptr,
                                       IID_PPV_ARGS(&m_bottomLevelAccelerationStructure));

    m_buildDesc.DestAccelerationStructureData = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();

    buildTransformInfo(dxrDevice);
}

void RaytraceScene::addStaticGeometry(const D3D12_VERTEX_BUFFER_VIEW* vertexBuffer, const std::vector<GeometryData>& geometryData)
{
    m_geometry[0].addGeometry(vertexBuffer, geometryData);
}

void RaytraceScene::addStaticGeometry(const VertexBuffer& vertexBuffer, const vec3& pos)
{
    m_geometry[0].addGeometry(vertexBuffer, pos);
}

void RaytraceScene::addStaticGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer, const vec3& pos)
{
    m_geometry[0].addGeometry(vertexBuffer, indexBuffer, pos);
}

void RaytraceScene::addStaticGeometry(const VertexBuffer& vertexBuffer, const mat4& mat)
{
    m_geometry[0].addGeometry(vertexBuffer, mat);
}

RaytraceId RaytraceScene::createGeometry()
{
    m_geometry.emplace_back();

    return m_geometry.size() - 1;
}

RaytraceId RaytraceScene::createGeometry(const VertexBuffer& vertexBuffer)
{
    m_geometry.emplace_back(vertexBuffer);

    return m_geometry.size() - 1;
}

RaytraceId RaytraceScene::createGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer)
{
    m_geometry.emplace_back(vertexBuffer, indexBuffer);

    return m_geometry.size() - 1;
}

RaytraceId RaytraceScene::addInstance(RaytraceId geometryId, const mat4& mat, uint32_t id)
{
    m_instances.push_back({ id, geometryId, mat });
    return m_instances.size() - 1;
}

void RaytraceScene::setInstanceTransform(RaytraceId instance, const mat4& mat)
{
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t k = 0; k < 4; k++)  m_instanceData[instance].Transform[i][k] = mat[k][i];
    }

    m_update = true;
}

void RaytraceScene::createInstanceDescription(ID3D12Device5* device)
{
    size_t size = sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * m_instances.size();

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_instanceBuffer)));

    CD3DX12_RANGE readRange(0, 0);
    m_instanceBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_instanceData));

    for (size_t i = 0; i < m_instances.size(); i++)
    {
        m_instanceData[i].AccelerationStructure = m_geometry[m_instances[i].geometryId].getAccelerationStructure()->GetGPUVirtualAddress();
        m_instanceData[i].InstanceMask = 1;
        m_instanceData[i].InstanceID = m_instances[i].id;

        for (size_t l = 0; l < 3; l++)
        {
            for (size_t m = 0; m < 4; m++)  m_instanceData[i].Transform[l][m] = m_instances[i].trandform[m][l];
        }
    }
}

void RaytraceScene::build()
{
    ComPtr<ID3D12Device5> dxrDevice;
    D3DInstance::GetInstance().device()->QueryInterface(IID_PPV_ARGS(&dxrDevice));

    for (RaytraceGeometry& geometry : m_geometry) geometry.init(dxrDevice.Get());

    //D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& topLevelInputs = m_buildDesc.Inputs;
    topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
    topLevelInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE | D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
    topLevelInputs.NumDescs = m_instances.size();
    topLevelInputs.pGeometryDescs = nullptr;
    topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

    D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
    dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);

    UINT64 scratchSize = topLevelPrebuildInfo.ScratchDataSizeInBytes;

    for (const RaytraceGeometry& geometry : m_geometry) scratchSize = max(scratchSize, geometry.scratchDataSize());

    dxrDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(scratchSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        nullptr,
        IID_PPV_ARGS(&m_scratchResource));

    dxrDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(topLevelPrebuildInfo.ResultDataMaxSizeInBytes, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
        D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
        nullptr,
        IID_PPV_ARGS(&m_topLevelAccelerationStructure));

    createInstanceDescription(dxrDevice.Get());

    for (RaytraceGeometry& geometry : m_geometry) geometry.setScratchBuffer(m_scratchResource->GetGPUVirtualAddress());

    m_buildDesc.DestAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
    m_buildDesc.ScratchAccelerationStructureData = m_scratchResource->GetGPUVirtualAddress();
    m_buildDesc.Inputs.InstanceDescs = m_instanceBuffer->GetGPUVirtualAddress();

    RenderingPipeline::RaytraceStartBuild();

    for (RaytraceGeometry& geometry : m_geometry)
    {
        RenderingPipeline::RaytraceBuildAs(geometry.getBuildDesc());
        RenderingPipeline::RaytraceBuildBarrier(geometry.getAccelerationStructure());
    }

    RenderingPipeline::RaytraceBuildAs(&m_buildDesc);
    RenderingPipeline::RaytraceFinishBuild();

    m_update = false;
}

void RaytraceScene::update(ID3D12GraphicsCommandList5* commandList)
{
    if (!m_update) return;

    commandList->BuildRaytracingAccelerationStructure(&m_buildDesc, 0, nullptr);
    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(m_topLevelAccelerationStructure.Get()));

    m_update = false;
}

void RaytraceScene::clear()
{
    m_transformData.clear();
    m_transformOffsets.clear();

    m_topLevelAccelerationStructure.Reset();
    m_instanceBuffer.Reset();
    m_scratchResource.Reset();

    m_geometry.clear();
    m_instances.clear();
}

} //namespace Render