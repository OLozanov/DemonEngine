#include "VertexBuffer.h"

#include "Render/D3D/D3DInstance.h"

#include "Render/DisplayData.h"

namespace Render
{

VertexBuffer::VertexBuffer()
: m_srvHandle(0)
, m_uavHandle(0)
, m_size(0)
{
}

VertexBuffer::VertexBuffer(UINT size)
: m_srvHandle(0)
, m_uavHandle(0)
, m_size(size)
{
    Render::D3DInstance& d3dInstance = D3DInstance::GetInstance();

    d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size * sizeof(Vertex)),
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        nullptr,
        IID_PPV_ARGS(&m_buffer));

    m_vertexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = sizeof(Vertex) * size;
}

VertexBuffer::~VertexBuffer()
{
    Render::D3DInstance& d3dInstance = D3DInstance::GetInstance();

    if (m_srvHandle != 0) d3dInstance.FreeDescriptor(m_srvHandle);
    if (m_uavHandle != 0) d3dInstance.FreeDescriptor(m_uavHandle);
}

void VertexBuffer::reset()
{
    m_buffer.Reset();
}

void VertexBuffer::resize(UINT size)
{
    m_buffer.Reset();

    m_size = size;

    Render::D3DInstance& d3dInstance = D3DInstance::GetInstance();
    d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size * sizeof(Vertex), D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        nullptr,
        IID_PPV_ARGS(&m_buffer));

    m_vertexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = sizeof(Vertex) * size;
}

void VertexBuffer::setData(const Vertex* data, UINT size)
{
    m_size = size;

    Render::D3DInstance& d3dInstance = D3DInstance::GetInstance();
    m_buffer = d3dInstance.createBuffer(data, size * sizeof(Vertex));

    m_vertexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = sizeof(Vertex) * size;
}

void VertexBuffer::setAccessFlags(bool read, bool write)
{
    Render::D3DInstance& d3dInstance = D3DInstance::GetInstance();

    if (read && m_srvHandle == 0)
    {
        m_srvHandle = d3dInstance.AllocateDescriptor();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = m_size;
        srvDesc.Buffer.StructureByteStride = sizeof(Vertex);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        d3dInstance.device()->CreateShaderResourceView(m_buffer.Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandle));
    }

    if (write && m_uavHandle == 0)
    {
        m_uavHandle = d3dInstance.AllocateDescriptor();

        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = m_size;
        uavDesc.Buffer.StructureByteStride = sizeof(Vertex);
        uavDesc.Buffer.CounterOffsetInBytes = 0;
        uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
        d3dInstance.device()->CreateUnorderedAccessView(m_buffer.Get(), nullptr, &uavDesc, d3dInstance.CpuDescriptor(m_uavHandle));
    }
}

} //namespace render