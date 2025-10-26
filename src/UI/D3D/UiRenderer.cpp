#include "UiRenderer.h"
#include "Render/D3D/D3DInstance.h"
#include "System/ErrorMsg.h"

#include "UI/Render.h"

#include <d3dcompiler.h>

namespace UI
{

UiRenderer::UiRenderer()
: m_d3dInstance(Render::D3DInstance::GetInstance())
, m_viewport(0.0f, 0.0f, static_cast<float>(1280), static_cast<float>(720))
, m_scissorRect(0, 0, static_cast<LONG>(1280), static_cast<LONG>(720))
{
}

void UiRenderer::init()
{
    setupShader();

    m_d3dInstance.device()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
    m_d3dInstance.device()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));

    m_commandList->Close();

    const UINT constantBufferSize = sizeof(ScreenConstantBuffer);

    ThrowIfFailed(m_d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_sceneConstantBuffer)));

    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(m_sceneConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_sceneConstantBufferData)));
}

void UiRenderer::setScreenSize(int width, int height)
{
    m_viewport.Width = width;
    m_viewport.Height = height;

    m_scissorRect.right = width;
    m_scissorRect.bottom = height;

    m_sceneConstantBufferData->screenMat[0] = { 1.0f / width * 2.0f, 0.0f };
    m_sceneConstantBufferData->screenMat[1] = { 0.0f, -1.0f / height * 2.0f };
    m_sceneConstantBufferData->screenMat[2] = { -1.0f, 1.0f };
}

void UiRenderer::setCursorSize(float width, float height)
{
    m_vertexBufferData[0] = Vertex2d(width, 0.0,    1.0, 0.0);
    m_vertexBufferData[1] = Vertex2d(width, height, 1.0, 1.0);
    m_vertexBufferData[2] = Vertex2d(0.0, 0.0,      0.0, 0.0);
    m_vertexBufferData[3] = Vertex2d(0.0, height,   0.0, 1.0);
}

void UiRenderer::setCaretSize(int width, int height)
{
    m_vertexBufferData[4] = Vertex2d(width, 0.0, 1.0, 0.0);
    m_vertexBufferData[5] = Vertex2d(width, height, 1.0, 1.0);
    m_vertexBufferData[6] = Vertex2d(0.0, 0.0, 0.0, 0.0);
    m_vertexBufferData[7] = Vertex2d(0.0, height, 0.0, 1.0);
}

void UiRenderer::setCaretColor(const vec4& color)
{
    for (size_t i = 0; i < 4; i++) m_vertexBufferData[4 + i].color = color;
}

void UiRenderer::setupShader()
{
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(m_d3dInstance.device()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
    CD3DX12_ROOT_PARAMETER1 rootParameters[3];

    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsConstants(4, 1, 0, D3D12_SHADER_VISIBILITY_ALL);
    rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 0;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // Allow input layout and deny uneccessary access to certain pipeline stages.
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
    ThrowIfFailed(m_d3dInstance.device()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

    // Create the pipeline state, which includes compiling and loading shaders.
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    std::wstring shaderPath = L".\\Shaders\\uilayer.hlsl";

    ThrowIfFailed(D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
    ThrowIfFailed(D3DCompileFromFile(shaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

    // Define the vertex input layout.
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.BlendState.RenderTarget[0] =
    {
        TRUE, FALSE,
        D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
    psoDesc.SampleDesc.Count = 1;

    ThrowIfFailed(m_d3dInstance.device()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));

    const UINT vertexBufferSize = 8 * sizeof(Vertex2d);

    ThrowIfFailed(m_d3dInstance.device()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)));

    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_vertexBufferData)));

    // Initialize the vertex buffer view.
    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(Vertex2d);
    m_vertexBufferView.SizeInBytes = vertexBufferSize;
}

