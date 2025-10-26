#pragma once

#include "Game/GameObject.h"
#include "Render/ParticleSystem.h"
#include "Utils/EventHandler.h"

namespace GameLogic
{

class ParticleEmitter : public GameObject
                      , public Render::ParticleSystem
{
public:
    ParticleEmitter(const vec3& pos, const mat3& mat, float speed, 
                    float size, float expand, float lifetime,
                    size_t particleNum, Image* image);

    void activate() override;
    void update(float dt) override;

private:
    void calculateBBox();

private:

    struct Particle
    {
        vec3 pos;
        vec3 velocity;
        float size;
        float lifetime;
    };

    bool m_active;
   
    size_t m_num;

    ResourcePtr<Image> m_image;

    float m_size;
    float m_speed;
    float m_expandSpeed;
    float m_lifetime;

    std::vector<Particle> m_particles;
};

} // namespace gamelogic