#include "CommandList.h"

#include "Render/D3D/D3DInstance.h"

#include "Render/D3D/RenderingPipeline.h"
#include "Render/Light.h"
#include "Render/DirectionalLight.h"

namespace Render
{

CommandList::CommandList()
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    d3dInstance.device()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
    d3dInstance.device()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));

    m_commandList->Close();
}

void CommandList::start()
{
    ID3D12DescriptorHeap* srvHeap = D3DInstance::GetInstance().descriptorHeap();

    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), nullptr);

    ID3D12DescriptorHeap* ppHeaps[] = { srvHeap };
    m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void CommandList::start(int mode)
{
    ID3D12DescriptorHeap* srvHeap = D3DInstance::GetInstance().descriptorHeap();

    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), RenderingPipeline::m_pipelineState[mode].Get());

    m_commandList->SetGraphicsRootSignature(RenderingPipeline::m_rootSignature[mode].Get());

    ID3D12DescriptorHeap* ppHeaps[] = { srvHeap };
    m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

void CommandList::setRenderMode(int mode)
{
    m_commandList->SetPipelineState(RenderingPipeline::m_pipelineState[mode].Get());
    m_commandList->SetGraphicsRootSignature(RenderingPipeline::m_rootSignature[mode].Get());
}

void CommandList::setDefaultViewport()
{
    m_commandList->RSSetViewports(1, &RenderingPipeline::m_viewport);
    m_commandList->RSSetScissorRects(1, &RenderingPipeline::m_scissorRect);
}

void CommandList::setViewport(int width, int height)
{
    CD3DX12_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
    CD3DX12_RECT scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height));

    viewport.Width = width;
    viewport.Height = height;

    scissorRect.right = width;
    scissorRect.bottom = height;

    m_commandList->RSSetViewports(1, &viewport);
    m_commandList->RSSetScissorRects(1, &scissorRect);
}

void CommandList::copy(ID3D12Resource* dst, ID3D12Resource* src)
{
    m_commandList->CopyResource(dst, src);
}

void CommandList::setConstant(UINT index, const Light& light)
{
    m_commandList->SetGraphicsRoot32BitConstants(index, 8, light.data(), 0);
}

void CommandList::setConstant(UINT index, const DirectionalLight& light)
{
    m_commandList->SetGraphicsRoot32BitConstants(index, 8, light.data(), 0);
}

void CommandList::setTopology(int topology)
{
    m_commandList->IASetPrimitiveTopology((D3D12_PRIMITIVE_TOPOLOGY)topology);
}

void CommandList::bindVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW* view, UINT slot)
{
    m_commandList->IASetVertexBuffers(slot, 1, view);
}

void CommandList::bindIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view)
{
    m_commandList->IASetIndexBuffer(view);
}

void CommandList::bind(UINT index, UINT handle)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();
    m_commandList->SetGraphicsRootDescriptorTable(index, d3dInstance.GpuDescriptor(handle));
}

void CommandList::bindConstantBuffer(UINT index, D3D12_GPU_VIRTUAL_ADDRESS buffer)
{
    m_commandList->SetGraphicsRootConstantBufferView(index, buffer);
}

void CommandList::bindFrameBuffer(const FrameBuffer& frameBuffer)
{
    m_commandList->OMSetRenderTargets(frameBuffer.size(), frameBuffer.renderTargets(), FALSE, frameBuffer.depthStencil());
}

void CommandList::bindFrameBuffer(const Bitmap& colorBuffer)
{
    m_commandList->OMSetRenderTargets(1, colorBuffer, FALSE, nullptr);
}

void CommandList::bindFrameBuffer(const Bitmap& colorBuffer, const Bitmap& depthBuffer)
{
    m_commandList->OMSetRenderTargets(1, colorBuffer, FALSE, depthBuffer);
}

void CommandList::bindDepthBuffer(const Bitmap& depthBuffer)
{
    m_commandList->OMSetRenderTargets(0, nullptr, FALSE, depthBuffer);
}

