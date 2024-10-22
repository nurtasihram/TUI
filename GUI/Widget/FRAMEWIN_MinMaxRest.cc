#include "WIDGET.h"
#include "GUI_Protected.h"
#include "WM_Intern.h"
#include "FRAMEWIN.h"
static void _InvalidateButton(FrameWin_Obj* pObj, int Id) {
	WM_Obj* pChild;
	for (pChild = pObj->pFirstChild; pChild; pChild = pChild->pNext)
		if (WM_GetId(pChild) == Id)
			WM_InvalidateWindow(pChild);
}
static void _RestoreMinimized(FrameWin_Obj* pObj) {
	if (!(pObj->Flags & FRAMEWIN_SF_MINIMIZED))
		return;
	/* When window was minimized, restore it */
	int OldHeight = 1 + pObj->rect.y1 - pObj->rect.y0;
	int NewHeight = 1 + pObj->rRestore.y1 - pObj->rRestore.y0;
	WM_ResizeWindow(pObj, 0, NewHeight - OldHeight);
	WM_ShowWindow(pObj->hClient);
	WM_ShowWindow(pObj->pMenu);
	FRAMEWIN__UpdatePositions(pObj);
	pObj->Flags &= ~FRAMEWIN_SF_MINIMIZED;
	_InvalidateButton(pObj, GUI_ID_MINIMIZE);
}
static void _RestoreMaximized(FrameWin_Obj* pObj) {
	/* When window was maximized, restore it */
	if (!(pObj->Flags & FRAMEWIN_SF_MAXIMIZED))
		return;
	Rect r = pObj->rRestore;
	WM_MoveTo(pObj, r.x0, r.y0);
	WM_SetSize(pObj, r.x1 - r.x0 + 1, r.y1 - r.y0 + 1);
	FRAMEWIN__UpdatePositions(pObj);
	pObj->Flags &= ~FRAMEWIN_SF_MAXIMIZED;
	_InvalidateButton(pObj, GUI_ID_MAXIMIZE);
}
static void _MinimizeFramewin(FrameWin_Obj* pObj) {
	_RestoreMaximized(pObj);
	if (pObj->Flags & FRAMEWIN_SF_MINIMIZED)
		return;
	int OldHeight = pObj->rect.y1 - pObj->rect.y0 + 1;
	int NewHeight = FRAMEWIN__CalcTitleHeight(pObj) + pObj->pEffect->EffectSize * 2 + 2;
	pObj->rRestore = pObj->rect;
	WM_HideWindow(pObj->hClient);
	WM_HideWindow(pObj->pMenu);
	WM_ResizeWindow(pObj, 0, NewHeight - OldHeight);
	FRAMEWIN__UpdatePositions(pObj);
	pObj->Flags |= FRAMEWIN_SF_MINIMIZED;
	_InvalidateButton(pObj, GUI_ID_MINIMIZE);
}
static void _MaximizeFramewin(FrameWin_Obj* pObj) {
	_RestoreMinimized(pObj);
	if (!(pObj->Flags & FRAMEWIN_SF_MAXIMIZED))
		return;
	WM_Obj* pParent = pObj->pParent;
	Rect r = pParent->rect;
	if (pParent->pParent == nullptr) {
		r.x1 = GUI_LCD_GetXSize();
		r.y1 = GUI_LCD_GetYSize();
	}
	pObj->rRestore = pObj->rect;
	WM_MoveTo(pObj, r.x0, r.y0);
	WM_SetSize(pObj, r.x1 - r.x0 + 1, r.y1 - r.y0 + 1);
	FRAMEWIN__UpdatePositions(pObj);
	pObj->Flags |= FRAMEWIN_SF_MAXIMIZED;
	_InvalidateButton(pObj, GUI_ID_MAXIMIZE);
}
void FRAMEWIN_Minimize(FrameWin_Obj *pObj) {
	if (!pObj)
		return;
	_MinimizeFramewin(pObj);
}
void FRAMEWIN_Maximize(FrameWin_Obj *pObj) {
	if (!pObj)
		return;
	_MaximizeFramewin(pObj);
}
void FRAMEWIN_Restore(FrameWin_Obj *pObj) {
	if (!pObj)
		return;
	_RestoreMinimized(pObj);
	_RestoreMaximized(pObj);
}
