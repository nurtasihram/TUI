#include "GUI.h"

static void  _DrawBitLine2BPP(int x, int y, uint8_t const *p, int Diff, int xsize, const RGBC *pTrans) {

	RGBC Pixels = *p;
	int CurrentPixel = Diff;

	if (GUI.DrawMode & DRAWMODE_TRANS) {
		if (pTrans)
			do {
				int Shift = (3 - CurrentPixel) << 1;
				int Index = (Pixels & (3 >> (6 - Shift))) >> Shift;
				if (Index)
					LCD_SetPixel(x, y, pTrans[Index]);
				x++;
				if (++CurrentPixel == 4) {
					CurrentPixel = 0;
					Pixels = *(++p);
				}
			} while (--xsize);
		else do {
			int Shift = (3 - CurrentPixel) << 1;
			int Index = (Pixels & (3 >> (6 - Shift))) >> Shift;
			if (Index)
				LCD_SetPixel(x, y, Index);
			x++;
			if (++CurrentPixel == 4) {
				CurrentPixel = 0;
				Pixels = *(++p);
			}
		} while (--xsize);
	}
	else if (pTrans)
		do {
			int Shift = (3 - CurrentPixel) << 1;
			int Index = (Pixels & (3 >> (6 - Shift))) >> Shift;
			LCD_SetPixel(x++, y, pTrans[Index]);
			if (++CurrentPixel == 4) {
				CurrentPixel = 0;
				Pixels = *(++p);
			}
		} while (--xsize);
	else do {
		int Shift = (3 - CurrentPixel) << 1;
		int Index = (Pixels & (0xC0 >> (6 - Shift))) >> Shift;
		LCD_SetPixel(x++, y, Index);
		if (++CurrentPixel == 4) {
			CurrentPixel = 0;
			Pixels = *(++p);
		}
	} while (--xsize);

}

static void  _DrawBitLine4BPP(int x, int y, uint8_t const *p, int Diff, int xsize, const RGBC *pTrans) {

	RGBC Pixels = *p;
	int CurrentPixel = Diff;

	if (GUI.DrawMode & DRAWMODE_TRANS) {
		if (pTrans)
			do {
				int Shift = (1 - CurrentPixel) << 2;
				int Index = (Pixels & (0xF >> (4 - Shift))) >> Shift;
				LCD_SetPixel(x++, y, pTrans[Index]);
				if (++CurrentPixel == 2) {
					CurrentPixel = 0;
					Pixels = *(++p);
				}
			} while (--xsize);
		else do {
			int Shift = (1 - CurrentPixel) << 2;
			int Index = (Pixels & (0xF >> (4 - Shift))) >> Shift;
			LCD_SetPixel(x++, y, Index);
			if (++CurrentPixel == 2) {
				CurrentPixel = 0;
				Pixels = *(++p);
			}
		} while (--xsize);
	}
	else if (pTrans) {
		do {
			int Shift = (1 - CurrentPixel) << 2;
			int Index = (Pixels & (0xF >> (4 - Shift))) >> Shift;
			if (Index)
				LCD_SetPixel(x, y, pTrans[Index]);
			x++;
			if (++CurrentPixel == 2) {
				CurrentPixel = 0;
				Pixels = *(++p);
			}
		} while (--xsize);
	}
	else do {
		int Shift = (1 - CurrentPixel) << 2;
		int Index = (Pixels & (0xF >> (4 - Shift))) >> Shift;
		if (Index)
			LCD_SetPixel(x, y, Index);
		x++;
		if (++CurrentPixel == 2) {
			CurrentPixel = 0;
			Pixels = *(++p);
		}
	} while (--xsize);

}

