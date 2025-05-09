#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

void print_bmp(const FT_Bitmap &bmp) {
}

int main(int argc, char **argv) {
	FT_Library library;
	FT_Face face;
	FT_Vector pen;
	FT_GlyphSlot slot;
	FT_Matrix matrix{ 0 };

	auto error = FT_Init_FreeType(&library);
	error = FT_New_Face(library, argv[1], 0, &face);
	slot = face->glyph;

	FT_Set_Pixel_Sizes(face, 24, 0);

	pen.x = 0;
	pen.y = 0;

	//matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
	//matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
	//matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
	//matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

	FT_Set_Transform(face, &matrix, &pen);

	wchar_t *chinese_str = L"繁";
	error = FT_Load_Char(face, chinese_str[0], FT_LOAD_RENDER);
	if (error) {
		printf("FT_Load_Char error\n");
		return -1;
	}

	slot->bitmap

	return 0;
}
