#include "Widget.h"

#include "TEXT.h"
#define TEXT_FONT_DEFAULT       &GUI_Font13_1
#define TEXT_DEFAULT_TEXT_COLOR GUI_BLACK
static CFont *_pDefaultFont = TEXT_FONT_DEFAULT;
static RGBC   _DefaultTextColor = TEXT_DEFAULT_TEXT_COLOR;
static void _FreeAttached(Text_Obj *pObj) {
	GUI_ALLOC_Free(pObj->pText);
	pObj->pText = nullptr;
}
static void _Paint(Text_Obj *pObj) {
	GUI.PenColor(pObj->TextColor);
	GUI.Font(pObj->pFont);
	if (pObj->BkColor == GUI_INVALID_COLOR)
		GUI.BkColor(WIDGET__GetBkColor(pObj));
	else
		GUI.BkColor(pObj->BkColor);
	GUI_Clear();
	const char *s = pObj->pText;
	if (!s)
		return;
	SRect r;
	GUI_SetTextMode(DRAWMODE_TRANS);
	WM_GetClientRect(&r);
	GUI_DispStringInRect(s, &r, pObj->Align);
}
static void _Delete(Text_Obj *pObj) {
	/* Delete attached objects (if any) */
	_FreeAttached(pObj);
}
static void _TEXT_Callback(WM_MESSAGE *pMsg) {
	Text_Obj *pObj = (Text_Obj *)pMsg->pWin;
	if (WIDGET_HandleActive(pObj, pMsg) == 0)
		return;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		_Paint(pObj);
		return;
	case WM_DELETE:
		_Delete(pObj);
		break;
	}
	WM_DefaultProc(pMsg);
}
Text_Obj *TEXT_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent,
						  int WinFlags, int ExFlags, int Id, const char *pText) {
	auto pObj = (Text_Obj *)WObj::Create(
		x0, y0, xsize, ysize, pParent, WinFlags, _TEXT_Callback,
		sizeof(Text_Obj) - sizeof(WObj));
	if (!pObj) 
		return 0;
	char *hMem = 0;
	WIDGET__Init(pObj, Id, 0);
	if (pText) {
		hMem = (char *)GUI_ALLOC_AllocZero(GUI__strlen(pText) + 1);
		if (hMem)
			GUI__strcpy(hMem, pText);
	}
	pObj->pText = hMem;
	pObj->Align = ExFlags;
	pObj->pFont = _pDefaultFont;
	pObj->BkColor = GUI_INVALID_COLOR;
	pObj->TextColor = _DefaultTextColor;
	return pObj;
}
Text_Obj *TEXT_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb) {
	return TEXT_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0, pCreateInfo->xSize, pCreateInfo->ySize,
						 pParent, WC_VISIBLE, pCreateInfo->Flags, pCreateInfo->Id, pCreateInfo->pName);
}
void TEXT_SetDefaultFont(CFont *pFont) {
	_pDefaultFont = pFont;
}
void TEXT_SetDefaultTextColor(RGBC Color) {
	_DefaultTextColor = Color;
}
CFont *TEXT_GetDefaultFont(void) {
	return _pDefaultFont;
}
void TEXT_SetTextColor(Text_Obj *pObj, RGBC Color) {
	if (!pObj)
		return;
	pObj->TextColor = Color;
	pObj->Invalidate();
}
void TEXT_SetBkColor(Text_Obj *pObj, RGBC Color) {
	if (!pObj)
		return;
	pObj->BkColor = Color;
	pObj->Invalidate();
}
void TEXT_SetFont(Text_Obj *pObj, CFont *pFont) {
	if (!pObj)
		return;
	pObj->pFont = pFont;
	pObj->Invalidate();
}
void TEXT_SetText(Text_Obj *pObj, const char *s) {
	if (!pObj)
		return;
	if (GUI__SetText(&pObj->pText, s))
		pObj->Invalidate();
}
void TEXT_SetTextAlign(Text_Obj *pObj, int Align) {
	if (!pObj)
		return;
	pObj->Align = Align;
	pObj->Invalidate();
}
