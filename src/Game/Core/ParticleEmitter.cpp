#include "ParticleEmitter.h"
#include "Render/SceneManager.h"

#include <random>

namespace GameLogic
{

ParticleEmitter::ParticleEmitter(const vec3& pos, const mat3& mat, float speed,
                                 float size, float expand, float lifetime,
                                 size_t particleNum, Image* image)
: ParticleSystem(pos, mat, particleNum)
, m_size(size)
, m_speed(speed)
, m_expandSpeed(expand)
, m_lifetime(lifetime)
, m_num(particleNum)
, m_image(image)
, m_active(true)
{
    m_particles.resize(m_num);

    std::mt19937 randomGenerator;
    std::uniform_real_distribution<float> distribution(-0.1f, 0.1f);

    randomGenerator.seed(1729);

    for (size_t i = 0; i < m_num; i++)
    {
        m_particles[i].pos = {};
        m_particles[i].velocity = vec3(distribution(randomGenerator), 1, distribution(randomGenerator)).normalized();
        m_particles[i].size = m_size;
        m_particles[i].lifetime = -(m_lifetime / m_num) * (i + 1);
    }

    calculateBBox();

    Render::SceneManager& sm = Render::SceneManager::GetInstance();
    sm.addObject(static_cast<DisplayObject*>(this));
}

void ParticleEmitter::calculateBBox()
{
    float width = m_size + m_lifetime * m_expandSpeed + 0.1;
    
    m_bbox.min = { -width, -m_size, -width };
    m_bbox.max = { width, m_size + m_lifetime * m_expandSpeed, width };
}

void ParticleEmitter::activate()
{
    m_active = !m_active;
}

void ParticleEmitter::update(float dt)
{
    ParticleSystem::clearParticles();

    for (size_t i = 0; i < m_num; i++)
    {
        Particle& particle = m_particles[i];

        if (particle.lifetime < 0.0f)
        {
            if (m_active) particle.lifetime += dt;
            continue;
        }

        if (particle.lifetime < m_lifetime)
        {
            particle.pos += particle.velocity * m_speed * dt;
            particle.size += m_expandSpeed * dt;
            particle.lifetime += dt;
        }
        else
        {
            particle.pos = {};
            particle.size = m_size;
            particle.lifetime = m_active ? 0.0f : -(m_lifetime / m_num) * (i + 1);

            if (!m_active) continue;
        }

        constexpr float fade_start = 0.6f;
        float fade = std::max(0.0f, particle.lifetime / m_lifetime - fade_start) / (1.0f - fade_start);
        vec4 color = { 1.0f, 1.0f, 1.0f, (1.0f - fade) * 0.8f };

        ParticleSystem::addParticle({ m_mat * particle.pos, particle.size, color, m_image->handle });
    }
}

} // namespace gamelogic