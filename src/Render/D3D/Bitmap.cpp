#include "Bitmap.h"
#include "Render/D3D/D3DInstance.h"

namespace Render
{

Bitmap::Bitmap(DXGI_FORMAT format, bool read, bool write)
: m_format(format)
, m_srvHandle(0)
, m_uavHandle(0)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    if (read) m_srvHandle = d3dInstance.AllocateDescriptor();
    if (write) m_uavHandle = d3dInstance.AllocateDescriptor();
}

Bitmap::~Bitmap()
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    if (m_srvHandle != 0) d3dInstance.FreeDescriptor(m_srvHandle);
    if (m_uavHandle != 0) d3dInstance.FreeDescriptor(m_uavHandle);
}

void Bitmap::createReadHandle(DXGI_FORMAT format)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    d3dInstance.device()->CreateShaderResourceView(m_buffer.Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandle));
}

void Bitmap::createWriteHandle(DXGI_FORMAT format)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    d3dInstance.device()->CreateUnorderedAccessView(m_buffer.Get(), nullptr, &uavDesc, d3dInstance.CpuDescriptor(m_uavHandle));
}

void Bitmap::createReadHandle(DXGI_FORMAT format, UINT count)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = static_cast<DXGI_FORMAT>(m_format + 1);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MipLevels = 1;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = count;

    d3dInstance.device()->CreateShaderResourceView(m_buffer.Get(), &srvDesc, d3dInstance.CpuDescriptor(m_srvHandle));
}

void Bitmap::createWriteHandle(DXGI_FORMAT format, UINT count)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    uavDesc.Format = format;
    uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
    uavDesc.Texture2DArray.ArraySize = count;

    d3dInstance.device()->CreateUnorderedAccessView(m_buffer.Get(), nullptr, &uavDesc, d3dInstance.CpuDescriptor(m_uavHandle));
}

void Bitmap::reset(ID3D12Resource* buffer)
{
    m_buffer = buffer;

    D3D12_RESOURCE_DESC desc = m_buffer->GetDesc();
    m_format = desc.Format;

    if (m_srvHandle != 0)
    {
        createReadHandle(m_format);
    }

    if (m_uavHandle != 0)
    {
        createWriteHandle(m_format);
    }

    m_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

void Bitmap::reset(ID3D12Resource* buffer, DXGI_FORMAT format)
{
    m_format = format;
    m_buffer = buffer;

    if (m_srvHandle != 0)
    {
        createReadHandle(m_format);
    }

    if (m_uavHandle != 0)
    {
        createWriteHandle(m_format);
    }

    m_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

void Bitmap::reset(UINT width, UINT height)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    D3D12_RESOURCE_FLAGS flags = (m_uavHandle != 0) ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;

    d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(m_format, width, height, 1, 0, 1, 0, flags),
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(&m_buffer));

    if (m_srvHandle != 0)
    {
        createReadHandle(m_format);
    }

    if (m_uavHandle != 0)
    {
        createWriteHandle(m_format);
    }

    m_state = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
}

D3D12_RESOURCE_STATES Bitmap::transitState(D3D12_RESOURCE_STATES state)
{
    D3D12_RESOURCE_STATES oldState = m_state;
    m_state = state;

    return oldState;
}

} //namespace render