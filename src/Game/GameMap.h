#pragma once

#include "Render/DisplayData.h"
#include "stdint.h"

enum PolyType
{
    PolyTransparent = 1,
    PolyNoclip = 2,
    PolyTwoside = 4,
    PolyZoneportal = 8,
    PolyInvisible = 0x10,
    PolySky = 0x20,
    PolyWater = 0x40
};

struct Poly
{
    uint32_t flags;

    vec4 plane;

    std::vector<Render::Vertex> verts;

    //Material* mat;
    std::string texname;
};

enum class MapEntryType : uint32_t
{
    entry_blocks = 0xDCDC000B,
    entry_leafs = 0xDCDC0001,
    entry_bsp = 0xDCDC0002,
    entry_portals = 0xDCDC0003,
    entry_zoneinfo = 0xDCDC0004,
    entry_objects = 0xDCDC0006,
    entry_surface = 0xDCDC0007,
};

enum class ObjectType : uint8_t
{
    object_omni_light = 0,
    object_static = 1,
    object_sound = 3,
    object_trigger = 4,
    object_player_start = 5,
    object_map_finish = 6,
    object_ladder = 7,
    object_spot_light = 8,
    object_fog_volume = 9,
    object_endlist = 0xFF
};

enum class EntityClass : uint16_t
{
    Crate = 0,
    Barrel = 1,
    SlideDoor = 10,
    Mover = 11,
    Animated = 20,
    SteamEmitter = 30,
    TechSwitch = 50,
    Medkit = 100,
    Vehicle = 200,
    Trooper = 500,
    Invalid = 0xFFFF
};

enum class SurfaceTopology : uint8_t
{
    Rectangle = 0,
    Triangle = 1,
    Invalid = 0xFF
};

struct GameMapEntry
{
    MapEntryType type;
    uint32_t size;
    uint32_t offset;
};

struct GameMapHeader
{
    uint32_t signature;
    uint8_t version;
    uint8_t entrynum;
    uint32_t eoffset;
};

//constexpr uint32_t DcmSignature = 0x004D4344;
//constexpr uint8_t DcmVersion = 100;