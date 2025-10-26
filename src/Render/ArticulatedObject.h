#pragma once

#include "Render/Render.h"

#include "Render/DisplayObject.h"
#include "Resources/Model.h"

namespace Render
{

class SkeletalData : public ListNode<SkeletalData>
{
protected:
	VertexBuffer m_vertexBuffer;
	Render::StreamBuffer<mat4> m_boneBuffer;

	UINT m_inputBuffer;
	UINT m_boneIdBuffer;

    std::vector<mat4> m_bspace;

    BBox m_vertsBBox;

    bool m_update;

public:

    bool isUpdateRequired() { return m_update; }
    void setUpdated() { m_update = false; }

	UINT inputBuffer() { return m_inputBuffer; }
	VertexBuffer& outputBuffer() { return m_vertexBuffer; }
	UINT boneIdBuffer() { return m_boneIdBuffer; }
	UINT boneBuffer() { return m_boneBuffer; }

	size_t vertexNum() { return m_vertexBuffer.size(); }
};

class ArticulatedObject : public DisplayObject, public SkeletalData
{
protected:
	ResourcePtr<Model> m_model;

	float m_frame_rate;

	bool m_run;
	bool m_repeat;

	unsigned short m_frame;
	float m_time;
	float m_dtime;

	unsigned short m_start_frame;
	unsigned short m_end_frame;

	std::vector<mat4> m_omat;

	const vec3* m_rot;
	const vec3* m_drot;

	void initGeometryData();
	void calcFrameBones();
	void animateMesh(int num);

public:

	static constexpr float default_frame_rate = 0.125;

public:

	ArticulatedObject();
	ArticulatedObject(Model* model, float frate = default_frame_rate);
	~ArticulatedObject();

	void setModel(Model* model, float frate);

	void run() { m_run = true; }
	void stop() { m_run = false; }
	bool isRunning() { return m_run; }

	const mat4& getSubmeshTransform(int n) { return m_omat[n]; }
	const mat4& getBoneTransform(int n) { return m_bspace[n] * mat4::Translate(m_model->bonepos(n)); }

	void setFrame(unsigned short kframe);
	void setAnimRange(unsigned short start, unsigned short end);
	void startAnimation();

	void setRepeat(bool rep) { m_repeat = rep; }

	float getAnimTime() { return m_time; }
	void setAnimTime(float time);

	void animate(float dt);
};

} //namespace render