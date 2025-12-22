#pragma once

#include "Resources/Resource.h"
#include "math/math3d.h"

#include "Render/DisplayData.h"
#include "Collision/Collision.h"

#include "Render/Render.h"

#include <vector>
#include "stdint.h"

struct MshHeader
{
    uint32_t signature;
    uint32_t vertnum;
    uint32_t trinum;
    uint16_t meshnum;
    uint16_t objnum;
    uint16_t bonenum;
    uint16_t keyframes;
    uint16_t constraints;
    uint16_t brnum;
    uint16_t cplgnum;
    uint16_t pad;
};

struct KeyFrame
{
    enum Flag
    {
        keyframe_hide = 1
    };

    uint32_t flags;
    vec3 pos;
    vec3 rot;
};

struct Bone
{
    float length;

    uint16_t parent;

    uint16_t cfirst;
    uint16_t clast;

    uint16_t constraint;
};

struct BoneSpace
{
    mat3 mat;
    vec3 pos;
};

struct BoneConstraint
{
    float min[3];
    float max[3];
};

struct BoneRoot
{
    uint32_t bid;
    mat4 basis;
    vec3 pos;
};

struct BoneParameters
{
    float length = 0.0f;
    float width = 0.0f;
};

class Model : public Resource
{
    Model() {}

    static const uint32_t MeshSignature = 0x3448534D;   // 'MSH4'

public:

    static constexpr uint16_t InvalidBoneIndex = -1;

public:

    ~Model();
    static Model* LoadModel(const char* name);

    const std::vector<Render::Vertex>& vertices() const { return m_vertices; }
    const std::vector<uint16_t>& indices() const { return m_indices; }
    const std::vector<Render::DisplayData>& meshes() const { return m_meshes; }
    const std::vector<uint16_t>& objectMeshes() const { return m_objmeshes; }
    const BBox& bbox() const { return m_bbox; }
    const BBox& corebbox() const { return m_corebbox; }
    const Render::VertexBuffer& vertexBuffer() const { return m_vertexBuffer; }
    const Render::IndexBuffer& indexBuffer() const { return m_indexBuffer; }

    void prepareSkeletalData();

    uint16_t vertexnum() const { return m_vertices.size(); }
    uint16_t meshnum() const { return m_meshnum; }
    uint16_t objnum() const { return m_objnum; }
    uint16_t rbonenum() const { return m_rbonenum; }
    uint16_t bonenum() const { return m_bonenum; }
    uint16_t framenum() const { return m_framenum; }

    const std::vector<Bone>& bones() const { return m_bones; }
    const std::vector<BoneRoot>& boneRoots() const { return m_boneRoots; }

    const std::vector<KeyFrame>& keyframes() const { return m_keyframes; }

    const BoneRoot& boneRoot(int br) const { return m_boneRoots[br]; }
    const Bone& bone(size_t b) const { return m_bones[b]; }
    const mat3& bonemat(size_t b) const { return m_bonespace[b].mat; }
    const vec3& bonepos(size_t b) const { return m_bonespace[b].pos; }
    const mat4& bonebasis(size_t b) const { return m_bbase[b]; }
    const mat4& invbonebasis(size_t b) const { return m_bbase_inv[b]; }
    const BoneParameters& boneParameters(size_t b) const { return m_boneparams[b]; }

    const BoneConstraint& constraint(size_t i) const { return m_constraints[i]; }

    const vec3* bonespos(int frame) const { return m_bonepos.data() + frame * m_rbonenum; }
    const vec3* dbpos(int frame) const { return m_dbonepos.data() + frame * m_rbonenum; }

    const vec3* bonesrot(int frame) const { return m_bonerot.data() + frame * m_bonenum; }
    const vec3* dbrot(int frame) const { return m_dbonerot.data() + frame * m_bonenum; }

    const KeyFrame& keyframe(uint32_t frame, uint32_t obj) const { return m_keyframes[m_objnum * frame + obj]; }
    const KeyFrame& dframe(uint32_t frame, uint32_t obj) const { return m_dframes[m_objnum * frame + obj]; }

    UINT boneIdBuffer() { return m_boneIdBuffer; }

    const std::vector<Collision::CollisionPolygon>& collisionData() { return m_polygons; }

private:

    void calculateBBox();
    void calculateCoreBBox();
    void calculateBoneData();
    void calculateAnimDeltas();

    BBox m_bbox;
    BBox m_corebbox;

    uint16_t m_meshnum;
    uint16_t m_objnum;
    uint16_t m_rbonenum;
    uint16_t m_bonenum;
    uint16_t m_framenum;

    std::vector<Render::DisplayData> m_meshes;

    // Geometry
    std::vector<Render::Vertex> m_vertices;
    std::vector<uint16_t> m_indices;
    std::vector<uint16_t> m_objmeshes;

    // Bones
    std::vector<uint32_t> m_boneid;
    std::vector<Bone> m_bones;
    std::vector<BoneSpace> m_bonespace;
    std::vector<BoneConstraint> m_constraints;
    std::vector<BoneParameters> m_boneparams;
    std::vector<BoneRoot> m_boneRoots;

    // Animation data
    std::vector<KeyFrame> m_keyframes;
    std::vector<KeyFrame> m_dframes;

    std::vector<vec3> m_bonepos;
    std::vector<vec3> m_dbonepos;

    std::vector<vec3> m_bonerot;
    std::vector<vec3> m_dbonerot;

    // rest bone matrices
    std::vector<mat4> m_bbase;
    std::vector<mat4> m_bbase_inv;

    // Skeletal data
    Render::Buffer<uint32_t> m_boneIdBuffer;

    std::vector<Collision::CollisionPolygon> m_polygons;
    
    Render::VertexBuffer m_vertexBuffer;
    Render::IndexBuffer m_indexBuffer;
};