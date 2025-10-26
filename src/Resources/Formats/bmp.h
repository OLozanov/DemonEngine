#pragma once

#include "stdint.h"

enum class BmpCompression : uint32_t
{
    RGB = 0,
    RLE8 = 1,
    RLE4 = 2,
    BITFIELDS = 3,
    JPEG = 4,
    PNG = 5
};

#pragma pack(push, 1)
struct BmpFileHeader
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offBits;
};

struct BmpInfoHeader 
{
    uint32_t        size;
    uint32_t        width;
    uint32_t        height;
    uint16_t        planes;
    uint16_t        bitCount;
    BmpCompression  compression;
    uint32_t        sizeImage;
    uint32_t        xPelsPerMeter;
    uint32_t        yPelsPerMeter;
    uint32_t        clrUsed;
    uint32_t        clrImportant;
};
#pragma pack(pop)
