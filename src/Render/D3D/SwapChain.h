#pragma once

#include "Render/D3D/D3DInstance.h"

namespace Render
{

class SwapChain
{
public:
    SwapChain() = default;
    SwapChain(IDXGISwapChain3* swapChain, bool depthBuffer = false);

    ~SwapChain();

    FrameBuffer& getFrameBuffer();
    void present();
    void resize(int width, int height);

    SwapChain& operator=(IDXGISwapChain3* swapChain);

private:
    void allocateDescriptors();
    void createRenderTargetViews();
    void createDepthBuffer();

private:
    static constexpr UINT FrameCount = 2;

    bool m_useDepth;

    ComPtr<IDXGISwapChain3> m_swapChain;
    D3D12_CPU_DESCRIPTOR_HANDLE m_depthHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE m_handles[FrameCount];

    ComPtr<ID3D12Resource> m_depthBuffer;

    FrameBuffer m_frameBuffers[FrameCount];
};

} //namespace render