static void  _DrawBitLine8BPP(int x, int y, uint8_t const *p, int xsize, const RGBC *pTrans) {

	RGBC Pixel;

	if (GUI.DrawMode & DRAWMODE_TRANS)
		if (pTrans)
			while (xsize--)
				LCD_SetPixel(x++, y, pTrans[*p++]);
		else while (xsize--)
			LCD_SetPixel(x++, y, *p++);
	else if (pTrans)
		while (xsize--) {
			Pixel = *p++;
			if (Pixel)
				LCD_SetPixel(x, y, pTrans[Pixel]);
			++x;
		}
	else while (xsize--) {
		Pixel = *p++;
		if (Pixel)
			LCD_SetPixel(x, y, Pixel);
		++x;
	}

}

static void  DrawBitLine16BPP(int x, int y, uint16_t const *p, int xsize, const RGBC *pTrans) {

	if ((GUI.DrawMode & DRAWMODE_TRANS) == 0)
		if (pTrans)
			while (xsize--)
				LCD_SetPixel(x++, y, pTrans[*p++]);
		else while (xsize--)
			LCD_SetPixel(x++, y, *p++);
	else if (pTrans)
		while (xsize--) {
			RGBC pixel = *p++;
			if (pixel)
				LCD_SetPixel(x, y, pTrans[pixel]);
			++x;
		}
	else while (xsize--) {
		RGBC pixel = *p++;
		if (pixel)
			LCD_SetPixel(x, y, pixel);
		++x;
	}

}

void GUI_LCD_DP1(int x0, int y0,
			 int xsize, int ysize,
			 int BytesPerLine,
			 const uint8_t *pData, int Diff,
			 const RGBC *pTrans) {

	RGBC C0 = pTrans[0], C1 = pTrans[1];

	int i, j, y1 = y0 + ysize, x1 = x0 + xsize;

	for (i = y0; i < y1; ++i) {

		int ofs = Diff;

		for (j = x0; j < x1; ++j) {

			LCD_SetPixel(j, i, pData[ofs >> 3] & (1 << (ofs & 0x7)) ? C1 : C0);

			++ofs;

		}

		pData += BytesPerLine;

	}

}

void GUI_LCD_DP1T(int x0, int y0,
			 int xsize, int ysize,
			 int BytesPerLine,
			 const uint8_t *pData, int Diff,
			 const RGBC *pTrans) {

	RGBC C1 = pTrans[1];

	int i, j, y1 = y0 + ysize, x1 = x0 + xsize;

	for (i = y0; i < y1; ++i) {

		int ofs = Diff;

		for (j = x0; j < x1; ++j) {

			if (pData[ofs >> 3] & (1 << (ofs & 0x7)))
				LCD_SetPixel(j, i, C1);

			++ofs;

		}

		pData += BytesPerLine;

	}

}

void LCD_DrawBitmap(int x0, int y0,
					   int xsize, int ysize,
					   int BitsPerPixel, int BytesPerLine,
					   const uint8_t *pData, int Diff,
					   const RGBC *pTrans) {

	int i = 0;

	switch (BitsPerPixel) {

	case 1:
		if (GUI.DrawMode & DRAWMODE_TRANS)
			GUI_LCD_DP1T(x0, y0, xsize, ysize, BytesPerLine, pData, Diff, pTrans);
		else
			GUI_LCD_DP1(x0, y0, xsize, ysize, BytesPerLine, pData, Diff, pTrans);

		break;

	case 2:
		for (; i < ysize; ++i, pData += BytesPerLine)
			_DrawBitLine2BPP(x0, i + y0, pData, Diff, xsize, pTrans);
		break;

	case 4:
		for (; i < ysize; ++i, pData += BytesPerLine)
			_DrawBitLine4BPP(x0, i + y0, pData, Diff, xsize, pTrans);
		break;

	case 8:
		for (; i < ysize; ++i, pData += BytesPerLine)
			_DrawBitLine8BPP(x0, i + y0, pData, xsize, pTrans);
		break;

	case 16:
		for (; i < ysize; ++i, pData += BytesPerLine)
			DrawBitLine16BPP(x0, i + y0, (const uint16_t *)pData, xsize, pTrans);
		break;

	}

}
