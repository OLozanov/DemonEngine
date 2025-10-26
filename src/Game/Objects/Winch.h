#pragma once

#include <string>

#include "Game/GameObject.h"
#include "Render/SceneManager.h"
#include "Render/StaticObject.h"
#include "Physics/PhysicsManager.h"

#include "Utils/EventHandler.h"

namespace GameLogic
{

class Winch : public GameObject
            , public Render::StaticObject
            , public Physics::StationaryBody
{
public:
    using OnActivatedEvent = Event<void (const std::string&)>;

private:
    enum class State
    {
        NotActivated,
        Turning,
        Activated,
    };

private:
    std::string m_targetId;

    vec3 m_pos;
    mat3 m_orientation;

    mat4 m_baseMat;
    vec3 m_bboxPos;

    State m_state;
    float m_angle;

    static constexpr float RotSpeed = 120;
    static constexpr float RotAngle = 360;

    void Init();

public:
    Winch(const vec3& pos, const mat3& rot, const std::string& targetId);
    Winch(FileStream & file);
    ~Winch();

    void update(float dt) override;
    void Activate();

    void pick();// override;

    //uint32_t type() override { return object_winch; }
    void save(FileStream & file) override;

    OnActivatedEvent OnActivated;
};

} // namespace gamelogic
