#include "ParticleSystem.h"

namespace Render
{

ParticleSystem::ParticleSystem(const vec3& pos, const mat3& mat, size_t num)
{
    m_mat = mat4::Translate(pos) * mat4(mat);

    m_buffer.resize(num);

    m_displayElement.material = nullptr;
    m_displayElement.offset = 0;
    m_displayElement.vertexnum = 0;

    m_displayData.emplace_back(DisplayBlock::display_sprite,
                               &m_mat,
                               m_buffer,
                               &m_displayElement);
}

void ParticleSystem::addParticle(const SpriteData& particle)
{
    if (m_ptr == m_buffer.size()) return;

    m_buffer[m_ptr++] = particle;
    m_displayElement.vertexnum = m_ptr;
}

void ParticleSystem::resize(size_t num)
{
    m_buffer.resize(num);
}

void ParticleSystem::clearParticles()
{
    m_ptr = 0;
    m_displayElement.vertexnum = 0;
}

} //namespace render