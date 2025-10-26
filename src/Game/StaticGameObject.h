#pragma once

#include "Utils/LinkedList.h"
#include "Render/StaticObject.h"
#include "Collision/BoxCollisionShape.h"
#include "Collision/PolygonalCollisionShape.h"
#include "Physics/PhysicsManager.h"
#include "Render/SceneManager.h"
#include "Resources/Model.h"

#include "Game/Constants.h"

namespace GameLogic
{

class StaticGameObject : public ListNode<StaticGameObject>
{
public:
    StaticGameObject();
    StaticGameObject(const vec3& pos, const mat3& mat, Model* model, uint8_t flags);
    virtual ~StaticGameObject();

private:
    void addCollisionModel();

protected:
    ResourcePtr<Model> m_model;

    Render::DisplayObject* m_geometry;
    Physics::StationaryBody* m_body;

    vec3 m_pos;
    mat3 m_orientation;

private:

    enum Flags
    {
        flag_collision = 1,
        flag_gi = 2
    };
};

} // namespace gamelogic