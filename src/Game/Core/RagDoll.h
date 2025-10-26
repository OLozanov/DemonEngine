#pragma once

#include "Game/GameObject.h"
#include "Render/ArticulatedObject.h"
#include "Physics/PhysicsManager.h"

namespace GameLogic
{

class RagDollBody;
class RagDollBone;

class RagDoll : public GameObject
              , public Render::ArticulatedObject
{
public:

    RagDoll(const vec3& pos, float animTime, Model* model);
    ~RagDoll();

    void remove();
    void update(float dt) override;

private:

    size_t initBone(size_t b, const mat4& pmat);

    vec3 m_pos;

    RagDollBody* m_body;
    std::vector<RagDollBone*> m_bones;
    std::vector<Physics::Constraint*> m_constraints;
};

class RagDollBody : public Physics::RigidBody
                  , public Hitable
{
public:
    RagDollBody(const vec3& pos, const mat3& orientation, float mass, Model* model);

    const vec3& pos() const { return m_pos + m_center; }
    const vec3& center() const { return m_center; }

private:
    void onCollide(const vec3& normal, float impulse) override;
    void hit(const vec3& point, const vec3& direction, uint32_t power) override;

private:
    vec3 m_center;
};

class RagDollBone : public Physics::RigidBody
                  , public Hitable
{
public:
    RagDollBone(size_t boneid, const vec3& pos, const mat3& orientation, float length, float width);

    size_t boneid() { return m_boneid; }

    const vec3& head() { return m_head; }
    const vec3& tail() { return m_tail; }
    float length() { return m_length; }

private:
    void onCollide(const vec3& normal, float impulse) override;
    void hit(const vec3& point, const vec3& direction, uint32_t power) override;

private:
    size_t m_boneid;

    vec3 m_head;
    vec3 m_tail;
    float m_length;

};

} // namespace GameLogic