#pragma once

#include <d3d12.h>
#include <vector>

namespace Render
{

class Barrier : public D3D12_RESOURCE_BARRIER
{
public:
    Barrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
    {
        Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        Transition.pResource = resource;
        Transition.StateBefore = before;
        Transition.StateAfter = after;
        Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    }
};

class BarrierBlock
{
    std::vector<D3D12_RESOURCE_BARRIER> m_barriers;

public:
    BarrierBlock() = default;

    BarrierBlock(std::initializer_list<D3D12_RESOURCE_BARRIER> barriers)
    : m_barriers(barriers)
    {
    }

    void transit(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
    {
        m_barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(resource, before, after));
    }

    template<class T>
    void transit(T& resource, D3D12_RESOURCE_STATES state)
    {
        D3D12_RESOURCE_STATES oldState = resource.transitState(state);

        if (state != oldState)
            m_barriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(resource, oldState, state));
    }

    void clear() { m_barriers.clear(); }

    bool empty() const { return m_barriers.empty(); }
    size_t size() const { return m_barriers.size(); }
    operator const D3D12_RESOURCE_BARRIER* () const { return m_barriers.data(); }

};

} //namespace render

#define STATE_VERTEX_BUFFER D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
#define STATE_INDEX_BUFFER D3D12_RESOURCE_STATE_INDEX_BUFFER
#define STATE_COPY_SOURCE D3D12_RESOURCE_STATE_COPY_SOURCE
#define STATE_COPY_DEST D3D12_RESOURCE_STATE_COPY_DEST
#define STATE_RENDER D3D12_RESOURCE_STATE_RENDER_TARGET
#define STATE_DEPTH_READ D3D12_RESOURCE_STATE_DEPTH_READ
#define STATE_DEPTH_WRITE D3D12_RESOURCE_STATE_DEPTH_WRITE
#define STATE_PRESENT D3D12_RESOURCE_STATE_PRESENT
#define STATE_PIXEL_SHADER_READ D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
#define STATE_SHADER_READ D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
#define STATE_READ (D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
#define STATE_WRITE D3D12_RESOURCE_STATE_UNORDERED_ACCESS