#include "windows.h"
#include "stdio.h"

#include "Font.h"
#include "..\Resources\Formats\dds.h"

#include <string>
#include <iostream>

unsigned long img_width = 512;
unsigned long img_height = 512;

Font* CreateFontImg(int fsize, char* fname, long flags)
{
    unsigned char* buf = nullptr;
    unsigned long bufsz;

    GLYPHMETRICS gm;
    TEXTMETRIC tm;
    MAT2 mat;

    HDC hDC = GetDC(NULL);

    unsigned long weight = 0;
    unsigned long italic = FALSE;
    unsigned long underline = FALSE;

    if (flags & TEXT_BOLD) weight = FW_BOLD;
    if (flags & TEXT_ITALIC) italic = TRUE;
    if (flags & TEXT_UNDERLINE) underline = TRUE;

    HFONT hFont = CreateFontA(fsize, 0, 0, 0, weight, italic, underline, 0, DEFAULT_CHARSET,
        OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        VARIABLE_PITCH | FF_DONTCARE, fname);

    SelectObject(hDC, hFont);

    GetTextMetrics(hDC, &tm);

    Font* font = new Font;

    font->height = fsize;

    font->iwidth = img_width;
    font->iheight = img_height;

    font->img = new unsigned char[font->iwidth * font->iheight];
    memset(font->img, 0, font->iwidth * font->iheight);

    memset(&mat, 0, sizeof(MAT2));
    mat.eM11.value = 1;
    mat.eM22.value = 1;

    unsigned long cx = GLYPH_SPACE;
    unsigned long cy = GLYPH_SPACE;

    unsigned long ch;

    for (ch = 0; ch < 0x100; ch++)
    {
        if (!ch)
        {
            gm.gmBlackBoxY = fsize;
            gm.gmBlackBoxX = fsize * 0.7;
        }
        else
        {
            bufsz = GetGlyphOutline(hDC, ch, GGO_GRAY8_BITMAP, &gm, 0, NULL, &mat);

            if (!bufsz)
            {
                font->glyphs[ch++] = font->glyphs[0];
                continue;
            }

            buf = new unsigned char[bufsz];
            GetGlyphOutline(hDC, ch, GGO_GRAY8_BITMAP, &gm, bufsz, buf, &mat);// == GDI_ERROR)
        }

        unsigned long offset = GLYPH_SPACE;//fsize - gm.gmptGlyphOrigin.y;//gm.gmBlackBoxY;

        unsigned long pitch = gm.gmBlackBoxX >> 2;
        if (gm.gmBlackBoxX & 3) pitch++;

        pitch *= 4;

        unsigned long i, k;

        unsigned char* ptr = buf;

        if ((cx + gm.gmBlackBoxX + GLYPH_SPACE * 2) > (font->iwidth))
        {
            cx = GLYPH_SPACE;
            cy += font->height + GLYPH_SPACE;
        }

        for (i = 0; i < gm.gmBlackBoxY; i++)
        {
            for (k = 0; k < gm.gmBlackBoxX; k++)
            {
                unsigned char byte;

                if (!ch) byte = 255;
                else byte = ptr[k] * GRAY_LEVEL;

                font->img[(cy + i + offset) * font->iwidth + (cx + k)] = byte;
            }

            ptr += pitch;
        }

        font->glyphs[ch].width = gm.gmBlackBoxX + GLYPH_SPACE;
        font->glyphs[ch].x = cx - GLYPH_SPACE;
        font->glyphs[ch].y = cy + 1;
        if (gm.gmptGlyphOrigin.y) font->glyphs[ch].oy = fsize - gm.gmptGlyphOrigin.y;
        else font->glyphs[ch].oy = 0;

        cx += gm.gmBlackBoxX + GLYPH_SPACE * 2;

        if (ch) free(buf);
    }

    return font;
}

