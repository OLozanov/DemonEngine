#pragma once

#include "Game/Constants.h"
#include "Game/Core/PhysicsObject.h"
#include "Utils/EventHandler.h"

class FileStream;

namespace GameLogic
{

class Debris : public PhysicsObject
{
public:
    using OnLifetimeExpiresEvent = Event<void (Debris*)>;

private:
    uint32_t m_type;
    float m_lifetime;

public:

    Debris(const vec3& pos, const mat3& rot, float lifetime, uint32_t type);
    ~Debris();

    void update(float dt) override;

    uint32_t type() override { return object_debris; }
    void save(FileStream & stream) override;
    void restore(FileStream & stream);

    OnLifetimeExpiresEvent OnLifetimeExpires;
};

} // namespace gamelogic