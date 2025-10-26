#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "Render/D3D/D3DInstance.h"

#include "Render/D3D/MemoryBarrier.h"
#include <initializer_list>

namespace Render
{

using Microsoft::WRL::ComPtr;

class ComputeContext
{
    ID3D12GraphicsCommandList* m_commandList;

public:

    ComputeContext(ID3D12GraphicsCommandList* commandList) : m_commandList(commandList) {}

    void setComputeMode(RenderingPipeline::ComputeMode mode)
    {
        m_commandList->SetPipelineState(RenderingPipeline::m_computePipelineState[mode].Get());
        m_commandList->SetComputeRootSignature(RenderingPipeline::m_computeRootSignature[mode].Get());
    }

    template<class T>
    void setConstant(UINT index, const T& constData)
    {
        m_commandList->SetComputeRoot32BitConstants(index, sizeof(T) / 4, &constData, 0);
    }

    void bindConstantBuffer(UINT index, D3D12_GPU_VIRTUAL_ADDRESS buffer)
    {
        m_commandList->SetComputeRootConstantBufferView(index, buffer);
    }

    void bind(UINT index, UINT handle)
    {
        D3DInstance& d3dInstance = D3DInstance::GetInstance();
        m_commandList->SetComputeRootDescriptorTable(index, d3dInstance.GpuDescriptor(handle));
    }

    void barrier(const std::vector<D3D12_RESOURCE_BARRIER>& barrierList)
    {
        m_commandList->ResourceBarrier(barrierList.size(), barrierList.data());
    }

    void barrier(std::initializer_list<Barrier> barriers)
    {
        m_commandList->ResourceBarrier(barriers.size(), barriers.begin());
    }

    /*template<size_t N>
    void barriers(std::array<Barrier, N> barriers)
    {
        m_commandList->ResourceBarrier(barriers, barriers.begin());
    }*/

    void barrier(const D3D12_RESOURCE_BARRIER& barrier)
    {
        m_commandList->ResourceBarrier(1, &barrier);
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

    void barrier(BarrierBlock& barrier)
    {
        if (barrier.empty()) return;

        m_commandList->ResourceBarrier(barrier.size(), barrier);

        barrier.clear();
    }

    void dispatch(UINT x, UINT y = 1, UINT z = 1)
    {
        m_commandList->Dispatch(x, y, z);
    }

    void finish()
    {
        m_commandList->Close();

        D3DInstance& d3dInstance = D3DInstance::GetInstance();
        d3dInstance.execute(m_commandList);
    }
};

} //namespace render