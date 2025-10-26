#include "Winch.h"

#include "Game/Constants.h"
#include "Resources/Resources.h"
#include "Collision/BoxCollisionShape.h"
#include "Utils/FileStream.h"

namespace GameLogic
{

Winch::Winch(const vec3& pos, const mat3& rot, const std::string& targetId)
: StaticObject(ResourceManager::GetModel("winch01.msh"), true)
, m_pos(pos)
, m_orientation(rot)
, m_state(State::NotActivated)
, m_angle(0)
, m_targetId(targetId)
{
    Init();
}

Winch::Winch(FileStream & file)
{
    file >> m_pos;

    file >> m_orientation[0];
    file >> m_orientation[1];

    m_orientation[2] = m_orientation[0] ^ m_orientation[1];
    
    file >> m_angle;

    file >> m_state;
    file >> m_targetId;

    Init();

    StaticObject::m_mat = m_baseMat * mat4::RotateX(m_angle / 180.0 * math::pi);
}

Winch::~Winch()
{
}

void Winch::Init()
{
    Render::SceneManager& sm = Render::SceneManager::GetInstance();

    StaticObject::m_mat = mat4::Translate(m_pos) * m_orientation;

    sm.addObject(static_cast<DisplayObject*>(this));

    m_baseMat = mat4::Translate(m_pos) * m_orientation;
    StaticObject::m_mat = m_baseMat;

    StationaryBody::m_bbox = RecalcBBox(StaticObject::m_bbox, m_orientation);

    vec3 mid = (StaticObject::m_bbox.max + StaticObject::m_bbox.min)*0.5;
    vec3 colbbox = StaticObject::m_bbox.max - mid;
    m_bboxPos = m_pos + m_orientation * mid;

    m_collisionShape = new Collision::BoxCollisionShape(m_orientation, m_pos, colbbox);

    m_layers = collision_solid | collision_actor | collision_pickable;

    Physics::PhysicsManager::GetInstance().addStationaryBody(static_cast<StationaryBody*>(this));

    m_object = static_cast<GameObject*>(this);
}

void Winch::update(float dt)
{
    if(m_state == State::Turning)
    {
        m_angle += RotSpeed*dt;

        if(m_angle >= RotAngle)
        {
            m_state = State::Activated;
            m_angle = RotAngle;
            OnActivated(m_targetId);
        }

        StaticObject::m_mat = m_baseMat * mat4::RotateX(m_angle / 180.0 * math::pi);
    }
}

void Winch::Activate()
{
    if(m_state == State::NotActivated) m_state = State::Turning;
}

void Winch::pick()
{
    Activate();
}

void Winch::save(FileStream & file)
{
    file << m_pos;
    
    file << m_orientation[0];
    file << m_orientation[1];

    file << m_angle;

    file << m_state;
    file << m_targetId;
}

} // namespace gamelogic
