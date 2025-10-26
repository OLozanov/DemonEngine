#pragma once

#include "Game/Weapon.h"
#include "Render/AnimatedObject.h"
#include "Render/StaticObject.h"
#include "Resources/Resources.h"

namespace GameLogic
{

class Pistols : public Weapon
{
public:
	Pistols();

	void equip() override;
	void unequip() override;

	void fire() override;
	void stopFire() override;

	void update(float dt) override;

private:

	enum class State
	{
		IdleRight,
		FireRight,
		IdleLeft,
		FireLeft
	};

	Render::AnimatedObject m_right;
	Render::AnimatedObject m_left;

	SoundPtr m_shotSound;

	State m_state;
	bool m_pending;
	bool m_fire;

	static constexpr float AnimSpeed = 0.035;
};

} // namespace gamelogic
