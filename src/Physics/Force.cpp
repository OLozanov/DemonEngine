#include "Force.h"
#include "Physics/PhysicsManager.h"

namespace Physics
{

ForceGenerator::~ForceGenerator()
{
    if(m_ref) PhysicsManager::GetInstance().removeForce(m_ref);
}

Force::Force(ForceGenerator& gen, RigidBody& rbody)
: generator(gen)
, body(rbody)
{
    generator.setReference(this);
}

Force::~Force()
{
    generator.setReference(nullptr);
}

} //namespace Physics