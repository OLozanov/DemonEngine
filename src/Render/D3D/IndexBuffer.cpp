#include "IndexBuffer.h"

#include "Render/D3D/D3DInstance.h"

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

void IndexBuffer::setData(const IndexType* data, UINT size)
{
    m_size = size;

    Render::D3DInstance& d3dInstance = D3DInstance::GetInstance();
    m_buffer = d3dInstance.createBuffer(data, size * sizeof(IndexType));

    m_indexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
    m_indexBufferView.Format = IndexFormat;
    m_indexBufferView.SizeInBytes = sizeof(IndexType) * size;
}

} //namespace render