#pragma once
#include "LCD.h"
typedef struct {
  int         NumEntries; 
  const RGBC* pPalEntries; 
} GUI_LCD_PHYSPALETTE; 
extern const uint8_t GUI_LCD_aMirror[256];
void GUI_LCD_L0_DrawHLine(int x0, int y0,  int x1);
void GUI_LCD_L0_DrawVLine(int x,  int y0,  int y1);
void GUI_LCD_L0_FillRect (int x0, int y0, int x1, int y1);
RGBC GUI_LCD_L0_GetPixel (int x,  int y);
void GUI_LCD_GetRect     (Rect *pRect);
void GUI_LCD_L0_SetPixel (int x,  int y, RGBC Color);
void GUI_LCD_L0_Init     (void);
