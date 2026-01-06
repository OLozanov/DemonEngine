
#define FONT_IMG_WIDTH 		1024
#define FONT_IMG_HEIGHT		512

#define GLYPH_SPACE			3

#define GRAY_LEVELS			65
#define GRAY_LEVEL			(256.0/GRAY_LEVELS)

#define TEXT_BOLD			1
#define TEXT_ITALIC			2
#define TEXT_UNDERLINE		4

struct Glyph
{
	unsigned long width;

	unsigned long x;
	unsigned long y;

	long ox;
	long oy;
};

struct Font
{
	unsigned long height;

	unsigned long iheight;
	unsigned long iwidth;
	unsigned char * img;

	Glyph glyphs[256];
};
