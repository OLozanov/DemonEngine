#include "RagDoll.h"
#include "Render/SceneManager.h"
#include "Game/Constants.h"
#include "Collision/BoxCollisionShape.h"
#include "Collision/PolygonalCollisionShape.h"

#undef max

#include <algorithm>

namespace GameLogic
{

RagDoll::RagDoll(const vec3& pos, float animTime, Model* model)
: ArticulatedObject(model)
, m_pos(pos)
, m_body(nullptr)
{
    Render::SceneManager::GetInstance().addObject(this);
    Render::SceneManager::GetInstance().registerSkeletalObject(this);

    m_mat = mat4::Translate(pos);

    float size = 0;

    for (int i = 0; i < 3; i++)
    {
        if (fabs(m_bbox.min[i]) > size) size = fabs(m_bbox.min[i]);
        if (fabs(m_bbox.max[i]) > size) size = fabs(m_bbox.max[i]);
    }
    
    size *= 1.5;
    m_bbox = { { -size, -size, -size }, { size, size, size } };

    setAnimRange(0.0, 14.0);
    setAnimTime(8.0f * m_frame_rate);

    const BBox& corebbox = m_model->corebbox();

    if ((corebbox.max - corebbox.min).length() > math::eps)
    {
        m_body = new RagDollBody(m_pos, m_omat[0], 60.0f, model);
        m_pos = m_body->location();
    }

    //m_bones.resize(m_model->bonenum());

    const vec3* bpos = m_model->bonespos(m_frame);
    const vec3* dbpos = m_model->dbpos(m_frame);

    for (int br = 0; br < m_model->boneRoots().size(); br++)
    {
        vec3 brpos = bpos[br] + dbpos[br] * m_dtime;

        mat4 tmat = m_body ? mat4::Translate(m_pos + brpos) : mat4::Translate(brpos);
        initBone(m_model->boneRoot(br).bid, tmat);
    }

#ifdef _DEBUG
    initDebugData();
#endif

}

RagDoll::~RagDoll()
{
    remove();

    if (m_body) delete m_body;

    for (size_t i = 0; i < m_bones.size(); i++) delete m_bones[i];
    for (size_t i = 0; i < m_constraints.size(); i++) delete m_constraints[i];
}

size_t RagDoll::initBone(size_t b, const mat4& pmat)
{
    vec3 crot = m_rot[b] + m_drot[b] * m_dtime;

    float length = m_model->bone(b).length;
    float width = m_model->boneParameters(b).width;

    // Pose bone matrix calculation scheme:
    // parent * (parent_rest.inverted * rest) * rotation
    mat4 mat = pmat * m_model->bonebasis(b) * mat4::Rotate(crot.x, crot.y, crot.z);

    size_t bid = -1;

    if (width > math::eps)
    {
        bid = m_bones.size();

        // Activate bones in T-pos
        /*vec3 pos = bonepos + bonemat[1] * length * 0.5f;
        if (m_body) pos += m_pos;

        RagDollBone* bone = new RagDollBone(b, pos, bonemat, length, width);
        m_bones.push_back(bone);*/

        vec3 pos = mat[3].xyz + mat[1] * length * 0.5f;

        RagDollBone* bone = new RagDollBone(b, pos, mat, length, width);
        m_bones.push_back(bone);
    }

    mat = mat * m_model->invbonebasis(b);

    if (m_model->bone(b).cfirst == Model::InvalidBoneIndex) return bid;

    for (size_t c = m_model->bone(b).cfirst; c <= m_model->bone(b).clast; c++)
    {
        size_t cid = initBone(c, mat);

        if (cid == -1) continue;

        if (bid != -1)
        {
            uint16_t constraintIdx = m_model->bone(c).constraint;

            Physics::Joint* joint = nullptr;

            if (constraintIdx != Model::InvalidBoneIndex)
            {
                const BoneConstraint& constraint = m_model->constraint(constraintIdx);
                joint = new Physics::Joint(m_bones[bid], m_bones[bid]->tail(), m_bones[cid], m_bones[cid]->head(), constraint.min, constraint.max);
            }
            else
                joint = new Physics::Joint(m_bones[bid], m_bones[bid]->tail(), m_bones[cid], m_bones[cid]->head());
            
            m_constraints.push_back(joint);
            Physics::PhysicsManager::GetInstance().addConstraint(joint);
        }
        else if (m_body)
        {
            uint16_t constraintIdx = m_model->bone(c).constraint;

            Physics::Joint* joint = nullptr;

            if (constraintIdx != Model::InvalidBoneIndex)
            {
                const BoneConstraint& constraint = m_model->constraint(constraintIdx);

                joint = new Physics::Joint(m_body, m_model->bonepos(c), m_bones[cid], m_bones[cid]->head(), constraint.min, constraint.max);
                joint->setReferenceFrame(m_model->bonemat(c));
            }
            else
                joint = new Physics::Joint(m_body, m_model->bonepos(c), m_bones[cid], m_bones[cid]->head());
            
            m_constraints.push_back(joint);
            Physics::PhysicsManager::GetInstance().addConstraint(joint);
        }
    }

    return bid;
}

#ifdef _DEBUG
void RagDoll::initDebugData()
{
    m_debugBoxData = { nullptr, 0, 24 };
    m_debugMat.resize(m_bones.size());
    m_debugBBox.resize(m_bones.size());

    const Render::VertexBuffer& bboxVertexBuffer = Render::SceneManager::GetInstance().bboxVertexBuffer();
    const Render::IndexBuffer& bboxIndexBuffer = Render::SceneManager::GetInstance().bboxIndexBuffer();

    for (size_t i = 0; i < m_bones.size(); i++)
    {
        m_displayData.emplace_back(Render::DisplayBlock::display_debug,
                                   &m_debugMat[i],
                                   bboxVertexBuffer,
                                   bboxIndexBuffer,
                                   &m_debugBoxData);

        float length = m_model->bone(m_bones[i]->boneid()).length;
        float width = m_model->boneParameters(m_bones[i]->boneid()).width;

        m_debugBBox[i] = { width, length * 0.5f, width };
    }
}
#endif

void RagDoll::remove()
{
    Physics::PhysicsManager& pm = Physics::PhysicsManager::GetInstance();

    for (size_t i = 0; i < m_constraints.size(); i++) pm.removeConstraint(m_constraints[i]);
    for (size_t i = 0; i < m_bones.size(); i++) pm.removeRigidBody(m_bones[i]);
}

void RagDoll::update(float dt)
{
    if (m_body)
    {
        m_pos = m_body->location();
        m_omat[0] = m_body->transformMat();
    }
    else
    {
        m_pos = {};

        for (size_t b = 0; b < m_bones.size(); b++) m_pos += m_bones[b]->location();

        m_pos *= 1.0f / m_bones.size();
        m_omat[0] = mat4::Translate(m_pos);
    }

    for (size_t b = 0; b < m_bones.size(); b++)
    {
        size_t bid = m_bones[b]->boneid();

        mat3 rot = m_bones[b]->orientation(); //m_model->bonemat(bid).transpose() * m_bones[b]->orientation();
        if (m_body) rot = m_body->orientation().transpose() * rot;
        
        vec3 bonepos = m_bones[b]->location() - m_pos - m_bones[b]->orientation()[1] * m_bones[b]->length() * 0.5f;
        if (m_body) bonepos = bonepos * m_body->orientation();

        mat4 bonemat = mat4(rot, bonepos);
        const mat4& localmat = m_model->invbonebasis(bid);

        m_bspace[bid] = bonemat * localmat;

#ifdef _DEBUG
        m_debugMat[b] = mat4(m_bones[b]->orientation(), m_bones[b]->location()) * mat4::Scale(m_debugBBox[b]);
#endif
    }

    m_boneBuffer.setData(m_bspace.data(), m_bspace.size());
    m_update = true;

    //animate(dt);
}

RagDollBody::RagDollBody(const vec3& pos, const mat3& orientation, float mass, Model* model)
: RigidBody(pos, mass, 0.3f, 0.9f, collision_solid)
{
    Physics::PhysicsManager::GetInstance().addRigidBody(static_cast<RigidBody*>(this));
    
    const BBox& bbox = model->corebbox();

    float bboxsz = bbox.max.length();
    RigidBody::m_bbox = { bboxsz, bboxsz, bboxsz };

    m_orientation = orientation;

    const auto& collisionData = model->collisionData();

    vec3 cbbox = (bbox.max - bbox.min) * 0.5f;
    vec3 center = (bbox.max + bbox.min) * 0.5f;
    //m_pos += m_center;

    //vec3& cbbox = bbox.max - m_center;
    //cbbox.x *= 0.8;

    if (!collisionData.empty()) m_collisionShape = new Collision::PolygonalCollisionShape(m_orientation, m_pos, collisionData.size(), collisionData.data());
    else m_collisionShape = new Collision::BoxCollisionShape(m_orientation, m_pos, cbbox, center);

    vec3 inertiaTensor = Physics::BoxInertiaTensor(bbox.max - bbox.min, mass);
    setInertia(inertiaTensor);

    m_object = static_cast<Hitable*>(this);

    m_layers = collision_solid | collision_hitable | collision_actor;

    m_rest = true;
}

void RagDollBody::onCollide(const vec3& normal, float impulse)
{

}

void RagDollBody::hit(const vec3& point, const vec3& direction, uint32_t power)
{
    vec3 localPoint = (point - RigidBody::m_pos) * RigidBody::m_orientation;
    vec3 localDir = (direction * RigidBody::m_orientation);

    applyImpulse(localDir * power, localPoint);
}

RagDollBone::RagDollBone(size_t boneid, const vec3& pos, const mat3& orientation, float length, float width)
: RigidBody(pos, 20.0f, 0.3, 0.9, collision_solid)
, m_boneid(boneid)
, m_length(length)
{
    m_orientation = orientation;

    RigidBody::m_bbox = { length, length, length };

    m_head = { 0, -length * 0.5f, 0 };
    m_tail = { 0, length * 0.5f, 0 };

    vec3 bbox = { 0.2f, length, 0.2f };

    m_collisionShape = new Collision::BoxCollisionShape(m_orientation, m_pos, { width, length * 0.5f,  width });

    vec3 inertiaTensor = Physics::BoxInertiaTensor(bbox, m_mass);
    setInertia(inertiaTensor);

    Physics::PhysicsManager::GetInstance().addRigidBody(static_cast<RigidBody*>(this));

    m_object = static_cast<Hitable*>(this);

    m_layers = collision_solid | collision_hitable | collision_actor;

    m_rest = true;
}

void RagDollBone::onCollide(const vec3& normal, float impulse)
{

}

void RagDollBone::hit(const vec3& point, const vec3& direction, uint32_t power)
{
    vec3 localPoint = (point - RigidBody::m_pos) * RigidBody::m_orientation;
    vec3 localDir = (direction * RigidBody::m_orientation);

    applyImpulse(localDir * power, localPoint);
}

} // namespace GameLogic