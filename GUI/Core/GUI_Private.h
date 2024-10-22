#pragma once
#include "GUI_Protected.h"
#include "LCD_Private.h"
void GUI_LCD_L0_DrawBitmap(int x0, int y0, int xsize, int ysize,
                       int BitsPerPixel, int BytesPerLine,
                       const uint8_t *pData, int Diff, const RGBC *pTrans);
