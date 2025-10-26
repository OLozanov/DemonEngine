#include "CubemapBuffer.h"
#include "Render/D3D/D3DInstance.h"

namespace Render
{

constexpr UINT CubeFaces = 6;

CubemapBuffer::CubemapBuffer(DXGI_FORMAT format)
: Bitmap(format)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();
    m_handle = (m_format == DXGI_FORMAT_D32_FLOAT) ? d3dInstance.AllocateDsvDescriptor() :
                                                     d3dInstance.AllocateRtvDescriptor();
}

void CubemapBuffer::createReadHandle(DXGI_FORMAT format)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.TextureCube.MipLevels = 1;
    srvDesc.TextureCube.ResourceMinLODClamp = 0;

    d3dInstance.device()->CreateShaderResourceView(m_buffer.Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandle));
}

void CubemapBuffer::createWriteHandle(DXGI_FORMAT format)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    uavDesc.Texture2DArray.ArraySize = CubeFaces;

    d3dInstance.device()->CreateUnorderedAccessView(m_buffer.Get(), nullptr, &uavDesc, d3dInstance.CpuDescriptor(m_uavHandle));
}

void CubemapBuffer::createDepthBuffer(UINT size)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
    depthStencilDesc.Texture2DArray.MipSlice = 0;
    depthStencilDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilDesc.Texture2DArray.ArraySize = CubeFaces;

    D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
    depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
    depthOptimizedClearValue.DepthStencil.Stencil = 0;

    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    if (m_uavHandle != 0) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    CD3DX12_RESOURCE_DESC texDesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        size,
        size,
        CubeFaces,
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

    if (m_srvHandle != 0) createReadHandle(static_cast<DXGI_FORMAT>(m_format + 1));
    if (m_uavHandle != 0) createWriteHandle(static_cast<DXGI_FORMAT>(m_format + 1));
}

void CubemapBuffer::createColorBuffer(UINT size)
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
    RTVDesc.Texture2DArray.MipSlice = 0;
    RTVDesc.Texture2DArray.FirstArraySlice = 0;
    RTVDesc.Texture2DArray.ArraySize = CubeFaces;

    D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    if (m_uavHandle != 0) flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    CD3DX12_RESOURCE_DESC texDesc(
        D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        0,
        size,
        size,
        CubeFaces,
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
        &clearValue,
        IID_PPV_ARGS(&m_buffer));

    d3dInstance.device()->CreateRenderTargetView(m_buffer.Get(), &RTVDesc, m_handle);

    if (m_srvHandle != 0) createReadHandle(m_format);
    if (m_uavHandle != 0) createWriteHandle(m_format);
}

void CubemapBuffer::reset(UINT size)
{
    if (m_format == DXGI_FORMAT_D32_FLOAT) createDepthBuffer(size);
    else createColorBuffer(size);

    m_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

} //namespace render