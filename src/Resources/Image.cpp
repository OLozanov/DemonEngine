#include "Image.h"

void downsample(uint8_t* in, uint8_t* out, int width, int height)
{
    auto in_pixel = [in, width, height](int x, int y) -> uint8_t*
    {
        return in + (y * width + x) * 4;
    };

    auto out_pixel = [out, width = width / 2, height = height / 2](int x, int y)->uint8_t*
    {
        return out + (y * width + x) * 4;
    };

    for (int y = 0; y < height / 2; y++)
    {
        for (int x = 0; x < width / 2; x++)
        {
            float r = in_pixel(x * 2, y * 2)[0] + in_pixel(x * 2 + 1, y * 2)[0] +
                in_pixel(x * 2, y * 2 + 1)[0] + in_pixel(x * 2 + 1, y * 2 + 1)[0];

            float g = in_pixel(x * 2, y * 2)[1] + in_pixel(x * 2 + 1, y * 2)[1] +
                in_pixel(x * 2, y * 2 + 1)[1] + in_pixel(x * 2 + 1, y * 2 + 1)[1];

            float b = in_pixel(x * 2, y * 2)[2] + in_pixel(x * 2 + 1, y * 2)[2] +
                in_pixel(x * 2, y * 2 + 1)[2] + in_pixel(x * 2 + 1, y * 2 + 1)[2];

            float a = in_pixel(x * 2, y * 2)[3] + in_pixel(x * 2 + 1, y * 2)[3] +
                in_pixel(x * 2, y * 2 + 1)[3] + in_pixel(x * 2 + 1, y * 2 + 1)[3];

            out_pixel(x, y)[0] = r * 0.25;
            out_pixel(x, y)[1] = g * 0.25;
            out_pixel(x, y)[2] = b * 0.25;
            out_pixel(x, y)[3] = a * 0.25;
        }
    }
}

void BuildMipMaps(Image* image)
{
    int width = image->width;
    int height = image->height;

    for (int i = 0; i < image->mipmaps - 1; i++)
    {
        size_t size = (width / 2) * (height / 2) * 4;

        image->data[i + 1] = new uint8_t[size];
        image->size[i + 1] = size;

        downsample(image->data[i], image->data[i + 1], width, height);

        width /= 2;
        height /= 2;
    }
}