#include "dds.h"
#include "Formats.h"
#include "Resources/Image.h"

#include "stdio.h"
#include "string.h"

#include <iostream>

Image * LoadDds(const char * fname)
{
    FILE 	*file;
    char 	sig[4];		//signature

    //Open file
    errno_t error = fopen_s(&file, fname, "rb");
    
    if(error)
    {
        std::cout << "Can't open file " << fname << std::endl;
        return nullptr;
    }

    //Read DDS signature
    fread(sig, 1, 4, file);
    if(strncmp(sig, "DDS ", 4) )
    {
        fclose(file);
        return nullptr;
    }

    Image * img = new Image;

    //Read header
    DDS_HEADER header;
    fread(&header, sizeof(DDS_HEADER), 1, file);

    //Volume texture?
    if( (header.dwCaps2 & DDSF_VOLUME) && (header.dwDepth > 0) ) img->depth = header.dwDepth;
    else img->depth = 1;

    img->width  = header.dwWidth;
    img->height = header.dwHeight;

    //MipMaps
    if(header.dwFlags & DDSF_MIPMAPCOUNT) img->mipmaps = header.dwMipMapCount;
    else img->mipmaps = 1;

    //Cube-maps
    if(header.dwCaps2 & DDSF_CUBEMAP)
    {
        img->faces = 0;

        if (header.dwCaps2 & DDSF_CUBEMAP_POSITIVEX) img->faces++;
        if (header.dwCaps2 & DDSF_CUBEMAP_NEGATIVEX) img->faces++;
        if (header.dwCaps2 & DDSF_CUBEMAP_POSITIVEY) img->faces++;
        if (header.dwCaps2 & DDSF_CUBEMAP_NEGATIVEY) img->faces++;
        if (header.dwCaps2 & DDSF_CUBEMAP_POSITIVEZ) img->faces++;
        if (header.dwCaps2 & DDSF_CUBEMAP_NEGATIVEZ) img->faces++;

        if( (img->faces != 6) || (img->width != img->height) )
        {
            fclose(file);
            return nullptr;
        }

    } else img->faces = 1;

    int elemsz = 0;
    bool compressed = false;

    //Image format
    if(header.ddspf.dwFlags & DDSF_FOURCC)
    {
        switch(header.ddspf.dwFourCC)
        {
            case FOURCC_DXT1:
                img->format = FORMAT_BC1_UNORM;
                elemsz = 8;
                compressed = true;
            break;

            case FOURCC_DXT2:
            case FOURCC_DXT3:
                img->format = FORMAT_BC2_UNORM;
                elemsz = 16;
                compressed = true;
            break;

            case FOURCC_DXT4:
            case FOURCC_DXT5:
                img->format = FORMAT_BC3_UNORM;
                elemsz = 16;
                compressed = true;
            break;

            case FOURCC_ATI1:
                img->format = FORMAT_BC4_UNORM;
                elemsz = 8;
                compressed = true;
            break;

            case FOURCC_ATI2:
                img->format = FORMAT_BC5_UNORM;
                elemsz = 16;
                compressed = true;
            break;

            case FOURCC_R8G8B8:
                img->format = FORMAT_R8G8B8A8_UNORM;
                elemsz = 3;
            break;

            case FOURCC_A8R8G8B8:
                img->format = FORMAT_R8G8B8A8_UNORM;
                elemsz = 4;
            break;

            case FOURCC_X8R8G8B8:
                img->format = FORMAT_B8G8R8X8_UNORM;
                elemsz = 4;
            break;

            case FOURCC_R5G6B5:
                img->format = FORMAT_B5G6R5_UNORM;
                elemsz = 2;
            break;

            case FOURCC_A8:
                img->format = FORMAT_A8_UNORM;
                elemsz = 1;
            break;

            case FOURCC_A2B10G10R10:
                img->format = FORMAT_R10G10B10A2_UNORM;
                elemsz = 4;
            break;

            case FOURCC_A8B8G8R8:
                img->format = FORMAT_R8G8B8A8_UNORM;
                elemsz = 4;
            break;

            case FOURCC_X8B8G8R8:
                img->format = FORMAT_B8G8R8X8_UNORM;
                elemsz = 4;
            break;

            case FOURCC_A2R10G10B10:
                img->format = FORMAT_R10G10B10A2_UNORM;
                elemsz = 4;
            break;

            case FOURCC_A16B16G16R16:
                img->format = FORMAT_R16G16B16A16_UNORM;
                elemsz = 8;
            break;

            case FOURCC_L8:
                img->format = FORMAT_R8_UNORM;
                elemsz = 1;
            break;

            case FOURCC_A8L8:
                img->format = FORMAT_R8G8_UNORM;
                elemsz = 2;
            break;

            case FOURCC_L16:
                img->format = FORMAT_R16_UNORM;
                elemsz = 2;
            break;

            case FOURCC_R16F:
                img->format = FORMAT_R16_FLOAT;
                elemsz = 2;
            break;

            case FOURCC_A16B16G16R16F:
                img->format = FORMAT_R16G16B16A16_FLOAT;
                elemsz = 8;
            break;

            case FOURCC_R32F:
                img->format = FORMAT_R32_FLOAT;
                elemsz = 4;
            break;

            case FOURCC_A32B32G32R32F:
                img->format = FORMAT_R32G32B32A32_FLOAT;
                elemsz = 16;
            break;

            case FOURCC_UNKNOWN:
            case FOURCC_X1R5G5B5:
            case FOURCC_A1R5G5B5:
            case FOURCC_A4R4G4B4:
            case FOURCC_R3G3B2:
            case FOURCC_A8R3G3B2:
            case FOURCC_X4R4G4B4:
            case FOURCC_A4L4:
            case FOURCC_D16_LOCKABLE:
            case FOURCC_D32:
            case FOURCC_D24X8:
            case FOURCC_D16:
            case FOURCC_D32F_LOCKABLE:
            case FOURCC_G16R16:
            case FOURCC_G16R16F:
            case FOURCC_G32R32F:
                //these are unsupported for now
            default:
                fclose(file);
                return nullptr;
        }
    }
    else if(header.ddspf.dwFlags == DDSF_RGBA && header.ddspf.dwRGBBitCount == 32)
    {
        img->format = FORMAT_B8G8R8A8_UNORM;
        elemsz = 4;
    }
    else if(header.ddspf.dwFlags == DDSF_RGB  && header.ddspf.dwRGBBitCount == 32)
    {
        img->format = FORMAT_R8G8B8A8_UNORM;
        elemsz = 4;
    }
    else if(header.ddspf.dwFlags == DDSF_RGB  && header.ddspf.dwRGBBitCount == 24)
    {
        img->format = FORMAT_B8G8R8X8_UNORM;
        elemsz = 3;
    }
    else if(header.ddspf.dwFlags == DDSF_ALPHA)
    {
        img->format = FORMAT_A8_UNORM;
        img->isAlpha = true;
        elemsz = 1;
    }
    else if(header.ddspf.dwRGBBitCount == 8)
    {
        img->format = FORMAT_R8_UNORM;
        elemsz = 1;
    }
    else
    {
        fclose(file);
        return nullptr;
    }

    img->elemsz = elemsz;

    img->data = new PUCHAR[img->mipmaps*img->faces];
    img->size = new long[img->mipmaps*img->faces];

    if (elemsz == 3) img->elemsz = 4;

    //Read image
    int ind = 0;

    for (int face = 0; face < img->faces; face++)
    {
        int w = img->width;
        int h = img->height;
        int d = img->depth;

        for(int mm = 0; mm < img->mipmaps; mm++)
        {
            int bw;
            int bh;

            if(compressed)
            {
                bw = (w + 3)/4;
                bh = (h + 3)/4;

            }else { bw = w; bh = h; }

            int size = bw*bh*d*img->elemsz;

            img->data[ind] = new unsigned char[size];
            img->size[ind] = size;

            if (elemsz == 3)
            {
                for (int k = 0; k < bh; k++)
                {
                    for (int i = 0; i < bw; i++)
                    {
                        size_t offset = (k * bw + i) * img->elemsz;
                        fread(img->data[ind] + offset, elemsz, 1, file);
                    }
                }
            
            } else fread(img->data[ind], size, 1, file);

            //Reduce MipMap size
            w = w >> 1;
            h = h >> 1;
            //d = d >> 1;

            if (w == 0) w = 1;
            if (h == 0) h = 1;

            ind++;
        }

    }

    fclose(file);

    return img;
}

