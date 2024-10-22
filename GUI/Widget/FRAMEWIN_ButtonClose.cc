#include "GUI_Protected.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
static void _cbClose(WM_MESSAGE* pMsg) {
	if (pMsg->MsgId == WM_NOTIFY_PARENT_REFLECTION)
		WM_DeleteWindow(pMsg->pWinSrc);
	else
		BUTTON_Callback(pMsg);
}
static void _Draw(void) {
	Rect r;
	WM_GetInsideRect(&r);
	WM_ADDORG(r.x0, r.y0);
	WM_ADDORG(r.x1, r.y1);
	int Size = r.x1 - r.x0 - 2;
	WM_ITERATE_START(&r); {
		int i;
		for (i = 2; i < Size; i++) {
			GUI_LCD_DrawHLine(r.x0 + i, r.y0 + i, r.x0 + i + 1);
			GUI_LCD_DrawHLine(r.x1 - i - 1, r.y0 + i, r.x1 - i);
		}
	} WM_ITERATE_END();
}
Button_Obj *FRAMEWIN_AddCloseButton(FrameWin_Obj *pObj, int Flags, int Off) {
	auto hButton = FRAMEWIN_AddButton(pObj, Flags, Off, GUI_ID_CLOSE);
	BUTTON_SetSelfDraw(hButton, 0, &_Draw);
	WM_SetCallback(hButton, _cbClose);
	return hButton;
}
