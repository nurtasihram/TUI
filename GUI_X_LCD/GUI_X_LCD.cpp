#include "GUI_X_LCD.h"

volatile int a__ = 0;

static inline void LCD_SetPixel(uint16_t x, uint16_t y, RGBC Color) {
	a__ = x * y *Color;
}

#pragma region LCD_SetBitmap
template<bool HasTrans>
static void _LCD_BMP_1BPP(
	int x0, int y0, int x1, int y1,
	int BytesPerLine,
	const uint8_t *pData, int Diff,
	const RGBC *pTrans) {
	auto C0 = pTrans[0], C1 = pTrans[1];
	for (int y = y0; y <= y1; ++y) {
		auto pLine = pData;
		auto Pix = *pLine >> Diff;
		auto diff = Diff;
		for (int x = x0; x <= x1; ++x) {
			auto Index = Pix & 1;
			if constexpr (HasTrans) {
				if (Index)
					LCD_SetPixel(x, y, C1);
			}
			else
				LCD_SetPixel(x, y, Index ? C1 : C0);
			Pix >>= 1;
			if (++diff >= 8) {
				diff = 0;
				Pix = *++pLine;
			}
		}
		pData += BytesPerLine;
	}
}
template<bool HasTrans>
static void _LCD_BMP_2BPP(
	int x0, int y0, int x1, int y1,
	int BytesPerLine,
	const uint8_t *pData, int Diff,
	const RGBC *pTrans) {
	for (int y = y0; y <= y1; ++y) {
		auto pLine = pData;
		auto Pix = *pLine >> Diff;
		auto diff = Diff >> 1;
		for (int x = x0; x <= x1; ++x) {
			auto Index = Pix & 3;
			if constexpr (HasTrans) {
				if (Index)
					LCD_SetPixel(x, y, pTrans[Index]);
			}
			else
				LCD_SetPixel(x, y, pTrans[Index]);
			Pix >>= 2;
			if (++diff >= 4) {
				diff = 0;
				Pix = *++pLine;
			}
		}
		pData += BytesPerLine;
	}
}
#pragma endregion

void LCD_SetBitmap(const BitmapRect &br) {
	switch (br.BitsPerPixel) {
		case 1:
			if (br.HasTrans)
				_LCD_BMP_1BPP<true>(br.x0, br.y0, br.x1, br.y1, br.BytesPerLine, (const uint8_t *)br.pData, br.BitsXOff, br.pPalEntries);
			else
				_LCD_BMP_1BPP<false>(br.x0, br.y0, br.x1, br.y1, br.BytesPerLine, (const uint8_t *)br.pData, br.BitsXOff, br.pPalEntries);
			break;
		case 2:
			if (br.HasTrans)
				_LCD_BMP_2BPP<true>(br.x0, br.y0, br.x1, br.y1, br.BytesPerLine, (const uint8_t *)br.pData, br.BitsXOff, br.pPalEntries);
			else
				_LCD_BMP_2BPP<false>(br.x0, br.y0, br.x1, br.y1, br.BytesPerLine, (const uint8_t *)br.pData, br.BitsXOff, br.pPalEntries);
			break;
		case 4:
			break;
		case 8:
			break;
		case 16:
			break;
		case 24:
			//GUI_X_SIM::SetBitmap(br.x0, br.y0, br.x1, br.y1, (const uint32_t *)br.pData, br.BytesPerLine);
			break;
	}
}
void LCD_GetBitmap(BitmapRect &br) {
	a__ = br.x0 * br.y0 * br.x1 * br.y1;
}
void LCD_FillRect(const SRect &r) {
	a__ = r.x0 * r.y0 * r.x1 * r.y1;
}

BitmapRect LCD_AllocBitmap(const SRect &r) {
	BitmapRect br = r;
	br.BytesPerLine = br.xsize() * 4;
	br.BitsPerPixel = 24;
	br.pData = GUI_MEM_Alloc(br.ysize() * br.BytesPerLine);
	return br;
}
void LCD_FreeBitmap(BitmapRect &br) {
	if (br.pData) {
		GUI_MEM_Free(br.pData);
		br.pData = nullptr;
	}
}

static uint16_t xSizeDisp = 800, ySizeDisp = 480;
SRect LCD_Rect() {
	return { 0, 0, xSizeDisp, ySizeDisp };
}
void GUI_X_LCD_Init() {
}