void CommandList::barrier(const BarrierList& barrierList)
{
    m_commandList->ResourceBarrier(barrierList.size(), barrierList.data());
}

void CommandList::barrier(const D3D12_RESOURCE_BARRIER& barrier)
{
    m_commandList->ResourceBarrier(1, &barrier);
}

/*void CommandList::barrier(Bitmap& bitmap, D3D12_RESOURCE_STATES state)
{
    D3D12_RESOURCE_STATES oldState = bitmap.transitState(state);
    
    if (state != oldState)
    {
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(bitmap, oldState, state);
        m_commandList->ResourceBarrier(1, &barrier);
    }
}*/

void CommandList::clearBuffer(const FrameBuffer& frameBuffer, const vec4& color)
{
    for (size_t i = 0; i < frameBuffer.size(); i++)
    {
        m_commandList->ClearRenderTargetView(frameBuffer.renderTargets()[i], &color[0], 0, nullptr);
    }
}
void CommandList::clearBuffer(const Bitmap& colorBuffer, const vec4& color)
{
    m_commandList->ClearRenderTargetView(colorBuffer, &color[0], 0, nullptr);
}

void CommandList::clearDepth(const FrameBuffer& frameBuffer, float depth)
{
    m_commandList->ClearDepthStencilView(*frameBuffer.depthStencil(), D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

void CommandList::clearDepth(const Bitmap& depthBuffer, float depth)
{
    m_commandList->ClearDepthStencilView(depthBuffer, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

void CommandList::clearDepthStencil(const FrameBuffer& frameBuffer, float depth, uint8_t stencil)
{
    m_commandList->ClearDepthStencilView(*frameBuffer.depthStencil(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
}

void CommandList::clearDepthStencil(const Bitmap& depthBuffer, float depth, uint8_t stencil)
{
    m_commandList->ClearDepthStencilView(depthBuffer, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil , 0, nullptr);
}

void CommandList::setStencilValue(unsigned int value)
{
    m_commandList->OMSetStencilRef(value);
}

void CommandList::draw(UINT num, UINT offset)
{
    m_commandList->DrawInstanced(num, 1, offset, 0);
}

void CommandList::drawInstanced(UINT instances, UINT num, UINT offset)
{
    m_commandList->DrawInstanced(num, instances, offset, 0);
}

void CommandList::drawIndexed(UINT num, UINT offset)
{
    m_commandList->DrawIndexedInstanced(num, 1, offset, 0, 0);
}

void CommandList::drawQuad()
{
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_commandList->DrawInstanced(4, 1, 0, 0);
}

void CommandList::drawMask(const DisplayBlock& block)
{
    m_commandList->SetGraphicsRoot32BitConstants(1, 16, block.mat, 0);
    m_commandList->IASetVertexBuffers(0, 1, block.vertexData);

    for (const DisplayData* data : block.displayData)
    {
        m_commandList->DrawInstanced(data->vertexnum, 1, data->offset, 0);
    }
}

void CommandList::draw(const DisplayBlock& block, const mat4& mat)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    const Material* material = 0;

    m_commandList->SetGraphicsRoot32BitConstants(1, 16, mat, 0);
    m_commandList->IASetVertexBuffers(0, 1, block.vertexData);
    if (block.indexData) m_commandList->IASetIndexBuffer(block.indexData);

    for (const DisplayData* data : block.displayData)
    {
        if (data->material != material)
        {
            material = data->material;

            m_commandList->SetGraphicsRoot32BitConstants(2, 8, &material->color, 0);

            UINT diffuse = material->maps[Material::map_diffuse];
            UINT normal = material->maps[Material::map_normal];
            UINT roughness = material->maps[Material::map_roughness];
            UINT metalness = material->maps[Material::map_metalness];
            UINT luminosity = material->maps[Material::map_luminosity];
            UINT height = material->maps[Material::map_height];

            m_commandList->SetGraphicsRootDescriptorTable(3, d3dInstance.GpuDescriptor(diffuse));
            m_commandList->SetGraphicsRootDescriptorTable(4, d3dInstance.GpuDescriptor(normal));
            m_commandList->SetGraphicsRootDescriptorTable(5, d3dInstance.GpuDescriptor(roughness));
            m_commandList->SetGraphicsRootDescriptorTable(6, d3dInstance.GpuDescriptor(metalness));
            m_commandList->SetGraphicsRootDescriptorTable(7, d3dInstance.GpuDescriptor(luminosity));
            m_commandList->SetGraphicsRootDescriptorTable(8, d3dInstance.GpuDescriptor(height));
        }

        if (block.indexData)
            m_commandList->DrawIndexedInstanced(data->vertexnum, 1, data->offset, 0, 0);
        else
            m_commandList->DrawInstanced(data->vertexnum, 1, data->offset, 0);
    }
}

void CommandList::drawSimple(const DisplayBlock& block, const mat4& mat)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    const Material* material = 0;

    m_commandList->SetGraphicsRoot32BitConstants(1, 16, mat, 0);
    m_commandList->IASetVertexBuffers(0, 1, block.vertexData);
    if (block.indexData) m_commandList->IASetIndexBuffer(block.indexData);

    for (const DisplayData* data : block.displayData)
    {
        if (data->material != material)
        {
            material = data->material;

            m_commandList->SetGraphicsRoot32BitConstants(2, 4, &material->color, 0);

            UINT diffuse = material->maps[Material::map_diffuse];
            m_commandList->SetGraphicsRootDescriptorTable(3, d3dInstance.GpuDescriptor(diffuse));
        }

        if (block.indexData)
            m_commandList->DrawIndexedInstanced(data->vertexnum, 1, data->offset, 0, 0);
        else
            m_commandList->DrawInstanced(data->vertexnum, 1, data->offset, 0);
    }
}

void CommandList::draw(const DisplayList& displayList)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    const Material* material = 0;

    for (const DisplayBlock* block : displayList)
    {
        m_commandList->SetGraphicsRoot32BitConstants(1, 16, block->mat, 0);
        m_commandList->IASetVertexBuffers(0, 1, block->vertexData);
        if (block->indexData) m_commandList->IASetIndexBuffer(block->indexData);

        for (const DisplayData* data : block->displayData)
        {
            if (data->material != material)
            {
                material = data->material;

                m_commandList->SetGraphicsRoot32BitConstants(2, 8, &material->color, 0);

                UINT diffuse = material->maps[Material::map_diffuse];
                UINT normal = material->maps[Material::map_normal];
                UINT roughness = material->maps[Material::map_roughness];
                UINT metalness = material->maps[Material::map_metalness];
                UINT luminosity = material->maps[Material::map_luminosity];
                UINT height = material->maps[Material::map_height];

                m_commandList->SetGraphicsRootDescriptorTable(3, d3dInstance.GpuDescriptor(diffuse));
                m_commandList->SetGraphicsRootDescriptorTable(4, d3dInstance.GpuDescriptor(normal));
                m_commandList->SetGraphicsRootDescriptorTable(5, d3dInstance.GpuDescriptor(roughness));
                m_commandList->SetGraphicsRootDescriptorTable(6, d3dInstance.GpuDescriptor(metalness));
                m_commandList->SetGraphicsRootDescriptorTable(7, d3dInstance.GpuDescriptor(luminosity));
                m_commandList->SetGraphicsRootDescriptorTable(8, d3dInstance.GpuDescriptor(height));
            }

            if (block->indexData)
                m_commandList->DrawIndexedInstanced(data->vertexnum, 1, data->offset, 0, 0);
            else
                m_commandList->DrawInstanced(data->vertexnum, 1, data->offset, 0);
        }
    }
}

void CommandList::draw(const InstancedList& instnacedList)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    const Material* material = 0;

    for (const InstanceData* data : instnacedList)
    {
        m_commandList->SetGraphicsRoot32BitConstants(1, 16, data->mat, 0);

        if (data->vertexData) m_commandList->IASetVertexBuffers(0, 1, data->vertexData);
        if (data->indexData) m_commandList->IASetIndexBuffer(data->indexData);
        
        m_commandList->IASetVertexBuffers(1, 1, data->instanceData);

        if (data->material != material)
        {
            material = data->material;

            m_commandList->SetGraphicsRoot32BitConstants(2, 8, &material->color, 0);

            UINT diffuse = material->maps[Material::map_diffuse];
            UINT normal = material->maps[Material::map_normal];
            UINT roughness = material->maps[Material::map_roughness];
            UINT metalness = material->maps[Material::map_metalness];
            UINT luminosity = material->maps[Material::map_luminosity];
            UINT height = material->maps[Material::map_height];

            m_commandList->SetGraphicsRootDescriptorTable(3, d3dInstance.GpuDescriptor(diffuse));
            m_commandList->SetGraphicsRootDescriptorTable(4, d3dInstance.GpuDescriptor(normal));
            m_commandList->SetGraphicsRootDescriptorTable(5, d3dInstance.GpuDescriptor(roughness));
            m_commandList->SetGraphicsRootDescriptorTable(6, d3dInstance.GpuDescriptor(metalness));
            m_commandList->SetGraphicsRootDescriptorTable(7, d3dInstance.GpuDescriptor(luminosity));
            m_commandList->SetGraphicsRootDescriptorTable(8, d3dInstance.GpuDescriptor(height));
        }

        if (data->indexData)
            m_commandList->DrawIndexedInstanced(data->vertexnum, data->instancenum, 0, 0, 0);
        else
            m_commandList->DrawInstanced(data->vertexnum, data->instancenum, 0, 0);
    }
}

void CommandList::drawDepth(const DisplayList& displayList)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    UINT diffuse = 0;

    for (const DisplayBlock* block : displayList)
    {
        m_commandList->SetGraphicsRoot32BitConstants(1, 16, block->mat, 0);
        m_commandList->IASetVertexBuffers(0, 1, block->vertexData);
        if (block->indexData) m_commandList->IASetIndexBuffer(block->indexData);

        for (const DisplayData* data : block->displayData)
        {
            /*UINT matDiffuse = data->material->maps[Material::map_diffuse];

            if (matDiffuse != diffuse)
            {
                diffuse = matDiffuse;
                m_commandList->SetGraphicsRootDescriptorTable(3, d3dInstance.GpuDescriptor(diffuse));
            }*/

            if (block->indexData)
                m_commandList->DrawIndexedInstanced(data->vertexnum, 1, data->offset, 0, 0);
            else
                m_commandList->DrawInstanced(data->vertexnum, 1, data->offset, 0);
        }
    }
}

void CommandList::drawColor(const DisplayList& displayList)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    const vec4 color = vec4(1.0, 1.0, 1.0, 1.0);

    m_commandList->SetGraphicsRoot32BitConstants(2, 4, &color, 0);

    for (const DisplayBlock* block : displayList)
    {
        m_commandList->SetGraphicsRoot32BitConstants(1, 16, block->mat, 0);
        m_commandList->IASetVertexBuffers(0, 1, block->vertexData);
        if (block->indexData) m_commandList->IASetIndexBuffer(block->indexData);

        for (const DisplayData* data : block->displayData)
        {
            if (block->indexData)
                m_commandList->DrawIndexedInstanced(data->vertexnum, 1, data->offset, 0, 0);
            else
                m_commandList->DrawInstanced(data->vertexnum, 1, data->offset, 0);
        }
    }
}

