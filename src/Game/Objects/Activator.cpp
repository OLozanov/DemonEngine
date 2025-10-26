#include "Activator.h"

#include "Collision/BoxCollisionShape.h"
#include "Game/Constants.h"

#include "System/AudioManager.h"

#include "Utils/FileStream.h"

namespace GameLogic
{

Activator::Activator(uint32_t type, 
                     const vec3& pos, const mat3& rot, 
                     Model* model, 
                     Sound* switchSound,
                     bool switchedOn,
                     const Render::AnimationRange& switchOnAnim,
                     const Render::AnimationRange& switchOffAnim,
                     bool useOnce, bool collision)
: AnimatedObject(pos, rot, model)
, m_type(type)
, m_pos(pos)
, m_orientation(rot)
, m_switchOnAnim(switchOnAnim)
, m_switchOffAnim(switchOffAnim)
, m_useOnce(useOnce)
, m_collision(collision)
, m_state(switchedOn ? Activator::State::SwitchedOn : Activator::State::SwitchedOff)
, m_used(false)
, m_switchSound(switchSound)
{
    setRepeat(false);
    if (switchedOn) setFrame(switchOnAnim.end);

    Render::SceneManager& sm = Render::SceneManager::GetInstance();

    sm.addObject(static_cast<DisplayObject*>(this));

    StationaryBody::m_bbox = RecalcBBox(DisplayObject::m_bbox, m_orientation);

    vec3 mid = (DisplayObject::m_bbox.max + DisplayObject::m_bbox.min) * 0.5;
    vec3 colbbox = DisplayObject::m_bbox.max - mid;

    m_collisionShape = new Collision::BoxCollisionShape(m_orientation, m_pos, colbbox);

    m_layers = collision_pickable;
    if (collision) m_layers |= collision_solid | collision_actor;

    Physics::PhysicsManager::GetInstance().addStationaryBody(static_cast<StationaryBody*>(this));

    m_object = static_cast<Usable*>(this);
}

Activator::~Activator()
{
}

void Activator::use()
{
    if (m_useOnce && m_used) return;

    switch (m_state)
    {
    case State::SwitchedOff:
    {
        m_state = State::SwitchingOn;
        m_used = true;

        setAnimRange(m_switchOnAnim);

        if (m_switchSound) AudioManager::Play(m_switchSound, m_pos);
    }
    break;
    case State::SwitchedOn:
    {
        m_state = State::SwitchingOff;
        m_used = true;

        setAnimRange(m_switchOffAnim);

        if (m_switchSound) AudioManager::Play(m_switchSound, m_pos);
    }
    break;
    default:
        return;
    }

    run();
}

void Activator::update(float dt)
{
    animate(dt);

    if (m_state == State::SwitchingOn && !isRunning())
    {
        m_state = State::SwitchedOn;
        OnSwitch(true);
    }
        
    if (m_state == State::SwitchingOff && !isRunning())
    {
        m_state = State::SwitchedOff;
        OnSwitch(false);
    }
}

uint32_t Activator::type()
{
    return m_type; 
}

void Activator::save(FileStream& file)
{
    file << m_pos;
    file << m_orientation;

    file << m_useOnce;
    file << m_collision;

    file << isRunning();
    file << getAnimTime();

    file << m_state;
    file << m_used;
}

} // namespace gamelogic