unsigned char * DXT1Decode(int width, int height, DXTColBlock * data)
{
    int w = width >> 2;
    int h = height >> 2;

    DXTColBlock * dxt = data;
    unsigned char * rgb = new unsigned char[width*height*3];

    RGBColBlock a, b, c, d;

    for(int k = 0; k < h; k++)
        for(int i = 0; i < w; i++)
        {
            a.B = (dxt->col0 & 31) << 3;
            a.G = ( (dxt->col0 >> 5) & 63) << 2;
            a.R = ( (dxt->col0 >> 11) & 31) << 3;

            b.B = (dxt->col1 & 31) << 3;
            b.G = ( (dxt->col1 >> 5) & 63) << 2;
            b.R = ( (dxt->col1 >> 11) & 31) << 3;

            c.B = (a.B << 1)/3 + b.B/3;
            c.G = (a.G << 1)/3 + b.G/3;
            c.R = (a.R << 1)/3 + b.R/3;

            d.B = a.B/3 + (b.B << 1)/3;
            d.G = a.G/3 + (b.G << 1)/3;
            d.R = a.R/3 + (b.R << 1)/3;

            unsigned char * col = rgb + ( (i << 2) + (k << 2)*width)*3;

            for(int m = 0; m < 4; m++)
            {
                unsigned char  row = dxt->row[m];

                for(int l = 0; l < 4; l++)
                {
                    switch(row & 3)
                    {
                        case 0:
                            col[0] = a.R;
                            col[1] = a.G;
                            col[2] = a.B;
                        break;

                        case 1:
                            col[0] = b.R;
                            col[1] = b.G;
                            col[2] = b.B;
                        break;

                        case 2:
                            col[0] = c.R;
                            col[1] = c.G;
                            col[2] = c.B;
                        break;

                        case 3:
                            col[0] = d.R;
                            col[1] = d.G;
                            col[2] = d.B;
                        break;
                    }

                    col += 3;
                    row = row >> 2;
                }

                col += (width - 4)*3;
            }

            dxt++;
    }

    return rgb;
}

unsigned char * DecodeImageData(Image * img, int i)
{
    //if(!img->compressed) return img->data[i];

    int width = img->width >> i;
    int height = img->height >> i;

    return DXT1Decode(width, height, (DXTColBlock*)img->data[i]);
}
