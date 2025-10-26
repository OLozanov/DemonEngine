#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <stdint.h>

namespace Render
{

using Microsoft::WRL::ComPtr;

class IndexBuffer
{
    ComPtr<ID3D12Resource> m_buffer;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

    UINT m_size;

public:

    IndexBuffer();

    void reset();

    UINT size() const { return m_size; }
    bool empty() const { return m_size == 0; }

    void setData(const uint16_t* data, UINT size);

    operator ID3D12Resource* () { return m_buffer.Get(); }
    operator const D3D12_INDEX_BUFFER_VIEW* () const { return &m_indexBufferView; }
};

} //namespace render