#include "D3DInstance.h"

#include "Resources/Image.h"
#include "System/ErrorMsg.h"

#include <set>

namespace Render
{

D3DInstance::D3DInstance()
: m_frameIndex(0)
, m_rtvDescriptorSize(0)
{
    init();
}

D3DInstance::~D3DInstance()
{
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForGpu();

    CloseHandle(m_fenceEvent);
}

D3DInstance& D3DInstance::GetInstance()
{
    static D3DInstance instance;

    return instance;
}

void D3DInstance::SelectAdapter(IDXGIFactory1* pFactory,
    IDXGIAdapter1** ppAdapter)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, //DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }
    else
    {
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}

void D3DInstance::getDisplayModes(IDXGIAdapter1* adapter)
{
    ComPtr<IDXGIOutput> output;

    adapter->EnumOutputs(0, &output);

    UINT modesNum = 0;
    output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &modesNum, nullptr);

    std::vector<DXGI_MODE_DESC> modeDesc(modesNum);

    output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &modesNum, modeDesc.data());

    std::set<DisplayMode> modes;

    for (const auto& desc : modeDesc) modes.insert({ desc.Width, desc.Height });

    for (const auto& mode : modes) m_displayModes.emplace_back(mode);
}

void D3DInstance::checkFeatureSupport()
{
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureData = {};

    m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureData, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS5));
    m_rtxSupport = featureData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}

void D3DInstance::init()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_dxgiFactory)));

    ComPtr<IDXGIAdapter1> hardwareAdapter;
    SelectAdapter(m_dxgiFactory.Get(), &hardwareAdapter);

    getDisplayModes(hardwareAdapter.Get());

    ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

    checkFeatureSupport();

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    // Create descriptor heaps.
    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount + RtvCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

    // Describe and create a depth stencil view (DSV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1 + DsvCount;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = DescriptorCount;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_cbvSrvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    m_srvHeapGpuPtr = m_srvHeap->GetGPUDescriptorHandleForHeapStart().ptr;
    m_srvHeapCpuPtr = m_srvHeap->GetCPUDescriptorHandleForHeapStart().ptr;
    m_rtvHeapOffset = FrameCount;
    m_dsvHeapOffset = 0;
    m_srvHeapOffset = 2;

    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

    createSyncObjects();
    createDefaultTextures();
}

IDXGISwapChain3* D3DInstance::createSwapChain(HWND hwnd, bool maximizable)
{
    RECT clientRect = {};
    GetClientRect(hwnd, &clientRect);
    UINT width = clientRect.right - clientRect.left;
    UINT height = clientRect.bottom - clientRect.top;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {};
    fullscreenDesc.RefreshRate = { 60000, 1000 };
    fullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    fullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    fullscreenDesc.Windowed = true;

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    IDXGISwapChain1* swapChain;
    ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
            hwnd,
            &swapChainDesc,
            &fullscreenDesc,
            nullptr,
            &swapChain
    ));

    if(!maximizable) m_dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

    IDXGISwapChain3* result = nullptr;
    swapChain->QueryInterface(&result);

    return result;
}

void D3DInstance::createSyncObjects()
{
    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_fenceValue = 1;

    // Create an event handle to use for frame synchronization.
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (m_fenceEvent == nullptr)
    {
        ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
    }

    // Wait for the command list to execute; we are reusing the same command 
    // list in our main loop but for now, we just want to wait for setup to 
    // complete before continuing.
    WaitForGpu();

    // Device removal
    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_deviceFence)));
    m_deviceRemovedEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    m_deviceFence->SetEventOnCompletion(UINT64_MAX, m_deviceRemovedEvent);

    HANDLE waitHandle;
    RegisterWaitForSingleObject(
        &waitHandle,
        m_deviceRemovedEvent,
        D3DInstance::OnDeviceRemoved,
        m_device.Get(),
        INFINITE,
        0
    );
}

void D3DInstance::OnDeviceRemoved(PVOID context, BOOLEAN)
{
    ID3D12Device* removedDevice = (ID3D12Device*)context;
    HRESULT reason = removedDevice->GetDeviceRemovedReason();

    std::stringstream sstream;
    sstream << "HRESULT: " << std::hex << reason;

    MessageBoxA(NULL, sstream.str().c_str(), "Device Removed.", 1);
}

