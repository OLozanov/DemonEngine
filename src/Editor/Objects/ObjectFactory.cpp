#include "ObjectFactory.h"

#include "Light.h"
#include "StaticObject.h"
#include "Sound.h"
#include "Trigger.h"
#include "PlayerStart.h"
#include "MapFinish.h"
#include "Ladder.h"
#include "FogVolume.h"

#include <string>

ObjectFactory::FactoryFunc ObjectFactory::Factories[] = { ObjectFactory::CreateOmniLight,
                                                          ObjectFactory::CreateStaticObject,
                                                          nullptr,
                                                          ObjectFactory::CreateSound,
                                                          ObjectFactory::CreateTrigger,
                                                          ObjectFactory::CreatePlayerStart,
                                                          ObjectFactory::CreateMapFinish,
                                                          ObjectFactory::CreateLadder,
                                                          ObjectFactory::CreateSpotLight,
                                                          ObjectFactory::CreateFogVolume };

Object* ObjectFactory::CreateOmniLight(FILE* file)
{
    vec3 pos;
    float radius;
    float falloff;
    float power;
    LightShadow shadow;
    bool dynamic;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&radius, sizeof(float), 1, file);
    fread(&falloff, sizeof(float), 1, file);
    fread(&power, sizeof(float), 1, file);
    fread(&shadow, sizeof(LightShadow), 1, file);

    uint8_t r;
    uint8_t g;
    uint8_t b;

    fread(&r, sizeof(uint8_t), 1, file);
    fread(&g, sizeof(uint8_t), 1, file);
    fread(&b, sizeof(uint8_t), 1, file);

    vec3 color = { r / 255.0f, g / 255.0f, b / 255.0f };

    OmniLight* light = new OmniLight(radius, falloff, power, color, shadow);
    light->setPos(pos);

    return light;
}

Object* ObjectFactory::CreateSpotLight(FILE* file)
{
    vec3 pos;
    vec3 dir;
    float radius;
    float falloff;
    float outerAngle;
    float innerAngle;
    float power;
    LightShadow shadow;
    bool dynamic;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&dir, sizeof(vec3), 1, file);
    fread(&radius, sizeof(float), 1, file);
    fread(&falloff, sizeof(float), 1, file);
    fread(&outerAngle, sizeof(float), 1, file);
    fread(&innerAngle, sizeof(float), 1, file);
    fread(&power, sizeof(float), 1, file);
    fread(&shadow, sizeof(LightShadow), 1, file);

    uint8_t r;
    uint8_t g;
    uint8_t b;

    fread(&r, sizeof(uint8_t), 1, file);
    fread(&g, sizeof(uint8_t), 1, file);
    fread(&b, sizeof(uint8_t), 1, file);

    vec3 color = { r / 255.0f, g / 255.0f, b / 255.0f };

    mat3 mat;
    mat[2] = dir;
    mat[0] = vec3::Orthogonal(dir);
    mat[1] = mat[0] ^ mat[2];

    SpotLight* light = new SpotLight(radius, falloff, outerAngle, innerAngle, power, color, shadow);
    light->setPos(pos);
    light->setOrientation(mat);

    return light;
}

Object* ObjectFactory::CreateStaticObject(FILE* file)
{
    vec3 pos;
    mat3 mat;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&mat[0], sizeof(vec3), 1, file);
    fread(&mat[1], sizeof(vec3), 1, file);

    mat[1] -= mat[0] * (mat[0] * mat[1]);   //ortagonize
    mat[2] = mat[0] ^ mat[1];

    std::string modelName;
    uint32_t len;

    fread(&len, sizeof(uint32_t), 1, file);
    modelName.resize(len);

    fread(modelName.data(), 1, len, file);

    uint8_t flags;
    fread(&flags, sizeof(uint8_t), 1, file);

    StaticObject* object = new StaticObject(modelName, flags & StaticObject::flag_collision, flags & StaticObject::flag_gi);
    object->setPos(pos);
    object->setOrientation(mat);

    return object;
}

Object* ObjectFactory::CreateSound(FILE* file)
{
    vec3 pos;
    float volume;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&volume, sizeof(float), 1, file);

    std::string source;

    uint32_t len;
    fread(&len, sizeof(uint32_t), 1, file);

    if (len > 0)
    {
        source.resize(len);
        fread(source.data(), 1, len, file);
    }

    AmbientSound* sound = new AmbientSound(volume, source);
    sound->setPos(pos);

    return sound;
}

Object* ObjectFactory::CreateTrigger(FILE* file)
{
    vec3 pos;
    vec3 size;
    uint16_t len;
    std::string id;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&size, sizeof(vec3), 1, file);

    fread(&len, sizeof(uint16_t), 1, file);

    if (len > 0)
    {
        id.resize(len);
        fread(id.data(), sizeof(char), len, file);
    }

    Trigger* trigger = new Trigger(pos, size, id);

    return trigger;
}

Object* ObjectFactory::CreatePlayerStart(FILE* file)
{
    vec3 pos;

    fread(&pos, sizeof(vec3), 1, file);

    PlayerStart* ps = new PlayerStart();
    ps->setPos(pos);

    return ps;
}

Object* ObjectFactory::CreateMapFinish(FILE* file)
{
    vec3 pos;
    vec3 size;

    std::string nextmap;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&size, sizeof(vec3), 1, file);

    uint16_t len;
    fread(&len, sizeof(uint16_t), 1, file);

    if (len > 0)
    {
        nextmap.resize(len);
        fread(nextmap.data(), 1, len, file);
    }

    MapFinish* mapFinish = new MapFinish(size, nextmap);
    mapFinish->setPos(pos);

    return mapFinish;
}

Object* ObjectFactory::CreateLadder(FILE* file)
{
    vec3 pos;
    float height;
    float ang;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&height, sizeof(float), 1, file);
    fread(&ang, sizeof(float), 1, file);

    Ladder* ladder = new Ladder();
    ladder->setPos(pos);
    ladder->setHeight(height);
    ladder->setAngle(ang);

    return ladder;
}

Object* ObjectFactory::CreateFogVolume(FILE* file)
{
    vec3 pos;
    vec3 size;
    vec3 color;
    float density;
    FogVolume::VolumeType type;
    bool lighting;

    fread(&pos, sizeof(vec3), 1, file);
    fread(&size, sizeof(vec3), 1, file);
    fread(&color, sizeof(vec3), 1, file);
    fread(&density, sizeof(float), 1, file);
    fread(&type, sizeof(uint8_t), 1, file);
    fread(&lighting, sizeof(bool), 1, file);

    FogVolume* fog = new FogVolume(pos, size, color, density, type, lighting);

    return fog;
}

Object* ObjectFactory::CreateObject(ObjectType type, FILE* file)
{
    uint8_t index = static_cast<uint8_t>(type);

    return Factories[index](file);
}