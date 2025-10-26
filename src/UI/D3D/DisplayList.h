#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <stdint.h>

using Microsoft::WRL::ComPtr;

class Image;

namespace UI
{

class DisplayList
{
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    D3D_PRIMITIVE_TOPOLOGY m_topology;
    UINT m_offset;
    UINT m_imageHandle;

public:

    DisplayList();

    void reset(ID3D12RootSignature* rootSignature);
    void finish();

    void setTopology(uint32_t topology);
    void draw(uint32_t vnum);
    void bind(const Image* img);

    operator ID3D12GraphicsCommandList* () { return m_commandList.Get(); }
};

} // namespace ui