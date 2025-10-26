#pragma once

#include "math/math3d.h"

namespace Render
{

struct SceneConstantBuffer
{
    mat4 projViewMat;
    mat4 worldMat;
    vec3 eyepos;
    float fovx;
    float fovy;
    float pad[3];
    vec4 topleft;
    vec4 xdir;
    vec4 ydir;
};

} //namespace render