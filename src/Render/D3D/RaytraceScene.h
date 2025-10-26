#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Render/D3D/d3dx12.h"

#include "Render/D3D/VertexBuffer.h"
#include "Render/D3D/IndexBuffer.h"

#include "Render/DisplayData.h"

#include "math/math3d.h"

namespace Render
{

using Microsoft::WRL::ComPtr;

class RaytraceGeometry
{
private:
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC m_buildDesc;

    ComPtr<ID3D12Resource> m_bottomLevelAccelerationStructure;
    std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> m_geometryDesc;

    ComPtr<ID3D12Resource> m_transformInfo;

    std::vector<vec4> m_transformData;
    std::vector<std::pair<UINT, UINT>> m_transformOffsets;

    UINT64 m_scratchDataSize;

    void buildTransformInfo(ID3D12Device5* device);

public:
    RaytraceGeometry() {}
    RaytraceGeometry(const D3D12_VERTEX_BUFFER_VIEW* vertexBuffer, const std::vector<GeometryData>& geometryData);
    RaytraceGeometry(const VertexBuffer& vertexBuffer);
    RaytraceGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer);

    void addGeometry(const D3D12_VERTEX_BUFFER_VIEW* vertexBuffer, const std::vector<GeometryData>& geometryData);
    void addGeometry(const VertexBuffer& vertexBuffer);
    void addGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer);

    void addGeometry(const VertexBuffer& vertexBuffer, const vec3& pos);
    void addGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer, const vec3& pos);

    void addGeometry(const VertexBuffer& vertexBuffer, const mat4& mat);

    ID3D12Resource* getAccelerationStructure() { return m_bottomLevelAccelerationStructure.Get(); }

    const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC* getBuildDesc() const { return &m_buildDesc; }
    UINT64 scratchDataSize() const { return m_scratchDataSize; }

    void setScratchBuffer(D3D12_GPU_VIRTUAL_ADDRESS buffer);

    void init(ID3D12Device5* dxrDevice);
};

using RaytraceId = size_t;

class RaytraceScene
{
    struct InstanceData
    {
        uint32_t id = 0;
        RaytraceId geometryId;
        mat4 trandform;
    };

private:
    D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC m_buildDesc;

    ComPtr<ID3D12Resource> m_instanceBuffer;
    ComPtr<ID3D12Resource> m_topLevelAccelerationStructure;
    ComPtr<ID3D12Resource> m_scratchResource;

    std::vector<RaytraceGeometry> m_geometry;

    std::vector<D3D12_RAYTRACING_INSTANCE_DESC> m_instanceDesc;

    std::vector<InstanceData> m_instances;
    D3D12_RAYTRACING_INSTANCE_DESC* m_instanceData;

    std::vector<vec4> m_transformData;
    std::vector<std::pair<UINT, UINT>> m_transformOffsets;

    bool m_update = false;

    void createInstanceDescription(ID3D12Device5* device);

public:

    RaytraceScene() {}

    void addStaticGeometry(const D3D12_VERTEX_BUFFER_VIEW* vertexBuffer, const std::vector<GeometryData>& geometryData);
    void addStaticGeometry(const VertexBuffer& vertexBuffer, const vec3& pos);
    void addStaticGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer, const vec3& pos);
    void addStaticGeometry(const VertexBuffer& vertexBuffer, const mat4& mat);

    RaytraceId createGeometry();
    RaytraceId createGeometry(const VertexBuffer& vertexBuffer);
    RaytraceId createGeometry(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer);

    RaytraceId addInstance(RaytraceId geometryId, const mat4& mat, uint32_t id = 0);
    void setInstanceTransform(RaytraceId instance, const mat4& mat);

    void build();
    void update(ID3D12GraphicsCommandList5* commandList);
    void clear();

    operator D3D12_GPU_VIRTUAL_ADDRESS() const { return m_topLevelAccelerationStructure->GetGPUVirtualAddress(); }
};

} //namespace Render