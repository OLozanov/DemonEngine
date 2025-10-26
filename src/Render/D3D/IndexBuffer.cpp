#include "IndexBuffer.h"

#include "Render/D3D/D3DInstance.h"

#include "Render/DisplayData.h"

namespace Render
{

IndexBuffer::IndexBuffer()
: m_size(0)
{
}

void IndexBuffer::reset()
{
    m_buffer.Reset();
}

void IndexBuffer::setData(const uint16_t* data, UINT size)
{
    m_size = size;

    Render::D3DInstance& d3dInstance = D3DInstance::GetInstance();
    m_buffer = d3dInstance.createBuffer(data, size * sizeof(uint16_t));

    m_indexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
    m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
    m_indexBufferView.SizeInBytes = sizeof(uint16_t) * size;
}

} //namespace render