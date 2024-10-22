#pragma once
#include "LCD_Protected.h"
#include "GUI.h"
#define GUI_LCD_PENCOLOR GUI.aColor[1]
#define GUI_LCD_BKCOLOR  GUI.aColor[0]
#define GUI_LCD_ACOLOR  GUI.aColor
void GUI_LCD_DrawBitmap(int x0, int y0, int xsize, int ysize,
					int BitsPerPixel, int BytesPerLine,
					const void *pPixel, const RGBC *pTrans);
