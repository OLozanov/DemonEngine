#include "Particle.h"
#include "Render/SceneManager.h"

namespace GameLogic
{

Particle::Particle(const vec3& pos, float size,
                   const vec3& velocity, float expand,
                   float lifetime, 
                   Image* image)
: m_pos(pos)
, m_size(size)
, m_velocity(velocity)
, m_expandSpeed(expand)
, m_lifetime(lifetime)
, m_time(lifetime)
, m_image(image)
{
    m_sprite.position = m_pos;
    m_sprite.size = m_size;
    m_sprite.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_sprite.image = image->handle;

    m_expandSpeed = 0.05;
}

void Particle::update(float dt)
{
    if (m_time > 0)
    {
        m_sprite.position += m_velocity * dt;
        m_sprite.size += m_expandSpeed * dt;

        constexpr float fade_start = 0.5f;
        float fade = std::max(0.0f, m_time / m_lifetime - fade_start) / (1.0f - fade_start);
        m_sprite.color.w = fade;

        Render::SceneManager::GetInstance().addSprite(m_sprite);

        m_time -= dt;
    }
    else OnLifetimeExpires(this);
}

} // namespace gamelogic