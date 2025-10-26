#pragma once

#include "Game/GameObject.h"
#include "Utils/EventHandler.h"
#include "Resources/Resources.h"

namespace GameLogic
{

class Explosion : public GameObject
{
public:
    using OnStopEvent = Event<void(Explosion*)>;

private:
    vec3 m_pos;
    float m_power;

    float m_lifetime;

    Render::SpriteData m_sprite;
    ResourcePtr<Image> m_image;

public:
    Explosion(const vec3& pos, float power);

    void update(float dt) override;

    OnStopEvent OnStop;
};

} // namespace gamelogic