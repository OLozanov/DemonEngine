#include "Physics/PhysicsManager.h"
#include "Collision/BoxCollisionShape.h"
#include "math.h"

namespace Physics
{

PhysicsManager * PhysicsManager::instance = nullptr;

bool AABBTest(Collision::TraceInfo & tinfo, const vec3& pos1, const vec3& pos2, const vec3& bbox1, const vec3& bbox2)
{
    bool intersect = true;
    int axis = 0;

    for(int i = 0; i < 3; i++)
    {
        float dist = fabs(pos2[i] - pos1[i]);
        float radius = bbox1[i] + bbox2[i];

        if(dist >= radius) return false;

        float penetration = radius - dist;

        if(i == 0 || penetration < tinfo.dist)
        {
            tinfo.dist = penetration;
            axis = i;

            intersect = true;
        }
    }

    if(intersect)
    {
        tinfo.norm = {};
        tinfo.norm[axis] = 1;
    }

    return intersect;
}

PhysicsManager& PhysicsManager::GetInstance()
{
    static PhysicsManager instance;

    return instance;
}

void PhysicsManager::resolveContactLinear(RigidBody& bodyA, RigidBody& bodyB, const Collision::ContactInfo& contactInfo)
{
    bodyA.m_rest = false;
    bodyB.m_rest = false;

    //Bouncing
    vec3 relVelocity = bodyB.m_velocity - bodyA.m_velocity;
    float speed = relVelocity * contactInfo.norm;

    bodyA.m_pos += contactInfo.norm * contactInfo.dist * 0.5;
    bodyB.m_pos -= contactInfo.norm * contactInfo.dist * 0.5;

    float bounce = (bodyA.m_bounce < bodyB.m_bounce) ? bodyA.m_bounce : bodyB.m_bounce;

    float impulse = -speed * (1 + bounce);
    impulse /= bodyA.m_invMass + bodyB.m_invMass;

    bodyA.m_velocity -= contactInfo.norm * impulse * bodyA.m_invMass;
    bodyB.m_velocity += contactInfo.norm * impulse * bodyB.m_invMass;

    bodyA.onCollide(contactInfo.norm, impulse);
    bodyB.onCollide(contactInfo.norm, impulse);

    //Friction
    vec3 tangent = relVelocity - contactInfo.norm * (contactInfo.norm * relVelocity);
    float tlen = tangent.normalize();

    float friction = impulse * 0.9;//dynamicFriction;
    //if(fabs(friction) > tlen) friction = -tlen;

    //Clamp tangential speed
    float tspeedA = bodyA.m_velocity * tangent;
    float tspeedB = bodyB.m_velocity * tangent;

    float fspeedA = friction * bodyA.m_invMass;
    float fspeedB = friction * bodyB.m_invMass;

    if (fabs(fspeedA) > fabs(tspeedA)) fspeedA = tspeedA;
    if (fabs(fspeedB) > fabs(tspeedB)) fspeedB = -tspeedB;

    bodyA.m_velocity -= tangent * fspeedA;
    bodyB.m_velocity += tangent * fspeedB;
}

void PhysicsManager::resolveContactLinear(RigidBody& body, const Collision::ContactInfo& contactInfo)
{
    constexpr float mu = 2;

    vec3 tangent = (body.m_velocity - contactInfo.norm * (contactInfo.norm * body.m_velocity));

    //Bouncing
    float speed = contactInfo.norm * body.m_velocity;
    float impulse = speed * (1 + body.m_bounce);

    body.m_pos += contactInfo.norm * contactInfo.dist;

    if (speed > 0) return;   // if moving away

    body.m_velocity -= contactInfo.norm * impulse;
    body.onCollide(contactInfo.norm, impulse);

    //Friction
    /*float tangentialSpeed = tangent.normalize();
    float fimpulse = -tangentialSpeed; // just skip multiply/delete m_invMass here

    if(fabs(fimpulse) > fabs(impulse)*mu) fimpulse = tangentialSpeed*(impulse*m_friction);

    m_velocity += tangent*fimpulse;*/
}

void PhysicsManager::resolveContact(RigidBody& bodyA, RigidBody& bodyB, const Collision::ContactInfo& contactInfo)
{
    bodyA.m_rest = false;
    bodyB.m_rest = false;

    mat3 inverseInertiaTensorA = bodyA.inverseInertiaTensor();
    mat3 inverseInertiaTensorB = bodyB.inverseInertiaTensor();

    vec3 contactPointA = contactInfo.point - bodyA.m_pos;
    vec3 contactPointB = contactInfo.point - bodyB.m_pos;

    vec3 velocityA = bodyA.m_velocity + (bodyA.m_angularVelocity ^ contactPointA);
    vec3 velocityB = bodyB.m_velocity + (bodyB.m_angularVelocity ^ contactPointB);

    vec3 relVelocity = velocityA - velocityB;

    vec3 angularA = contactPointA ^ contactInfo.norm;
    vec3 angularB = contactPointB ^ contactInfo.norm;

    float speed = relVelocity * contactInfo.norm;

    float m = 1.0f / (bodyA.m_invMass + bodyB.m_invMass + 
                     (inverseInertiaTensorA * angularA) * angularA +
                     (inverseInertiaTensorB * angularB) * angularB);

    // position resolution
    vec3 rotImpulseA = inverseInertiaTensorA * angularA;
    vec3 rotImpulseB = inverseInertiaTensorB * angularB;

    bool rotation = bodyA.m_rotation && bodyB.m_rotation;

    if (rotation)
    {
        float motion = contactInfo.dist * m;

        vec3 rotationA = rotImpulseA * motion;
        float angleA = rotationA.normalize();

        bodyA.m_orientation = mat3::Rotate(rotationA, angleA) * bodyA.m_orientation;
        bodyA.m_pos += contactInfo.norm * bodyA.m_invMass * motion;

        vec3 rotationB = rotImpulseB * motion;
        float angleB = -rotationB.normalize();

        bodyB.m_orientation = mat3::Rotate(rotationB, angleB) * bodyB.m_orientation;
        bodyB.m_pos -= contactInfo.norm * bodyB.m_invMass * motion;
    }
    else
    {
        float totalMassInv = 1.0 / (bodyA.m_mass + bodyB.m_mass);

        float distA = contactInfo.dist * bodyA.m_mass * totalMassInv;
        float distB = contactInfo.dist * bodyB.m_mass * totalMassInv;

        bodyA.m_pos += contactInfo.norm * distA;
        bodyB.m_pos -= contactInfo.norm * distB;
    }

    if (speed > 0) return;   // if moving away

    // velocity resolution
    float bounce = fabs(speed) < 0.05f ? 0.0f : 
                   (bodyA.m_bounce < bodyB.m_bounce) ? bodyA.m_bounce : bodyB.m_bounce;

    float Jv = contactInfo.norm * bodyA.m_velocity + angularA * bodyA.m_angularVelocity -
               contactInfo.norm * bodyB.m_velocity - angularB * bodyB.m_angularVelocity;

    float lambda = -m * (Jv + bounce * speed);

    bodyA.m_velocity += contactInfo.norm * bodyA.m_invMass * lambda;

    if (bodyA.m_rotation)
    {
        bodyA.m_angularMomentum += angularA * lambda;
        bodyA.m_angularVelocity = inverseInertiaTensorA * bodyA.m_angularMomentum;
    }

    bodyB.m_velocity -= contactInfo.norm * bodyB.m_invMass * lambda;

    if (bodyB.m_rotation)
    {
        bodyB.m_angularMomentum -= angularB * lambda;
        bodyB.m_angularVelocity = inverseInertiaTensorB * bodyB.m_angularMomentum;
    }

    float impulse = m * speed;

    bodyA.onCollide(contactInfo.norm, impulse * 0.1f);
    bodyB.onCollide(-contactInfo.norm, impulse * 0.1f);

    // friction
    velocityA = bodyA.m_velocity + (bodyA.m_angularVelocity ^ contactPointA);
    velocityB = bodyB.m_velocity + (bodyB.m_angularVelocity ^ contactPointB);

    relVelocity = velocityA - velocityB;

    float friction = (bodyA.m_friction > bodyB.m_friction) ? bodyA.m_friction : bodyB.m_friction;

    vec3 tangent = -(relVelocity - contactInfo.norm * (contactInfo.norm * relVelocity));
    tangent.normalize();

    vec3 tangularA = contactPointA ^ tangent;
    vec3 tangularB = contactPointB ^ tangent;

    m = 1.0f / (bodyA.m_invMass + bodyB.m_invMass +
               (inverseInertiaTensorA * tangularA) * tangularA +
               (inverseInertiaTensorB * tangularB) * tangularB);

    Jv = tangent * bodyA.m_velocity + tangularA * bodyA.m_angularVelocity -
         tangent * bodyB.m_velocity - tangularB * bodyB.m_angularVelocity;
    
    lambda = -m * Jv;

    float bound = friction * fabs(impulse);
    lambda = std::max(-bound, std::min(lambda, bound));

    bodyA.m_velocity += tangent * bodyA.m_invMass * lambda;
    
    if (bodyA.m_rotation)
    {
        bodyA.m_angularMomentum += tangularA * lambda;
        bodyA.m_angularVelocity = inverseInertiaTensorA * bodyA.m_angularMomentum;
    }

    bodyB.m_velocity -= tangent * bodyB.m_invMass * lambda;

    if (bodyB.m_rotation)
    {
        bodyB.m_angularMomentum -= tangularB * lambda;
        bodyB.m_angularVelocity = inverseInertiaTensorB * bodyB.m_angularMomentum;
    }
}

void PhysicsManager::resolveContact(RigidBody& body, const Collision::ContactInfo& contactInfo, float dt)
{
    body.m_rest = false;

    if (!body.m_rotation)
    {
        resolveContactLinear(body, contactInfo);
        return;
    }

    mat3 inverseInertiaTensor = body.inverseInertiaTensor();

    vec3 contactPoint = contactInfo.point - body.m_pos;
    vec3 velocity = body.m_velocity + (body.m_angularVelocity ^ contactPoint);

    vec3 angular = contactPoint ^ contactInfo.norm;

    //Bouncing
    float speed = contactInfo.norm * velocity;
    float bounce = (fabs(speed) < 0.05) ? 0 : 0.1;

    // Effective mass
    float m = 1.0f / (body.m_invMass + (inverseInertiaTensor * angular) * angular);
    
    // position resolution
    vec3 rotImpulse = inverseInertiaTensor * angular;

    if (rotImpulse.length() > math::eps)
    {
        float motion = contactInfo.dist * m;

        vec3 rotation = rotImpulse * motion;
        float angle = rotation.normalize();

        body.m_orientation = mat3::Rotate(rotation, angle) * body.m_orientation;
        body.m_pos += contactInfo.norm * body.m_invMass * motion;
    }
    else
        body.m_pos += contactInfo.norm * contactInfo.dist;

    if (speed > 0) return;

    // velocity resolution
    float Jv = contactInfo.norm * body.m_velocity + angular * body.m_angularVelocity;
    float lambda = -m * (Jv + bounce * speed);

    body.m_velocity += contactInfo.norm * body.m_invMass * lambda;
    body.m_angularMomentum += angular * lambda;
    body.m_angularVelocity = inverseInertiaTensor * body.m_angularMomentum;

    float impulse = m * speed;

    body.onCollide(contactInfo.norm, lambda);
    
    //Friction
    //inverseInertiaTensor = body.inverseInertiaTensor();
    //vec3 cvelocity = -velocity.normalized();

    velocity = body.m_velocity + (body.m_angularVelocity ^ contactPoint);
    vec3 tangent = -(velocity - contactInfo.norm * (contactInfo.norm * velocity));
    tangent.normalize();

    vec3 tangular = contactPoint ^ tangent;

    //float vcos = cvelocity * -contactInfo.norm;
    //float vsin = cvelocity * tangent;
    //float vcos = -contactInfo.norm.y;
    //float vsin = 1.0 - vcos * vcos; //-tangent.y;
    //float tdist = contactInfo.dist / vcos * vsin;
    //float bias = fabs(vcos) > math::eps ? tdist / dt : 0.0f;

    m = 1.0f / (body.m_invMass + (inverseInertiaTensor * tangular) * tangular);
    Jv = tangent * body.m_velocity + tangular * body.m_angularVelocity;
    lambda = -m * Jv;

    float bound = body.m_friction * fabs(impulse); // body.m_mass * 9.8f;
    lambda = std::max(-bound, std::min(lambda, bound));

    body.m_velocity += tangent * body.m_invMass * lambda;
    body.m_angularMomentum += tangular * lambda;
    body.m_angularVelocity = inverseInertiaTensor * body.m_angularMomentum;
}

void PhysicsManager::resolveCollisions(float dt)
{
    Collision::ContactInfo contactInfo;

    //Static world collision
    for(RigidBody * object : m_bodies)
    {
        bool contact = false;

        for(StationaryBody * staticObj : m_stationaryBodies)
        {
            if(object->m_rest) continue;
            if(!(object->m_layers & staticObj->m_layers)) continue;

            vec3 staticPos = (staticObj->m_bbox.min + staticObj->m_bbox.max)*0.5;
            vec3 staticBBox = staticObj->m_bbox.max - staticPos;

            staticPos += staticObj->m_collisionShape->pos();

            // Narrow phase
            if(!AABBTest(object->m_pos, object->m_bbox, staticPos, staticBBox)) continue;

            // Narrow phase
            Collision::ContactInfo objContactInfo;

            if(object->m_collisionShape->testCollision(staticObj->m_collisionShape, objContactInfo))
            {
                if(!contact || objContactInfo.dist > contactInfo.dist)
                {
                    contact = true;
                    contactInfo = objContactInfo;
                }
            }
        }

        if(contact) resolveContact(*object, contactInfo, dt);
        //if (contact) object->resolveStaticCollision(contactInfo);
    }

    //Objects collision
    for(auto itA = m_bodies.begin(); itA != m_bodies.end(); itA++)
    {
        for(auto itB = itA.next(); itB != m_bodies.end(); itB++)
        {
            RigidBody * objA = *itA;
            RigidBody * objB = *itB;

            if(objA->m_rest && objB->m_rest) continue;
            if(!(objA->m_layers & objB->m_layers)) continue;

            if (objA->m_adjacentBody == objB || objB->m_adjacentBody == objA) continue;

            // Broad Phase
            if(!AABBTest(objA->m_pos, objA->m_bbox, objB->m_pos, objB->m_bbox)) continue;

            // Narrow phase
            if(objA->m_collisionShape->testCollision(objB->m_collisionShape, contactInfo))
                resolveContact(*objA, *objB, contactInfo);
                //RigidBody::resolveCollision(*objA, *objB, contactInfo);
        }
    }
}

void PhysicsManager::update(float dt)
{
    for (Force* force : m_forces) force->generator.update(force->body, dt);

    for (RigidBody* object : m_bodies) object->integrate(dt);

    resolveCollisions(dt);

    for (Constraint* joint : m_constraints)
    {
        joint->evaluate();
        joint->resolve(dt);
    }

    for (RigidBody* object : m_bodies) object->postUpdate(dt);
}

void PhysicsManager::run(float dt)
{
    //constexpr int IterationLimit = 10;
    //constexpr double UpdateInterval = 1.0/600;
    constexpr int Iterations = 5;

    /*if(dt/UpdateInterval > IterationLimit)
    {
        update(dt);
        return;
    }

    float t = 0;

    while(true)
    {
        if(t + UpdateInterval >= dt)
        {
            update(dt - t);
            break;
        }

        update(UpdateInterval);
        t += UpdateInterval;
    }*/

    for (int i = 0; i < Iterations; i++) update(dt / Iterations);
}

bool PhysicsManager::traceRay(const vec3 & origin,
                              const vec3 & ray,
                              unsigned long mask,
                              Collision::TraceRayInfo& traceInfo,
                              float maxdist)
{
    bool intersect = false;

    traceInfo.dist = std::numeric_limits<float>::infinity();
    traceInfo.object = nullptr;

    for(StationaryBody * body : m_stationaryBodies)
    {
        if(!(body->m_layers & mask)) continue;

        Collision::TraceRayInfo tinfo;

        if(body->m_collisionShape->traceRay(origin, ray, tinfo) && tinfo.dist < maxdist)
        {
            if(tinfo.dist < traceInfo.dist)
            {
                traceInfo.dist = tinfo.dist;
                traceInfo.norm = tinfo.norm;
                traceInfo.material = tinfo.material;
                traceInfo.object = body->m_object;
                traceInfo.layers = body->m_layers;
                intersect = true;
            }
        }
    }

    for(RigidBody * body : m_bodies)
    {
        if(!(body->m_layers & mask)) continue;

        Collision::TraceRayInfo tinfo;

        if(body->m_collisionShape->traceRay(origin, ray, tinfo) && tinfo.dist < maxdist)
        {
            if(tinfo.dist < traceInfo.dist)
            {
                traceInfo.dist = tinfo.dist;
                traceInfo.norm = tinfo.norm;
                traceInfo.material = tinfo.material;
                traceInfo.object = body->m_object;
                traceInfo.layers = body->m_layers;
                intersect = true;
            }
        }
    }

    return intersect;
}

PhysicsManager::CollisionResult PhysicsManager::testCollision(const Collision::CollisionShape * shape,
                                                              const vec3& bbox,
                                                              unsigned long layers,
                                                              Collision::ContactInfo& contactInfo,
                                                              bool staticOnly)
{
    ObjectRef objectRef = nullptr;

    Collision::ContactInfo objContactInfo;
    bool contact = false;

    for(StationaryBody * staticObj : m_stationaryBodies)
    {
        if(!(layers & staticObj->m_layers)) continue;

        vec3 staticPos = (staticObj->m_bbox.min + staticObj->m_bbox.max)*0.5;
        vec3 staticBBox = staticObj->m_bbox.max - staticPos;

        staticPos += staticObj->m_collisionShape->pos();

        // Narrow phase
        if(!AABBTest(shape->pos(), bbox, staticPos, staticBBox)) continue;

        // Narrow phase
        if(shape->testCollision(staticObj->m_collisionShape, objContactInfo))
        {
            if(!contact || objContactInfo.dist > contactInfo.dist)
            {
                contact = true;
                contactInfo = objContactInfo;

                objectRef = staticObj->m_object;
            }
        }
    }

    if(staticOnly) return {contact, objectRef};

    for(RigidBody * object : m_bodies)
    {
        if(!(layers & object->m_layers)) continue;

        // Broad phase
        if(!AABBTest(shape->pos(), bbox, object->m_pos, object->m_bbox)) continue;

        // Narrow phase
        if(shape->testCollision(object->m_collisionShape, objContactInfo))
        {
            if(!contact || objContactInfo.dist > contactInfo.dist)
            {
                contact = true;
                contactInfo = objContactInfo;

                objectRef = object->m_object;
            }
        }
    }

    return {contact, objectRef};
}

void PhysicsManager::traceBBox(Collision::TraceInfo& tinfo, unsigned long layers, const vec3& bbox, const vec3 begin, const vec3 end)
{
    tinfo.fraction = 1.0;

    for(StationaryBody * staticObj : m_stationaryBodies)
    {
        if(!(layers & staticObj->m_layers)) continue;

        //Broad phase
        vec3 staticPos = (staticObj->m_bbox.min + staticObj->m_bbox.max)*0.5;
        vec3 staticBBox = staticObj->m_bbox.max - staticPos;

        staticPos += staticObj->m_collisionShape->pos();

        if(!AABBDynTest(bbox, staticBBox, begin, end, staticPos)) continue;

        //Narrow phase
        Collision::TraceInfo objTraceInfo;
        objTraceInfo.fraction = 1.0;

        if(staticObj->m_collisionShape->traceBBox(objTraceInfo, bbox, begin, end) &&
            objTraceInfo.fraction < tinfo.fraction)
        {
            tinfo = objTraceInfo;
        }
    }
}

bool PhysicsManager::testHeight(const vec3& pos, const vec3& bbox, unsigned long layers, float& height, float& tilt)
{
    bool intersect = false;

    for(StationaryBody * staticObj : m_stationaryBodies)
    {
        if(!(layers & staticObj->m_layers)) continue;

        vec3 staticPos = (staticObj->m_bbox.min + staticObj->m_bbox.max)*0.5;
        vec3 staticBBox = staticObj->m_bbox.max - staticPos;

        staticPos += staticObj->m_collisionShape->pos();

        // Narrow phase
        vec3 testBBox = bbox;
        testBBox.y *= 2;

        if(!AABBTest(pos, testBBox, staticPos, staticBBox)) continue;

        // Narrow phase
        float objHeight;
        float objTilt;

        if(staticObj->m_collisionShape->testHeight(pos, bbox, objHeight, objTilt))
        {
            if(!intersect || objHeight > height)
            {
                intersect = true;
                height = objHeight;
                tilt = objTilt;
            }
        }
    }

    return intersect;
}

} //namespace Physics