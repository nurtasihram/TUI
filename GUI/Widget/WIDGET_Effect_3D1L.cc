#include "WIDGET.h"
#include "GUIDebug.h"
#include "GUI.h"
#include "GUI_Protected.h"
#include "WM_Intern.h"
#define WIDGET_H2P(hWin) (WIDGET*)hWin
static void _WIDGET_EFFECT_3D1L_DrawUpRect(const Rect* pRect) {
	Rect r = *pRect;
	GUI_SetColor(0xE7E7E7);
	GUI_DrawHLine(r.y0, r.x0, r.x1 - 1);
	GUI_DrawVLine(r.x0, r.y0 + 1, r.y1 - 1);
	GUI_SetColor(0x606060);
	GUI_DrawHLine(r.y1, r.x0, r.x1);
	GUI_DrawVLine(r.x1, r.y0, r.y1 - 1);
}
static void _WIDGET_EFFECT_3D1L_DrawUp(void) {
	Rect r;
	WM_GetClientRect(&r);
	_WIDGET_EFFECT_3D1L_DrawUpRect(&r);
}
static void _WIDGET_EFFECT_3D1L_DrawDownRect(const Rect* pRect) {
	Rect r = *pRect;
	GUI_SetColor(0x606060);
	GUI_DrawHLine(r.y0, r.x0, r.x1 - 1);
	GUI_DrawVLine(r.x0, r.y0 + 1, r.y1 - 1);
	GUI_SetColor(0xE7E7E7);
	GUI_DrawHLine(r.y1, r.x0, r.x1);
	GUI_DrawVLine(r.x1, r.y0, r.y1 - 1);
}
static void _WIDGET_EFFECT_3D1L_DrawDown(void) {
	Rect r;
	WM_GetClientRect(&r);
	_WIDGET_EFFECT_3D1L_DrawDownRect(&r);
}
static void _WIDGET_EFFECT_3D1L_GetRect(Rect* pRect) {
	WM_GetClientRect(pRect);
	GUI__ReduceRect(pRect, pRect, 1);
}
void WIDGET_SetDefaultEffect_3D1L(void) {
	WIDGET_SetDefaultEffect(&WIDGET_Effect_3D1L);
}
const WIDGET_EFFECT WIDGET_Effect_3D1L = {
	_WIDGET_EFFECT_3D1L_DrawUp,
	_WIDGET_EFFECT_3D1L_DrawDown,
	_WIDGET_EFFECT_3D1L_DrawUpRect,
	_WIDGET_EFFECT_3D1L_DrawDownRect,
	_WIDGET_EFFECT_3D1L_GetRect,
	1
};
