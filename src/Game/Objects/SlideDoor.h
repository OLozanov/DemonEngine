#pragma once

#include "Render/SceneManager.h"

#include "Game/GameObject.h"
#include "Render/StaticObject.h"
#include "Physics/PhysicsManager.h"
#include "Resources/Resources.h"
#include "System/Timer.h"

namespace GameLogic
{

enum class LockType
{
    Unlocked = 0,
    Locked = 1,
    Red = 2,
    Yellow = 3,
    Blue = 4,
};

class SlideDoor : public GameObject
                , public Render::StaticObject
				, public Physics::StationaryBody
{
	enum class State
	{
		Closing,
		Closed,
		Opening,
		Opened
	};

public:

	SlideDoor(const std::string& id, const vec3& pos, const mat3& mat, bool closed = false, bool gi = false);

	void open();
	void close();

	void release();

	void activate() override;
	void update(float dt) override;

private:
	void updateTransform();

private:
	const vec3 m_pos;
	const mat3 m_mat;
	vec3 m_baseBBoxPos;
	vec3 m_bboxPos;
	float m_distance;
	float m_slideSpeed;

	State m_state;
	float m_slidePos;

	Timer m_timer;

	SoundPtr m_slideSound;

	bool m_closed;

	bool m_gi;
	Render::RaytraceId m_rtInstance;

	static constexpr float CloseTime = 2.0f;
};

} // namespace gamelogic