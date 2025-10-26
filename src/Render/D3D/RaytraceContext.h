#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "Render/D3D/D3DInstance.h"
#include "Render/D3D/RenderingPipeline.h"
#include "Render/D3D/RaytraceScene.h"
#include "Render/D3D/MemoryBarrier.h"

#include <initializer_list>

namespace Render
{

class RaytraceContext
{
    ID3D12GraphicsCommandList5* m_commandList;

public:

    RaytraceContext(ID3D12GraphicsCommandList5* commandList) : m_commandList(commandList) {}

    operator ID3D12GraphicsCommandList* () { return m_commandList; }

    void finish()
    {
        m_commandList->Close();

        D3DInstance& d3dInstance = D3DInstance::GetInstance();
        d3dInstance.execute(m_commandList);
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

    void bind(UINT index, D3D12_GPU_VIRTUAL_ADDRESS address)
    {
        m_commandList->SetComputeRootShaderResourceView(index, address);
    }

    void barrier(const std::vector<D3D12_RESOURCE_BARRIER>& barrierList)
    {
        m_commandList->ResourceBarrier(barrierList.size(), barrierList.data());
    }

    void barrier(std::initializer_list<Barrier> barriers)
    {
        m_commandList->ResourceBarrier(barriers.size(), barriers.begin());
    }

    void barrier(const D3D12_RESOURCE_BARRIER& barrier)
    {
        m_commandList->ResourceBarrier(1, &barrier);
    }

    void barrier(BarrierBlock& barrier)
    {
        if (barrier.empty()) return;

        m_commandList->ResourceBarrier(barrier.size(), barrier);

        barrier.clear();
    }

    void dispatchRays(UINT width, UINT height)
    {
        D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
        dispatchDesc.HitGroupTable.StartAddress = RenderingPipeline::m_hitGroupShaderTable->GetGPUVirtualAddress();
        dispatchDesc.HitGroupTable.SizeInBytes = RenderingPipeline::m_hitGroupShaderTable->GetDesc().Width;
        dispatchDesc.HitGroupTable.StrideInBytes = dispatchDesc.HitGroupTable.SizeInBytes;
        dispatchDesc.MissShaderTable.StartAddress = RenderingPipeline::m_missShaderTable->GetGPUVirtualAddress();
        dispatchDesc.MissShaderTable.SizeInBytes = RenderingPipeline::m_missShaderTable->GetDesc().Width;
        dispatchDesc.MissShaderTable.StrideInBytes = dispatchDesc.MissShaderTable.SizeInBytes;
        dispatchDesc.RayGenerationShaderRecord.StartAddress = RenderingPipeline::m_rayGenShaderTable->GetGPUVirtualAddress();
        dispatchDesc.RayGenerationShaderRecord.SizeInBytes = RenderingPipeline::m_rayGenShaderTable->GetDesc().Width;
        dispatchDesc.Width = width;
        dispatchDesc.Height = height;
        dispatchDesc.Depth = 1;

        m_commandList->DispatchRays(&dispatchDesc);
    }

    void updateScene(RaytraceScene& raytraceScene) 
    {
        raytraceScene.update(m_commandList);
    }
};

} //namespace render