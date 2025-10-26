#pragma once

#include "math/math3d.h"
#include <vector>

#include "Render/DisplayData.h"

enum class PolyType { Back, Front, Plane, Split };
enum class PointType { Back, Front, Plane };
enum class VolumeStatus { Inside, Outside, Border };

enum PolygonFlag
{
    PolyTransparent = 1,
    PolyNoCollision = 2,
    PolyTwoSide = 4,
    PolyZonePortal = 8,
    PolyInvisible = 0x10,
    PolySky = 0x20,
    PolyWater = 0x40
};

enum class TextureMapping : uint8_t
{
    TCoords,
    TSpace
};

using Vertex = Render::Vertex;

using VertexList = std::vector<Vertex>;

struct BlockPolygon;

struct EditPolygon
{
    BlockPolygon* origin;

    vec4 plane;

    VolumeStatus status;

    bool splitter;
    bool border;
	uint8_t flags;
	ResourcePtr<Material> material;

    std::vector<Vertex> vertices;
};

using PolygonList = std::vector<EditPolygon>;

float RayIntersect(const vec3& origin1, const vec3& origin2, const vec3& ray1, const vec3& ray2, float& s, float& t);

PolyType ClassifyPolygon(const vec4& plane, const VertexList& verts);
void SplitPoly(const vec4& plane, const VertexList& verts, VertexList& left, VertexList& right);

PolyType ClassifyPolygon(const vec4& plane, const std::vector<vec3>& verts);
void SplitPoly(const vec4& plane, const std::vector<vec3>& verts, std::vector<vec3>& left, std::vector<vec3>& right);

bool TriangleIntersect(const vec3& v0, const vec3& v1, const vec3& v2,
                       const vec3& origin, const vec3& ray, float& dist);

bool PolyIntersect(const EditPolygon& poly, const vec3& origin, const vec3& ray, float& dst, bool frontCull = false);

void TriangleTangentSpace(const vec3& a,
                          const vec3& b,
                          const vec3& c,
                          const vec2& ta,
                          const vec2& tb,
                          const vec2& tc,
                          vec3& s,
                          vec3& t);