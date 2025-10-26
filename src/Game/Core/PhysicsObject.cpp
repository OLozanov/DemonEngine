#include "PhysicsObject.h"

#include "System/AudioManager.h"
#include "Physics/PhysicsManager.h"
#include "Render/SceneManager.h"

#include "Collision/BoxCollisionShape.h"
#include "Collision/PolygonalCollisionShape.h"

#include "Utils/FileStream.h"

namespace GameLogic
{

PhysicsObject::PhysicsObject(const vec3& pos, const mat3& mat, float mass, unsigned long layers, Model* model)
: RigidBody(pos, mass, 0.2f, 0.9f)
, StaticObject(pos, mat, model, true)
, m_bumpImpulse(60)
{
    Physics::PhysicsManager::GetInstance().addRigidBody(static_cast<RigidBody*>(this));
    Render::SceneManager::GetInstance().addObject(static_cast<StaticObject*>(this));

    float bboxsz = StaticObject::m_bbox.max.length();
    RigidBody::m_bbox = { bboxsz, bboxsz, bboxsz };

    m_orientation = StaticObject::m_mat;

    const auto& collisionData = m_model->collisionData();

    if (!collisionData.empty()) m_collisionShape = new Collision::PolygonalCollisionShape(m_orientation, m_pos, collisionData.size(), collisionData.data());
    else m_collisionShape = new Collision::BoxCollisionShape(m_orientation, m_pos, StaticObject::m_bbox.max);

    m_layers = layers;

    vec3 inertiaTensor = Physics::BoxInertiaTensor(StaticObject::m_bbox.max - StaticObject::m_bbox.min, m_mass);
    setInertia(inertiaTensor);

    m_object = static_cast<Hitable*>(this);

    /*const vec3& bbox = StaticObject::m_bbox.max;
    float volume = bbox.x * bbox.y * bbox.z * 8;

    if (mass < 10) volume = 0.8;

    float dimension = std::min(bbox.x, std::min(bbox.y, bbox.z));

    static const vec3 points[] = { {0.5, 0.5, 0.5},
                                {-0.5, 0.5, 0.5},
                                {0.5, 0.5, -0.5},
                                {-0.5, 0.5, -0.5},
                                {0.5, -0.5, 0.5},
                                {-0.5, -0.5, 0.5},
                                {0.5, -0.5, -0.5},
                                {-0.5, -0.5, -0.5},
                                {0.0, 0.0, 0.0} };

    m_buoyancy.reset(new  BuoyancyGenerator(*SceneManager::GetInstance()->getWorld(), dimension / 2.0, volume / 8));
    PhysicsManager::GetInstance()->addForce(*m_buoyancy, *this);

    for (int i = 0; i < 8; i++)
    {
        vec3 applyPoint = { points[i].x * bbox.x, points[i].y * bbox.y, points[i].z * bbox.z };
        m_buoyancy->addPoint(applyPoint);
    }*/
}

void PhysicsObject::update(float dt)
{
    if (m_rest) return;

    DisplayObject::m_mat = transformMat();
    Render::SceneManager::GetInstance().moveObject(static_cast<StaticObject*>(this));
}

void PhysicsObject::onCollide(const vec3& normal, float impulse)
{
    if (m_bumpSnd && impulse > m_bumpImpulse) AudioManager::Play(m_bumpSnd, m_pos);
}

void PhysicsObject::hit(const vec3& point, const vec3& direction, uint32_t power)
{
    vec3 localPoint = (point - RigidBody::m_pos) * RigidBody::m_orientation;
    vec3 localDir = (direction * RigidBody::m_orientation);

    applyImpulse(localDir * power, localPoint);
}

void PhysicsObject::remove()
{
    Physics::PhysicsManager::GetInstance().removeRigidBody(this);
    Render::SceneManager::GetInstance().removeObject(this);
}

void PhysicsObject::save(FileStream& file)
{
    file << m_pos;
    file << m_velocity;

    file << m_angularMomentum;
    file << m_angularVelocity;

    file << m_orientation[0];
    file << m_orientation[1];

    file << m_rest;
}

void PhysicsObject::restore(FileStream& file)
{
    file >> m_pos;
    file >> m_velocity;

    file >> m_angularMomentum;
    file >> m_angularVelocity;

    file >> m_orientation[0];
    file >> m_orientation[1];
    m_orientation[2] = m_orientation[0] ^ m_orientation[1];

    file >> m_rest;

    DisplayObject::m_mat = transformMat();
    Render::SceneManager::GetInstance().moveObject(static_cast<StaticObject*>(this));
}

} // namespace gamelogic