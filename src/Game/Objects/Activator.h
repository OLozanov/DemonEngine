#pragma once

#include "Game/GameObject.h"
#include "Render/SceneManager.h"
#include "Render/AnimatedObject.h"
#include "Physics/PhysicsManager.h"
#include "Resources/Resources.h"

namespace GameLogic
{

// Animated switches, etc.
class Activator : public GameObject
                , public Usable
                , public Render::AnimatedObject
                , public Physics::StationaryBody
{
public:
    using OnSwitchEvent = Event<void(bool switchOn)>;

    enum class State : uint8_t
    {
        SwitchedOff,
        SwitchingOn,
        SwitchedOn,
        SwitchingOff
    };

public:
    Activator(uint32_t type, 
              const vec3& pos, const mat3& rot, 
              Model* model, 
              Sound* switchSound,
              bool switchedOn, 
              const Render::AnimationRange& switchOnAnim,
              const Render::AnimationRange& switchOffAnim,
              bool useOnce = true, bool collision = true);
    ~Activator();

    void use() override;
    void update(float dt) override;

    uint32_t type() override;

    void save(FileStream& file) override;

    OnSwitchEvent OnSwitch;

private:
    uint32_t m_type;

    vec3 m_pos;
    mat3 m_orientation;

    Render::AnimationRange m_switchOnAnim;
    Render::AnimationRange m_switchOffAnim;

    bool m_useOnce;
    bool m_collision;

    State m_state;
    bool m_used;

    SoundPtr m_switchSound;
};

} // namespace gamelogic#pragma once
