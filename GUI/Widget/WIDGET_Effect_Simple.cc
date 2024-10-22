#include "WIDGET.h"
#include "GUIDebug.h"
#include "GUI.h"
#include "GUI_Protected.h"
#include "WM_Intern.h"
static void _WIDGET_EFFECT_Simple_DrawUpRect(const Rect* pRect) {
	GUI_CONTEXT Context;
	GUI_SaveContext(&Context);
	GUI_SetColor(GUI_BLACK);
	GUI_DrawRect(pRect->x0, pRect->y0, pRect->x1, pRect->y1);          /* Draw rectangle around it */
	GUI_RestoreContext(&Context);
}
static void _WIDGET_EFFECT_Simple_DrawUp(void) {
	Rect r;
	WM_GetClientRect(&r);
	_WIDGET_EFFECT_Simple_DrawUpRect(&r);
}
static void _WIDGET_EFFECT_Simple_DrawDownRect(const Rect* pRect) {
	GUI_CONTEXT Context;
	GUI_SaveContext(&Context);
	GUI_SetColor(GUI_BLACK);
	GUI_DrawRect(pRect->x0, pRect->y0, pRect->x1, pRect->y1);          /* Draw rectangle around it */
	GUI_RestoreContext(&Context);
}
static void _WIDGET_EFFECT_Simple_DrawDown(void) {
	Rect r;
	WM_GetClientRect(&r);
	_WIDGET_EFFECT_Simple_DrawDownRect(&r);
}
static void _WIDGET_EFFECT_Simple_GetRect(Rect* pRect) {
	WM_GetClientRect(pRect);
	GUI__ReduceRect(pRect, pRect, 1);
}
void WIDGET_SetDefaultEffect_Simple(void) {
	WIDGET_SetDefaultEffect(&WIDGET_Effect_Simple);
}
const WIDGET_EFFECT WIDGET_Effect_Simple = {
	_WIDGET_EFFECT_Simple_DrawUp,
	_WIDGET_EFFECT_Simple_DrawDown,
	_WIDGET_EFFECT_Simple_DrawUpRect,
	_WIDGET_EFFECT_Simple_DrawDownRect,
	_WIDGET_EFFECT_Simple_GetRect,
	1
};
