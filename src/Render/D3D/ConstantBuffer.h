#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "Render/D3D/D3DInstance.h"

namespace Render
{

template<class T>
class ConstantBuffer
{
    ComPtr<ID3D12Resource> m_buffer;
    T* m_mappedData;
public:

    explicit ConstantBuffer(UINT size = 1)
    {
        D3DInstance& d3dInstance = D3DInstance::GetInstance();

        UINT bufferSize = sizeof(T) * size;
        bufferSize = ceil(bufferSize / 256.0f) * 256;

        d3dInstance.device()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_buffer));

        CD3DX12_RANGE readRange(0, 0);
        m_buffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData));
    }

    operator D3D12_GPU_VIRTUAL_ADDRESS() const { return m_buffer->GetGPUVirtualAddress(); }
    T& operator*() const { return *m_mappedData; }
    T* operator->() const { return m_mappedData; }
    T& operator[](size_t n) const { return m_mappedData[n]; }
    
    ConstantBuffer<T>& operator=(const T& obj) 
    {
        memcpy(m_mappedData, &obj, sizeof(T));
        return *this;
    }
};

} //namespace render