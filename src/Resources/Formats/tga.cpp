#include "tga.h"
#include "Formats.h"
#include "Resources/Image.h"

#include "stdio.h"
#include "string.h"

#include <iostream>

Image* LoadTga(const char* fname)
{
    FILE* file;

    //Open file
    errno_t error = fopen_s(&file, fname, "rb");

    if (error)
    {
        std::cout << "Can't open file " << fname << std::endl;
        return nullptr;
    }

    Image* img = nullptr;

    TgaHeader header;

    fread(&header, sizeof(TgaHeader), 1, file);

    if (header.dataType == TgaDataType::Rgb)
    {
        if (header.bitsPerPixel != 32 && header.bitsPerPixel != 24)
        {
            fclose(file);
            return nullptr;
        }

        img = new Image;

        img->width = header.width;
        img->height = header.height;
        img->depth = 0;

        img->mipmaps = 1;
        img->faces = 1;

        img->elemsz = 4;

        if (header.bitsPerPixel == 24)
            img->format = FORMAT_B8G8R8X8_UNORM;

        if (header.bitsPerPixel == 32)
            img->format = FORMAT_B8G8R8A8_UNORM;

        size_t size = img->width * img->height * 4;

        img->data = new uint8_t*[1];
        img->size = new long[1];

        img->data[0] = new uint8_t[size];

        if (header.idLength) fseek(file, header.idLength, SEEK_CUR);

        if (header.bitsPerPixel == 32)
        {
            fread(img->data[0], 1, size, file);
        }
        else
        {
            for (size_t i = 0; i < header.width * header.height; i++) fread(img->data[0] + i * 4, 3, 1, file);
        }
    }

    fclose(file);

    return img;
}