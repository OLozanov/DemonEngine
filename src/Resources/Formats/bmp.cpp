#include "bmp.h"

#include "Resources/Image.h"
#include <iostream>

#include <algorithm>

#undef min

Image* LoadBmp(const char* fname)
{
	FILE* file;

	//Open file
	errno_t error = fopen_s(&file, fname, "rb");

	if (error)
	{
		std::cout << "Can't open file " << fname << std::endl;
		return nullptr;
	}

	BmpFileHeader fileHeader;
	BmpInfoHeader infoHeader;

	fread(&fileHeader, sizeof(BmpFileHeader), 1, file);
	if (fileHeader.type != 0x4d42)
	{
		std::cout << "LoadBMP: incorrect header type" << std::endl;
		fclose(file);

		return nullptr;
	}

	size_t infoSz = fileHeader.offBits - sizeof(BmpFileHeader);

	fread(&infoHeader, std::min(infoSz, sizeof(BmpInfoHeader)), 1, file);

	if (infoSz > sizeof(BmpInfoHeader)) fseek(file, fileHeader.offBits, SEEK_SET);

	if (infoHeader.bitCount != 32 && infoHeader.bitCount != 24)
	{
		fclose(file);
		return nullptr;
	}

	if (infoHeader.compression != BmpCompression::RGB && 
		infoHeader.compression != BmpCompression::BITFIELDS)
	{
		fclose(file);
		return nullptr;
	}

	Image* img = new Image;

	img->width = infoHeader.width;
	img->height = infoHeader.height;
	img->depth = 0;

	img->mipmaps = log2(std::min(infoHeader.width, infoHeader.height));
	img->faces = 1;

	img->format = infoHeader.bitCount == 32 ? FORMAT_B8G8R8A8_UNORM : FORMAT_B8G8R8X8_UNORM;
	img->elemsz = 4;

	img->data = new uint8_t*[img->mipmaps];
	img->size = new long[img->mipmaps];

	size_t size = infoHeader.width * infoHeader.height * sizeof(uint32_t);

	img->data[0] = new uint8_t[size];
	img->size[0] = size;

	if (infoHeader.bitCount == 32)
	{
		fread(img->data[0], 1, size, file);
	}
	else
	{
		for (size_t i = 0; i < infoHeader.width * infoHeader.height; i++) fread(img->data[0] + i * 4, 3, 1, file);
	}

	BuildMipMaps(img);

	fclose(file);

	return img;
}