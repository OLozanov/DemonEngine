#include "Model.h"
#include "Resources.h"
#include <iostream>

Model::~Model()
{
}

Model* Model::LoadModel(const char* name)
{
    FILE* file;

    //Open file
    errno_t error = fopen_s(&file, name, "rb");

    if (error)
    {
        std::cout << "Can't open file " << name << std::endl;
        return nullptr;
    }

    MshHeader header;

    fread(&header, sizeof(header), 1, file);

    if (header.signature != MeshSignature)
    {
        fclose(file);
        return 0;
    }

    Model* model = new Model;

    model->m_vertices.resize(header.vertnum);
    model->m_boneid.resize(header.vertnum);
    model->m_indices.resize(header.trinum * 3);

    fread(model->m_vertices.data(), sizeof(Render::Vertex), header.vertnum, file);
    
    if (header.bonenum > 0)
        fread(model->m_boneid.data(), sizeof(uint32_t), header.vertnum, file);

    fread(model->m_indices.data(), sizeof(uint16_t), header.trinum * 3, file);

    model->calculateBBox();

    model->m_meshes.resize(header.meshnum);

    std::string matname;
    model->m_meshnum = header.meshnum;

    for (uint16_t i = 0; i < header.meshnum; i++)
    {
        uint32_t vertexnum;
        uint32_t offset;

        fread(&vertexnum, sizeof(uint32_t), 1, file);
        fread(&offset, sizeof(uint32_t), 1, file);
        
        model->m_meshes[i].vertexnum = vertexnum * 3;
        model->m_meshes[i].offset = offset * 3;

        uint16_t len;

        fread(&len, sizeof(uint16_t), 1, file);
        
        if (len)
        {
            matname.resize(len);

            fread(matname.data(), 1, len, file);
            model->m_meshes[i].material = ResourceManager::GetMaterial(matname);
        }
    }

    model->m_objmeshes.resize(header.objnum);
    model->m_objnum = header.objnum;

    fread(model->m_objmeshes.data(), sizeof(uint16_t), header.objnum, file);

    //Bones
    model->m_rbonenum = header.brnum;
    model->m_bonenum = header.bonenum;

    if (header.bonenum > 0)
    {
        model->m_bones.resize(header.bonenum);
        fread(model->m_bones.data(), sizeof(Bone), header.bonenum, file);

        model->m_bonespace.resize(header.bonenum);
        fread(model->m_bonespace.data(), sizeof(BoneSpace), header.bonenum, file);

        model->m_constraints.resize(header.constraints);
        fread(model->m_constraints.data(), sizeof(BoneConstraint), header.constraints, file);

        model->m_boneRoots.resize(header.brnum);
        fread(model->m_boneRoots.data(), sizeof(BoneRoot), header.brnum, file);

        model->m_bbase.resize(header.bonenum);
        model->m_bbase_inv.resize(header.bonenum);
    }

    //Animation data
    model->m_framenum = header.keyframes;

    if (header.keyframes != 0)
    {
        model->m_keyframes.resize(model->m_framenum * model->m_objnum);
        model->m_dframes.resize(model->m_framenum * model->m_objnum);

        model->m_bonepos.resize(model->m_framenum * model->m_rbonenum);
        model->m_dbonepos.resize(model->m_framenum * model->m_rbonenum);

        model->m_bonerot.resize(model->m_framenum * model->m_bonenum);
        model->m_dbonerot.resize(model->m_framenum * model->m_bonenum);

        fread(model->m_keyframes.data(), sizeof(KeyFrame), model->m_framenum * model->m_objnum, file);
        fread(model->m_bonepos.data(), sizeof(vec3), model->m_framenum * model->m_rbonenum, file);
        fread(model->m_bonerot.data(), sizeof(vec3), model->m_framenum * model->m_bonenum, file);

        model->calculateAnimDeltas();
    }

    //Read collision info
    if (header.cplgnum)
    {
        model->m_polygons.resize(header.cplgnum);

        for (int p = 0; p < header.cplgnum; p++)
        {
            unsigned short vnum;

            fread(&vnum, sizeof(short), 1, file);

            model->m_polygons[p].verts.resize(vnum);

            fread(model->m_polygons[p].verts.data(), sizeof(vec3), vnum, file);

            vec3 a = model->m_polygons[p].verts[1] - model->m_polygons[p].verts[0];
            vec3 b = model->m_polygons[p].verts[vnum - 1] - model->m_polygons[p].verts[0];

            model->m_polygons[p].plane.xyz = a ^ b;
            model->m_polygons[p].plane.xyz.normalize();
            model->m_polygons[p].plane.w = -(model->m_polygons[p].plane.xyz * model->m_polygons[p].verts[0]);
        }
    }

    //prepare bone data
    if (model->m_bonenum > 0)
    {
        model->m_boneparams.resize(header.bonenum);

        model->calculateBoneData();
        model->calculateCoreBBox();
    }

    fclose(file);

    model->m_vertexBuffer.setData(model->m_vertices.data(), model->m_vertices.size());
    model->m_indexBuffer.setData(model->m_indices.data(), model->m_indices.size());

    return model;
}

