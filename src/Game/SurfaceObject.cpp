#include "SurfaceObject.h"

namespace GameLogic
{

SurfaceObject::SurfaceObject(const vec3& pos,
                             const BBox& bbox,
                             Render::Surface * surface,
                             uint32_t xsize,
                             uint32_t ysize,
                             const std::vector<vec3>& verts)
{
    m_pos = pos;

    m_geometry = surface;
    Render::SceneManager::GetInstance().addObject(m_geometry);
    Render::SceneManager::GetInstance().addStaticGeometry(surface->vertexBuffer(), surface->indexBuffer(), m_pos);

    if (verts.empty()) return;

    int pnum = (xsize - 1) * (ysize - 1) * 2;
    m_collisionMesh.resize(pnum);

    int cplg = 0;

    for (int k = 0; k < xsize - 1; k++)
    {
        for (int i = 0; i < ysize - 1; i++)
        {
            int v = i * xsize + k;

            {
                Collision::CollisionPolygon& poly = m_collisionMesh[cplg];
                poly.verts.resize(3);

                //
                poly.verts[0] = vec3(verts[v].x, verts[v].y, verts[v].z);
                poly.verts[1] = vec3(verts[v + 1].x, verts[v + 1].y, verts[v + 1].z);
                poly.verts[2] = vec3(verts[v + xsize].x, verts[v + xsize].y, verts[v + xsize].z);

                vec3 a = poly.verts[1] - poly.verts[0];
                vec3 b = poly.verts[2] - poly.verts[0];
                vec3 norm = a ^ b;
                norm.normalize();

                m_collisionMesh[cplg].plane.xyz = norm;
                m_collisionMesh[cplg].plane.w = -poly.verts[0] * norm;
                cplg++;
            }

            //
            {
                Collision::CollisionPolygon& poly = m_collisionMesh[cplg];
                poly.verts.resize(3);

                poly.verts[0] = vec3(verts[v + xsize].x, verts[v + xsize].y, verts[v + xsize].z);
                poly.verts[1] = vec3(verts[v + 1].x, verts[v + 1].y, verts[v + 1].z);
                poly.verts[2] = vec3(verts[v + xsize + 1].x, verts[v + xsize + 1].y, verts[v + xsize + 1].z);

                vec3 a = poly.verts[1] - poly.verts[0];
                vec3 b = poly.verts[2] - poly.verts[0];
                vec3 norm = a ^ b;
                norm.normalize();

                m_collisionMesh[cplg].plane.xyz = norm;
                m_collisionMesh[cplg].plane.w = -poly.verts[0] * norm;
                cplg++;
            }
        }
    }

    Collision::PolygonalCollisionShape * collisionShape = new Collision::PolygonalCollisionShape(m_orientation, m_pos, pnum, m_collisionMesh.data());

    m_body = new Physics::StationaryBody(collisionShape, bbox, collision_map);
    Physics::PhysicsManager::GetInstance().addStationaryBody(m_body);
}

SurfaceObject::SurfaceObject(const vec3& pos,
                             const BBox& bbox,
                             Render::Surface * surface,
                             uint32_t res,
                             const std::vector<vec3>& verts)
{
    m_pos = pos;

    m_geometry = surface;
    Render::SceneManager::GetInstance().addObject(m_geometry);
    Render::SceneManager::GetInstance().addStaticGeometry(surface->vertexBuffer(), surface->indexBuffer(), m_pos);

    if (verts.empty()) return;

    int tri1 = (res)*(res+1)/2;
    int tri2 = (res-1)*(res)/2;

    int pnum = tri1+tri2;

    m_collisionMesh.resize(pnum);

    int cplg = 0;

    int p = 0;
    int v = 0;

    int num = res - 1;

    for(int k = 0; k < res; k++, num--)
    {
        int vnext = v + num+1;

        for(int i = 0; i < num; i++)
        {
            int v1 = v + i;
            int v2 = vnext + i;

            Collision::CollisionPolygon& poly = m_collisionMesh[cplg];
            poly.verts.resize(3);

            poly.verts[0] = vec3(verts[v1].x, verts[v1].y, verts[v1].z);
            poly.verts[1] = vec3(verts[v2].x, verts[v2].y, verts[v2].z);
            poly.verts[2] = vec3(verts[v1+1].x, verts[v1+1].y, verts[v1+1].z);

            vec3 a = poly.verts[1] - poly.verts[0];
            vec3 b = poly.verts[2] - poly.verts[0];
            vec3 norm = a ^ b;
            norm.normalize();

            m_collisionMesh[cplg].plane.xyz = norm;
            m_collisionMesh[cplg].plane.w = -poly.verts[0] * norm;
            cplg++;
        }

        v = vnext;
    }

    v = 0;
    num = res - 1;

    for(int k = 0; k < res-1; k++, num--)
    {
        int vnext = v + num+1;

        for(int i = 0; i < num-1; i++)
        {
            int v1 = v + i;
            int v2 = vnext + i;

            Collision::CollisionPolygon& poly = m_collisionMesh[cplg];
            poly.verts.resize(3);

            poly.verts[0] = vec3(verts[v2].x, verts[v2].y, verts[v2].z);
            poly.verts[1] = vec3(verts[v2+1].x, verts[v2+1].y, verts[v2+1].z);
            poly.verts[2] = vec3(verts[v1+1].x, verts[v1+1].y, verts[v1+1].z);

            vec3 a = poly.verts[1] - poly.verts[0];
            vec3 b = poly.verts[2] - poly.verts[0];
            vec3 norm = a^b;
            norm.normalize();

            m_collisionMesh[cplg].plane.xyz = norm;
            m_collisionMesh[cplg].plane.w = -poly.verts[0] * norm;
            cplg++;
            p += 3;
        }

        v = vnext;
    }

    Collision::PolygonalCollisionShape * collisionShape = new Collision::PolygonalCollisionShape(m_orientation, m_pos, pnum, m_collisionMesh.data());

    m_body = new Physics::StationaryBody(collisionShape, bbox, collision_map);
    Physics::PhysicsManager::GetInstance().addStationaryBody(m_body);
}

} // namespace gamelogic