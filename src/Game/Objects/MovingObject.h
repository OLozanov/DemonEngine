#pragma once

#include "Game/GameObject.h"
#include "Render/SceneManager.h"
#include "Render/StaticObject.h"
#include "Physics/PhysicsManager.h"

#include "Resources/Resources.h"

namespace GameLogic
{

class MovingObject : public GameObject
                   , public Render::StaticObject
                   , public Physics::StationaryBody
{
    enum class State
    {
        NotActivated,
        Moving,
        Stopped,
    };

private:
    State m_state;

    std::string m_modelname;
    std::string m_soundname;

    ResourcePtr<Sound> m_sound;

    vec3 m_pos;
    mat3 m_orientation;
    vec3 m_baseBBoxPos;
    vec3 m_bboxPos;

    vec3 m_dir;
    float m_movedist;
    float m_speed;

    float m_dist;

    void init();

public:
    MovingObject(const std::string& id, const vec3& pos, const mat3& mat, const vec3& dir, float dist, float speed,
                const std::string& modelname, const std::string& sound);
    MovingObject(FileStream & file);

    ~MovingObject();

    void update(float dt) override;
    void activate() override;

    //uint32_t type() override { return object_moving; }
    void save(FileStream & file) override;
};

} // namespace gamelogic
