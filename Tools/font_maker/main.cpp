#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "wx_dialog.h"

using namespace WX;

void print_byte(uint8_t byte) {
	for (int i = 0; i < 8; ++i) {
		Console.Log((byte & 0x80) ? _T("XX") : _T("__"));
		byte <<= 1;
	}
	Console.Log(_T(","));
}

void print_bmp(const FT_Bitmap &bmp) {
	static auto pix_mode = [](FT_Pixel_Mode mode) {
		switch (mode) {
			case FT_PIXEL_MODE_NONE:
				return "none";
			case FT_PIXEL_MODE_MONO:
				return "mono";
			case FT_PIXEL_MODE_GRAY:
				return "gray";
			case FT_PIXEL_MODE_GRAY2:
				return "gray2";
			case FT_PIXEL_MODE_GRAY4:
				return "gray4";
			case FT_PIXEL_MODE_LCD:
				return "LCD";
			case FT_PIXEL_MODE_LCD_V:
				return "LCDV";
			case FT_PIXEL_MODE_BGRA:
				return "BGRA";
		}
		return "UNKNOWN";
	};
	printf("rows:       %d\n"
		   "width:      %d\n"
		   "pitch:      %d\n"
		   "num_grays:  %d\n"
		   "pixel_mode:   %s\n"
		   //"palette_mode: %s\n"
		   //"palette: \n "
		   "buffer:  \n",
		   bmp.rows, 
		   bmp.width,
		   bmp.pitch, 
		   bmp.num_grays,
		   pix_mode((FT_Pixel_Mode)bmp.pixel_mode));
	auto lpBuffer = (const uint8_t *)bmp.buffer;
	for (int y = 0; y < bmp.rows; ++y) {
		auto lpLine = lpBuffer;
		for (int x = 0; x < bmp.width; x += 8)
			print_byte(*lpLine++);
		printf("\n");
		lpBuffer += bmp.pitch;
	}
}

void print_lib_info(FT_Face face) {
	printf("charmaps: \n");
	for (int i = 0, num = face->num_charmaps; i < num; ++i) {
		char buff[5]{ 0 };
		auto charmap = face->charmaps[i];
		for (int k = 0; k < 4; ++k)
			buff[3 - k] = (charmap->encoding >> (8 * k)) & 0xff;
		printf("- %s, %d\n", buff, charmap->encoding_id);
	}
	printf("num_glyph: %d\n", face->num_glyphs);

}

void fmk_main() {
	FT_Library library;
	FT_Face face;
	assertl(!FT_Init_FreeType(&library));
	//ChooserFile cf;
	//assertl(cf.Filter(S("Font Files (*.ttf;*.otf)\0"
	//					"*.ttf;*.otf\0"
	//					"All Files (*.*)\0"
	//					"*.*\0"))
	//		.Title(S("Select a font file"))
	//		.File(S("C:\\Windows\\Fonts"))
	//		.OpenFile());
	assertl(!FT_New_Face(library, "C:\\Users\\mylit\\OneDrive\\Desktop\\simsun.ttc", 0, &face));
	print_lib_info(face);

	FT_Vector pen{ 0 };
	FT_Matrix matrix{ 0 };
	matrix.xx = (FT_Fixed)(0x10000L);
	matrix.xy = (FT_Fixed)(0);
	matrix.yx = (FT_Fixed)(0);
	matrix.yy = (FT_Fixed)(0x10000L);
	FT_Set_Transform(face, &matrix, &pen);
	FT_Set_Char_Size(face, 0, 50 * 64, 0, 100);
	FT_Set_Pixel_Sizes(face, 0, 18);

	assertl(!FT_Select_Charmap(face, FT_ENCODING_UNICODE));

	const wchar_t *pString = L"李";
	printf("%02X-%02X\n", pString[0] & 0xFF, pString[0] >> 8);

	auto gInd = FT_Get_Char_Index(face, pString[0]);
	assertl(!FT_Load_Glyph(face, gInd, FT_LOAD_DEFAULT));
	assertl(!FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO));
	print_bmp(face->glyph->bitmap);

}

void print_gbk2unicode_list() {
	ChooserFile cf;
	assertl(cf.Filter(S("Code page index file (*.txt)\0"
						"*.txt\0"))
			.File(" ")
			.Title(S("Select a font file"))
			.SaveFile());
	File cp_file = File::Create(cf.File()).CreateAlways().Accesses(FileAccess::GenericWrite);
	for (uint8_t lo = 0xA1; lo <= 0xA9; ++lo)
		for (uint8_t hi = 0xA1; hi <= 0xFE; ++hi) {
			const char buff[2] = { lo, hi };
			cp_file.Write(buff, 2);
		}
	for (uint8_t lo = 0xB0; lo <= 0xF7; ++lo)
		for (uint8_t hi = 0xA1; hi <= 0xFE; ++hi) {
			const char buff[2] = { lo, hi };
			cp_file.Write(buff, 2);
		}
	printf("GB2312 Unicode index file wrote!\n");
}

int main() {
	print_gbk2unicode_list();
	try {
		fmk_main();
	} catch (const Exception &err) {
		Console.Log(err.toString());
	} catch (...) {
		Console.Log(_T("Unknown error"));
	}
	system("pause");
	return 0;
}

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd) {
	Console.Alloc();
	Console.Title(_T("Font Maker"));
	Console.Select();
	Console.Reopen();
	return main();
}
