#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include "Render/D3D/d3dx12.h"
#include <wrl.h>

#include "Render/D3D/FrameBuffer.h"

class Image;

namespace Render
{

using Microsoft::WRL::ComPtr;

class D3DInstance
{
public:

    D3DInstance();
    ~D3DInstance();

    static D3DInstance& GetInstance();

    void init();

    bool rtxSupport() { return m_rtxSupport; }

    IDXGISwapChain3* createSwapChain(HWND hwnd, bool maximizable = true);
    void createTexture(Image* image);
    ID3D12Resource* createBuffer(const void* data, UINT size);

    inline ID3D12Device* device() { return m_device.Get(); }
    inline ID3D12DescriptorHeap* descriptorHeap() { return m_srvHeap.Get(); }

    void execute(ID3D12CommandList* commandList);

    inline D3D12_CPU_DESCRIPTOR_HANDLE CpuDescriptor(UINT handle) { return { m_srvHeapCpuPtr + handle * (ULONG_PTR)m_cbvSrvDescriptorSize }; }
    inline D3D12_GPU_DESCRIPTOR_HANDLE GpuDescriptor(UINT handle) { return { m_srvHeapGpuPtr + handle * (ULONG_PTR)m_cbvSrvDescriptorSize }; }

    inline D3D12_CPU_DESCRIPTOR_HANDLE AllocateRtvDescriptor() { return { m_rtvHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_rtvHeapOffset++ * (ULONG_PTR)m_rtvDescriptorSize }; }
    inline D3D12_CPU_DESCRIPTOR_HANDLE AllocateDsvDescriptor() { return { m_dsvHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_dsvHeapOffset++ * (ULONG_PTR)m_dsvDescriptorSize }; }

    UINT AllocateDescriptor();
    void FreeDescriptor(UINT descriptor);

    void WaitForGpu();

private:
    static void SelectAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter);

    void checkFeatureSupport();

    void createSyncObjects();
    void createDefaultTextures();

    static void OnDeviceRemoved(PVOID context, BOOLEAN);

private:
    static constexpr UINT FrameCount = 2;
    static constexpr UINT DescriptorCount = 512;
    static constexpr UINT DsvCount = 40;
    static constexpr UINT RtvCount = 13;

    // Feature support
    bool m_rtxSupport = false;

    // Pipeline objects.
    ComPtr<ID3D12Device> m_device;
    ComPtr<IDXGIFactory4> m_dxgiFactory;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
    UINT m_rtvDescriptorSize;
    UINT m_dsvDescriptorSize;
    UINT m_cbvSrvDescriptorSize;
    UINT64 m_srvHeapGpuPtr;
    SIZE_T m_srvHeapCpuPtr;
    UINT m_rtvHeapOffset;
    UINT m_dsvHeapOffset;
    UINT m_srvHeapOffset;

    UINT m_descriptorsCount;
    std::vector<UINT> m_freeDescriptors;    // for Srv heap

    // Synchronization objects.
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    HANDLE m_deviceRemovedEvent;
    ComPtr<ID3D12Fence> m_deviceFence;

    ComPtr<ID3D12Resource> m_blankTexture;
};

} //namespace render
