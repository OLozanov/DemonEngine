#pragma once

#include "Render/Image.h"
#include "Resources/Resource.h"

struct Image : public Resource
{
	int width;
	int height;

	int depth;
	int faces;
	int mipmaps;

	ImageFormat format;

	int	 elemsz;
	bool isAlpha;

	long * size = nullptr;
	unsigned char ** data = nullptr;

	ImageBuffer* buffer = nullptr;
	ImageHandle handle = 0;

	~Image()
	{
		if (!data) return;

		int num = faces*mipmaps;
		for (int i = 0; i < num; i++) delete [] data[i];

		delete [] size;
		delete [] data;

		if (buffer) buffer->Release();
		if (handle)	FreeHandle(handle);
	}
};

void BuildMipMaps(Image* image);
