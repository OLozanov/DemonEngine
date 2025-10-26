#pragma once

#include "math/math3d.h"
#include "Render/World.h"

namespace Render
{

namespace Clipping
{

bool FrustumAABBVis(const vec3& pos, const vec3* frustum, const vec4& plane, const vec3& bbpos, const vec3& size);
bool FrustumLeafVis(const vec3& pos, const vec3* frustum, const vec4& plane, const BBox& bbox);
bool FrustumObjVis(const vec3& pos, const vec3* frustum, const vec4& plane, const DisplayObject& obj);

bool LeafVis(const vec3& pos, const Portal& portal, const BBox& bbox);
bool ObjVis(const vec3& pos, const Portal& portal, const DisplayObject* obj);
bool AABBVis(const vec3& pos, const Portal& portal, const vec3& box, const vec3& bbpos);

bool LeafVisDir(const vec3& dir, const vec3* verts, unsigned long vnum, const BBox& bbox);
bool LeafVisDir(const vec3& dir, const Portal& portal, const BBox& bbox);
bool ObjVisDir(const vec3& dir, const vec3* verts, unsigned long vnum, const DisplayObject* obj);
bool ObjVisDir(const vec3& dir, const Portal& portal, const DisplayObject* obj);

bool PortalVis(const vec3& pos, Portal& portal, Portal& testPortal);
bool PortalVisDir(const vec3& dir, Portal& portal, Portal& tport);
bool PortalVisFrustum(const vec3& pos, const vec3* frustum, const vec4& plane, Portal& tport);

} //namespace clipping

} //namespace render