#include "RenderBuffer.h"
#include "Render/D3D/D3DInstance.h"

namespace Render
{

ColorBuffer::ColorBuffer(DXGI_FORMAT format, bool read, bool write)
: Bitmap(format, read, write)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();
    m_handle = d3dInstance.AllocateRtvDescriptor();
}

void ColorBuffer::reset(UINT width, UINT height)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = m_format;
    clearValue.Color[0] = 0.0;
    clearValue.Color[1] = 0.0;
    clearValue.Color[2] = 0.0;
    clearValue.Color[3] = 1.0;

    D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
    RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    RTVDesc.Texture2D.MipSlice = 0;

    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    if (m_uavHandle != 0) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(m_format, width, height, 1, 0, 1, 0, flags),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue,
        IID_PPV_ARGS(&m_buffer));

    d3dInstance.device()->CreateRenderTargetView(m_buffer.Get(), &RTVDesc, m_handle);

    if (m_srvHandle != 0) createReadHandle(m_format);
    if (m_uavHandle != 0) createWriteHandle(m_format);

    m_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

DepthBuffer::DepthBuffer(DXGI_FORMAT format, bool read, bool write)
: Bitmap(format, read, write)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();
    m_handle = d3dInstance.AllocateDsvDescriptor();
}

void DepthBuffer::reset(UINT width, UINT height)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = m_format;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
    depthStencilDesc.Texture2D.MipSlice = 0;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = m_format;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    if (m_uavHandle != 0) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    CD3DX12_RESOURCE_DESC texDesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        width,
        height,
        1,
        1,
        m_format,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(&m_buffer));

    d3dInstance.device()->CreateDepthStencilView(m_buffer.Get(), &depthStencilDesc, m_handle);

    if (m_srvHandle != 0) createReadHandle(static_cast<DXGI_FORMAT>(m_format + 1));
    if (m_uavHandle != 0) createWriteHandle(static_cast<DXGI_FORMAT>(m_format + 1));

    m_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

void DepthBuffer::reset(UINT width, UINT height, UINT count)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = m_format;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
    depthStencilDesc.Texture2DArray.MipSlice = 0;
    depthStencilDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilDesc.Texture2DArray.ArraySize = count;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = m_format;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    if (m_uavHandle != 0) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    CD3DX12_RESOURCE_DESC texDesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        width,
        height,
        count,
        1,
        m_format,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        flags);

    d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(&m_buffer));

    d3dInstance.device()->CreateDepthStencilView(m_buffer.Get(), &depthStencilDesc, m_handle);

    if (m_srvHandle != 0) createReadHandle(static_cast<DXGI_FORMAT>(m_format + 1), count);
    if (m_uavHandle != 0) createWriteHandle(static_cast<DXGI_FORMAT>(m_format + 1), count);

    m_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

RenderBuffer::RenderBuffer(DXGI_FORMAT format, DXGI_FORMAT depthFormat, bool unorderdAccess)
: FrameBuffer(1)
, m_depthFormat(depthFormat)
, m_unorderedAccess(unorderdAccess)
{
    m_format.resize(1);
    m_format[0] = format;
}

RenderBuffer::RenderBuffer(std::initializer_list<DXGI_FORMAT> format, DXGI_FORMAT depthFormat, bool unorderdAccess)
: FrameBuffer(format.size())
, m_depthFormat(depthFormat)
, m_format(format)
, m_unorderedAccess(unorderdAccess)
{
}

void RenderBuffer::allocateDescriptors()
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    if (m_depthFormat != DXGI_FORMAT_UNKNOWN)
    {
        m_handles[0] = d3dInstance.AllocateDsvDescriptor();
        m_srvHandles[0] = d3dInstance.AllocateDescriptor();
    }

    for (int i = 0; i < m_format.size(); i++)
    {
        m_handles[i+1] = d3dInstance.AllocateRtvDescriptor();
        m_srvHandles[i+1] = d3dInstance.AllocateDescriptor();
    }
}

void RenderBuffer::createBuffers(int width, int height)
{
    if (m_depthFormat != DXGI_FORMAT_UNKNOWN) createDepthBuffer(width, height);
    for (int i = 0; i < m_format.size(); i++) createColorBuffer(i + 1, width, height, m_format[i]);
}

void RenderBuffer::createArrays(int width, int height, int count)
{
    if (m_depthFormat != DXGI_FORMAT_UNKNOWN) createDepthArray(width, height, count);
    for (int i = 0; i < m_format.size(); i++) createColorArray(i + 1, width, height, count, m_format[i]);
}

void RenderBuffer::createDepthBuffer(int width, int height)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = m_depthFormat;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
    depthStencilDesc.Texture2D.MipSlice = 0;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = m_depthFormat;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    CD3DX12_RESOURCE_DESC texDesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        width,
        height,
        1,
        1,
        m_depthFormat,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    d3dInstance.device()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &texDesc,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&m_buffers[0]));

    d3dInstance.device()->CreateDepthStencilView(m_buffers[0].Get(), &depthStencilDesc, m_handles[0]);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = static_cast<DXGI_FORMAT>(m_depthFormat + 1);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    d3dInstance.device()->CreateShaderResourceView(m_buffers[0].Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandles[0]));
}

