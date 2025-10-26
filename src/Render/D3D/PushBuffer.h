#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "Render/D3D/D3DInstance.h"

namespace Render
{

using Microsoft::WRL::ComPtr;

template<class T>
class PushBuffer
{
    ComPtr<ID3D12Resource> m_buffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    T* m_bufferData;
    size_t m_count;
    size_t m_ptr;

public:

    PushBuffer(UINT count = 64)
    : m_count(count)
    , m_ptr(0)
    {
        D3DInstance& d3dInstance = D3DInstance::GetInstance();

        const UINT size = sizeof(T) * m_count;

        d3dInstance.device()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(size),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_buffer));

        CD3DX12_RANGE readRange(0, size-1);
        m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&m_bufferData));

        m_vertexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(T);
        m_vertexBufferView.SizeInBytes = size;
    }

    void clear()
    {
        m_ptr = 0;
    }

    size_t size() const
    {
        return m_ptr;
    }

    bool empty()
    {
        return m_ptr == 0;
    }

    void expand()
    {
        D3DInstance& d3dInstance = D3DInstance::GetInstance();

        ID3D12Resource* newBuffer;
        T* newData;

        const UINT size = sizeof(T) * m_count * 2;

        d3dInstance.device()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(size),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&newBuffer));

        CD3DX12_RANGE readRange(0, size-1);
        newBuffer->Map(0, &readRange, reinterpret_cast<void**>(&newData));
    
        memcpy(newData, m_bufferData, m_count * sizeof(T));

        m_buffer = newBuffer;
        m_bufferData = newData;

        m_count *= 2;

        m_vertexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(T);
        m_vertexBufferView.SizeInBytes = size;
    }

    void push(const T& element)
    {
        if (m_ptr == m_count) expand();

        m_bufferData[m_ptr] = element;
        m_ptr++;
    }

    T& operator[](UINT i) { return m_bufferData[i]; }
    const T& operator[](UINT i) const { return m_bufferData[i]; }

    operator const D3D12_VERTEX_BUFFER_VIEW* () const { return &m_vertexBufferView; }
};

} //namespace render