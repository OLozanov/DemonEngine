#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "math/math3d.h"
#include "Render/DisplayObject.h"
#include "Render/D3D/RenderBuffer.h"
#include "Render/D3D/MemoryBarrier.h"

#include <initializer_list>

namespace Render
{

using Microsoft::WRL::ComPtr;

class Light;
class DirectionalLight;
class CubemapBuffer;

using BarrierList = std::vector<D3D12_RESOURCE_BARRIER>;

class CommandList
{
    static constexpr size_t BarrierCount = 8;

    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    D3D12_RESOURCE_BARRIER m_barriers[BarrierCount];

public:

    CommandList();

    ID3D12CommandAllocator* allocator() { return m_commandAllocator.Get(); }
    operator ID3D12GraphicsCommandList* () { return m_commandList.Get(); }
  
    void start();
    void start(int mode);

    void setRenderMode(int mode);

    void setDefaultViewport();
    void setViewport(int width, int height);

    void setTopology(int topology);
    
    template<class T>
    void setConstant(UINT index, const T& constData)
    {
        m_commandList->SetGraphicsRoot32BitConstants(index, sizeof(T) / 4, &constData, 0);
    }

    template<class T>
    void setConstant(UINT index, const T* constData, UINT n)
    {
        m_commandList->SetGraphicsRoot32BitConstants(index, sizeof(T) / 4 * n, constData, 0);
    }

    template<>
    void setConstant<float>(UINT index, const float& value)
    {
        float val = value;
        m_commandList->SetGraphicsRoot32BitConstants(index, sizeof(float) /  4, &val, 0);
    }

    void copy(ID3D12Resource* dst, ID3D12Resource* src);

    void setConstant(UINT index, const Light& light);
    void setConstant(UINT index, const DirectionalLight& light);

    void bindVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW* view, UINT slot = 0);
    void bindIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* view);

    void bind(UINT index, UINT handle);
    void bindConstantBuffer(UINT index, D3D12_GPU_VIRTUAL_ADDRESS buffer);
    void bindFrameBuffer(const FrameBuffer& frameBuffer);
    void bindFrameBuffer(const Bitmap& colorBuffer);
    void bindFrameBuffer(const Bitmap& colorBuffer, const Bitmap& depthBuffer);
    void bindDepthBuffer(const Bitmap& depthBuffer);

    void barrier(const BarrierList& barrierList);
    void barrier(const D3D12_RESOURCE_BARRIER& barrier);

    void barrier(BarrierBlock& barrier)
    {
        if (barrier.empty()) return;

        m_commandList->ResourceBarrier(barrier.size(), barrier);

        barrier.clear();
    }

    void barrier(std::initializer_list<Barrier> barriers)
    {
        m_commandList->ResourceBarrier(barriers.size(), barriers.begin());
    }

    template<class T>
    void barrier(T& resource, D3D12_RESOURCE_STATES state)
    {
        D3D12_RESOURCE_STATES oldState = resource.transitState(state);

        if (state != oldState)
        {
            CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, oldState, state);
            m_commandList->ResourceBarrier(1, &barrier);
        }
    }

    void clearBuffer(const FrameBuffer& frameBuffer, const vec4& color);
    void clearBuffer(const Bitmap& colorBuffer, const vec4& color);
    void clearDepth(const FrameBuffer& frameBuffer, float depth = 1.0f);
    void clearDepth(const Bitmap& depthBuffer, float depth = 1.0f);
    void clearDepthStencil(const FrameBuffer& frameBuffer, float depth = 1.0f, uint8_t stencil = 0);
    void clearDepthStencil(const Bitmap& depthBuffer, float depth = 1.0f, uint8_t stencil = 0);

    void setStencilValue(unsigned int value);

    void draw(UINT num, UINT offset = 0);
    void drawInstanced(UINT instances, UINT num, UINT offset = 0);
    void drawIndexed(UINT num, UINT offset = 0);
    void drawQuad();
    void drawMask(const DisplayBlock& displayBlock);
    void draw(const DisplayBlock& displayBlock, const mat4& mat);
    void drawSimple(const DisplayBlock& displayBlock, const mat4& mat);
    void draw(const DisplayList& displayList);
    void draw(const InstancedList& instnacedList);
    void drawDepth(const DisplayList& displayList);
    void drawColor(const DisplayList& displayList);
    void drawSimple(const DisplayList& displayList);
    void drawLayered(const DisplayList& displayList);
    void drawRefract(const DisplayList& displayList);

    void submit(ID3D12GraphicsCommandList* bundle);

    void finish();
};

} //namespace render