void Model::calculateBBox()
{
    m_bbox.min = { 0, 0, 0 };
    m_bbox.max = { 0, 0, 0 };

    for (const auto& vert : m_vertices)
    {
        if (vert.position.x > m_bbox.max.x) m_bbox.max.x = vert.position.x;
        if (vert.position.y > m_bbox.max.y) m_bbox.max.y = vert.position.y;
        if (vert.position.z > m_bbox.max.z) m_bbox.max.z = vert.position.z;

        if (vert.position.x < m_bbox.min.x) m_bbox.min.x = vert.position.x;
        if (vert.position.y < m_bbox.min.y) m_bbox.min.y = vert.position.y;
        if (vert.position.z < m_bbox.min.z) m_bbox.min.z = vert.position.z;
    }
}

void Model::calculateCoreBBox()     //BBox of the core/body of articulated figure
{
    m_corebbox.min = { 0, 0, 0 };
    m_corebbox.max = { 0, 0, 0 };

    for (size_t i = 0; i < m_vertices.size(); i++)
    {
        if (m_boneid[i] != -1) continue;

        const vec3& vert = m_vertices[i].position;

        if (vert.x > m_corebbox.max.x) m_corebbox.max.x = vert.x;
        if (vert.y > m_corebbox.max.y) m_corebbox.max.y = vert.y;
        if (vert.z > m_corebbox.max.z) m_corebbox.max.z = vert.z;

        if (vert.x < m_corebbox.min.x) m_corebbox.min.x = vert.x;
        if (vert.y < m_corebbox.min.y) m_corebbox.min.y = vert.y;
        if (vert.z < m_corebbox.min.z) m_corebbox.min.z = vert.z;
    }
}

void Model::calculateBoneData()
{
    for (size_t i = 0; i < m_bonenum; i++)
    {
        m_bbase[i] = mat4(m_bonespace[i].mat, m_bonespace[i].pos);
        m_bbase_inv[i] = mat4(m_bonespace[i].mat, m_bonespace[i].pos).inverse();
    }

    for (int v = 0; v < m_vertices.size(); v++)
    {
        uint32_t bid = m_boneid[v];

        //vec3 dvert = (bid != -1) ? m_bbase[bid] * m_vertices[v].position : m_vertices[v].position;

        if (bid != -1)
        {
            vec3 dvert = m_bbase_inv[bid] * m_vertices[v].position;

            if (dvert.y > 0.0f && dvert.y < m_bones[bid].length)
            {
                float length = dvert.y;
                float width = sqrt(dvert.x * dvert.x + dvert.z * dvert.z);

                m_boneparams[bid].length = std::max(m_boneparams[bid].length, length);
                m_boneparams[bid].width = std::max(m_boneparams[bid].width, width);
            }
        }
    }
}

void Model::calculateAnimDeltas()
{
    static constexpr float frame_rate = 0.125;

    float animSpeed = 1.0 / frame_rate;

    // Meshes
    for (int m = 0; m < m_objnum; m++)
    {
        for (unsigned int f = 0; f < (m_framenum - 1); f++)
        {
            KeyFrame& frame = m_keyframes[m_objnum * f + m];
            KeyFrame& frame_next = m_keyframes[m_objnum * (f + 1) + m];

            KeyFrame& dframe = m_dframes[m_objnum * f + m];

            bool hide = frame.flags & KeyFrame::keyframe_hide;
            bool hide_next = frame_next.flags & KeyFrame::keyframe_hide;

            bool visible = !hide && !hide_next;

            uint32_t flags = visible ? 0 : 1;

            dframe.flags = flags;
            dframe.pos = (frame_next.pos - frame.pos) * animSpeed;
            dframe.rot = (frame_next.rot - frame.rot) * animSpeed;
        }

        KeyFrame& kframe = m_keyframes[m_objnum * (m_framenum - 1) + m];
        KeyFrame& dframe = m_dframes[m_objnum * (m_framenum - 1) + m];

        dframe.flags = kframe.flags;
        dframe.pos = { 0,0,0 };
        dframe.rot = { 0,0,0 };
    }

    if (m_bonenum == 0) return;

    // Root bones
    for (unsigned int f = 0; f < (m_framenum - 1); f++)
    {
        vec3* pos = m_bonepos.data() + f * m_rbonenum;
        vec3* pos_next = m_bonepos.data() + (f + 1) * m_rbonenum;
        vec3* pos_delta = m_dbonepos.data() + f * m_rbonenum;

        for (int b = 0; b < m_rbonenum; b++)
        {
            pos_delta[b] = pos_next[b] - pos[b];
            pos_delta[b] = pos_delta[b] * animSpeed;
        }
    }

    // Bones
    for (unsigned int f = 0; f < (m_framenum - 1); f++)
    {
        vec3* rot = m_bonerot.data() + f * m_bonenum;
        vec3* rot_next = m_bonerot.data() + (f + 1) * m_bonenum;
        vec3* rot_delta = m_dbonerot.data() + f * m_bonenum;

        for (int b = 0; b < m_bonenum; b++)
        {
            rot_delta[b] = rot_next[b] - rot[b];
            rot_delta[b] = rot_delta[b] * animSpeed;
        }
    }
}

void Model::prepareSkeletalData()
{
    if (!m_boneIdBuffer)
    {
        m_boneIdBuffer.setData(m_boneid.data(), m_boneid.size());
        m_vertexBuffer.setAccessFlags(true, false);
    }
}