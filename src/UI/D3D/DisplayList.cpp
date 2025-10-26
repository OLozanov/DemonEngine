#include "DisplayList.h"

#include "Render/D3D/D3DInstance.h"

#include "Resources/Image.h"

namespace UI
{

DisplayList::DisplayList()
: m_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
, m_offset(0)
, m_imageHandle(0)
{
    Render::D3DInstance& d3dInstance = Render::D3DInstance::GetInstance();

    d3dInstance.device()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&m_commandAllocator));
    d3dInstance.device()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));

    m_commandList->Close();
}

void DisplayList::reset(ID3D12RootSignature * rootSignature)
{
    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), nullptr);
    m_commandList->SetGraphicsRootSignature(rootSignature);
    
    m_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; 
    m_offset = 0;
    m_imageHandle = 0;

    m_commandList->IASetPrimitiveTopology(m_topology);
    //m_commandList->SetGraphicsRootDescriptorTable(1, Win32App::GetD3DInstance().GpuDescriptor(m_imageHandle));
}

void DisplayList::finish()
{
    m_commandList->Close();
}

void DisplayList::setTopology(uint32_t topology)
{
    if (m_topology != topology)
    {
        m_topology = static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology);
        m_commandList->IASetPrimitiveTopology(m_topology);
    }
}

void DisplayList::draw(uint32_t vnum)
{
    m_commandList->DrawInstanced(vnum, 1, m_offset, 0);
    m_offset += vnum;
}

void DisplayList::bind(const Image* img)
{
    if (m_imageHandle != img->handle)
    {
        m_imageHandle = img->handle;
        m_commandList->SetGraphicsRootDescriptorTable(1, Render::D3DInstance::GetInstance().GpuDescriptor(m_imageHandle));
    }
}

} // namespace ui