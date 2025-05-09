#define DLL_IMPORTS

#include "GUI_X.h"
#include "GUI_X_SIM.h"

#pragma region LCD
static inline void GUI_X_LCD_SetPixel(uint16_t x, uint16_t y, RGBC Color) {
	GUI_X_SIM::Dot(x, y, Color);
}

#pragma region GUI_X_LCD_SetBitmap
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
					GUI_X_LCD_SetPixel(x, y, C1);
			}
			else
				GUI_X_LCD_SetPixel(x, y, Index ? C1 : C0);
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
					GUI_X_LCD_SetPixel(x, y, pTrans[Index]);
			}
			else
				GUI_X_LCD_SetPixel(x, y, pTrans[Index]);
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

void GUI_X_LCD_SetBitmap(const BitmapRect &br) {
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
			GUI_X_SIM::SetBitmap(br.x0, br.y0, br.x1, br.y1, (const uint32_t *)br.pData, br.BytesPerLine);
			break;
	}
}
void GUI_X_LCD_GetBitmap(BitmapRect &br) {
	GUI_X_SIM::GetBitmap(br.x0, br.y0, br.x1, br.y1, (uint32_t *)br.pData, br.BytesPerLine);
}
void GUI_X_LCD_FillRect(const SRect &r, RGBC c) {
	//auto color = GUI.PenColor();
	//if (color != RGB_BLACK && color != RGB_WHITE) {
	//	for (int x0 = r.x0; x0 <= r.x1; ++x0)
	//		for (int y0 = r.y0; y0 <= r.y1; ++y0)
	//			GUI_X_SIM::Fill(x0, y0, x0, y0, ((x0 & 1) ^ (y0 & 1)) ? RGB_BLACK : RGB_WHITE);
	//	return;
	//}
	GUI_X_SIM::Fill(r.x0, r.y0, r.x1, r.y1, c);
}

BitmapRect GUI_X_LCD_AllocBitmap(const SRect &r) {
	BitmapRect br = r;
	br.BytesPerLine = br.xsize() * 4;
	br.BitsPerPixel = 24;
	br.pData = GUI_MEM_Alloc(br.ysize() * br.BytesPerLine);
	return br;
}
void GUI_X_LCD_FreeBitmap(BitmapRect &br) {
	if (br.pData) {
		GUI_MEM_Free(br.pData);
		br.pData = nullptr;
	}
}

static uint16_t xSizeDisp = 0, ySizeDisp = 0;
SRect GUI_X_LCD_Rect() {
	return { 0, 0, xSizeDisp, ySizeDisp };
}
#pragma endregion

#pragma region Memory
void GUI__strcpy(char *s, const char *c) {
	strcpy(s, c);
}
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len) {
	memcpy(sDest, pSrc, Len);
}

void *GUI_MEM_Alloc(size_t Size) {
	return Size ? malloc(Size) : nullptr;
}
void *GUI_MEM_AllocZero(size_t Size) {
	if (auto ptr = GUI_MEM_Alloc(Size)) {
		memset(ptr, 0, Size);
		return ptr;
	}
	return nullptr;
}
void *GUI_MEM_Realloc(void *pOld, size_t NewSize) {
	return realloc(pOld, NewSize);
}
void GUI_MEM_Free(void *ptr) {
	if (ptr)
		free(ptr);
}
#pragma endregion

static MOUSE_STATE _MouseNow;
static KEY_STATE _KeyNow;
static SRect _RectNow;

MOUSE_STATE GUI_X_MOUSE_State() { return _MouseNow; }
KEY_STATE GUI_X_KEY_State() { return _KeyNow; }
SRect GUI_X_RECT_State() { return _RectNow; }

void GUI_X_Init() {
	GUI_X_SIM::LoadDll(_T("GUI_X_SIM.dll"));
	GUI_X_SIM::IntResize([](uint16_t cx, uint16_t cy) {
		GUI_RECT_Store({ 0, 0, cx, cy });
	});
	GUI_X_SIM::IntMouse([](int16_t x, int16_t y, int8_t key) {
		GUI_MOUSE_Store({ { x, y }, key });
	});
	GUI_X_SIM::IntKey([](uint16_t vk, uint8_t bPressed) {
		GUI_KEY_Store({ vk, bPressed });
	});
	GUI_X_SIM::CreateDisplay(L"TUI - User Interface", 800, 480);
}
