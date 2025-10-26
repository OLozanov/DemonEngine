#include "Vehicle.h"

#include "Game/Constants.h"

#include "Collision/BoxCollisionShape.h"
#include "Collision/PolygonalCollisionShape.h"

#include "Render/SceneManager.h"

#include <cmath>

namespace GameLogic
{

DragForceGenerator::DragForceGenerator(float dragFactor)
: m_dragFactor(dragFactor)
{
}

void DragForceGenerator::update(Physics::RigidBody& body, float dt)
{
    const vec3& velocity = body.velocity();
    float drag = velocity * velocity * m_dragFactor;

    body.applyForce(-velocity.normalized() * drag);
}

Vehicle::Vehicle(const vec3& pos, const mat3& orientation, float mass, Model* model, Model* wheelModel)
: RigidBody(pos, mass, 0.35f, 0.9f)
, StaticObject(pos, orientation, model, true)
, m_dragForce(15.0f)
, m_steering(0.0f)
{
    Physics::PhysicsManager::GetInstance().addRigidBody(static_cast<RigidBody*>(this));
    Physics::PhysicsManager::GetInstance().addForce(m_dragForce, *this);
    Render::SceneManager::GetInstance().addObject(static_cast<StaticObject*>(this));

    float bboxsz = StaticObject::m_bbox.max.length();
    RigidBody::m_bbox = { bboxsz, bboxsz, bboxsz };

    m_orientation = StaticObject::m_mat;

    const auto& collisionData = m_model->collisionData();

    if (!collisionData.empty()) m_collisionShape = new Collision::PolygonalCollisionShape(m_orientation, m_pos, collisionData.size(), collisionData.data());
    else m_collisionShape = new Collision::BoxCollisionShape(m_orientation, m_pos, StaticObject::m_bbox.max);

    m_layers = collision_solid | collision_actor | collision_pickable;

    vec3 inertiaTensor = Physics::BoxInertiaTensor(StaticObject::m_bbox.max - StaticObject::m_bbox.min, m_mass);
    setInertia(inertiaTensor);

    m_object = static_cast<Usable*>(this);

    setupWheels(wheelModel);
}

Vehicle::~Vehicle()
{
    for (size_t i = 0; i < m_suspension.size(); i++)
    {
        Physics::PhysicsManager::GetInstance().removeConstraint(m_suspension[i]);
        delete m_suspension[i];
    }

    for (StaticObject* wheel : m_wheels)
    {
        Render::SceneManager::GetInstance().removeObject(wheel);
        delete wheel;
    }
}

void Vehicle::setupWheels(Model* wheelModel)
{
    m_wheelPos = { {-0.8f, -0.51f, -0.87f},
                   {0.8f, -0.51f, -0.87f},
                   {0.8f, -0.51f, 1.039f},
                   {-0.8f, -0.51f, 1.039f} };

    for (size_t i = 0; i < 4; i++)
    {
        Physics::Suspension* suspension = new Physics::Suspension(this, m_wheelPos[i], collision_solid, 0.25f);
        Physics::PhysicsManager::GetInstance().addConstraint(suspension);

        m_suspension.push_back(suspension);
    }

    for (size_t i = 0; i < 4; i++)
    {
        StaticObject* wheel = new StaticObject(m_wheelPos[i], mat3(), wheelModel, true);
        Render::SceneManager::GetInstance().addObject(wheel);

        m_wheels.push_back(wheel);
    }

    m_wheelParams.resize(m_wheelPos.size());
}

void Vehicle::onCollide(const vec3& normal, float impulse)
{
    //if (m_bumpSnd && impulse > m_bumpImpulse) AudioManager::Play(m_bumpSnd, m_pos);
}

void Vehicle::use()
{
    OnMount(this);
}

void Vehicle::input(int key, bool keyDown)
{
    //if (!keyDown) return;

    switch (key)
    {
    case 'W':
        m_moveForward = keyDown;
    break;

    case 'S':
        m_moveBack = keyDown;
    break;

    case 'A':
        m_moveLeft = keyDown;
    break;

    case 'D':
        m_moveRight = keyDown;
    break;
    }

    if (m_moveForward)
    {
        m_suspension[0]->setMotor(20.0f);
        m_suspension[1]->setMotor(20.0f);
    }
    else if (m_moveBack)
    {
        m_suspension[0]->setMotor(-15.0f);
        m_suspension[1]->setMotor(-15.0f);
    }
    else
    {
        m_suspension[0]->setMotor(0.0f);
        m_suspension[1]->setMotor(0.0f);
    }
}

void Vehicle::update(float dt)
{
    static constexpr float steerang = 15.0f / 180.0f * math::pi;
    static constexpr float steervel = 30.0f / 180.0f * math::pi;

    if (m_moveLeft)
    {
        m_steering -= steervel * dt;
        m_steering = std::max(m_steering, -steerang);
    }
    else if (m_moveRight)
    {
        m_steering += steervel * dt;
        m_steering = std::min(m_steering, steerang);
    }
    else
    {
        if (m_steering > math::eps)
        {
            m_steering -= steervel * dt;
            m_steering = std::max(m_steering, 0.0f);
        }
        else if (m_steering < -math::eps)
        {
            m_steering += steervel * dt;
            m_steering = std::min(m_steering, 0.0f);
        }
    }

    m_suspension[2]->setSteering(m_steering);
    m_suspension[3]->setSteering(m_steering);

    //if (m_rest) return;

    DisplayObject::m_mat = transformMat();
    Render::SceneManager::GetInstance().moveObject(static_cast<StaticObject*>(this));

    for (size_t i = 0; i < m_suspension.size(); i++)
    {
        float speed = m_suspension[i]->wheelSpeed();
        m_wheelParams[i].dang = fabs(speed) > math::eps ? speed : m_wheelParams[i].dang * pow(0.8f, dt);
        m_wheelParams[i].ang += m_wheelParams[i].dang * dt;
        m_wheelParams[i].ang = std::remainderf(m_wheelParams[i].ang, math::pi2);

        if (fabs(m_wheelParams[i].dang) < math::eps) m_wheelParams[i].dang = 0.0f;

        const mat3& orientation = m_suspension[i]->orientation();
        mat3 wheelRot = orientation * mat3::RotateX(m_wheelParams[i].ang);
        vec3 pos = m_wheelPos[i] - orientation[1] * m_suspension[i]->suspensionDist();

        mat4 transform = mat4(wheelRot, DisplayObject::m_mat * pos);
        m_wheels[i]->setMat(transform);
        Render::SceneManager::GetInstance().moveObject(m_wheels[i]);
    }
}

}// namespace GameLogic