#include "SwapChain.h"

#include "System/ErrorMsg.h"

namespace Render
{

SwapChain::SwapChain(IDXGISwapChain3* swapChain, bool depthBuffer)
: m_useDepth(depthBuffer)
{
    m_swapChain = swapChain;

    allocateDescriptors();

    if (m_useDepth) createDepthBuffer();
    createRenderTargetViews();
}

SwapChain::~SwapChain()
{
    m_swapChain->SetFullscreenState(false, nullptr);
}

FrameBuffer& SwapChain::getFrameBuffer()
{
    UINT frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    return m_frameBuffers[frameIndex];
}

void SwapChain::present()
{
    m_swapChain->Present(1, 0);
    D3DInstance::GetInstance().WaitForGpu();
}

void SwapChain::resize(int width, int height)
{
    if (!m_swapChain) return;

    D3DInstance::GetInstance().WaitForGpu();

    for (UINT n = 0; n < FrameCount; n++) m_frameBuffers[n].reset();

    BOOL fullscreen;
    ThrowIfFailed(m_swapChain->GetFullscreenState(&fullscreen, nullptr));

    DXGI_SWAP_CHAIN_DESC desc = {};
    m_swapChain->GetDesc(&desc);
    ThrowIfFailed(m_swapChain->ResizeBuffers(FrameCount, width, height, desc.BufferDesc.Format, desc.Flags));

    if (m_useDepth) createDepthBuffer();

    createRenderTargetViews();
}

SwapChain& SwapChain::operator=(IDXGISwapChain3* swapChain)
{
    m_swapChain = swapChain;

    allocateDescriptors();

    if (m_useDepth) createDepthBuffer();
    createRenderTargetViews();

    return *this;
}

void SwapChain::allocateDescriptors()
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    for (UINT n = 0; n < FrameCount; n++)
    {
        m_handles[n] = d3dInstance.AllocateRtvDescriptor();
    }

    if (m_useDepth) m_depthHandle = d3dInstance.AllocateDsvDescriptor();
}

void SwapChain::createRenderTargetViews()
{
    D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
    RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    RTVDesc.Texture2D.MipSlice = 0;

    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    for (UINT n = 0; n < FrameCount; n++)
    {
        ComPtr<ID3D12Resource> buffer;

        ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&buffer)));
        d3dInstance.device()->CreateRenderTargetView(buffer.Get(), &RTVDesc, m_handles[n]);
        m_frameBuffers[n].reset(buffer.Get(), &m_handles[n]);

        if (m_useDepth) m_frameBuffers[n].resetDepth(&m_depthHandle);
    }
}

void SwapChain::createDepthBuffer()
{
    DXGI_SWAP_CHAIN_DESC1 desc;

    m_swapChain->GetDesc1(&desc);

    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
    depthStencilDesc.Texture2D.MipSlice = 0;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    CD3DX12_RESOURCE_DESC texDesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        desc.Width,
        desc.Height,
        1,
        1,
        DXGI_FORMAT_D32_FLOAT,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(&m_depthBuffer));

    d3dInstance.device()->CreateDepthStencilView(m_depthBuffer.Get(), &depthStencilDesc, m_depthHandle);
}

} //namespace render