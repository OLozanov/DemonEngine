#include "SlideDoor.h"

#include "Resources/Resources.h"

#include "Collision/BoxCollisionShape.h"
#include "Collision/PolygonalCollisionShape.h"

#include "System/AudioManager.h"

#include "Game/Constants.h"

namespace GameLogic
{
   
SlideDoor::SlideDoor(const std::string& id, const vec3& pos, const mat3& mat, bool closed, bool gi)
: GameObject(id)
, StaticObject(pos, mat, ResourceManager::GetModel("Tech/door1.msh"), true)
, m_pos(pos)
, m_mat(mat)
, m_slideSpeed(0.02)
, m_state(State::Closed)
, m_slidePos(0)
, m_slideSound(ResourceManager::GetSound("Tech/door1.wav"))
, m_closed(closed)
, m_gi(gi)
{
    m_distance = (StaticObject::m_bbox.max[0] - StaticObject::m_bbox.min[0]) * 0.9;
 
	Render::SceneManager::GetInstance().addObject(static_cast<StaticObject*>(this));

	if (m_gi)
	{
		Render::RaytraceId geometryId = Render::SceneManager::GetInstance().addGeometry(StaticObject::m_model.get());
		m_rtInstance = Render::SceneManager::GetInstance().addGeometryInstance(geometryId, StaticObject::m_mat);
	}

	StationaryBody::m_bbox = RecalcBBox(m_model->bbox(), m_mat);

	const auto& collisionData = m_model->collisionData();
	if (!collisionData.empty()) m_collisionShape = new Collision::PolygonalCollisionShape(m_mat, m_pos, collisionData.size(), collisionData.data());
	else
	{
		const BBox& modelBBox = m_model->bbox();

		vec3 mid = (modelBBox.max + modelBBox.min) * 0.5;
		vec3 colbbox = modelBBox.max - mid;
		m_baseBBoxPos = m_pos + m_mat * mid;
		m_bboxPos = m_baseBBoxPos;

		m_collisionShape = new Collision::BoxCollisionShape(m_mat, m_bboxPos, colbbox);

		StationaryBody::m_bbox = RecalcBBox({ -colbbox, colbbox }, m_mat);
	}

	m_layers = collision_solid | collision_actor | collision_hitable;

	Physics::PhysicsManager::GetInstance().addStationaryBody(static_cast<StationaryBody*>(this));

	m_timer.OnFire.bind(this, &SlideDoor::close);
}

void SlideDoor::updateTransform()
{
	vec3 pos = m_pos + m_mat[0] * m_slidePos;
	StaticObject::m_mat = mat4::Translate(pos) * mat4(m_mat);

	m_bboxPos = m_baseBBoxPos + m_mat[0] * m_slidePos;

	if (m_gi) Render::SceneManager::GetInstance().setGeometryTransform(m_rtInstance, StaticObject::m_mat);
}

void SlideDoor::open()
{
	if (m_closed) return;

	m_timer.stop();

	if (m_state == State::Closed) AudioManager::Play(m_slideSound, m_pos);
	if (m_state != State::Opened) m_state = State::Opening;
}

void SlideDoor::close()
{
	if (m_state != State::Closed)
	{
		m_state = State::Closing;
		AudioManager::Play(m_slideSound, m_pos);
	}
}

void SlideDoor::release()
{
	if (m_closed) return;

	m_timer.start(CloseTime, true);
}

void SlideDoor::activate()
{
	m_closed = false;
	open();
}

void SlideDoor::update(float dt)
{
	switch (m_state)
	{
	case State::Opening:
		m_slidePos += m_slideSpeed;

		if (m_slidePos >= m_distance)
		{
			m_slidePos = m_distance;
			m_state = State::Opened;

			//PlayAudio(m_open_snd);
		}

		updateTransform();

		break;

	case State::Closing:
		m_slidePos -= m_slideSpeed;

		if (m_slidePos <= 0)
		{
			m_slidePos = 0;
			m_state = State::Closed;

			//PlayAudio(m_close_snd);
		}

		updateTransform();

		break;
	}
}

} // namespace gamelogic