void CommandList::drawSimple(const DisplayList& displayList)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    const Material* material = 0;

    for (const DisplayBlock* block : displayList)
    {
        m_commandList->SetGraphicsRoot32BitConstants(1, 16, block->mat, 0);
        m_commandList->IASetVertexBuffers(0, 1, block->vertexData);
        if (block->indexData) m_commandList->IASetIndexBuffer(block->indexData);

        for (const DisplayData* data : block->displayData)
        {
            if (data->material != material)
            {
                material = data->material;

                m_commandList->SetGraphicsRoot32BitConstants(2, 4, &material->color, 0);

                UINT diffuse = material->maps[Material::map_diffuse];
                m_commandList->SetGraphicsRootDescriptorTable(3, d3dInstance.GpuDescriptor(diffuse));
            }

            if (block->indexData)
                m_commandList->DrawIndexedInstanced(data->vertexnum, 1, data->offset, 0, 0);
            else
                m_commandList->DrawInstanced(data->vertexnum, 1, data->offset, 0);
        }
    }
}

void CommandList::drawLayered(const DisplayList& displayList)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    const Material* material = 0;

    for (const DisplayBlock* block : displayList)
    {
        m_commandList->SetGraphicsRoot32BitConstants(1, 16, block->mat, 0);
        m_commandList->IASetVertexBuffers(0, 1, block->vertexData);
        if (block->indexData) m_commandList->IASetIndexBuffer(block->indexData);

        for (size_t i = 0; i < block->displayData.size(); i++)
        {
            const DisplayData* data = block->displayData[i];

            if (data->material != material)
            {
                material = data->material;

                m_commandList->SetGraphicsRoot32BitConstants(2, 8, &material->color, 0);

                UINT diffuse = material->maps[Material::map_diffuse];
                UINT normal = material->maps[Material::map_normal];
                UINT roughness = material->maps[Material::map_roughness];
                UINT metalness = material->maps[Material::map_metalness];
                UINT luminosity = material->maps[Material::map_luminosity];
                UINT height = material->maps[Material::map_height];

                m_commandList->SetGraphicsRootDescriptorTable(3, d3dInstance.GpuDescriptor(diffuse));
                m_commandList->SetGraphicsRootDescriptorTable(4, d3dInstance.GpuDescriptor(normal));
                m_commandList->SetGraphicsRootDescriptorTable(5, d3dInstance.GpuDescriptor(roughness));
                m_commandList->SetGraphicsRootDescriptorTable(6, d3dInstance.GpuDescriptor(metalness));
                m_commandList->SetGraphicsRootDescriptorTable(7, d3dInstance.GpuDescriptor(luminosity));
                m_commandList->SetGraphicsRootDescriptorTable(8, d3dInstance.GpuDescriptor(height));
            }

            // mask buffer
            m_commandList->IASetVertexBuffers(1, 1, block->layersData + i);

            if (block->indexData)
                m_commandList->DrawIndexedInstanced(data->vertexnum, 1, data->offset, 0, 0);
            else
                m_commandList->DrawInstanced(data->vertexnum, 1, data->offset, 0);
        }
    }
}

