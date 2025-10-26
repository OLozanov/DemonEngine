#pragma once

#include "Render/D3D/D3DInstance.h"
#include "math/math3d.h"

using Microsoft::WRL::ComPtr;

namespace UI
{

struct DrawCommand;
class Vertex2d;

constexpr UINT BlankImage = 1;

class UiRenderer
{
public:

    enum Topology
    {
        topology_point_list = 1,
        topology_line_list = 2,
        topology_line_strip = 3,
        topology_triangle_list = 4,
        topology_triangle_strip = 5,
    };

    enum Flags
    {
        sdf = 1
    };

private:
    struct ScreenConstantBuffer
    {
        vec2 screenMat[3];
        float padding[58]; // Padding so the constant buffer is 256-byte aligned.
    };
    static_assert((sizeof(ScreenConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

    Render::D3DInstance& m_d3dInstance;

    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;

    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    ComPtr<ID3D12Resource> m_sceneConstantBuffer;
    ScreenConstantBuffer* m_sceneConstantBufferData;

    UINT m_imageHandle;
    D3D_PRIMITIVE_TOPOLOGY m_topology;
    uint32_t m_flags;
    float m_param;

    ID3D12Resource* m_vertexBuffer;
    Vertex2d* m_vertexBufferData;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    void setupShader();

public:
    UiRenderer();

    void init();
    void setScreenSize(int width, int height);
    void setCursorSize(float width, float height);
    void setCaretSize(int width, int height);
    void setCaretColor(const vec4& color);

    void beginDraw();
    void endDraw();

    void resetClipArea();
    void setClipArea(const D3D12_RECT& rect);

    void setDrawOffset(const vec2& offset);
    void draw(const D3D12_VERTEX_BUFFER_VIEW* vertexBufferView, const std::vector<DrawCommand>& commands);
    void draw(const D3D12_VERTEX_BUFFER_VIEW* vertexBufferView, ID3D12GraphicsCommandList* bundle);

    void drawCursor(UINT image);
    void drawCaret();

    void render();
};

} //namespace ui