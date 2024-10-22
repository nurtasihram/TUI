#include "WM.h"
#include "SCROLLBAR.h"
#define SCROLLBAR_USE_3D           1
#define SCROLLBAR_BKCOLOR0_DEFAULT 0x808080
#define SCROLLBAR_BKCOLOR1_DEFAULT GUI_BLACK
#define SCROLLBAR_COLOR0_DEFAULT   0xc0c0c0
#define SCROLLBAR_COLOR1_DEFAULT   GUI_BLACK
#define SCROLLBAR_DEFAULT_WIDTH    11
RGBC SCROLLBAR__aDefaultBkColor[2] = { SCROLLBAR_BKCOLOR0_DEFAULT, SCROLLBAR_BKCOLOR1_DEFAULT };
RGBC SCROLLBAR__aDefaultColor[2] = { SCROLLBAR_COLOR0_DEFAULT, SCROLLBAR_COLOR1_DEFAULT };
int16_t SCROLLBAR__DefaultWidth = SCROLLBAR_DEFAULT_WIDTH;
static int _GetArrowSize(ScrollBar_Obj *pObj) {
	unsigned int 
		xSize = WIDGET__GetXSize(pObj),
		ySize = WIDGET__GetYSize(pObj);
	unsigned int r = ySize / 2 + 5;
	return r > xSize - 5 ? xSize - 5 : r;
}
static void _WIDGET__RECT2VRECT(const Widget *pWidget, SRect *pRect) {
	if (!(pWidget->State & WIDGET_STATE_VERTICAL))
		return;
	int xSize = pWidget->rect.x1 - pWidget->rect.x0 + 1;
	int x0 = pRect->x0,
		x1 = pRect->x1;
	pRect->x0 = pRect->y0;
	pRect->x1 = pRect->y1;
	pRect->y1 = xSize - 1 - x0;
	pRect->y0 = xSize - 1 - x1;
}
static void _CalcPositions(ScrollBar_Obj *pObj, SCROLLBAR_POSITIONS *pPos) {
	SRect r = pObj->rect;
	int x0 = r.x0, y0 = r.y0;
	pPos->x1 = (pObj->State & WIDGET_STATE_VERTICAL) ? r.y1 : r.x1;
	WObj *pWin;
	SRect rSub;
	if (pObj->Id == GUI_ID_HSCROLL) {
		pWin = WM_GetScrollbarV(pObj->pParent);
		if (pWin) {
			rSub = pWin->Rect();
			if (r.x1 == rSub.x1)
				r.x1 = rSub.x0 - 1;
		}
	}
	if (pObj->Id == GUI_ID_VSCROLL) {
		pWin = WM_GetScrollbarH(pObj->pParent);
		if (pWin) {
			rSub = pWin->Rect();
			if (r.y1 == rSub.y1)
				r.y1 = rSub.y0 - 1;
		}
	}
	r -= Point(x0, y0);
	_WIDGET__RECT2VRECT(pObj, &r);
	int NumItems = pObj->NumItems;
	int xSize = r.x1 - r.x0 + 1,
		xSizeArrow = _GetArrowSize(pObj),
		xSizeThumbArea = xSize - 2 * xSizeArrow,
		ThumbSize = GUI__DivideRound(xSizeThumbArea * pObj->PageSize, NumItems);
	if (ThumbSize < 4)
		ThumbSize = 4;
	if (ThumbSize > xSizeThumbArea)
		ThumbSize = xSizeThumbArea;
	int xSizeMoveable = xSizeThumbArea - ThumbSize;
	pPos->x0_LeftArrow = r.x0;
	pPos->x1_LeftArrow = xSizeArrow - 1;
	pPos->x1_RightArrow = xSize - 1;
	pPos->x0_RightArrow = xSize - xSizeArrow;
	pPos->x0_Thumb = pPos->x1_LeftArrow + 1 + GUI__DivideRound(xSizeMoveable * pObj->v, NumItems - pObj->PageSize);
	pPos->x1_Thumb = pPos->x0_Thumb + ThumbSize - 1;
	pPos->xSizeMoveable = xSizeMoveable;
	pPos->ThumbSize = ThumbSize;
}
static void _DrawTriangle(Widget *pWidget, int x, int y, int Size, int Inc) {
	if (pWidget->State & WIDGET_STATE_VERTICAL)
		for (; Size >= 0; Size--, x += Inc)
			GUI_DrawHLine(x, y - Size, y + Size);
	else for (; Size >= 0; Size--, x += Inc)
		GUI_DrawVLine(x, y - Size, y + Size);
}
static void _Paint(ScrollBar_Obj *pObj) {
	SCROLLBAR_POSITIONS Pos;
	_CalcPositions(pObj, &Pos);
	SRect rClient;
	WIDGET__GetClientRect(pObj, &rClient);
	SRect r = rClient;
	int ArrowSize = ((r.y1 - r.y0) / 3) - 1,
		ArrowOff = 3 + ArrowSize + ArrowSize / 3;
	GUI.PenColor(pObj->aColor[0]);
	r = rClient;
	r.x0 = Pos.x0_LeftArrow;
	r.x1 = Pos.x1_LeftArrow;
	WIDGET__FillRectEx(pObj, &r);
	GUI.PenColor(pObj->aBkColor[1]);
	_DrawTriangle(pObj, r.x0 + ArrowOff, (r.y1 - r.y0) >> 1, ArrowSize, -1);
	WIDGET__EFFECT_DrawUpRect(pObj, &r);
	GUI.PenColor(pObj->aBkColor[0]);
	r.x0 = Pos.x1_LeftArrow + 1;
	r.x1 = Pos.x0_Thumb - 1;
	WIDGET__FillRectEx(pObj, &r);
	r = rClient;
	r.x0 = Pos.x1_Thumb + 1;
	r.x1 = Pos.x0_RightArrow - 1;
	WIDGET__FillRectEx(pObj, &r);
	r = rClient;
	r.x0 = Pos.x0_Thumb;
	r.x1 = Pos.x1_Thumb;
	GUI.PenColor(pObj->aColor[0]);
	WIDGET__FillRectEx(pObj, &r);
	WIDGET__EFFECT_DrawUpRect(pObj, &r);
	GUI.PenColor(pObj->aColor[0]);
	r.x0 = Pos.x0_RightArrow;
	r.x1 = Pos.x1_RightArrow;
	WIDGET__FillRectEx(pObj, &r);
	GUI.PenColor(pObj->aBkColor[1]);
	_DrawTriangle(pObj, r.x1 - ArrowOff, (r.y1 - r.y0) >> 1, ArrowSize, 1);
	WIDGET__EFFECT_DrawUpRect(pObj, &r);
	if (Pos.x1_RightArrow == Pos.x1)
		return;
	r.x0 = Pos.x1_RightArrow + 1;
	r.x1 = Pos.x1;
	GUI.PenColor(pObj->aColor[0]);
	WIDGET__FillRectEx(pObj, &r);
}
static void _ScrollbarPressed(ScrollBar_Obj *pObj) {
	WIDGET_OrState(pObj, SCROLLBAR_STATE_PRESSED);
	if (pObj->Status & WC_VISIBLE)
		WM_NotifyParent(pObj, WM_NOTIFICATION_CLICKED);
}
static void _ScrollbarReleased(ScrollBar_Obj *pObj) {
	WIDGET_AndState(pObj, SCROLLBAR_STATE_PRESSED);
	if (pObj->Status & WC_VISIBLE)
		WM_NotifyParent(pObj, WM_NOTIFICATION_RELEASED);
}
static void _OnTouch(ScrollBar_Obj *pObj, WM_MESSAGE *pMsg) {
	PidState *pState = (PidState *)pMsg->Data;
	if (!pState)
		return;
	if (!pState->Pressed) {
		if (pObj->State & SCROLLBAR_STATE_PRESSED)
			_ScrollbarReleased(pObj);
		return;
	}
	int Sel = pObj->v;
	SCROLLBAR_POSITIONS Pos;
	_CalcPositions(pObj, &Pos);
	int Range = pObj->NumItems - pObj->PageSize;
	if (pObj->State & WIDGET_STATE_VERTICAL) {
		int t = pState->x;
		pState->x = pState->y;
		pState->y = t;
	}
	int x = pState->x;
	if (x <= Pos.x1_LeftArrow)
		Sel--;
	else if (x < Pos.x0_Thumb)
		Sel -= pObj->PageSize;
	else if (x <= Pos.x1_Thumb) {
		if (Pos.xSizeMoveable > 0) {
			x = x - Pos.ThumbSize / 2 - Pos.x1_LeftArrow - 1;
			Sel = GUI__DivideRound(Range * x, Pos.xSizeMoveable);
		}
	}
	else if (x < Pos.x0_RightArrow)
		Sel += pObj->PageSize;
	else if (x <= Pos.x1_RightArrow)
		Sel++;
	WM_SetCapture(pObj, 1);
	SCROLLBAR_SetValue(pObj, Sel);
	if ((pObj->State & SCROLLBAR_STATE_PRESSED) == 0)
		_ScrollbarPressed(pObj);
}
static void _OnKey(ScrollBar_Obj *pObj, WM_MESSAGE *pMsg) {
	const WM_KEY_INFO *pKeyInfo = (const WM_KEY_INFO *)pMsg->Data;
	int Key = pKeyInfo->Key;
	if (pKeyInfo->PressedCnt <= 0)
		return;
	switch (Key) {
	case GUI_KEY_RIGHT:
	case GUI_KEY_DOWN:
		SCROLLBAR_Inc(pObj);
		break;
	case GUI_KEY_LEFT:
	case GUI_KEY_UP:
		SCROLLBAR_Dec(pObj);
		break;
	default:
		return;
	}
}
static void _OnSetScrollState(ScrollBar_Obj *pObj, const WM_SCROLL_STATE *pState) {
	if (pState->NumItems == pObj->NumItems &&
		pObj->PageSize == pState->PageSize &&
		pObj->v == pState->v)
		return;
	pObj->NumItems = pState->NumItems;
	pObj->PageSize = pState->PageSize;
	pObj->v = pState->v;
	pObj->Invalidate();
}
void SCROLLBAR__InvalidatePartner(ScrollBar_Obj *pObj) {     /* Invalidate the partner, since it is also affected */
	WM_GetScrollPartner(pObj)->Invalidate();
	WM_SendMessageNoPara(pObj->Parent(), WM_NOTIFY_CLIENTCHANGE);   /* Client area may have changed */
}
static void _SCROLLBAR_Callback(WM_MESSAGE *pMsg) {
	ScrollBar_Obj *pObj = (ScrollBar_Obj *)pMsg->pWin;
	if (WIDGET_HandleActive(pObj, pMsg) == 0)
		return;
	switch (pMsg->MsgId) {
	case WM_DELETE:
		SCROLLBAR__InvalidatePartner(pObj);
		break;
	case WM_PAINT:
		_Paint(pObj);
		return;
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		break;
	case WM_KEY:
		_OnKey(pObj, pMsg);
		break;
	case WM_SET_SCROLL_STATE:
		_OnSetScrollState(pObj, (const WM_SCROLL_STATE *)pMsg->Data);
		break;
	case WM_GET_SCROLL_STATE:
	{
		WM_SCROLL_STATE *wsc = (WM_SCROLL_STATE *)pMsg->Data;
		wsc->NumItems = pObj->NumItems;
		wsc->PageSize = pObj->PageSize;
		wsc->v = pObj->v;
		break;
	}
	}
	WM_DefaultProc(pMsg);
}
ScrollBar_Obj *SCROLLBAR_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent,
									int WinFlags, int ExFlags, int Id) {
	if (xsize == 0 && ysize == 0) {
		auto &&rect = pParent->InsideRect();
		if (ExFlags & SCROLLBAR_CF_VERTICAL) {
			xsize = SCROLLBAR__DefaultWidth;
			x0 = rect.x1 + 1 - xsize;
			y0 = rect.y0;
			ysize = rect.y1 - rect.y0 + 1;
		}
		else {
			ysize = SCROLLBAR__DefaultWidth;
			y0 = rect.y1 + 1 - ysize;
			x0 = rect.x0;
			xsize = rect.x1 - rect.x0 + 1;
		}
	}
	auto pObj = (ScrollBar_Obj *)WObj::Create(
		x0, y0, xsize, ysize, pParent, WinFlags, _SCROLLBAR_Callback,
		sizeof(ScrollBar_Obj) - sizeof(WObj));
	if (!pObj) 
		return 0;
	uint16_t InitState = 0;
	if (ExFlags & SCROLLBAR_CF_VERTICAL)
		InitState |= WIDGET_CF_VERTICAL;
	if (ExFlags & SCROLLBAR_CF_FOCUSSABLE)
		InitState |= WIDGET_STATE_FOCUSSABLE;
	if (Id != GUI_ID_HSCROLL && Id != GUI_ID_VSCROLL)
		InitState |= WIDGET_STATE_FOCUSSABLE;
	WIDGET__Init(pObj, Id, InitState);
	pObj->aBkColor[0] = SCROLLBAR__aDefaultBkColor[0];
	pObj->aBkColor[1] = SCROLLBAR__aDefaultBkColor[1];
	pObj->aColor[0] = SCROLLBAR__aDefaultColor[0];
	pObj->aColor[1] = SCROLLBAR__aDefaultColor[1];
	pObj->NumItems = 100;
	pObj->PageSize = 10;
	pObj->v = 0;
	SCROLLBAR__InvalidatePartner(pObj);
	return pObj;
}
ScrollBar_Obj *SCROLLBAR_CreateAttached(WObj *pParent, int SpecialFlags) {
	int Id;
	int WinFlags;
	if (SpecialFlags & SCROLLBAR_CF_VERTICAL) {
		Id = GUI_ID_VSCROLL;
		WinFlags = WC_VISIBLE | WC_STAYONTOP | WC_ANCHOR_RIGHT | WC_ANCHOR_TOP | WC_ANCHOR_BOTTOM;
	}
	else {
		Id = GUI_ID_HSCROLL;
		WinFlags = WC_VISIBLE | WC_STAYONTOP | WC_ANCHOR_BOTTOM | WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT;
	}
	ScrollBar_Obj *hThis = SCROLLBAR_CreateEx(0, 0, 0, 0, pParent, WinFlags, SpecialFlags, Id);
	WM_NotifyParent(hThis, WM_NOTIFICATION_SCROLLBAR_ADDED);
	return hThis;
}
ScrollBar_Obj *SCROLLBAR_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb) {
	return SCROLLBAR_CreateEx(
		pCreateInfo->x0 + x0, pCreateInfo->y0 + y0, pCreateInfo->xSize, pCreateInfo->ySize,
		pParent, 0, pCreateInfo->Flags, pCreateInfo->Id);
}
void SCROLLBAR_Dec(ScrollBar_Obj *pObj) {
	SCROLLBAR_AddValue(pObj, -1);
}
void SCROLLBAR_Inc(ScrollBar_Obj *pObj) {
	SCROLLBAR_AddValue(pObj, 1);
}
void SCROLLBAR_AddValue(ScrollBar_Obj *pObj, int Add) {
	if (!pObj)
		return;
	SCROLLBAR_SetValue(pObj, pObj->v + Add);
}
void SCROLLBAR_SetValue(ScrollBar_Obj *pObj, int v) {
	if (!pObj)
		return;
	int Max = pObj->NumItems - pObj->PageSize;
	if (Max < 0)
		Max = 0;
	if (v < 0)
		v = 0;
	if (v > Max)
		v = Max;
	if (pObj->v == v)
		return;
	pObj->v = v;
	pObj->Invalidate();
	WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
}
void SCROLLBAR_SetNumItems(ScrollBar_Obj *pObj, int NumItems) {
	if (!pObj)
		return;
	if (pObj->NumItems == NumItems)
		return;
	pObj->NumItems = NumItems;
	pObj->Invalidate();
}
void SCROLLBAR_SetPageSize(ScrollBar_Obj *pObj, int PageSize) {
	if (!pObj)
		return;
	if (pObj->PageSize == PageSize)
		return;
	pObj->PageSize = PageSize;
	pObj->Invalidate();
}
void SCROLLBAR_SetState(ScrollBar_Obj *pObj, const WM_SCROLL_STATE *pState) {
	if (!pObj)
		return;
	SCROLLBAR_SetPageSize(pObj, pState->PageSize);
	SCROLLBAR_SetNumItems(pObj, pState->NumItems);
	SCROLLBAR_SetValue(pObj, pState->v);
}
void SCROLLBAR_SetWidth(ScrollBar_Obj *pObj, int Width) {
	WIDGET_SetWidth(pObj, Width);
	SCROLLBAR__InvalidatePartner(pObj);
}
int SCROLLBAR_GetDefaultWidth(void) {
	return SCROLLBAR__DefaultWidth;
}
RGBC SCROLLBAR_GetDefaultBkColor(unsigned int Index) {
	if (Index < GUI_COUNTOF(SCROLLBAR__aDefaultBkColor))
		return SCROLLBAR__aDefaultBkColor[Index];
	return 0;
}
RGBC SCROLLBAR_GetDefaultColor(unsigned int Index) {
	if (Index < GUI_COUNTOF(SCROLLBAR__aDefaultColor))
		return SCROLLBAR__aDefaultColor[Index];
	return 0;
}
int  SCROLLBAR_SetDefaultWidth(int DefaultWidth) {
	int OldWidth = SCROLLBAR__DefaultWidth;
	SCROLLBAR__DefaultWidth = DefaultWidth;
	return OldWidth;
}
RGBC SCROLLBAR_SetDefaultBkColor(RGBC Color, unsigned int Index) {
	if (Index < GUI_COUNTOF(SCROLLBAR__aDefaultBkColor))
		SCROLLBAR__aDefaultBkColor[Index] = Color;
	return 0;
}
RGBC SCROLLBAR_SetDefaultColor(RGBC Color, unsigned int Index) {
	if (Index < GUI_COUNTOF(SCROLLBAR__aDefaultColor))
		SCROLLBAR__aDefaultColor[Index] = Color;
	return 0;
}
int SCROLLBAR_GetValue(ScrollBar_Obj *pObj) {
	if (!pObj)
		return 0;
	return pObj->v;
}
