#pragma once

#include "Game/Core/PhysicsObject.h"

namespace GameLogic
{

class Item : public PhysicsObject
           , public Pickable
{
public:
    Item(const vec3& pos, const mat3& mat, float triggerSize, float mass, Model* model);

    void update(float dt) override;
};

} // namespace gamelogic