void WriteFont(Font* font, char* fname, char* iname)
{
    std::string img_name(iname);
    std::string desc_name(fname);

    img_name += ".dds";
    desc_name += ".dcf";

    //Font image
    DDS_HEADER head = {};

    char signature[] = "DDS ";

    FILE* fimg;

    //Open file
    errno_t error = fopen_s(&fimg, img_name.c_str(), "wb");

    if (error)
    {
        std::cout << "Can't open file " << fname << std::endl;
        return;
    }

    head.dwSize = sizeof(DDS_HEADER);
    head.dwFlags = DDSF_CAPS | DDSF_HEIGHT | DDSF_WIDTH | DDSF_PITCH | DDSF_PIXELFORMAT;
    head.dwWidth = font->iwidth;
    head.dwHeight = font->iheight;

    head.dwMipMapCount = 1;

    head.dwPitchOrLinearSize = (font->iwidth * 8 + 7) / 8;

    head.ddspf.dwSize = sizeof(DDS_PIXELFORMAT);
    head.ddspf.dwFlags = DDSF_ALPHA;
    //head.ddspf.dwFourCC = FOURCC_A8;
    head.ddspf.dwRGBBitCount = 8;

    fwrite(signature, 4, 1, fimg);
    fwrite(&head, sizeof(DDS_HEADER), 1, fimg);
    fwrite(font->img, font->iwidth * font->iheight, 1, fimg);

    fclose(fimg);

    //Font description
    FILE* fdesc;
    error = fopen_s(&fdesc, desc_name.c_str(), "wa");

    if (error)
    {
        std::cout << "Can't open file " << fname << std::endl;
        return;
    }

    fprintf(fdesc, "font\n");
    fprintf(fdesc, "{\n");

    fprintf(fdesc, "	size = %d\n", font->height);
    fprintf(fdesc, "	image = '%s'\n\n", img_name.c_str());

    for (int g = 0; g < 256; g++)
    {
        fprintf(fdesc, "	glyph %d\n", g);
        fprintf(fdesc, "	{\n");

        fprintf(fdesc, "		width = %d\n", font->glyphs[g].width);
        fprintf(fdesc, "		x = %d\n", font->glyphs[g].x);
        fprintf(fdesc, "		y = %d\n", font->glyphs[g].y);
        fprintf(fdesc, "		oy = %d\n", font->glyphs[g].oy);

        fprintf(fdesc, "	}\n\n");
    }

    fprintf(fdesc, "}");

    fclose(fdesc);
}

void Usage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "FontExporter <font name> <param1> <param2> ..." << std::endl;
    std::cout << "parameters:" << std::endl;
    std::cout << "      -s <size> -- font size" << std::endl;
    std::cout << "      -w <width> -- image width" << std::endl;
    std::cout << "      -h <height> -- image height" << std::endl;
    std::cout << "      -i <name> -- image file name" << std::endl;
    std::cout << "      -f <name> -- font description file name" << std::endl;
    std::cout << "      -b -- bold" << std::endl;
    std::cout << "      -t -- italic" << std::endl;
    std::cout << "      -u -- underline" << std::endl;
}

int main(int argc, char* argv[])
{
    std::cout << "Fonts exporting tool" << std::endl << std::endl;

    unsigned long size = 16;
    unsigned long flags = 0;
    char* fnt;
    char* fname;
    char* iname;

    if (argc <= 1)
    {
        Usage();
        return 0;
    }
    else
    {
        if (argv[1][0] == '-')
        {
            std::cout << "Error: invalid name" << std::endl;
            return 1;
        }

        fnt = argv[1];
        fname = argv[1];
        iname = argv[1];

        int arg = 2;

        while (arg < argc)
        {
            if (argv[arg][0] != '-')
            {
                std::cout << "Error: syntax error" << std::endl;
                return 1;
            }

            switch (argv[arg][1])
            {
            case 's':
                arg++;
                size = atoi(argv[arg]);
                break;

            case 'w':
                arg++;
                img_width = atoi(argv[arg]);
                break;

            case 'h':
                arg++;
                img_height = atoi(argv[arg]);
                break;

            case 'i':
                arg++;
                iname = argv[arg];
                break;

            case 'f':
                arg++;
                fname = argv[arg];
                break;

            case 'b':
                flags |= TEXT_BOLD;
                break;

            case 't':
                flags |= TEXT_ITALIC;
                break;

            case 'u':
                flags |= TEXT_UNDERLINE;
                break;

            default:
                std::cout << "Unknown param: " << argv[arg][1] << std::endl;
            }

            arg++;
        }
    }

    Font* font = CreateFontImg(size, fnt, flags);
    WriteFont(font, fname, iname);

    return 0;
}
