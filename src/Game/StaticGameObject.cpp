#include "StaticGameObject.h"

namespace GameLogic
{

StaticGameObject::StaticGameObject()
: m_geometry(nullptr)
, m_body(nullptr)
{
}

StaticGameObject::StaticGameObject(const vec3& pos, const mat3& mat, Model* model, uint8_t flags)
: m_model(model)
{
    m_geometry = new Render::StaticObject(pos, mat, model);
    Render::SceneManager::GetInstance().addObject(m_geometry);

    m_pos = pos;
    m_orientation = mat;

    if (flags & flag_collision) addCollisionModel();
    if (flags & flag_gi) Render::SceneManager::GetInstance().addStaticGeometry(m_model->vertexBuffer(), m_geometry->mat());
}

StaticGameObject::~StaticGameObject()
{
    if (m_geometry) delete m_geometry;
    if (m_body) delete m_body;
}

void StaticGameObject::addCollisionModel()
{
    BBox bbox = RecalcBBox(m_model->bbox(), m_orientation);

    Collision::CollisionShape* collisionShape;

    const auto& collisionData = m_model->collisionData();
    if (!collisionData.empty()) collisionShape = new Collision::PolygonalCollisionShape(m_orientation, m_pos, collisionData.size(), collisionData.data());
    else
    {
        const BBox& modelBBox = m_model->bbox();

        vec3 mid = (modelBBox.max + modelBBox.min) * 0.5;
        vec3 colbbox = modelBBox.max - mid;
        m_pos = m_pos + m_orientation * mid;

        collisionShape = new Collision::BoxCollisionShape(m_orientation, m_pos, colbbox);

        bbox = RecalcBBox({ -colbbox, colbbox }, m_orientation);
    }

    m_body = new Physics::StationaryBody(collisionShape, bbox, collision_solid | collision_actor | collision_hitable);

    Physics::PhysicsManager::GetInstance().addStationaryBody(m_body);
}

} // namespace gamelogic