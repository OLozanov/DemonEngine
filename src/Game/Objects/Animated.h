#pragma once

#include "Game/GameObject.h"
#include "Render/SceneManager.h"
#include "Render/AnimatedObject.h"
#include "Physics/PhysicsManager.h"

namespace GameLogic
{

class Animated : public GameObject
               , public Render::AnimatedObject
               , public Physics::StationaryBody
{
public:
    Animated(const vec3& pos, const mat3& rot, const std::string& model, bool running = true, bool loop = true, bool collision = true);
    ~Animated();

    void activate() override;
    void update(float dt) override;

    void save(FileStream& file) override;

private:
    std::string m_model;

    vec3 m_pos;
    mat3 m_orientation;

    bool m_collision;
};

} // namespace gamelogic