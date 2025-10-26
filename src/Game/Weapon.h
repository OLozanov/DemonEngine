#pragma once

#include "Utils/EventHandler.h"

namespace GameLogic
{

class Weapon
{
public:
	using OnAttackEvent = Event<void()>;

	virtual ~Weapon() {}

	virtual void equip() = 0;
	virtual void unequip() = 0;

	virtual void fire() = 0;
	virtual void stopFire() = 0;

	virtual void update(float dt) = 0;

	OnAttackEvent onAttack;
};

} // namespace gamelogic