void CommandList::drawRefract(const DisplayList& displayList)
{
    D3DInstance& d3dInstance = D3DInstance::GetInstance();

    const Material* material = 0;

    for (const DisplayBlock* block : displayList)
    {
        m_commandList->SetGraphicsRoot32BitConstants(1, 16, block->mat, 0);
        m_commandList->IASetVertexBuffers(0, 1, block->vertexData);
        if (block->indexData) m_commandList->IASetIndexBuffer(block->indexData);

        for (const DisplayData* data : block->displayData)
        {
            if (data->material != material)
            {
                material = data->material;

                UINT diffuse = material->maps[Material::map_diffuse];
                UINT normal = material->maps[Material::map_normal];
                m_commandList->SetGraphicsRootDescriptorTable(3, d3dInstance.GpuDescriptor(diffuse));
                m_commandList->SetGraphicsRootDescriptorTable(4, d3dInstance.GpuDescriptor(normal));
            }

            if (block->indexData)
                m_commandList->DrawIndexedInstanced(data->vertexnum, 1, data->offset, 0, 0);
            else
                m_commandList->DrawInstanced(data->vertexnum, 1, data->offset, 0);
        }
    }
}

void CommandList::submit(ID3D12GraphicsCommandList* bundle)
{
    m_commandList->ExecuteBundle(bundle);
}

void CommandList::finish()
{
    m_commandList->Close();
}

} //namespace render