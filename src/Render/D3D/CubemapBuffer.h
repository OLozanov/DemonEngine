#pragma once

#include "Render/D3D/FrameBuffer.h"
#include "Render/D3D/Bitmap.h"

#include <vector>

namespace Render
{

class CubemapBuffer : public Bitmap
{
public:
    explicit CubemapBuffer(DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);

    void reset(UINT size);

private:
    void createReadHandle(DXGI_FORMAT format);
    void createWriteHandle(DXGI_FORMAT format);

    void createDepthBuffer(UINT size);
    void createColorBuffer(UINT size);
};

} //namespace render