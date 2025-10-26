#pragma once

#include "Render/DisplayObject.h"
#include "Resources/Model.h"

namespace Render
{

struct AnimationRange
{
    uint16_t start;
    uint16_t end;
};

class AnimatedObject : public DisplayObject
{
protected:
    ResourcePtr<Model> m_model;

    void initData();

public:

    AnimatedObject() {}
    AnimatedObject(Model* model);
    AnimatedObject(const vec3& pos, const vec3& rot, Model* model = nullptr);
    AnimatedObject(const vec3& pos, const mat3& mat, Model* model = nullptr);

    ~AnimatedObject() {}

    void run() { m_run = true; }
    void stop() { m_run = false; }
    bool isRunning() { return m_run; }

    void run(float time);

    void setFrameRate(float rate) { m_frame_rate = rate; }

    void setFrame(unsigned short kframe);
    void setAnimRange(unsigned short start, unsigned short end);
    void setAnimRange(const AnimationRange& range);

    void setRepeat(bool rep) { m_repeat = rep; }
    bool isRepeated() { return m_repeat; }

    float getAnimTime() { return m_time; }
    void setTime(float time);

    void animate(float dt);

private:

    void setObjectsFlags();
    void animateObject(int num);

private:

    std::vector<mat4> m_objmat;
    std::vector<uint32_t> m_objflags;
    std::vector<int> m_objnum;

    float m_frame_rate;

    bool m_run;
    bool m_repeat;

    unsigned short m_frame;
    float m_time;
    float m_dtime;

    unsigned short m_start_frame;
    unsigned short m_end_frame;

    static constexpr float default_frame_rate = 0.125;
};

} //namespace render