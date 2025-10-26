#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include "Render\D3D\d3dx12.h"
#include <wrl.h>

#include <vector>

namespace Render
{

using Microsoft::WRL::ComPtr;

class FrameBuffer
{
public:
    FrameBuffer()
    : m_rtHandles(nullptr)
    , m_depthHandle(nullptr)
    {
    }

    FrameBuffer(UINT rtCount, D3D12_CPU_DESCRIPTOR_HANDLE* rtHandles = nullptr, D3D12_CPU_DESCRIPTOR_HANDLE* depthHandle = nullptr)
    : m_rtCount(rtCount)
    , m_rtHandles(rtHandles)
    , m_depthHandle(depthHandle)
    {
    }

    UINT size() const { return m_rtCount; }
    const D3D12_CPU_DESCRIPTOR_HANDLE* renderTargets() const { return m_rtHandles; }
    const D3D12_CPU_DESCRIPTOR_HANDLE* depthStencil() const { return m_depthHandle; }

    const std::vector<D3D12_RESOURCE_BARRIER>& writeBarriers() const { return m_writeBarriers; }
    const std::vector<D3D12_RESOURCE_BARRIER>& readBarriers() const { return m_readBarriers; }

    void reset()
    {
        m_writeBarriers.clear();
        m_readBarriers.clear();
    }

    void reset(ID3D12Resource* buffer, const D3D12_CPU_DESCRIPTOR_HANDLE* handle)
    {
        m_rtCount = 1;
        m_rtHandles = handle;

        m_writeBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(buffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
        m_readBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(buffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
    }

    void resetDepth(const D3D12_CPU_DESCRIPTOR_HANDLE* handle)
    {
        m_depthHandle = handle;
    }

protected:
    UINT m_rtCount;
    const D3D12_CPU_DESCRIPTOR_HANDLE* m_rtHandles;
    const D3D12_CPU_DESCRIPTOR_HANDLE* m_depthHandle;

    std::vector<D3D12_RESOURCE_BARRIER> m_writeBarriers;
    std::vector<D3D12_RESOURCE_BARRIER> m_readBarriers;
};

} //namespace render