void UiRenderer::beginDraw()
{
    ID3D12DescriptorHeap* srvHeap = m_d3dInstance.descriptorHeap();
    m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get());
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    m_commandList->SetGraphicsRootConstantBufferView(0, m_sceneConstantBuffer->GetGPUVirtualAddress());

    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    ID3D12DescriptorHeap* ppHeaps[] = { srvHeap };
    m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    const Render::FrameBuffer& frameBuffer = Win32App::GetFrameBuffer();

    const std::vector<D3D12_RESOURCE_BARRIER>& barrierList = Win32App::GetFrameBuffer().writeBarriers();
    m_commandList->ResourceBarrier(1, barrierList.data());

    const D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle = frameBuffer.renderTargets();
    m_commandList->OMSetRenderTargets(1, rtvHandle, FALSE, nullptr);

    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_commandList->SetGraphicsRootDescriptorTable(2, m_d3dInstance.GpuDescriptor(BlankImage));

    m_imageHandle = 1;
    m_topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    m_flags = 0;
    m_param = 0;

    m_commandList->SetGraphicsRoot32BitConstants(1, 2, &m_flags, 2);
}

void UiRenderer::endDraw()
{
    const std::vector<D3D12_RESOURCE_BARRIER>& barrierList = Win32App::GetFrameBuffer().readBarriers();

    m_commandList->ResourceBarrier(1, barrierList.data());
    m_commandList->Close();
}

void UiRenderer::resetClipArea()
{
    m_commandList->RSSetScissorRects(1, &m_scissorRect);
}

void UiRenderer::setClipArea(const D3D12_RECT& rect)
{
    m_commandList->RSSetScissorRects(1, &rect);
}

void UiRenderer::setDrawOffset(const vec2& offset)
{
    m_commandList->SetGraphicsRoot32BitConstants(1, 2, &offset, 0);
}

void UiRenderer::draw(const D3D12_VERTEX_BUFFER_VIEW* vertexBufferView, const std::vector<DrawCommand>& commands)
{
    m_commandList->IASetVertexBuffers(0, 1, vertexBufferView);

    UINT offset = 0;

    for (const DrawCommand& cmd : commands)
    {
        if (m_imageHandle != cmd.imgHandle)
        {
            m_imageHandle = cmd.imgHandle;
            m_commandList->SetGraphicsRootDescriptorTable(2, m_d3dInstance.GpuDescriptor(cmd.imgHandle));
        }

        if (m_topology != cmd.topology)
        {
            m_topology = static_cast<D3D_PRIMITIVE_TOPOLOGY>(cmd.topology);
            m_commandList->IASetPrimitiveTopology(m_topology);
        }

        if (m_flags != cmd.flags)
        {
            m_flags = cmd.flags;
            m_commandList->SetGraphicsRoot32BitConstants(1, 1, &m_flags, 2);
        }

        if (m_flags && m_param != cmd.param)
        {
            m_param = cmd.param;
            m_commandList->SetGraphicsRoot32BitConstants(1, 1, &m_param, 3);
        }

        m_commandList->DrawInstanced(cmd.vnum, 1, offset, 0);
        offset += cmd.vnum;
    }
}

void UiRenderer::draw(const D3D12_VERTEX_BUFFER_VIEW* vertexBufferView, ID3D12GraphicsCommandList* bundle)
{
    m_commandList->IASetVertexBuffers(0, 1, vertexBufferView);
    m_commandList->ExecuteBundle(bundle);
}

void UiRenderer::drawCursor(UINT image)
{
    if (m_topology != D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP) m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    if (m_flags != 0)
    {
        m_flags = 0;
        m_commandList->SetGraphicsRoot32BitConstants(1, 1, &m_flags, 2);
    }

    m_commandList->SetGraphicsRootDescriptorTable(2, m_d3dInstance.GpuDescriptor(image));

    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandList->DrawInstanced(4, 1, 0, 0);
}

void UiRenderer::drawCaret()
{
    if (m_topology != D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP) m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    m_commandList->SetGraphicsRootDescriptorTable(2, m_d3dInstance.GpuDescriptor(1));

    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandList->DrawInstanced(4, 1, 4, 0);
}

void UiRenderer::render()
{
    m_d3dInstance.execute(m_commandList.Get());
}

} //namespace ui