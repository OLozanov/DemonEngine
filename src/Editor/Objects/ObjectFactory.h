#pragma once

#include <stdint.h>
#include "Object.h"

class ObjectFactory
{
    ObjectFactory() = delete;
    ~ObjectFactory() = delete;

    using FactoryFunc = Object* (*)(FILE* file);

    static FactoryFunc Factories[];

    static Object* CreateOmniLight(FILE* file);
    static Object* CreateSpotLight(FILE* file);
    static Object* CreateStaticObject(FILE* file);
    static Object* CreateSound(FILE* file);
    static Object* CreateTrigger(FILE* file);
    static Object* CreatePlayerStart(FILE* file);
    static Object* CreateMapFinish(FILE* file);
    static Object* CreateLadder(FILE* file);
    static Object* CreateFogVolume(FILE* file);

public:

   static Object* CreateObject(ObjectType type, FILE* file);
};