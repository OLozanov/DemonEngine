#pragma once

#include "stdint.h"

enum class DcmEntryType : uint32_t
{
    entry_editorinfo = 0xDEDE000E,
    entry_blocks = 0xDEDE000B,
    entry_mapinfo = 0xDEDE0000,
    entry_geometry = 0xDEDE0001,
    entry_leafs = 0xDEDE0002,
    entry_nodes = 0xDEDE0003,
    entry_portals = 0xDEDE0004,
    entry_zoneinfo = 0xDEDE0005,
    entry_objects = 0xDEDE0006,
    entry_entities = 0xDEDE0007,
    entry_editor_surfaces = 0xDEDE0008,
    entry_surfaces = 0xDEDE0009,
};

struct DcmEntry
{
    DcmEntryType type;
    uint32_t size;
    uint32_t offset;
};

struct DcmHeader
{
    uint32_t signature;
    uint8_t version;
    uint8_t entrynum;
    uint32_t eoffset;
};

constexpr uint32_t DcmSignature = 0x004D4344;
constexpr uint8_t DcmVersion = 200;