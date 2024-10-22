#include "GUI_Protected.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
static void _cbMin(WM_MESSAGE *pMsg) {
	if (pMsg->MsgId == WM_NOTIFY_PARENT_REFLECTION) {
		auto pObj = (FrameWin_Obj *)pMsg->pWinSrc;
		if (pObj->Flags & FRAMEWIN_SF_MINIMIZED)
			FRAMEWIN_Restore(pObj);
		else
			FRAMEWIN_Minimize(pObj);
	}
	else
		BUTTON_Callback(pMsg);
}
static void _DrawMin(void) {
	Rect r;
	WM_GetInsideRect(&r);
	WM_ADDORG(r.x0, r.y0);
	WM_ADDORG(r.x1, r.y1);
	int Size = (r.x1 - r.x0 + 1) >> 1;
	WM_ITERATE_START(&r) {
		int i;
		for (i = 1; i < Size; i++)
			GUI_LCD_DrawHLine(r.x0 + i, r.y1 - i - (Size >> 1), r.x1 - i);
	} WM_ITERATE_END();
}
static void _DrawRestore(void) {
	Rect r;
	WM_GetInsideRect(&r);
	WM_ADDORG(r.x0, r.y0);
	WM_ADDORG(r.x1, r.y1);
	int Size = (r.x1 - r.x0 + 1) >> 1;
	WM_ITERATE_START(&r); {
		int i;
		for (i = 1; i < Size; i++)
			GUI_LCD_DrawHLine(r.x0 + i, r.y0 + i + (Size >> 1), r.x1 - i);
	} WM_ITERATE_END();
}
static void _Draw(void) {
	auto pObj = (FrameWin_Obj *)WM_GetParent(WM_GetActiveWindow());
	if (pObj->Flags & FRAMEWIN_SF_MINIMIZED)
		_DrawRestore();
	else
		_DrawMin();
}
Button_Obj *FRAMEWIN_AddMinButton(FrameWin_Obj *pObj, int Flags, int Off) {
	auto hButton = FRAMEWIN_AddButton(pObj, Flags, Off, GUI_ID_MINIMIZE);
	BUTTON_SetSelfDraw(hButton, 0, &_Draw);
	WM_SetCallback(hButton, _cbMin);
	return hButton;
}