void D3DInstance::createDefaultTextures()
{
    D3D12_CPU_DESCRIPTOR_HANDLE blackHandle = CpuDescriptor(0);

    D3D12_SHADER_RESOURCE_VIEW_DESC blackDesc = {};
    blackDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(4, 4, 4, 4);
    blackDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    blackDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    blackDesc.Texture2D.MipLevels = 1;
    m_device->CreateShaderResourceView(nullptr, &blackDesc, blackHandle);

    // Describe and create a Texture2D.
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.Width = 1;
    textureDesc.Height = 1;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(&m_blankTexture)));

    D3D12_CPU_DESCRIPTOR_HANDLE whiteHandle = CpuDescriptor(1);

    D3D12_SHADER_RESOURCE_VIEW_DESC whiteDesc = {};
    whiteDesc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(5, 5, 5, 5);
    whiteDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    whiteDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    whiteDesc.Texture2D.MipLevels = 1;
    m_device->CreateShaderResourceView(m_blankTexture.Get(), &whiteDesc, whiteHandle);
}

void D3DInstance::createTexture(Image* image)
{
    WaitForGpu();

    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), nullptr);

    ComPtr<ID3D12Resource> textureUploadHeap;
    ID3D12Resource* texture;

    // Describe and create a Texture2D.
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = image->mipmaps;
    textureDesc.Format = image->format;
    textureDesc.Width = image->width;
    textureDesc.Height = image->height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&texture)));

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture, 0, image->mipmaps);

    // Create the GPU upload buffer.
    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&textureUploadHeap)));

    std::vector<D3D12_SUBRESOURCE_DATA> textureData(image->mipmaps);

    int width = image->width;
    int height = image->height;

    if (image->format >= DXGI_FORMAT_BC1_TYPELESS &&
        image->format <= DXGI_FORMAT_BC5_SNORM)
    {
        width /= 4;
        height /= 4;
    }

    for (int i = 0; i < image->mipmaps; i++)
    {
        textureData[i].pData = image->data[i];
        textureData[i].RowPitch = width * image->elemsz;
        textureData[i].SlicePitch = textureData[i].RowPitch * height;

        width = width >> 1;
        height = height >> 1;

        if (width == 0) width = 1;
        if (height == 0) height = 1;
    }

    UpdateSubresources(m_commandList.Get(), texture, textureUploadHeap.Get(), 0, 0, image->mipmaps, textureData.data());
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    // Describe and create a SRV for the texture.
    image->buffer = texture;
    image->handle = AllocateDescriptor();

    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = CpuDescriptor(image->handle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = image->format == DXGI_FORMAT_A8_UNORM ? D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(5, 5, 5, 3) : D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = image->mipmaps;
    m_device->CreateShaderResourceView(texture, &srvDesc, srvHandle);

    // Close the command list and execute it to begin the initial GPU setup.
    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    WaitForGpu();
}

ID3D12Resource* D3DInstance::createBuffer(const void* data, UINT size)
{
    WaitForGpu();

    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), nullptr);

    ComPtr<ID3D12Resource> vertexUploadHeap;
    ID3D12Resource* vertexBuffer;

    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer)));

    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(size),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexUploadHeap)));

    D3D12_SUBRESOURCE_DATA vertexSubresourceData;
    vertexSubresourceData.pData = data;
    vertexSubresourceData.RowPitch = size;
    vertexSubresourceData.SlicePitch = size;

    UpdateSubresources(m_commandList.Get(), vertexBuffer, vertexUploadHeap.Get(), 0, 0, 1, &vertexSubresourceData);
    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(vertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    m_commandList->Close();
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    WaitForGpu();

    return vertexBuffer;
}

void D3DInstance::WaitForGpu()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.

    // Signal and increment the fence value.
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;

    // Wait until the previous frame is finished.
    if (m_fence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void D3DInstance::execute(ID3D12CommandList* commandList)
{
    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { commandList };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

UINT D3DInstance::AllocateDescriptor()
{
    if (!m_freeDescriptors.empty())
    {
        UINT descriptor = m_freeDescriptors.back();
        m_freeDescriptors.pop_back();

        return descriptor;
    }

    return m_srvHeapOffset++;
}

void D3DInstance::FreeDescriptor(UINT descriptor)
{
    m_freeDescriptors.push_back(descriptor);
}

} //namespace render