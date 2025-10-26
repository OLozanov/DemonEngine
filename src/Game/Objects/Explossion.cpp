#include "Explosion.h"
#include "Render/SceneManager.h"

namespace GameLogic
{
    
Explosion::Explosion(const vec3& pos, float power)
: m_pos(pos)
, m_power(power)
, m_lifetime(0.1f)
, m_image(ResourceManager::GetImage("Effects/explosion.dds"))
{
    m_sprite.position = m_pos;
    m_sprite.color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    m_sprite.size = 0.5f;
    m_sprite.image = m_image->handle;
}

void Explosion::update(float dt)
{
    if (m_lifetime > dt)
    {
        Render::SceneManager::GetInstance().addSprite(m_sprite);

        m_sprite.size += 5.0f * dt;

        m_lifetime -= dt;

    } else OnStop(this);
}

} // namespace gamelogic