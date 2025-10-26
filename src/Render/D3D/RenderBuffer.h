#pragma once

#include "Render/D3D/FrameBuffer.h"
#include "Render/D3D/Bitmap.h"
#include <initializer_list>

namespace Render
{

class ColorBuffer : public Bitmap
{
public:
    explicit ColorBuffer(DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool read = true, bool write = false);

    void reset(UINT width, UINT height);
};

class DepthBuffer : public Bitmap
{
public:
    explicit DepthBuffer(DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT, bool read = true, bool write = false);

    void reset(UINT width, UINT height);
    void reset(UINT width, UINT height, UINT count);
};

class RenderBuffer : public FrameBuffer
{
public:
    RenderBuffer() {}
    RenderBuffer(DXGI_FORMAT format, DXGI_FORMAT depthFormat = DXGI_FORMAT_D32_FLOAT, bool unorderdAccess = false);
    RenderBuffer(std::initializer_list<DXGI_FORMAT> format, DXGI_FORMAT depthFormat = DXGI_FORMAT_D32_FLOAT, bool unorderdAccess = false);

    void reset();
    void reset(int width, int height, int count = 1);

    void attachDepthBuffer(const Bitmap& buffer);
    void attachColorBuffer(const Bitmap& buffer);

    UINT depthBuffer() { return m_srvHandles[0]; }
    UINT colorBuffer(int n) { return m_srvHandles[n + 1]; }

    ID3D12Resource* depth() { return m_buffers[0].Get(); }

    operator ID3D12Resource* () const { return m_buffers[1].Get(); }

private:
    void allocateDescriptors();
    void createBuffers(int width, int height);
    void createArrays(int width, int height, int count);
    void createDepthBuffer(int width, int height);
    void createDepthArray(int width, int height, int count);
    void createColorBuffer(int n, int width, int height, DXGI_FORMAT format);
    void createColorArray(int n, int width, int height, int count, DXGI_FORMAT format);
    void createBarrierLists();

    DXGI_FORMAT m_depthFormat;
    std::vector<DXGI_FORMAT> m_format;

    std::vector<ComPtr<ID3D12Resource>> m_buffers;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_handles;

    std::vector<UINT> m_srvHandles;

    bool m_unorderedAccess;
};

} //namespace render