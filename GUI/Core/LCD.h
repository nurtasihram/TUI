#pragma once
#include <stdint.h>
#include <stddef.h>
/*      *************************************************************
		*                                                           *
		*                Constants                                  *
		*                                                           *
		*************************************************************
*/
#define GUI_LCD_ERR0 (0x10)
#define GUI_LCD_ERR_CONTROLLER_NOT_FOUND (GUI_LCD_ERR0+1)
#define GUI_LCD_ERR_MEMORY               (GUI_LCD_ERR0+2)
/*
	  *********************************
	  *                               *
	  *      Drawing modes            *
	  *                               *
	  *********************************
*/
#define DRAWMODE_NORMAL (0)
#define DRAWMODE_TRANS  (1<<1)
#define DRAWMODE_REV    (1<<2)
/*      *************************************************************
		*                                                           *
		*                Typedefs                                   *
		*                                                           *
		*************************************************************
*/
typedef int DRAWMODE;
typedef uint32_t RGBC;
typedef struct {
	int x, y;
} Point;
typedef struct {
	int x0, y0, x1, y1;
} Rect;
typedef struct {
	int         NumEntries;
	char        HasTrans;
	const RGBC *pPalEntries;
} LogPalette;
RGBC GUI_GetBkColor(void);
RGBC GUI_GetColor(void);
void GUI_SetColor(RGBC);
void GUI_SetBkColor(RGBC);
RGBC GUI_LCD_GetPixel(int x, int y);
void GUI_LCD_SetPixel(int x0, int y0);
void GUI_LCD_DrawHLine(int x0, int y0, int x1);
void GUI_LCD_DrawVLine(int x, int y0, int y1);
void GUI_LCD_FillRect(int x0, int y0, int x1, int y1);
int  GUI_LCD_GetXSize(void);
int  GUI_LCD_GetYSize(void);
void GUI_LCD_SetClipRectMax(void);
void GUI_LCD_SetClipRectEx(const Rect *pRect);
#define GUI_LCD_GET_XSIZE() GUI_LCD_GetXSize()
#define GUI_LCD_GET_YSIZE() GUI_LCD_GetYSize()
RGBC GUI_LCD_MixColors256(RGBC Color, RGBC BkColor, unsigned Intens);
DRAWMODE GUI_LCD_SetDrawMode(DRAWMODE dm);
