#pragma once

#include <stdint.h>
#include "Surface.h"

class SurfaceFactory
{
    SurfaceFactory() = delete;
    ~SurfaceFactory() = delete;

    static Surface* CreateBezierPatch(FILE* file);
    static Surface* CreateBezierTriangle(FILE* file);
    static Surface* CreateBSpline(FILE* file);

    using FactoryFunc = Surface * (*)(FILE* file);

    static FactoryFunc Factories[];

public:

    static Surface* CreateSurface(SurfaceType type, FILE* file);
};