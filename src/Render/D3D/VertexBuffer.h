#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <vector>

namespace Render
{

using Microsoft::WRL::ComPtr;

struct Vertex;

class VertexBuffer
{
    ComPtr<ID3D12Resource> m_buffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    UINT m_srvHandle;
    UINT m_uavHandle;

    UINT m_size;

public:

    VertexBuffer();
    explicit VertexBuffer(UINT size);
    ~VertexBuffer();

    UINT size() const { return m_size; }
    bool empty() const { return m_size == 0; }

    void reset();
    void resize(UINT size);
    void setData(const Vertex* data, UINT size);
    void setAccessFlags(bool read, bool write);

    template<class T>
    void setData(const T* data, UINT size)
    {
        m_size = size;

        Render::D3DInstance& d3dInstance = D3DInstance::GetInstance();
        m_buffer = d3dInstance.createBuffer(data, size * sizeof(T));

        m_vertexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(T);
        m_vertexBufferView.SizeInBytes = sizeof(T) * size;
    }

    UINT readHandle() const { return m_srvHandle; }
    UINT writeHandle() const { return m_uavHandle; }

    operator UINT() const { return m_srvHandle; }
    operator ID3D12Resource*() const { return m_buffer.Get(); }
    operator const D3D12_VERTEX_BUFFER_VIEW* () const { return &m_vertexBufferView; }
};

// For Editor objects
template<class T>
class VertexArray
{
    ComPtr<ID3D12Resource> m_buffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    T* m_data;

    UINT m_size;

public:

    VertexArray()
    : m_size(0)
    {
    }

    VertexArray(const VertexArray<T>& vertices)
    {
        resize(vertices.m_size);
        memcpy(m_data, vertices.m_data, m_size * sizeof(T));
    }

    VertexArray(const std::vector<T>& vertices)
    {
        resize(vertices.size());
        memcpy(m_data, vertices.data(), m_size * sizeof(T));
    }

    void resize(UINT size)
    {
        m_buffer.Reset();

        m_size = size;

        Render::D3DInstance& d3dInstance = D3DInstance::GetInstance();
        d3dInstance.device()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(size * sizeof(T), D3D12_RESOURCE_FLAG_NONE),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_buffer));

        m_vertexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(T);
        m_vertexBufferView.SizeInBytes = sizeof(T) * size;

        CD3DX12_RANGE readRange(0, 0);

        m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&m_data));
    }

    bool empty() const { return m_size == 0; }
    size_t size() const { return m_size; }
    T* data() { return m_data; }
    const T* data() const { return m_data; }

    T& operator[](size_t i) 
    {
        return m_data[i];
    }

    const T& operator[](size_t i) const
    {
        return m_data[i];
    }

    VertexArray& operator=(const std::vector<T>& vertices)
    {
        size_t newsize = vertices.size();

        if (m_size != newsize)
        {
            m_size = newsize;
            resize(newsize);
        }

        memcpy(m_data, vertices.data(), m_size * sizeof(T));

        return *this;
    }

    operator ID3D12Resource* () { return m_buffer.Get(); }
    operator const D3D12_VERTEX_BUFFER_VIEW* () const { return &m_vertexBufferView; }
};

} //namespace render