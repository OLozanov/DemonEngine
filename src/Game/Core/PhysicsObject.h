#pragma once

#include "Game/GameObject.h"
#include "Render/StaticObject.h"
#include "Physics/RigidBody.h"

#include "Resources/Resources.h"

#include <string>

namespace GameLogic
{

class PhysicsObject : public GameObject
                    , public Hitable
                    , public Physics::RigidBody
                    , public Render::StaticObject
{
protected:
    SoundPtr m_bumpSnd;
    float m_bumpImpulse;

    //std::unique_ptr<BuoyancyGenerator> m_buoyancy;

public:

    PhysicsObject(const vec3& pos, const mat3& mat, float mass, unsigned long layers, Model* model);
    ~PhysicsObject() {}

    void update(float dt) override;
    void onCollide(const vec3& normal, float impulse) override;

    void remove();

    void save(FileStream& stream) override;
    void restore(FileStream& stream);

private:

    const vec3& center() const override { return m_pos; }
    void hit(const vec3& point, const vec3& direction, uint32_t power) override;

    static float RestVelocity(const BBox& bbox);
};

} // namespace gamelogic