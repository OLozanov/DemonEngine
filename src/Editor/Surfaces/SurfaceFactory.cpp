#include "SurfaceFactory.h"
#include "BezierSurface.h"
#include "BezierTriangleSurface.h"
#include "BSplineSurface.h"

#include "EditorResources.h"

SurfaceFactory::FactoryFunc SurfaceFactory::Factories[] = { SurfaceFactory::CreateBezierPatch,
                                                            SurfaceFactory::CreateBezierTriangle,
                                                            SurfaceFactory::CreateBSpline };

Surface* SurfaceFactory::CreateBezierPatch(FILE* file)
{
    vec3 pos;

    uint32_t xpow;
    uint32_t ypow;

    uint32_t xres;
    uint32_t yres;

    uint32_t flags;

    TextureMapping mapMode;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&xpow, sizeof(uint32_t), 1, file);
    fread(&ypow, sizeof(uint32_t), 1, file);
    fread(&xres, sizeof(uint32_t), 1, file);
    fread(&yres, sizeof(uint32_t), 1, file);
    fread(&flags, sizeof(uint32_t), 1, file);
    fread(&mapMode, sizeof(TextureMapping), 1, file);

    vec3 s, t;
    vec2 uv;

    std::vector<vec4> cp;
    cp.resize((xpow + 1) * (ypow + 1));

    fread(cp.data(), sizeof(vec4), cp.size(), file);

    fread(&s, sizeof(vec3), 1, file);
    fread(&t, sizeof(vec3), 1, file);
    fread(&uv, sizeof(vec2), 1, file);

    std::vector<vec2> tcoord;

    if (mapMode == TextureMapping::TCoords)
    {
        tcoord.resize(xres * yres);
        fread(tcoord.data(), sizeof(vec2), tcoord.size(), file);
    }

    BezierSurface* surface = new BezierSurface(pos, xpow, ypow, xres, yres, cp, tcoord, s, t, uv, flags);

    uint16_t tlen;    
    fread(&tlen, sizeof(uint16_t), 1, file);

    std::string tname;
    tname.resize(tlen);
    fread(tname.data(), 1, tlen, file);

    surface->setMaterial(LoadMaterial(tname));

    return surface;
}

Surface* SurfaceFactory::CreateBezierTriangle(FILE* file)
{
    vec3 pos;

    uint32_t power;
    uint32_t res;

    uint32_t flags;
    TextureMapping mapMode;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&power, sizeof(uint32_t), 1, file);
    fread(&res, sizeof(uint32_t), 1, file);
    fread(&flags, sizeof(uint32_t), 1, file);
    fread(&mapMode, sizeof(TextureMapping), 1, file);

    std::vector<vec4> cp;
    cp.resize((power + 1) * (power + 2) / 2);

    fread(cp.data(), sizeof(vec4), cp.size(), file);

    vec3 s, t;
    vec2 uv;

    fread(&s, sizeof(vec3), 1, file);
    fread(&t, sizeof(vec3), 1, file);
    fread(&uv, sizeof(vec2), 1, file);

    std::vector<vec2> tcoord;

    if (mapMode == TextureMapping::TCoords)
    {
        tcoord.resize(res * (res + 1) / 2);
        fread(tcoord.data(), sizeof(vec2), tcoord.size(), file);
    }

    BezierTriangleSurface* surface = new BezierTriangleSurface(pos, power, res, cp, tcoord, s, t, uv, flags);

    uint16_t tlen;
    fread(&tlen, sizeof(uint16_t), 1, file);

    std::string tname;
    tname.resize(tlen);
    fread(tname.data(), 1, tlen, file);

    surface->setMaterial(LoadMaterial(tname));

    return surface;
}

Surface* SurfaceFactory::CreateBSpline(FILE* file)
{
    vec3 pos;

    uint32_t xpow;
    uint32_t ypow;

    uint32_t xsize;
    uint32_t ysize;

    uint32_t xres;
    uint32_t yres;

    uint32_t flags;

    TextureMapping mapMode;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&xpow, sizeof(uint32_t), 1, file);
    fread(&ypow, sizeof(uint32_t), 1, file);
    fread(&xres, sizeof(uint32_t), 1, file);
    fread(&yres, sizeof(uint32_t), 1, file);
    fread(&xsize, 1, sizeof(uint32_t), file);
    fread(&ysize, 1, sizeof(uint32_t), file);
    fread(&flags, sizeof(uint32_t), 1, file);
    fread(&mapMode, sizeof(TextureMapping), 1, file);

    mat3 texMat;

    fread(&texMat, 1, sizeof(mat3), file);

    vec3 s, t;
    vec2 uv;

    fread(&s, sizeof(vec3), 1, file);
    fread(&t, sizeof(vec3), 1, file);
    fread(&uv, sizeof(vec2), 1, file);

    std::vector<vec4> cp;
    cp.resize((xpow + 1) * (ypow + 1));

    fread(cp.data(), sizeof(vec4), cp.size(), file);

    BSplineSurface* surface = new BSplineSurface(pos, xpow, ypow, xsize, ysize, xres, yres, cp, texMat, s, t, uv, flags, mapMode);

    uint16_t tlen;
    fread(&tlen, sizeof(uint16_t), 1, file);

    std::string tname;
    tname.resize(tlen);
    fread(tname.data(), 1, tlen, file);

    surface->setMaterial(LoadMaterial(tname));

    return surface;
}

Surface* SurfaceFactory::CreateSurface(SurfaceType type, FILE* file)
{
    uint8_t index = static_cast<uint8_t>(type);

    return Factories[index](file);
}