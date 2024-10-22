#pragma once
#include "Widget.h"
struct ProgBar_Obj : public Widget {
	int v;
	CFont* pFont;
	RGBC BarColor[2];
	RGBC TextColor[2];
	char * pText;
	int16_t XOff, YOff;
	int16_t TextAlign;
	int Min, Max;
};
ProgBar_Obj *PROGBAR_CreateEx      (int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id);
ProgBar_Obj *PROGBAR_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb);
#define PROGBAR_Create(x0, y0, xsize, ysize, Flags) PROGBAR_CreateEx(x0, y0, xsize, ysize, 0, Flags, 0, 0)
#define PROGBAR_CreateAsChild(x0, y0, xsize, ysize, pParent, Id, Flags) PROGBAR_CreateEx(x0, y0, xsize, ysize, pParent, Flags, 0, Id)
#define PROGBAR_Delete(pObj)        WM_DeleteWindow(pObj)
#define PROGBAR_Paint(pObj)         WM_Paint(pObj)
void PROGBAR_SetBarColor (ProgBar_Obj *pObj, unsigned int index, RGBC color);
void PROGBAR_SetFont     (ProgBar_Obj *pObj, CFont* pfont);
void PROGBAR_SetMinMax   (ProgBar_Obj *pObj, int Min, int Max);
void PROGBAR_SetText     (ProgBar_Obj *pObj, const char* s);
void PROGBAR_SetTextAlign(ProgBar_Obj *pObj, int Align);
void PROGBAR_SetTextColor(ProgBar_Obj *pObj, unsigned int index, RGBC color);
void PROGBAR_SetTextPos  (ProgBar_Obj *pObj, int XOff, int YOff);
void PROGBAR_SetValue    (ProgBar_Obj *pObj, int v);
