#pragma once

#include "Physics/RigidBody.h"
#include "Physics/Constraint.h"
#include "Physics/Force.h"

#include <utility>

namespace Physics
{

class StationaryBody : public ListNode<StationaryBody>
{
protected:
    Collision::CollisionShape * m_collisionShape;

    BBox m_bbox;
    uint32_t m_layers;

    ObjectRef m_object;

public:

    StationaryBody()
    : m_collisionShape(nullptr)
    {
    }

    StationaryBody(Collision::CollisionShape * collisionShape, const BBox& bbox, unsigned long layers)
    : m_collisionShape(collisionShape)
    , m_bbox(bbox)
    , m_layers(layers)
    , m_object(nullptr)
    {
    }

    ~StationaryBody()
    {
        delete m_collisionShape;
    }

    void setCollision(Collision::CollisionShape * collisionShape, const BBox& bbox, unsigned long layers)
    {
        m_collisionShape = collisionShape;
        m_bbox = bbox;
        m_layers = layers;
    }

    const vec3& pos() { return m_collisionShape->pos(); }
    const BBox& bbox() { return m_bbox; }

    virtual void onRayIntersect(const vec3& point, const vec3& direction, int func) {}

    friend class PhysicsManager;
};

class PhysicsManager
{

private:
    static PhysicsManager * instance;

    PhysicsManager() {}
    PhysicsManager(PhysicsManager& sm) = delete;
    PhysicsManager& operator =(PhysicsManager& sm) = delete;

    LinkedList<StationaryBody> m_stationaryBodies;
    LinkedList<RigidBody> m_bodies;

    LinkedList<Force> m_forces;

    LinkedList<Constraint> m_constraints;

    static void resolveContactLinear(RigidBody& bodyA, RigidBody& bodyB, const Collision::ContactInfo& contactInfo);
    static void resolveContactLinear(RigidBody& body, const Collision::ContactInfo& contactInfo);

    static void resolveContact(RigidBody& bodyA, RigidBody& bodyB, const Collision::ContactInfo& contactInfo);
    static void resolveContact(RigidBody& body, const Collision::ContactInfo& contactInfo, float dt);

    void resolveCollisions(float dt);

public:

    using CollisionResult = std::pair<bool, ObjectRef>;

    static PhysicsManager& GetInstance();

    void addRigidBody(RigidBody * body) { m_bodies.append(body); }
    void removeRigidBody(RigidBody * body) { m_bodies.remove(body); }

    void addStationaryBody(StationaryBody * body) { m_stationaryBodies.append(body); }
    void removeStationaryBody(StationaryBody * body) { m_stationaryBodies.remove(body); }

    Force * addForce(ForceGenerator& generator, RigidBody& body)
    {
        Force * force = new Force(generator, body);
        m_forces.append(force);

        return force;
    }

    void removeForce(Force * force)
    {
        m_forces.remove(force);
        delete force;
    }

    void addConstraint(Constraint* constraint) { m_constraints.append(constraint); }
    void removeConstraint(Constraint* constraint) { m_constraints.remove(constraint); }

    void reset()
    {
        m_stationaryBodies.clear();
        m_bodies.clear();
        m_constraints.clear();
        m_forces.destroy();
    }

    void update(float dt);
    void run(float dt);

    bool traceRay(const vec3 & origin,
                  const vec3 & ray,
                  unsigned long mask,
                  Collision::TraceRayInfo& traceInfo,
                  float maxdist = std::numeric_limits<float>::infinity());

    void traceBBox(Collision::TraceInfo& tinfo, unsigned long layers, const vec3& bbox, const vec3 begin, const vec3 end);
    bool testHeight(const vec3& pos, const vec3& bbox, unsigned long layers, float& height, float& tilt);

    CollisionResult testCollision(const Collision::CollisionShape * shape,
                                    const vec3& bbox,
                                    unsigned long layers,
                                    Collision::ContactInfo& contactInfo,
                                    bool staticOnly = false);
};

} //namespace Physics
