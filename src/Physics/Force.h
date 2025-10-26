#pragma once

#include "Utils/LinkedList.h"

namespace Physics
{

class RigidBody;
class Force;

class ForceGenerator
{
    Force * m_ref;

    void setReference(Force * force) { m_ref = force; }

public:

    ForceGenerator() : m_ref(nullptr) {}
    virtual ~ForceGenerator();

    virtual void update(RigidBody& body, float dt) = 0;

    friend class Force;
};

struct Force : public ListNode<Force>
{
    ForceGenerator& generator;
    RigidBody& body;

    Force(ForceGenerator& gen, RigidBody& rbody);
    ~Force();
};

} //namespace Physics
