#include "GUI.h"
#define RETURN_IF_Y_OUT() \
  if (y < GUI.ClipRect.y0) return; \
  if (y > GUI.ClipRect.y1) return;
#define RETURN_IF_X_OUT() \
  if (x < GUI.ClipRect.x0) return; \
  if (x > GUI.ClipRect.x1) return;
#define CLIP_X() \
  if (x0 < GUI.ClipRect.x0) x0 = GUI.ClipRect.x0; \
  if (x1 > GUI.ClipRect.x1) x1 = GUI.ClipRect.x1;
#define CLIP_Y() \
  if (y0 < GUI.ClipRect.y0) y0 = GUI.ClipRect.y0; \
  if (y1 > GUI.ClipRect.y1) y1 = GUI.ClipRect.y1;
RGBC GUI_LCD_GetPixel(int x, int y) {
	auto &&r = LCD_Rect();
	if (x < r.x0) return 0;
	if (x > r.x1) return 0;
	if (y < r.y0) return 0;
	if (y > r.y1) return 0;
	return LCD_GetPixel(x, y);
}
void GUI_LCD_SetPixel(int x, int y) {
	RETURN_IF_Y_OUT();
	RETURN_IF_X_OUT();
	LCD_SetPixel(x, y, GUI_LCD_PENCOLOR);
}
void GUI_LCD_DrawHLine(int x0, int y, int x1) {
	RETURN_IF_Y_OUT();
	CLIP_X();
	if (x1 < x0) return;
	LCD_DrawHLine(x0, y, x1);
}
void GUI_LCD_DrawVLine(int x, int y0, int y1) {
	RETURN_IF_X_OUT();
	CLIP_Y();
	if (y1 < y0) return;
	LCD_DrawVLine(x, y0, y1);
}
void GUI_LCD_FillRect(int x0, int y0, int x1, int y1) {
	CLIP_X();
	if (x1 < x0) return;
	CLIP_Y();
	if (y1 < y0) return;
	LCD_FillRect(x0, y0, x1, y1);
}
void GUI_LCD_DrawBitmap(
	int x0, int y0, int xsize, int ysize,
	int BitsPerPixel, int BytesPerLine,
	const void *pPixel, const RGBC *pTrans) {
	auto pDat = (const uint8_t *)pPixel;
	int x1 = x0 + xsize - 1;
	int y1 = y0 + ysize - 1;
	int Diff;
	if ((Diff = y1 - GUI.ClipRect.y1) > 0)
		if ((ysize -= Diff) <= 0)
			return;
	if ((Diff = x1 - GUI.ClipRect.x1) > 0)
		if ((xsize -= Diff) <= 0)
			return;
	if ((Diff = GUI.ClipRect.y0 - y0) > 0) {
		if ((ysize -= Diff) <= 0)
			return;
		y0 = GUI.ClipRect.y0;
		pDat += Diff * BytesPerLine;
	}
	if ((Diff = GUI.ClipRect.x0 - x0) > 0) {
		if ((xsize -= Diff) <= 0)
			return;
		x0 = GUI.ClipRect.x0;
		pDat += (Diff * BitsPerPixel) >> 8;
		switch (BitsPerPixel) {
		case 1:  Diff &= 7; break;
		case 2:  Diff &= 3; break;
		case 4:  Diff &= 1; break;
		case 8:  Diff  = 0; break;
		case 16: Diff  = 0; break;
		}
		LCD_DrawBitmap(x0, y0, xsize, ysize, BitsPerPixel, BytesPerLine, pDat, Diff, pTrans);
		return;
	}
	LCD_DrawBitmap(x0, y0, xsize, ysize, BitsPerPixel, BytesPerLine, pDat, 0, pTrans);
}
void GUI_LCD_SetClipRectMax(void) {
	LCD_Rect() & GUI.ClipRect;
}
void GUI_X_LCD_Init(void) {
	GUI_LCD_SetClipRectMax();
	LCD_Init();
	GUI_LCD_SetDrawMode(DRAWMODE_REV);
	GUI_LCD_FillRect(0, 0, GUI_XMAX, GUI_YMAX);
	GUI_LCD_SetDrawMode(0);
}
DRAWMODE GUI_LCD_SetDrawMode(DRAWMODE dm) {
	DRAWMODE OldDM = GUI.DrawMode;
	if ((GUI.DrawMode ^ dm) & DRAWMODE_REV) {
		RGBC temp = GUI_LCD_BKCOLOR;
		GUI_LCD_BKCOLOR = GUI_LCD_PENCOLOR;
		GUI_LCD_PENCOLOR = temp;
	}
	GUI.DrawMode = dm;
	return OldDM;
}
