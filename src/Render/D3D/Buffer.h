#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "Render/D3D/D3DInstance.h"

namespace Render
{

using Microsoft::WRL::ComPtr;

enum BufferFlags : uint8_t
{
    BufferRead = 1,
    BufferWrite = 2,
    BufferReadBack = 4
};

template<class T>
class Buffer
{
    ComPtr<ID3D12Resource> m_buffer;

    uint8_t m_flags;

    UINT m_srvHandle;
    UINT m_uavHandle;

    D3D12_RESOURCE_STATES m_state;

public:
    explicit Buffer(uint8_t flags = BufferRead)
    : m_srvHandle(0)
    , m_flags(flags)
    {
    }

    explicit Buffer(UINT size, uint8_t flags = BufferRead)
    : m_srvHandle(0)
    , m_flags(flags)
    {
        resize(size);
    }

    ~Buffer()
    {
        if (m_srvHandle != 0) D3DInstance::GetInstance().FreeDescriptor(m_srvHandle);
        if (m_uavHandle != 0) D3DInstance::GetInstance().FreeDescriptor(m_uavHandle);
    }

    void resize(UINT size)
    {
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

        if (m_flags & BufferWrite) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        D3DInstance& d3dInstance = D3DInstance::GetInstance();
        d3dInstance.device()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(size * sizeof(T), flags),
            D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
            nullptr,
            IID_PPV_ARGS(&m_buffer));

        if (m_srvHandle == 0) m_srvHandle = d3dInstance.AllocateDescriptor();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = size;
        srvDesc.Buffer.StructureByteStride = sizeof(T);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        d3dInstance.device()->CreateShaderResourceView(m_buffer.Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandle));

        if (m_flags & BufferWrite)
        {
            if (m_uavHandle == 0) m_uavHandle = d3dInstance.AllocateDescriptor();

            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = DXGI_FORMAT_UNKNOWN;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.NumElements = size;
            uavDesc.Buffer.StructureByteStride = sizeof(T);
            uavDesc.Buffer.CounterOffsetInBytes = 0;
            uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
            d3dInstance.device()->CreateUnorderedAccessView(m_buffer.Get(), nullptr, &uavDesc, d3dInstance.CpuDescriptor(m_uavHandle));
        }

        m_state = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
    }

    void setData(const T* data, UINT size)
    {
        D3DInstance& d3dInstance = D3DInstance::GetInstance();

        m_buffer = d3dInstance.createBuffer(data, size * sizeof(T));

        if (m_srvHandle == 0) m_srvHandle = d3dInstance.AllocateDescriptor();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = size;
        srvDesc.Buffer.StructureByteStride = sizeof(T);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        d3dInstance.device()->CreateShaderResourceView(m_buffer.Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandle));

        m_state = D3D12_RESOURCE_STATE_GENERIC_READ;
    }

    D3D12_RESOURCE_STATES transitState(D3D12_RESOURCE_STATES state)
    {
        D3D12_RESOURCE_STATES oldState = m_state;
        m_state = state;

        return oldState;
    }

    UINT readHandle() const { return m_srvHandle; }
    UINT writeHandle() const { return m_uavHandle; }

    operator ID3D12Resource* () const { return m_buffer.Get(); }

    operator bool() const { return static_cast<bool>(m_buffer); }
    operator UINT() const { return m_srvHandle; }
};

template<class T>
class StreamBuffer
{
    ComPtr<ID3D12Resource> m_buffer;
    T* m_mappedData;
    
    UINT m_srvHandle;
    UINT m_uavHandle;

    size_t m_size;

public:

    StreamBuffer()
    : m_srvHandle(0)
    , m_uavHandle(0)
    , m_size(0)
    {
    }

    explicit StreamBuffer(UINT size)
    : m_srvHandle(0)
    , m_uavHandle(0)
    , m_size(0)
    {
        resize(size);
    }

    ~StreamBuffer()
    {
        if (m_srvHandle != 0) D3DInstance::GetInstance().FreeDescriptor(m_srvHandle);
    }

    void resize(UINT size)
    {
        m_size = size;

        D3DInstance& d3dInstance = D3DInstance::GetInstance();
        d3dInstance.device()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(size * sizeof(T)),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_buffer));

        CD3DX12_RANGE readRange(0, 0);

        m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData));

        if (m_srvHandle == 0) m_srvHandle = d3dInstance.AllocateDescriptor();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = size;
        srvDesc.Buffer.StructureByteStride = sizeof(T);
        srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
        d3dInstance.device()->CreateShaderResourceView(m_buffer.Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandle));
    }

    void setData(const T* data, UINT size)
    {
        memcpy(m_mappedData, data, size * sizeof(T));
    }

    size_t size() const { return m_size; }

    T& operator[](UINT i) { return m_mappedData[i]; }
    const T& operator[](UINT i) const { return m_mappedData[i]; }

    operator bool() const { return static_cast<bool>(m_buffer); }
    operator UINT() const { return m_srvHandle; }
};

} //namespace render