void RenderBuffer::createDepthArray(int width, int height, int count)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = m_depthFormat;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
    depthStencilDesc.Texture2DArray.MipSlice = 0;
    depthStencilDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilDesc.Texture2DArray.ArraySize = count;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = m_depthFormat;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    CD3DX12_RESOURCE_DESC texDesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        width,
        height,
        count,
        1,
        m_depthFormat,
        1,
        0,
        D3D12_TEXTURE_LAYOUT_UNKNOWN,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &depthOptimizedClearValue,
        IID_PPV_ARGS(&m_buffers[0]));

    d3dInstance.device()->CreateDepthStencilView(m_buffers[0].Get(), &depthStencilDesc, m_handles[0]);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = static_cast<DXGI_FORMAT>(m_depthFormat + 1);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = count;
    d3dInstance.device()->CreateShaderResourceView(m_buffers[0].Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandles[0]));
}

void RenderBuffer::createColorBuffer(int n, int width, int height, DXGI_FORMAT format)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = format;
    clearValue.Color[0] = 0.0;
    clearValue.Color[1] = 0.0;
    clearValue.Color[2] = 0.0;
    clearValue.Color[3] = 1.0;

    D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
    RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    RTVDesc.Texture2D.MipSlice = 0;

    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    if (m_unorderedAccess) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 0, 1, 0, flags),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue,
        IID_PPV_ARGS(&m_buffers[n]));

    d3dInstance.device()->CreateRenderTargetView(m_buffers[n].Get(), &RTVDesc, m_handles[n]);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    d3dInstance.device()->CreateShaderResourceView(m_buffers[n].Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandles[n]));
}

void RenderBuffer::createColorArray(int n, int width, int height, int count, DXGI_FORMAT format)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = format;
    clearValue.Color[0] = 0.0;
    clearValue.Color[1] = 0.0;
    clearValue.Color[2] = 0.0;
    clearValue.Color[3] = 1.0;

    D3D12_RENDER_TARGET_VIEW_DESC RTVDesc = {};
    RTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
    RTVDesc.Texture2DArray.MipSlice = 0;
    RTVDesc.Texture2DArray.FirstArraySlice = 0;
    RTVDesc.Texture2DArray.ArraySize = count;

    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    if (m_unorderedAccess) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, count, 0, 1, 0, flags),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        &clearValue,
        IID_PPV_ARGS(&m_buffers[n]));

    d3dInstance.device()->CreateRenderTargetView(m_buffers[n].Get(), &RTVDesc, m_handles[n]);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = count;
    d3dInstance.device()->CreateShaderResourceView(m_buffers[n].Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandles[n]));
}

void RenderBuffer::createBarrierLists()
{
    for (size_t i = 0; i < m_buffers.size() - 1; i++)
    {
        m_writeBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(m_buffers[i + 1].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
        m_readBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(m_buffers[i + 1].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
    }

    if (m_depthFormat != DXGI_FORMAT_UNKNOWN)
    {
        m_writeBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(m_buffers[0].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
        m_readBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(m_buffers[0].Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
    }
}

void RenderBuffer::reset()
{
    m_handles.clear();
    m_rtCount = 0;

    m_writeBarriers.clear();
    m_readBarriers.clear();

    m_depthHandle = nullptr;
    m_rtHandles = nullptr;
}

void RenderBuffer::reset(int width, int height, int count)
{
    if (m_handles.empty())
    {
        m_buffers.resize(m_format.size() + 1);
        m_handles.resize(m_format.size() + 1);
        m_srvHandles.resize(m_format.size() + 1);

        size_t barrierSize = (m_depthFormat == DXGI_FORMAT_UNKNOWN) ? m_buffers.size() - 1 : m_handles.size();

        m_writeBarriers.reserve(barrierSize);
        m_readBarriers.reserve(barrierSize);

        allocateDescriptors();
    }
    else
    {
        int i = (m_depthFormat == DXGI_FORMAT_UNKNOWN) ? 1 : 0;
        for (; i < m_buffers.size(); i++) m_buffers[i].Reset();

        m_writeBarriers.clear();
        m_readBarriers.clear();
    }

    if (count > 1)
        createArrays(width, height, count);
    else
        createBuffers(width, height);
    
    createBarrierLists();

    m_depthHandle = m_buffers[0] ? &m_handles[0] : nullptr;
    m_rtHandles = m_handles.size() > 1 ? &m_handles[1] : nullptr;
}

void RenderBuffer::attachDepthBuffer(const Bitmap& buffer)
{
    m_depthHandle = buffer;

    m_writeBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(buffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
    m_readBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(buffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
}

void RenderBuffer::attachColorBuffer(const Bitmap& buffer)
{
    m_handles.push_back(buffer);

    m_writeBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(buffer, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
    m_readBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(buffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

    m_rtHandles = &m_handles[0];
    m_rtCount++;
}

} //namespace render