#include "GUI.h"

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
					LCD_SetPixel({ x, y }, C1);
			}
			else
				LCD_SetPixel({ x, y }, Index ? C1 : C0);
			Pix >>= 1;
			if (++diff >= 8) {
				diff = 0;
				Pix = *++pLine;
			}
		}
		pData += BytesPerLine;
	}
}
#include<stdio.h>
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
					LCD_SetPixel({ x, y }, pTrans[Index]);
			}
			else
				LCD_SetPixel({ x, y }, pTrans[Index]);
			Pix >>= 2;
			if (++diff >= 4) {
				diff = 0;
				Pix = *++pLine;
			}
		}
		pData += BytesPerLine;
	}
}

void LCD_DrawBitmap(const BitmapRect &bc) {
	switch (bc.BitsPerPixel) {
	case 1:
		if (bc.HasTrans)
			_LCD_BMP_1BPP<true>(bc.x0, bc.y0, bc.x1, bc.y1, bc.BytesPerLine, (const uint8_t *)bc.pData, bc.BitsXOff, bc.pPalEntries);
		else
			_LCD_BMP_1BPP<false>(bc.x0, bc.y0, bc.x1, bc.y1, bc.BytesPerLine, (const uint8_t *)bc.pData, bc.BitsXOff, bc.pPalEntries);
		break;
	case 2:
		if (bc.HasTrans)
			_LCD_BMP_2BPP<true>(bc.x0, bc.y0, bc.x1, bc.y1, bc.BytesPerLine, (const uint8_t *)bc.pData, bc.BitsXOff, bc.pPalEntries);
		else
			_LCD_BMP_2BPP<false>(bc.x0, bc.y0, bc.x1, bc.y1, bc.BytesPerLine, (const uint8_t *)bc.pData, bc.BitsXOff, bc.pPalEntries);
		break;
	case 4:
		break;
	case 8:
		break;
	case 16:
		break;
	}
}
