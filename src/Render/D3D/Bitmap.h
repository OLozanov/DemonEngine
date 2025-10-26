#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include "Render\D3D\d3dx12.h"
#include <wrl.h>

namespace Render
{

using Microsoft::WRL::ComPtr;

class Bitmap
{
public:

    explicit Bitmap(DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN, bool read = true, bool write = false);

    ~Bitmap();

    void reset(ID3D12Resource* buffer);
    void reset(ID3D12Resource* buffer, DXGI_FORMAT format);
    void reset(UINT width, UINT height);

    operator ID3D12Resource* () const { return m_buffer.Get(); }

    UINT readHandle() const { return m_srvHandle; }
    UINT writeHandle() const { return m_uavHandle; }

    D3D12_RESOURCE_STATES transitState(D3D12_RESOURCE_STATES state);
    
    operator UINT() const { return m_srvHandle; }

    operator const D3D12_CPU_DESCRIPTOR_HANDLE& () const { return m_handle; }
    operator const D3D12_CPU_DESCRIPTOR_HANDLE* () const { return &m_handle; }

protected:
    void createReadHandle(DXGI_FORMAT format);
    void createWriteHandle(DXGI_FORMAT format);

    void createReadHandle(DXGI_FORMAT format, UINT count);
    void createWriteHandle(DXGI_FORMAT format, UINT count);

protected:
    DXGI_FORMAT m_format;
    ComPtr<ID3D12Resource> m_buffer;

    D3D12_CPU_DESCRIPTOR_HANDLE m_handle;

    UINT m_srvHandle;
    UINT m_uavHandle;

    D3D12_RESOURCE_STATES m_state;
};

} //namespace render
