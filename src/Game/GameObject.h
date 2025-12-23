#pragma once

#include "math/math3d.h"
#include "Utils/LinkedList.h"
#include "Utils/EventHandler.h"
#include <string>

class FileStream;

namespace GameLogic
{

class Hitable : public ListNode<Hitable>
{
public:
    Hitable();
    virtual ~Hitable();

    virtual const vec3& center() const { return vec3(); }
    virtual void hit(const vec3& point, const vec3& direction, uint32_t power) {}

    static void ClearObjects();
    static void AddObject(Hitable* object);
    static void Impact(const vec3& center, uint32_t power);

private:
    static LinkedList<Hitable> ObjectList;

    static constexpr float DistFactor = 0.05f;
    static constexpr float PowerFactor = 0.5f;
};

class Pickable : public ListNode<Pickable>
{
public:
    using PickupEvent = Event<void()>;

    Pickable(float size, const vec3& pos);
    Pickable(const vec3& bbox, const vec3& pos);
    virtual ~Pickable();

    void test(const vec3& pos, const vec3& bbox);

    static void ClearObjects();
    static void TestObjects(const vec3& pos, const vec3& bbox);

    PickupEvent OnPickup;

protected:
    vec3 m_pos;
    vec3 m_bbox;

    bool m_triggered;

private:
    static LinkedList<Pickable> ObjectList;
};

class Usable
{
public:

    Usable() {}
    virtual ~Usable() {}

    virtual void use() = 0;
};

class GameObject : public ListNode<GameObject>
{
protected:

    std::string m_id;

public:

    GameObject() {}
    GameObject(const std::string& id) : m_id(id) {}

    virtual ~GameObject() {}

    virtual void update(float dt) = 0;
    virtual void activate() {}

    virtual uint32_t type() { return 0; }
    virtual void save(FileStream& file);
    virtual void restore(FileStream& file);

    const std::string& id() { return m_id; }
};

} // namespace gamelogic