#pragma once
#include "Widget.h"
#define SLIDER_STATE_PRESSED    WIDGET_STATE_USER0
#define SLIDER_CF_VERTICAL WIDGET_CF_VERTICAL
struct Slider_Obj : public Widget {
	RGBC aBkColor[2];
	RGBC aColor[2];
	int Min, Max, v;
	int Flags;
	int NumTicks;
	int16_t Width;
};
Slider_Obj *SLIDER_CreateEx      (int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id);
Slider_Obj *SLIDER_CreateIndirect(const GUI_WIDGET_CREATE_INFO* pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK* cb);
#define SLIDER_Create(x0, y0, xsize, ysize, pParent, Id, WinFlags, SpecialFlags) \
	SLIDER_CreateEx(x0, y0, xsize, ysize, pParent, WinFlags, SpecialFlags, Id)
void SLIDER_Inc        (Slider_Obj *pObj);
void SLIDER_Dec        (Slider_Obj *pObj);
void SLIDER_SetBkColor (Slider_Obj *pObj, RGBC Color);
void SLIDER_SetWidth   (Slider_Obj *pObj, int Width);
void SLIDER_SetValue   (Slider_Obj *pObj, int v);
int  SLIDER_GetValue   (Slider_Obj *pObj);
void SLIDER_SetRange   (Slider_Obj *pObj, int Min, int Max);
void SLIDER_SetNumTicks(Slider_Obj *pObj, int NumTicks);
void SLIDER_SetDefaultBkColor(RGBC Color);
