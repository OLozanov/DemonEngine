#include "Animated.h"

#include "Resources/Resources.h"
#include "Collision/BoxCollisionShape.h"
#include "Game/Constants.h"

#include "Utils/FileStream.h"

namespace GameLogic
{

Animated::Animated(const vec3& pos, const mat3& rot, const std::string& model, bool running, bool loop, bool collision)
: AnimatedObject(pos, rot, ResourceManager::GetModel(model))
, m_model(model)
, m_pos(pos)
, m_orientation(rot)
, m_collision(collision)
{
    if (running) run();
    if (loop) setRepeat(true);

    if (collision)
    {
        Render::SceneManager& sm = Render::SceneManager::GetInstance();

        sm.addObject(static_cast<DisplayObject*>(this));

        StationaryBody::m_bbox = RecalcBBox(DisplayObject::m_bbox, m_orientation);

        vec3 mid = (DisplayObject::m_bbox.max + DisplayObject::m_bbox.min) * 0.5;
        vec3 colbbox = DisplayObject::m_bbox.max - mid;

        m_collisionShape = new Collision::BoxCollisionShape(m_orientation, m_pos, colbbox);

        m_layers = collision_solid | collision_actor;

        Physics::PhysicsManager::GetInstance().addStationaryBody(static_cast<StationaryBody*>(this));

        m_object = static_cast<GameObject*>(this);
    }
}

Animated::~Animated()
{
}

void Animated::activate()
{
    if (isRunning()) stop();
    else run();
}

void Animated::update(float dt)
{
    animate(dt);
}

void Animated::save(FileStream& file)
{
    file << m_model;

    file << m_pos;
    file << m_orientation;

    file << m_collision;

    file << isRunning();
    file << isRepeated();
    file << getAnimTime();
}

} // namespace gamelogic