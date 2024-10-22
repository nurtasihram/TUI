#include "GUI_Protected.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
static void _cbMax(WM_MESSAGE *pMsg) {
	if (pMsg->MsgId != WM_NOTIFY_PARENT_REFLECTION) {
		BUTTON_Callback(pMsg);
		return;
	}
	auto pWin = (FrameWin_Obj *)pMsg->pWinSrc;
	if (pWin->Flags & FRAMEWIN_SF_MAXIMIZED)
		FRAMEWIN_Restore(pWin);
	else
		FRAMEWIN_Maximize(pWin);
}
static void _DrawMax(void) {
	Rect r;
	WM_GetInsideRect(&r);
	WM_ADDORG(r.x0, r.y0);
	WM_ADDORG(r.x1, r.y1);
	WM_ITERATE_START(&r) {
		GUI_LCD_DrawHLine(r.x0 + 1, r.y0 + 1, r.x1 - 1);
		GUI_LCD_DrawHLine(r.x0 + 1, r.y0 + 2, r.x1 - 1);
		GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - 1, r.x1 - 1);
		GUI_LCD_DrawVLine(r.x0 + 1, r.y0 + 1, r.y1 - 1);
		GUI_LCD_DrawVLine(r.x1 - 1, r.y0 + 1, r.y1 - 1);
	} WM_ITERATE_END();
}
static void _DrawRestore(void) {
	Rect r;
	WM_GetInsideRect(&r);
	WM_ADDORG(r.x0, r.y0);
	WM_ADDORG(r.x1, r.y1);
	int Size = ((r.x1 - r.x0 + 1) << 1) / 3;
	WM_ITERATE_START(&r) {
		GUI_LCD_DrawHLine(r.x1 - Size, r.y0 + 1, r.x1 - 1);
		GUI_LCD_DrawHLine(r.x1 - Size, r.y0 + 2, r.x1 - 1);
		GUI_LCD_DrawHLine(r.x0 + Size, r.y0 + Size, r.x1 - 1);
		GUI_LCD_DrawVLine(r.x1 - Size, r.y0 + 1, r.y1 - Size);
		GUI_LCD_DrawVLine(r.x1 - 1, r.y0 + 1, r.y0 + Size);
		GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - Size, r.x0 + Size);
		GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - Size + 1, r.x0 + Size);
		GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - 1, r.x0 + Size);
		GUI_LCD_DrawVLine(r.x0 + 1, r.y1 - Size, r.y1 - 1);
		GUI_LCD_DrawVLine(r.x0 + Size, r.y1 - Size, r.y1 - 1);
	} WM_ITERATE_END();
}
static void _Draw(void) {
	auto pObj = (FrameWin_Obj *)WM_GetParent(WM_GetActiveWindow());
	if (pObj->Flags & FRAMEWIN_SF_MAXIMIZED)
		_DrawRestore();
	else 
		_DrawMax();
}
Button_Obj *FRAMEWIN_AddMaxButton(FrameWin_Obj *pObj, int Flags, int Off) {
	auto hButton = FRAMEWIN_AddButton(pObj, Flags, Off, GUI_ID_MAXIMIZE);
	BUTTON_SetSelfDraw(hButton, 0, &_Draw);
	WM_SetCallback(hButton, _cbMax);
	return hButton;
}
