#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Render/D3D/d3dx12.h"

#include "math/math3d.h"

namespace Render
{

using Microsoft::WRL::ComPtr;

class CommandList;

class RenderingPipeline
{
public:
    enum RenderMode
    {
        rm_color = 0,
        rm_color_wire = 1,
        rm_color_point = 2,
        rm_color_line = 3,
        rm_color_range = 4,
        rm_simple = 5,
        rm_simple_decal = 6,
        rm_simple_layered = 7,
        rm_sprite_simple = 8,
        rm_sprite = 9,
        rm_sky = 10,
        rm_shadow = 11,
        rm_shadow_cascaded = 12,
        rm_shadow_cube = 13,
        rm_gbuffer = 14,
        rm_gbuffer_overlay = 15,
        rm_gbuffer_instanced = 16,
        rm_gbuffer_layered = 17,
        rm_omnilight = 18,
        rm_spotlight = 19,
        rm_dirlight = 20,
        rm_emissive = 21,
        rm_transparent = 22,
        rm_fog = 23,
        rm_tone_mapping = 24,
        rm_count
    };

    enum ComputeMode
    {
        cm_skin = 0,
        cm_denoise_spatial = 1,
        cm_disocclusion_blur = 2,
        cm_ambient = 3,
        cm_lightgrid = 4,
        cm_lighting = 5,
        cm_count
    };

    static void Init();

    static void ResizeViewport(int width, int height);

    static void StartRender(CommandList& commandList, RenderingPipeline::RenderMode mode);
    static ID3D12GraphicsCommandList* StartCompute(ComputeMode mode);
    static ID3D12GraphicsCommandList5* StartRaytracing();

    static void RaytraceStartBuild();
    static void RaytraceBuildAs(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC* bottomLevelAsDes);
    static void RaytraceBuildBarrier(ID3D12Resource* resource);
    static void RaytraceFinishBuild();

private:
    static void SetupColorShader();
    static void SetupColorPointShader();
    static void SetupColorLineShader();
    static void SetupColorRangeShader();
    static void SetupSimpleShader();
    static void SetupSimpleSpriteShader();
    static void SetupSpriteShader();
    static void SetupSkyShader();
    static void SetupShadowShader();
    static void SetupCascadedShadowShader();
    static void SetupShadowCubeShader();
    static void SetupGBufferShader();
    static void SetupOmniLightShader();
    static void SetupSpotLightShader();
    static void SetupDirectionalLightShader();
    static void SetupEmissiveShader();
    static void SetupTransparentShader();
    static void SetupFogShader();
    static void SetupToneMappingShader();

#ifndef EDITOR
    static void SetupSkinShader();
    static void SetupDenoiseShader();
    static void SetupDisocclusionShader();
    static void SetupAmbientShader();
    static void SetupLightGridShader();
    static void SetupLightingShader();

    static void CreateRaytracingSignatures();
    static void SetupRaytracingPipeline();
    static void BuildRaytracingShaderTables();
#endif

    static void InitResources();

private:
    static CD3DX12_VIEWPORT m_viewport;
    static CD3DX12_RECT m_scissorRect;

    static ComPtr<ID3D12RootSignature> m_rootSignature[rm_count];
    static ComPtr<ID3D12PipelineState> m_pipelineState[rm_count];

    static ComPtr<ID3D12RootSignature> m_computeRootSignature[cm_count];
    static ComPtr<ID3D12PipelineState> m_computePipelineState[cm_count];

    static ComPtr<ID3D12RootSignature> m_dxrGlobalRootSignature;
    static ComPtr<ID3D12RootSignature> m_dxrLocalRootSignature;
    static ComPtr<ID3D12StateObject> m_dxrState;

    static ComPtr<ID3D12CommandAllocator> m_computeCommandAllocator[cm_count];
    static ComPtr<ID3D12GraphicsCommandList> m_computeCommandList[cm_count];

    static ComPtr<ID3D12CommandAllocator> m_dxrCommandAllocator;
    static ComPtr<ID3D12GraphicsCommandList5> m_dxrCommandList;

    static ComPtr<ID3D12Resource> m_missShaderTable;
    static ComPtr<ID3D12Resource> m_hitGroupShaderTable;
    static ComPtr<ID3D12Resource> m_rayGenShaderTable;

    static constexpr DXGI_FORMAT DepthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    friend class SceneManager;
    friend class CommandList;
    friend class ComputeContext;
    friend class RaytraceContext;
};

} //namespace Render