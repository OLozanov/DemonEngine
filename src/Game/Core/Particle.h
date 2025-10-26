#pragma once

#include "Game/GameObject.h"
#include "Render/DisplayData.h"
#include "Utils/EventHandler.h"

namespace GameLogic
{

class Particle : public GameObject
{
public:
    using OnLifetimeExpiresEvent = Event<void(Particle*)>;

    OnLifetimeExpiresEvent OnLifetimeExpires;

public:

    Particle(const vec3& pos, float size, 
             const vec3& velocity, float expand,
             float lifetime, 
             Image* image);

    void update(float dt) override;

private:

    vec3 m_pos;
    float m_size;   
    ResourcePtr<Image> m_image;

    vec3 m_velocity;
    float m_expandSpeed;

    float m_lifetime;
    float m_time;
    Render::SpriteData m_sprite;
};

} // namespace gamelogic