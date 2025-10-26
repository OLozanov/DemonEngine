#include "Item.h"
#include "Game/Constants.h"

namespace GameLogic
{

Item::Item(const vec3& pos, const mat3& mat, float triggerSize, float mass, Model* model)
: PhysicsObject(pos, mat, mass, collision_solid | collision_hitable, model)
, Pickable(triggerSize, pos)
{
}

void Item::update(float dt)
{
    PhysicsObject::update(dt);
    Pickable::m_pos = Physics::RigidBody::m_pos;
}

} // namespace gamelogic