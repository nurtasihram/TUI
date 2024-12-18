#define USE_AYXANDAR
#include "SimDisp.h"

#include "GUI.h"

Ayxandar SimDisp::Ayx;

static inline void LCD_SetPixel(Point p, RGBC Color) {
	SimDisp::Ayx.Dot({ p.x, p.y }, Color);
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
			SimDisp::Ayx.SetRect({ br.x0, br.y0, br.x1, br.y1 }, (uint32_t *)br.pData, br.BytesPerLine);
			break;
	}
}
#pragma endregion

void LCD_GetBitmap(BitmapRect &br) {
	if (!br.pData) {
		br.BytesPerLine = br.xsize() * 4;
		br.pData = GUI_MEM_AllocZero(br.ysize() * br.BytesPerLine);
	}
	br.BitsPerPixel = 24;
	SimDisp::Ayx.GetRect({ br.x0, br.y0, br.x1, br.y1 }, (uint32_t *)br.pData, br.BytesPerLine);
}
void LCD_FillRect(const SRect &r) {
	SimDisp::Ayx.Fill(GUI.PenColor(), { r.x0, r.y0, r.x1, r.y1 });
}

