#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include "Render/D3D/d3dx12.h"
#include <wrl.h>

#include "Render/D3D/D3DInstance.h"
#include "Render/D3D/RenderingPipeline.h"

#include "Render/D3D/SwapChain.h"
#include "Render/D3D/FrameBuffer.h"
#include "Render/D3D/RenderBuffer.h"
#include "Render/D3D/CubemapBuffer.h"
#include "Render/D3D/CommandList.h"
#include "Render/D3D/ComputeContext.h"
#include "Render/D3D/RaytraceContext.h"
#include "Render/D3D/MemoryBarrier.h"
#include "Render/D3D/Buffer.h"
#include "Render/D3D/Bitmap.h"
#include "Render/D3D/PushBuffer.h"
#include "Render/D3D/VertexBuffer.h"
#include "Render/D3D/IndexBuffer.h"
#include "Render/D3D/ConstantBuffer.h"
#include "Render/D3D/RaytraceScene.h"

namespace Render
{

using GpuInstance = D3DInstance;

enum Topology
{
    topology_pointlist = D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
    topology_linelist = D3D_PRIMITIVE_TOPOLOGY_LINELIST,
    topology_linestrip = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
    topology_trianglelist = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
    topology_trianglestrip = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
};

}

#undef min
#undef max