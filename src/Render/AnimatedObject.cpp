#include "AnimatedObject.h"

namespace Render
{

AnimatedObject::AnimatedObject(Model* model)
: m_model(model)
, m_frame_rate(default_frame_rate)
, m_run(false)
, m_repeat(true)
, m_time(0)
, m_dtime(0)
, m_frame(0)
{
    initData();
    setTime(0);

    m_start_frame = 0;
    m_end_frame = m_model->framenum() - 1;
}

AnimatedObject::AnimatedObject(const vec3& pos, const vec3& rot, Model* model)
: m_model(model)
, m_frame_rate(default_frame_rate)
, m_run(false)
, m_repeat(true)
, m_time(0)
, m_dtime(0)
, m_frame(0)
{
    m_mat = mat4::Translate(pos) * mat4::Rotate(rot.x/180.0*math::pi, rot.y/180.0*math::pi, rot.z/180.0*math::pi);
    initData();
    setTime(0);

    m_start_frame = 0;
    m_end_frame = m_model->framenum() - 1;
}

AnimatedObject::AnimatedObject(const vec3& pos, const mat3& mat, Model* model)
: m_model(model)
, m_frame_rate(default_frame_rate)
, m_run(false)
, m_repeat(true)
, m_time(0)
, m_dtime(0)
, m_frame(0)
{
    m_mat = mat4::Translate(pos) * mat4(mat);
    initData();
    setTime(0);

    m_start_frame = 0;
    m_end_frame = m_model->framenum() - 1;
}

void AnimatedObject::initData()
{
    m_dynamic = true;

    m_bbox = m_model->bbox();

    std::vector<const DisplayData*> displayDataRegular;
    std::vector<const DisplayData*> displayDataTransparent;
    std::vector<const DisplayData*> displayDataEmissive;

    const auto& objMeshes = m_model->objectMeshes();
    const auto& meshes = m_model->meshes();

    m_objmat.resize(objMeshes.size());
    m_objflags.resize(objMeshes.size());

    uint16_t mid = 0;

    for (size_t i = 0; i < objMeshes.size(); i++)
    {
        for (uint16_t m = 0; m < objMeshes[i]; m++, mid++)
        {
            const auto& mesh = meshes[mid];

            switch (mesh.material->type)
            {
            case Material::material_regular:
                displayDataRegular.push_back(&mesh);
            break;

            case Material::material_transparent:
                displayDataTransparent.push_back(&mesh);
            break;

            case Material::material_emissive:
                displayDataEmissive.push_back(&mesh);
            break;
            }
        }

        if (!displayDataRegular.empty())
        {
            m_displayData.emplace_back(DisplayBlock::display_regular,
                                       &m_objmat[i],
                                       m_model->vertexBuffer(),
                                       m_model->indexBuffer(),
                                       displayDataRegular);
            
            m_objnum.push_back(i);

            displayDataRegular.clear();
        }

        if (!displayDataTransparent.empty())
        {
            m_displayData.emplace_back(DisplayBlock::display_transparent,
                                       &m_objmat[i],
                                       m_model->vertexBuffer(),
                                       m_model->indexBuffer(),
                                       displayDataTransparent);

            m_objnum.push_back(i);

            displayDataTransparent.clear();
        }

        if (!displayDataEmissive.empty())
        {
            m_displayData.emplace_back(DisplayBlock::display_emissive,
                                       &m_objmat[i],
                                       m_model->vertexBuffer(),
                                       m_model->indexBuffer(),
                                       displayDataEmissive);

            m_objnum.push_back(i);

            displayDataEmissive.clear();
        }
    }
}

void AnimatedObject::setObjectsFlags()
{
    for (int i = 0; i < m_objnum.size(); i++)
    {
        uint32_t flags = m_objflags[m_objnum[i]];
        m_displayData[i].hide = flags & KeyFrame::keyframe_hide;
    }
}

void AnimatedObject::animateObject(int num)
{
    const KeyFrame& kframe = m_model->keyframe(m_frame, num);
    const KeyFrame& dkframe = m_model->dframe(m_frame, num);

    vec3 crot = kframe.rot + dkframe.rot * m_dtime;
    vec3 cpos = kframe.pos + dkframe.pos * m_dtime;

    mat4 tmat = mat4::Translate({ cpos.x, cpos.y, cpos.z });

    m_objmat[num] = m_mat * tmat * mat4::Rotate(crot.x, crot.y, crot.z);
    m_objflags[num] = kframe.flags;
}

void AnimatedObject::run(float time)
{
    m_time = time;
    m_frame = m_time / m_frame_rate;
    m_dtime = m_time - m_frame * m_frame_rate;

    for (int i = 0; i < m_model->objnum(); i++) animateObject(i);
    
    setObjectsFlags();

    m_run = true;
}

void AnimatedObject::setFrame(unsigned short kframe)
{
    m_frame = kframe;
    m_time = kframe * m_frame_rate;
    m_dtime = 0;

    for (int i = 0; i < m_model->objnum(); i++) animateObject(i);

    setObjectsFlags();
}

void AnimatedObject::setAnimRange(unsigned short start, unsigned short end)
{
    m_start_frame = start;
    m_end_frame = end;
}

void AnimatedObject::setAnimRange(const AnimationRange& range)
{
    m_start_frame = range.start;
    m_end_frame = range.end;

    setFrame(range.start);
}

void AnimatedObject::setTime(float time)
{
    m_time = time;
    m_frame = m_time / m_frame_rate;
    m_dtime = m_time - m_frame * m_frame_rate;

    for (int i = 0; i < m_model->objnum(); i++) animateObject(i);

    setObjectsFlags();
}

void AnimatedObject::animate(float dt)
{
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

    for (int i = 0; i < m_model->objnum(); i++) animateObject(i);

    setObjectsFlags();
}

} //namespace render