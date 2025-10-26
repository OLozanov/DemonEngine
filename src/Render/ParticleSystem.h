#pragma once

#include "Render/DisplayObject.h"
#include "Render/Render.h"

namespace Render
{

class ParticleSystem : public DisplayObject
{
private:
    VertexArray<SpriteData> m_buffer;
    size_t m_ptr = 0;

    DisplayData m_displayElement;

protected:
    void addParticle(const SpriteData& particle);
    void resize(size_t num);
    void clearParticles();

public:
    ParticleSystem() {}
    ParticleSystem(const vec3& pos, const mat3& mat, size_t num);

    ~ParticleSystem() {}
};

} //namespace render