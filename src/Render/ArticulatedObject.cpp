#include "Render/ArticulatedObject.h"

namespace Render
{

ArticulatedObject::ArticulatedObject()
: m_model(nullptr)
, m_run(false)
, m_repeat(true)
, m_time(0)
, m_dtime(0)
, m_frame(0)
{
    m_dynamic = true;
}

ArticulatedObject::ArticulatedObject(Model* model, float frate)
: m_frame_rate(frate)
, m_model(model)
, m_run(true)
, m_repeat(true)
, m_time(0)
, m_dtime(0)
, m_frame(0)
{
    m_dynamic = true;

    m_bspace.resize(model->bonenum());

    m_model->prepareSkeletalData();

    m_omat.resize(m_model->meshnum());

    m_start_frame = 0;
    m_end_frame = m_model->keyframes().size() - 1;

    uint16_t vertnum = m_model->vertexnum();
    
    m_vertexBuffer.resize(vertnum);
    m_boneBuffer.resize(m_model->bonenum());
    m_inputBuffer = m_model->vertexBuffer();
    m_boneIdBuffer = m_model->boneIdBuffer();

    m_vertexBuffer.setAccessFlags(false, true);

    initGeometryData();
}

void ArticulatedObject::initGeometryData()
{
    const BBox& bbox = m_model->bbox();

    float size = std::max(std::max(std::max(fabs(bbox.min.x), fabs(bbox.min.y)), fabs(bbox.min.z)),
                          std::max(std::max(fabs(bbox.max.x), fabs(bbox.max.y)), fabs(bbox.max.z)));

    m_bbox = { {-size, -size, -size}, {size, size, size} };

    DisplayBlock displayBlock;

    displayBlock.type = DisplayBlock::display_regular;
    displayBlock.mat = &m_omat[0];
    displayBlock.vertexData = m_vertexBuffer;
    displayBlock.indexData = m_model->indexBuffer();

    for (const auto& mesh : m_model->meshes()) displayBlock.displayData.push_back(&mesh);

    m_displayData.emplace_back(displayBlock);
}

ArticulatedObject::~ArticulatedObject()
{
}

void ArticulatedObject::setModel(Model* model, float frate)
{
    m_model.reset(model);
    m_frame_rate = frate;

    m_run = true;

    m_bspace.resize(model->bonenum());

    m_model->prepareSkeletalData();

    m_omat.resize(m_model->meshnum());

    m_start_frame = 0;
    m_end_frame = m_model->keyframes().size() - 1;

    initGeometryData();
}

void ArticulatedObject::animateMesh(int num)
{
    const KeyFrame& kframe = m_model->keyframe(m_frame, num);
    const KeyFrame& dkframe = m_model->dframe(m_frame, num);

    vec3 crot = kframe.rot + dkframe.rot * m_dtime;
    vec3 cpos = kframe.pos + dkframe.pos * m_dtime;

    mat4 tmat = mat4::Translate({ cpos.x, cpos.y, cpos.z });

    m_omat[num] = m_mat * tmat * mat4::Rotate(crot.x, crot.y, crot.z);
}

void ArticulatedObject::calcFrameBones()
{
    const vec3* bpos = m_model->bonespos(m_frame);
    const vec3* dbpos = m_model->dbpos(m_frame);

    m_rot = m_model->bonesrot(m_frame);
    m_drot = m_model->dbrot(m_frame);

    // Pose bone matrix calculation scheme:
    // parent * (parent_rest.inverted * rest) * rotation

    for (size_t br = 0; br < m_model->boneRoots().size(); br++)
    {
        vec3 brpos = bpos[br] + dbpos[br] * m_dtime;

        mat4 rmat = mat4::Translate(brpos);

        int b = m_model->boneRoot(br).bid;

        const Bone& bone = m_model->bone(b);

        vec3 crot = m_rot[b] + m_drot[b] * m_dtime;

        mat4 mat = rmat * m_model->bonebasis(b) * mat4::Rotate(crot.x, crot.y, crot.z);
        m_bspace[b] = mat * m_model->invbonebasis(b);
    }

    for (size_t i = 0; i < m_model->bonenum(); i++)
    {
        const Bone& bone = m_model->bone(i);

        if (bone.parent == Model::InvalidBoneIndex) continue;

        vec3 crot = m_rot[i] + m_drot[i] * m_dtime;

        mat4 mat = m_bspace[bone.parent] * m_model->bonebasis(i) * mat4::Rotate(crot.x, crot.y, crot.z);
        m_bspace[i] = mat * m_model->invbonebasis(i);
    }
}

void ArticulatedObject::setFrame(unsigned short kframe)
{
    m_frame = kframe;
    m_time = kframe * m_frame_rate;
    m_dtime = 0;
}

void ArticulatedObject::setAnimRange(unsigned short start, unsigned short end)
{
    m_start_frame = start;
    m_end_frame = end;
}

void ArticulatedObject::startAnimation()
{
    if (m_run) return;

    m_frame = m_start_frame;
    m_time = m_frame * m_frame_rate;
    m_dtime = 0;

    m_run = true;
}

void ArticulatedObject::setAnimTime(float time)
{
    m_time = time;
    m_frame = m_time / m_frame_rate;
    m_dtime = m_time - m_frame * m_frame_rate;

    calcFrameBones();
    for (int m = 0; m < m_model->objnum(); m++) animateMesh(m);

    m_boneBuffer.setData(m_bspace.data(), m_bspace.size());

    m_update = true;
}

void ArticulatedObject::animate(float dt)
{
    for (int m = 0; m < m_model->objnum(); m++) animateMesh(m);

    if (!m_run) return;

    m_time += dt;
    if (m_time >= m_end_frame * m_frame_rate)
    {
        if (m_repeat) m_time = m_start_frame * m_frame_rate;
        else
        {
            m_time = m_end_frame * m_frame_rate;
            m_run = false;
        }
    }

    m_frame = m_time / m_frame_rate;
    m_dtime = m_time - m_frame * m_frame_rate;

    calcFrameBones();

    m_boneBuffer.setData(m_bspace.data(), m_bspace.size());

    m_update = true;
}

} //namespace render