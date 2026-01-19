#include "Character.h"

#include "Collision/BoxCollisionShape.h"
#include "Render/SceneManager.h"

#include "System/AudioManager.h"

#include "Game/Constants.h"

namespace GameLogic
{

constexpr vec3 cbbox = { 0.2, 0.5, 0.2 };

Character::Character(const vec3& pos, float ang, const CharacterParams& params)
: Actor(params.bbox, pos, params.mass, params.health)
, ArticulatedObject(ResourceManager::GetModel(params.model))
, m_staticCollision(new Collision::BoxCollisionShape(m_orientation, m_pos, Actor::m_bbox))
, m_staticBody(m_staticCollision, { -Actor::m_bbox, Actor::m_bbox }, collision_actor)
, m_ang(ang)
, m_timer(0)
, m_state(State::Idle)
, m_damage(params.damage)
, m_attackDistance(params.attackDistance)
, m_attackTime(params.attackTime)
, m_speed(params.speed)
, m_hit(false)
, m_distribution(0.0, 1.0)
, m_weaponBone(params.weaponBone)
, m_weaponTransform(mat4::Translate(params.weaponPos) * 
					mat4::Rotate(params.weaponRot.x, params.weaponRot.y, params.weaponRot.z))
{
	m_layers = collision_hitable | collision_character;
	m_object = static_cast<Hitable*>(this);

    Physics::PhysicsManager::GetInstance().addRigidBody(this);
    Render::SceneManager::GetInstance().addObject(this);
    Render::SceneManager::GetInstance().registerSkeletalObject(this);

	Physics::PhysicsManager::GetInstance().addStationaryBody(&m_staticBody);

    m_animations = { params.idleAnim,
                     params.moveAnim,
                     params.attackAnim,
                     params.deathAnim };

    m_pos = pos;
	m_mat = mat4::Translate(m_pos) * mat4::RotateY(m_ang);

	setAnimation(m_animations[0]);
	setAnimTime(m_animations[0].start);

	m_weapon = std::make_unique<Render::StaticObject>(ResourceManager::GetModel("Weapon/pistol.msh"), true);
	m_weapon->hideSubmesh(1);

	m_shotSound = ResourceManager::GetSound("Weapon/pistol.wav");

	Render::SceneManager::GetInstance().addObject(m_weapon.get());

	m_randomGenerator.seed(1729);
}

Character::~Character()
{
	if (m_weapon) Render::SceneManager::GetInstance().removeObject(m_weapon.get());
}

void Character::setAnimation(const AnimSet& anim)
{
    setAnimRange(anim.start / 5, anim.stop / 5);
    setFrame(anim.start / 5);
}

void Character::onDeath(uint32_t damage, const vec3& impulse)
{
	//setAnimation(m_animations[State::Dead]);
	//setRepeat(false);
	//run();

	stop();

	m_weapon->hideSubmesh(1, true);

	m_state = State::Dead;

	Physics::PhysicsManager::GetInstance().removeRigidBody(this);
	Physics::PhysicsManager::GetInstance().removeStationaryBody(&m_staticBody);

	OnDeath(this, impulse);
}

bool Character::targetVisibility()
{
	if (!m_target) return false;

	vec3 dir = m_target->pos() - m_pos;
	float dist = dir.normalize();

	Collision::TraceRayInfo tinfo;

	if (Physics::PhysicsManager::GetInstance().traceRay(m_pos, dir, collision_solid | collision_target, tinfo, dist))
		if (tinfo.object == m_target) return true;

	return false;
}

void Character::onCollide(const vec3& normal, float impulse)
{
	if (fabs(impulse) > 50.0f)
	{
		onDeath(m_health, normal * impulse * 10.0f);
		m_health = 0;
	}
}

void Character::update(float dt)
{
	if (m_state == State::Dead) return;

    if (m_state != State::Idle) animate(dt);

	m_mat = mat4::Translate(m_pos + vec3{ 0, 0.2, 0 }) * mat4::RotateY(m_ang);

	if (m_weapon)
	{
		const mat4& handTransform = getBoneTransform(m_weaponBone);
		mat4 weaponTransform = m_omat[0] * handTransform * m_weaponTransform;

		m_weapon->setMat(weaponTransform);
		Render::SceneManager::GetInstance().moveObject(m_weapon.get());
	}

	bool attack = false;

	if (m_state == State::Attack && targetVisibility())
	{
		if (m_timer > dt) m_timer -= dt;
		else startAnimation();

		//Hit target
		if ((fabs(getAnimTime() - m_attackTime) < 0.1))
		{
			if (!m_hit) attack = true;
			m_hit = true;

			if (m_weapon) m_weapon->hideSubmesh(1, false);
		}
		else
		{
			m_hit = false;
			if (m_weapon) m_weapon->hideSubmesh(1, true);
		}

		vec3 dir = m_target->pos() - m_pos;
		float dist = dir.length();

		dir.y = 0;
		dir.normalize();

		m_ang = atan2(-dir.z, dir.x) + math::pi * 0.5;
	}

	if (m_target && m_health > 0)
	{
		float dist = (m_pos - m_target->location()).length();

		if (dist < m_attackDistance + 0.05)
		{
			if (m_state != State::Attack)
			{
				m_state = State::Attack;
				setAnimation(m_animations[State::Attack]);
				setRepeat(false);
			}
		}
		else if (dist <= 10)
		{
			if (m_state != State::Move && targetVisibility())
			{
				m_state = State::Move;
				setAnimation(m_animations[State::Move]);
				setRepeat(true);
				run();
			}
		}
		else
		{
			if (m_state != State::Idle)
			{
				m_state = State::Idle;
				setAnimation(m_animations[State::Idle]);
				setRepeat(true);
				run();
			}
		}

		if ((m_pos.y - m_target->location().y) > 2.5f)
		{
			if (m_state != State::Idle)
			{
				m_state = State::Idle;
				setAnimation(m_animations[State::Idle]);
			}
		}

		if ((dist < m_attackDistance + 0.05) && attack)
		{
			float rnum = m_distribution(m_randomGenerator);

			if (rnum > 0.5) m_target->damage(m_damage);
			AudioManager::Play(m_shotSound, m_pos);

			m_timer = 0.8;
		}

		if (m_target->health() == 0)
		{
			m_target = nullptr;

			m_state = State::Idle;
			setAnimation(m_animations[State::Idle]);

			return;
		}
	}

    if (m_target && m_state == State::Move)
    {
        vec3 dir = m_target->pos() - m_pos;
		float dist = dir.length();
		
		dir.y = 0;
        dir.normalize();

		vec3 velocity = (dist < m_attackDistance) ? vec3{0, 0, 0} : dir * m_speed;
		m_velocity.x = velocity.x;
		m_velocity.z = velocity.z;
		m_rest = false;

        m_ang = atan2(-dir.z, dir.x) + math::pi * 0.5;
    }
	else
	{
		m_velocity.x = 0;
		m_velocity.z = 0;
	}
}

} // namespace gamelogic