#include "WIDGET.h"
#include "GUIDebug.h"
#include "GUI.h"
#include "GUI_Protected.h"
#include "WM_Intern.h"
#define WIDGET_H2P(hWin) (WIDGET*)hWin
static void _WIDGET_EFFECT_3D_DrawUpRect(const Rect* pRect) {
	Rect r = *pRect;
	GUI_SetColor(0x000000);
	GUI_DrawRect(r.x0, r.y0, r.x1, r.y1);
	GUI_SetColor(0xffffff);
	GUI_DrawHLine(r.y0 + 1, r.x0 + 1, r.x1 - 2);
	GUI_DrawVLine(r.x0 + 1, r.y0 + 1, r.y1 - 2);
	GUI_SetColor(0x555555);
	GUI_DrawHLine(r.y1 - 1, r.x0 + 1, r.x1 - 1);
	GUI_DrawVLine(r.x1 - 1, r.y0 + 1, r.y1 - 2);
}
static void _WIDGET_EFFECT_3D_DrawDownRect(const Rect* pRect) {
	Rect r = *pRect;
	GUI_SetColor(0x808080);
	GUI_DrawHLine(r.y0, r.x0, r.x1);
	GUI_DrawVLine(r.x0, r.y0 + 1, r.y1);
	GUI_SetColor(0x0);
	GUI_DrawHLine(r.y0 + 1, r.x0 + 1, r.x1 - 1);
	GUI_DrawVLine(r.x0 + 1, r.y0 + 2, r.y1 - 1);
	GUI_SetColor(0xffffff);
	GUI_DrawHLine(r.y1, r.x0 + 1, r.x1);
	GUI_DrawVLine(r.x1, r.y0 + 1, r.y1);
	GUI_SetColor(0xc0c0c0);
	GUI_DrawHLine(r.y1 - 1, r.x0 + 2, r.x1 - 1);
	GUI_DrawVLine(r.x1 - 1, r.y0 + 2, r.y1 - 1);
}
static void _WIDGET_EFFECT_3D_DrawDown(void) {
	Rect r;
	WM_GetClientRect(&r);
	_WIDGET_EFFECT_3D_DrawDownRect(&r);
}
static void _WIDGET_EFFECT_3D_GetRect(Rect* pRect) {
	WM_GetClientRect(pRect);
	GUI__ReduceRect(pRect, pRect, 2);
}
void WIDGET_EFFECT_3D_DrawUp(void) {
	Rect r;
	WM_GetClientRect(&r);
	_WIDGET_EFFECT_3D_DrawUpRect(&r);
}
void WIDGET_SetDefaultEffect_3D(void) {
	WIDGET_SetDefaultEffect(&WIDGET_Effect_3D);
}
const WIDGET_EFFECT WIDGET_Effect_3D = {
	WIDGET_EFFECT_3D_DrawUp,
	_WIDGET_EFFECT_3D_DrawDown,
	_WIDGET_EFFECT_3D_DrawUpRect,
	_WIDGET_EFFECT_3D_DrawDownRect,
	_WIDGET_EFFECT_3D_GetRect,
	2
};
