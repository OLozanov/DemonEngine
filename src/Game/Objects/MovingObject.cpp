#include "MovingObject.h"

#include "System/AudioManager.h"

#include "Collision/BoxCollisionShape.h"
#include "Collision/PolygonalCollisionShape.h"

#include "Utils/FileStream.h"

#include "Game/Constants.h"

namespace GameLogic
{

MovingObject::MovingObject(const std::string& id, const vec3& pos, const mat3& mat, const vec3& dir, float dist, float speed,
                            const std::string& modelname, const std::string& sound)
: GameObject(id)
, StaticObject(ResourceManager::GetModel(modelname.c_str()), true)
, m_state(State::NotActivated)
, m_pos(pos)
, m_orientation(mat)
, m_modelname(modelname)
, m_soundname(sound)
, m_dir(dir)
, m_movedist(dist)
, m_speed(speed)
, m_dist(0)
{
    if (!m_soundname.empty()) m_sound = ResourceManager::GetSound(m_soundname);

    init();
}

MovingObject::MovingObject(FileStream & file)
{
    GameObject::restore(file);

    file >> m_orientation[0];
    file >> m_orientation[1];

    m_orientation[2] = m_orientation[0] ^ m_orientation[1];

    file >> m_modelname;
    file >> m_soundname;

    file >> m_state;

    file >> m_dir;
    file >> m_movedist;
    file >> m_speed;
    file >> m_dist;

    init();

    //StaticObject::m_mat = m_baseMat*mat4::Translate(m_dir*m_dist);
    //m_pos = m_basePos + m_dir*m_dist;
}

MovingObject::~MovingObject()
{
}

void MovingObject::init()
{
    Render::SceneManager& sm = Render::SceneManager::GetInstance();

    StaticObject::m_mat = mat4::Translate(m_pos) * m_orientation;

    sm.addObject(static_cast<DisplayObject*>(this));

    BBox bbox = RecalcBBox(m_model->bbox(), m_orientation);

    Collision::CollisionShape* collisionShape;

    const auto& collisionData = m_model->collisionData();
    if (!collisionData.empty()) collisionShape = new Collision::PolygonalCollisionShape(m_orientation, m_pos, collisionData.size(), collisionData.data());
    else
    {
        const BBox& modelBBox = m_model->bbox();

        vec3 mid = (modelBBox.max + modelBBox.min) * 0.5;
        vec3 colbbox = modelBBox.max - mid;
        m_baseBBoxPos = m_pos + m_orientation * mid;
        m_bboxPos = m_baseBBoxPos;

        collisionShape = new Collision::BoxCollisionShape(m_orientation, m_bboxPos, colbbox);

        bbox = RecalcBBox({ -colbbox, colbbox }, m_orientation);
    }

    StationaryBody::m_bbox = bbox;
    m_collisionShape = collisionShape;
    m_layers = collision_solid | collision_actor | collision_hitable;

    Physics::PhysicsManager::GetInstance().addStationaryBody(static_cast<StationaryBody*>(this));
}

void MovingObject::update(float dt)
{
    if(m_state == State::Moving)
    {
        m_dist += m_speed*dt;

        if(m_dist >= m_movedist)
        {
            m_state = State::Stopped;
            m_dist = m_movedist;
        }
   
        vec3 pos = m_pos + m_dir * m_dist;
        StaticObject::m_mat = mat4::Translate(pos) * mat4(m_orientation);

        m_bboxPos = m_baseBBoxPos + m_dir * m_dist;
    }
}

void MovingObject::activate()
{
    if (m_state == State::NotActivated)
    {
        m_state = State::Moving;
        if (m_sound) AudioManager::Play(m_sound, m_pos);
    }
}

void MovingObject::save(FileStream & file)
{
    GameObject::save(file);

    file << m_orientation[0];
    file << m_orientation[1];

    m_orientation[2] = m_orientation[0] ^ m_orientation[1];

    file << m_modelname;
    file << m_soundname;

    file << m_state;

    file << m_dir;
    file << m_movedist;
    file << m_speed;
    file << m_dist;
}

} // namespace gamelogic
