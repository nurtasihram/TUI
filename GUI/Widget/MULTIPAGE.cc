#include "GUI_ARRAY.h"

#include "SCROLLBAR.h"
#include "MULTIPAGE.h"
#define MAX(a, b)	((a > b) ? a : b)
#define MULTIPAGE_FONT_DEFAULT       &GUI_Font13_1
#define MULTIPAGE_ALIGN_DEFAULT      (MULTIPAGE_ALIGN_LEFT | MULTIPAGE_ALIGN_TOP)
#define MULTIPAGE_NUMCOLORS          2
#define MULTIPAGE_BKCOLOR0_DEFAULT   0xD0D0D0 /* disabled page */
#define MULTIPAGE_BKCOLOR1_DEFAULT   0xC0C0C0 /* enabled page */
#define MULTIPAGE_TEXTCOLOR0_DEFAULT 0x808080 /* disabled page */
#define MULTIPAGE_TEXTCOLOR1_DEFAULT 0x000000 /* enabled page */
#define MULTIPAGE_STATE_ENABLED     (1<<0)
#define MULTIPAGE_STATE_SCROLLMODE  WIDGET_STATE_USER0
CFont   *MULTIPAGE__pDefaultFont = MULTIPAGE_FONT_DEFAULT;
unsigned MULTIPAGE__DefaultAlign = MULTIPAGE_ALIGN_DEFAULT;
RGBC     MULTIPAGE__DefaultBkColor[2] = { MULTIPAGE_BKCOLOR0_DEFAULT, MULTIPAGE_BKCOLOR1_DEFAULT };
RGBC     MULTIPAGE__DefaultTextColor[2] = { MULTIPAGE_TEXTCOLOR0_DEFAULT, MULTIPAGE_TEXTCOLOR1_DEFAULT };
static void _AddScrollbar(MultiPage_Obj *pObj, int x, int y, int w, int h) {
	if (auto pScroll = WM_GetScrollbarH(pObj)) {
		WM_MoveChildTo(pScroll, x, y);
		pScroll->Size({ w, h });
	}
	else {
		pScroll = SCROLLBAR_Create(x, y, w, h,
								   pObj, GUI_ID_HSCROLL, WC_VISIBLE, 0);
		WIDGET_SetEffect(pScroll, pObj->pEffect);
	}
	pObj->State |= MULTIPAGE_STATE_SCROLLMODE;
}
static void _SetScrollbar(MultiPage_Obj *pObj, int NumItems) {
	auto pScroll = WM_GetScrollbarH(pObj);
	SCROLLBAR_SetNumItems(pScroll, NumItems);
	SCROLLBAR_SetPageSize(pScroll, 1);
	if (pObj->ScrollState >= NumItems)
		pObj->ScrollState = 0;
	SCROLLBAR_SetValue(pScroll, pObj->ScrollState);
}
static void _DeleteScrollbar(MultiPage_Obj *pObj) {
	WM_GetScrollbarH(pObj)->Delete();
	pObj->State &= ~MULTIPAGE_STATE_SCROLLMODE;
}
static void _ShowPage(MultiPage_Obj *pObj, unsigned Index) {
	WObj *pWin = 0;
	if ((int)Index < pObj->Handles.NumItems) {
		MULTIPAGE_PAGE *pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, Index);
		pWin = pPage->pWin;
	}
	WObj *pClient = pObj->pClient;
	WObj *pChild;
	for (pChild = pClient->pFirstChild; pChild; pChild = pChild->pNext)
		if (pChild == pWin) {
			pChild->Visible(true);
			pChild->Focus();
		}
		else
			pChild->Visible(false);
}
static void _SetEnable(MultiPage_Obj *pObj, unsigned Index, int State) {
	if ((int)Index >= pObj->Handles.NumItems)
		return;
	MULTIPAGE_PAGE *pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, Index);
	if (State)
		pPage->Status |= MULTIPAGE_STATE_ENABLED;
	else
		pPage->Status &= ~MULTIPAGE_STATE_ENABLED;
}
static int _GetEnable(MultiPage_Obj *pObj, unsigned Index) {
	if ((int)Index >= pObj->Handles.NumItems)
		return 0;
	MULTIPAGE_PAGE *pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, Index);
	return (pPage->Status & MULTIPAGE_STATE_ENABLED) ? 1 : 0;
}
static void _CalcClientRect(MultiPage_Obj *pObj, SRect *pRect) {
	WIDGET__GetInsideRect(pObj, pRect);
	if (pObj->Align & MULTIPAGE_ALIGN_BOTTOM)
		pRect->y1 -= GUI_GetYSizeOfFont(pObj->Font) + 6;
	else
		pRect->y0 += GUI_GetYSizeOfFont(pObj->Font) + 6;
}
static void _CalcBorderRect(MultiPage_Obj *pObj, SRect *pRect) {
	WM__GetClientRectWin(pObj, pRect);
	if (pObj->Align & MULTIPAGE_ALIGN_BOTTOM)
		pRect->y1 -= GUI_GetYSizeOfFont(pObj->Font) + 6;
	else
		pRect->y0 += GUI_GetYSizeOfFont(pObj->Font) + 6;
}
static int _GetPageSizeX(MultiPage_Obj *pObj, unsigned Index) {
	if ((int)Index >= pObj->Handles.NumItems)
		return 0;
	MULTIPAGE_PAGE *pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, Index);
	GUI.Font(pObj->Font);
	return GUI_GetStringDistX(&pPage->acText) + 10;
}
static int _GetPagePosX(MultiPage_Obj *pObj, unsigned Index) {
	unsigned i, r = 0;
	for (i = 0; i < Index; i++)
		r += _GetPageSizeX(pObj, i);
	return r;
}
static int _GetTextWidth(MultiPage_Obj *pObj) {
	return _GetPagePosX(pObj, pObj->Handles.NumItems);
}
static void _GetTextRect(MultiPage_Obj *pObj, SRect *pRect) {
	int Height = GUI_GetYSizeOfFont(pObj->Font) + 6;
	SRect rBorder;
	_CalcBorderRect(pObj, &rBorder);
	pRect->y0 = (pObj->Align & MULTIPAGE_ALIGN_BOTTOM) ?
		rBorder.y1 : 0;
	pRect->y1 = pRect->y0 + Height;
	int Width = (pObj->State & MULTIPAGE_STATE_SCROLLMODE) ?
		rBorder.x1 - ((Height * 3) >> 1) - 3 :
		_GetTextWidth(pObj);
	if (pObj->Align & MULTIPAGE_ALIGN_RIGHT) {
		pRect->x0 = rBorder.x1 - Width;
		pRect->x1 = rBorder.x1;
	}
	else {
		pRect->x0 = 0;
		pRect->x1 = Width;
	}
}
static void _UpdatePositions(MultiPage_Obj *pObj) {
	SRect rBorder;
	int Width;
	Width = _GetTextWidth(pObj);
	_CalcBorderRect(pObj, &rBorder);
	/* Set scrollmode according to the text width */
	if (Width > rBorder.x1) {
		SRect rText;
		int Size, x0, y0, NumItems = 0;
		Size = ((GUI_GetYSizeOfFont(pObj->Font) + 6) * 3) >> 2;
		x0 = (pObj->Align & MULTIPAGE_ALIGN_RIGHT) ? (rBorder.x0) : (rBorder.x1 - 2 * Size + 1);
		y0 = (pObj->Align & MULTIPAGE_ALIGN_BOTTOM) ? (rBorder.y1) : (rBorder.y0 - Size + 1);
		/* A scrollbar is required so we add one to the multipage */
		_AddScrollbar(pObj, x0, y0, 2 * Size, Size);
		_GetTextRect(pObj, &rText);
		while (Width >= MAX((rText.x1 - rText.x0 + 1), 1)) {
			Width -= _GetPageSizeX(pObj, NumItems++);
		}
		_SetScrollbar(pObj, NumItems + 1);
	}
	else {
		/* Scrollbar is no longer required. We delete it if there was one */
		_DeleteScrollbar(pObj);
	}
	/* Move and resize the client area to the updated positions */
	_CalcClientRect(pObj, &rBorder);
	WM_MoveChildTo(pObj->pClient, rBorder.x0, rBorder.y0);
	pObj->pClient->Size(rBorder.size());
	pObj->Invalidate();
}
static void _DrawTextItem(MultiPage_Obj *pObj, const char *pText, unsigned Index,
						  const SRect *pRect, int x0, int w, int ColorIndex) {
	SRect r = *pRect;
	r.x0 += x0;
	r.x1 = r.x0 + w;
	WIDGET__EFFECT_DrawUpRect(pObj, &r);
	r /= pObj->pEffect->EffectSize;
	if (pObj->Selection == Index) {
		if (pObj->Align & MULTIPAGE_ALIGN_BOTTOM) {
			r.y0 -= pObj->pEffect->EffectSize + 1;
			if (pObj->pEffect->EffectSize > 1) {
				GUI.PenColor(GUI_WHITE);
				GUI_DrawVLine(r.x0 - 1, r.y0, r.y0 + 1);
				GUI.PenColor(0x555555);
				GUI_DrawVLine(r.x1 + 1, r.y0, r.y0 + 1);
			}
		}
		else {
			r.y1 += pObj->pEffect->EffectSize + 1;
			if (pObj->pEffect->EffectSize > 1) {
				GUI.PenColor(GUI_WHITE);
				GUI_DrawVLine(r.x0 - 1, r.y1 - 2, r.y1 - 1);
				GUI.PenColor(0x555555);
				GUI_DrawVLine(r.x1 + 1, r.y1 - 2, r.y1 - 1);
			}
		}
	}
	GUI.PenColor(pObj->aBkColor[ColorIndex]);
	WIDGET__FillRectEx(pObj, &r);
	GUI.BkColor(pObj->aBkColor[ColorIndex]);
	GUI.PenColor(pObj->aTextColor[ColorIndex]);
	GUI_DispStringAt(pText, r.x0 + 4, pRect->y0 + 3);
}
static void _Paint(MultiPage_Obj *pObj) {
	SRect rBorder;
	_CalcBorderRect(pObj, &rBorder);
	WIDGET__EFFECT_DrawUpRect(pObj, &rBorder);
	if (pObj->Handles.NumItems <= 0)
		return;
	int x0 = 0;
	if (pObj->State & MULTIPAGE_STATE_SCROLLMODE)
		x0 = (pObj->Align & MULTIPAGE_ALIGN_RIGHT) ?
		-_GetPagePosX(pObj, pObj->ScrollState) :
		-_GetPagePosX(pObj, pObj->ScrollState);
	SRect rText;
	_GetTextRect(pObj, &rText);
	SRect rClip = rText;
	rClip.y0 = rText.y0 - 1;
	rClip.y1 = rText.y1 + 1;
	WObj::SetUserClipRect(&rClip);
	GUI.Font(pObj->Font);
	int i, w = 0;
	for (i = 0; i < pObj->Handles.NumItems; i++) {
		MULTIPAGE_PAGE *pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, i);
		x0 += w;
		w = GUI_GetStringDistX(&pPage->acText) + 10;
		_DrawTextItem(pObj, &pPage->acText, i, &rText, x0, w, (pPage->Status & MULTIPAGE_STATE_ENABLED) ? 1 : 0);
	}
	WObj::SetUserClipRect(nullptr);
}
static bool _ClickedOnMultipage(MultiPage_Obj *pObj, Point Pos) {
	SRect rText;
	_GetTextRect(pObj, &rText);
	if (Pos.y < rText.y0 || Pos.y > rText.y1)
		return true;
	if (pObj->Handles.NumItems <= 0 || Pos.x < rText.x0 || Pos.x > rText.x1)
		return false;
	int w = 0, x0 = rText.x0;
	if (pObj->State & MULTIPAGE_STATE_SCROLLMODE)
		x0 -= _GetPagePosX(pObj, pObj->ScrollState);
	for (int i = 0; i < pObj->Handles.NumItems; i++) {
		x0 += w;
		w = _GetPageSizeX(pObj, i);
		if (Pos.x >= x0 && Pos.x <= x0 + w - 1) {
			MULTIPAGE_SelectPage(pObj, i);
			WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
			return 1;
		}
	}
	return 0;
}
static void _OnTouch(MultiPage_Obj *pObj, WM_MESSAGE *pMsg) {
	int Notification;
	if (auto pState = (PidState *)pMsg->Data) {
		if (pState->Pressed) {
			Point Pos = *pState;
			if (_ClickedOnMultipage(pObj, Pos))
				pObj->BringToTop();
			else {
				Pos += pObj->Position();
				if (auto pBelow = WObj::ScreenToWin(Pos, pObj)) {
					*pState = Pos - pBelow->Position();
					pMsg->pWin = pBelow;
					pBelow->cb(pMsg);
				}
			}
			Notification = WM_NOTIFICATION_CLICKED;
		}
		else
			Notification = WM_NOTIFICATION_RELEASED;
	}
	else
		Notification = WM_NOTIFICATION_MOVED_OUT;
	WM_NotifyParent(pObj, Notification);
}
static void _Callback(WM_MESSAGE *pMsg) {
	auto pObj = (MultiPage_Obj *)pMsg->pWin;
	int Handled = WIDGET_HandleActive(pObj, pMsg);
	switch (pMsg->MsgId) {
	case WM_PAINT:
		_Paint(pObj);
		break;
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		break;
	case WM_NOTIFY_PARENT:
		if (pMsg->Data == WM_NOTIFICATION_VALUE_CHANGED)
			if (pMsg->pWinSrc->ID() == GUI_ID_HSCROLL) {
				pObj->ScrollState = SCROLLBAR_GetValue((ScrollBar_Obj *)pMsg->pWinSrc);
				pObj->Invalidate();
			}
		break;
	case WM_GET_CLIENT_WINDOW:
		pMsg->Data = (size_t)pObj->pClient;
		break;
	case WM_GET_INSIDE_RECT:
		_CalcClientRect(pObj, (SRect *)pMsg->Data);
		break;
	case WM_WIDGET_SET_EFFECT:
		WIDGET_SetEffect((ScrollBar_Obj *)WM_GetScrollbarH(pObj), (const Widget::Effect *)pMsg->Data);
		break;
	case WM_SIZE:
		_UpdatePositions(pObj);
		break;
	case WM_DELETE:
		GUI_ARRAY_Delete(&pObj->Handles);
		break;
	default:
		if (Handled)
			WM_DefaultProc(pMsg);
	}
}
static void _ClientCallback(WM_MESSAGE *pMsg) {
	auto pObj = pMsg->pWin;
	auto pParent = (MultiPage_Obj *)pObj->Parent();
	switch (pMsg->MsgId) {
	case WM_PAINT:
		GUI.BkColor(pParent->aBkColor[1]);
		GUI_Clear();
		break;
	case WM_TOUCH:
		pParent->Focus();
		pParent->BringToTop();
		break;
	case WM_GET_CLIENT_WINDOW:
		pMsg->Data = (size_t)pObj;
		break;
	case WM_GET_INSIDE_RECT:
		WM_DefaultProc(pMsg);
	}
}
MultiPage_Obj *MULTIPAGE_CreateEx(
	int x0, int y0, int xsize, int ysize, WObj *pParent,
	int WinFlags, int ExFlags, int Id) {
	auto pObj = (MultiPage_Obj *)WObj::Create(
		x0, y0, xsize, ysize, pParent, WinFlags, &_Callback,
		sizeof(MultiPage_Obj) - sizeof(WObj));
	if (!pObj) 
		return 0;
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE);
	pObj->aBkColor[0] = MULTIPAGE__DefaultBkColor[0];
	pObj->aBkColor[1] = MULTIPAGE__DefaultBkColor[1];
	pObj->aTextColor[0] = MULTIPAGE__DefaultTextColor[0];
	pObj->aTextColor[1] = MULTIPAGE__DefaultTextColor[1];
	pObj->Font = MULTIPAGE__pDefaultFont;
	pObj->Align = MULTIPAGE__DefaultAlign;
	pObj->Selection = 0xffff;
	pObj->ScrollState = 0;
	pObj->State = 0;
	SRect rClient;
	_CalcClientRect(pObj, &rClient);
	pObj->pClient = WObj::Create(
		rClient.x0, rClient.y0,
		rClient.x1 - rClient.x0 + 1,
		rClient.y1 - rClient.y0 + 1,
		pObj, WC_VISIBLE |
		WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT |
		WC_ANCHOR_TOP | WC_ANCHOR_BOTTOM
		, &_ClientCallback, 0);
	_UpdatePositions(pObj);
	return pObj;
}
MultiPage_Obj *MULTIPAGE_CreateIndirect(
	const GUI_WIDGET_CREATE_INFO *pCreateInfo,
	WObj *pParent, int x0, int y0, WM_CALLBACK *cb) {
	return MULTIPAGE_CreateEx(
		pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,
		pCreateInfo->xSize, pCreateInfo->ySize,
		pParent, 0, pCreateInfo->Flags, pCreateInfo->Id);
}
void MULTIPAGE_AddPage(MultiPage_Obj *pObj, WObj *pWin, const char *pText) {
	if (!pObj)
		return;
	if (!pWin) {
		WObj *pClient = pObj->pClient;
		WObj *pChild;
		for (pChild = pClient->pFirstChild; pChild && !pWin; pChild = pChild->pNext) {
			pWin = pChild;
			int i;
			for (i = 0; i < pObj->Handles.NumItems; i++) {
				MULTIPAGE_PAGE *pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, i);
				if (pPage->pWin == pChild)
					return;
			}
		}
	}
	else
		pWin->Parent(pObj->pClient, 0);
	if (pWin) {
		MULTIPAGE_PAGE Page;
		char NullByte = 0;
		if (!pText)
			pText = &NullByte;
		Page.pWin = pWin;
		Page.Status = MULTIPAGE_STATE_ENABLED;
		if (GUI_ARRAY_AddItem(&pObj->Handles, &Page, (int)sizeof(MULTIPAGE_PAGE) + (int)GUI__strlen(pText)) == 0) {
			MULTIPAGE_PAGE *pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, pObj->Handles.NumItems - 1);
			GUI__memcpy(&pPage->acText, pText, GUI__strlen(pText) + 1);
		}
		MULTIPAGE_SelectPage(pObj, pObj->Handles.NumItems - 1);
	}
}
void MULTIPAGE_DeletePage(MultiPage_Obj *pObj, unsigned Index, int Delete) {
	if (!pObj)
		return;
	if ((int)Index >= pObj->Handles.NumItems)
		return;
	auto pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, Index);
	auto pWin = pPage->pWin;
	if (Index == pObj->Selection) {
		if (Index == ((unsigned)pObj->Handles.NumItems - 1)) {
			_ShowPage(pObj, Index - 1);
			pObj->Selection--;
		}
		else
			_ShowPage(pObj, Index + 1);
	}
	else if (Index < pObj->Selection)
		pObj->Selection--;
	GUI_ARRAY_DeleteItem(&pObj->Handles, Index);
	_UpdatePositions(pObj);
	if (Delete)
		pWin->Delete();
}
void MULTIPAGE_SelectPage(MultiPage_Obj *pObj, unsigned Index) {
	if (!pObj)
		return;
	if ((int)Index >= pObj->Handles.NumItems)
		return;
	if (Index == pObj->Selection && _GetEnable(pObj, Index))
		return;
	_ShowPage(pObj, Index);
	pObj->Selection = Index;
	_UpdatePositions(pObj);
}
void MULTIPAGE_DisablePage(MultiPage_Obj *pObj, unsigned Index) {
	if (!pObj)
		return;
	_SetEnable(pObj, Index, 0);
	pObj->Invalidate();
}
void MULTIPAGE_EnablePage(MultiPage_Obj *pObj, unsigned Index) {
	if (!pObj)
		return;
	_SetEnable(pObj, Index, 1);
	pObj->Invalidate();
}
void MULTIPAGE_SetText(MultiPage_Obj *pObj, const char *pText, unsigned Index) {
	if (!pObj)
		return;
	if (!pText)
		return;
	if ((int)Index >= pObj->Handles.NumItems)
		return;
	MULTIPAGE_PAGE *pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, Index);
	MULTIPAGE_PAGE Page;
	Page.pWin = pPage->pWin;
	Page.Status = pPage->Status;
	if (!GUI_ARRAY_SetItem(&pObj->Handles, Index, &Page, (int)sizeof(MULTIPAGE_PAGE) + (int)GUI__strlen(pText)))
		return;
	pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, Index);
	GUI__memcpy(&pPage->acText, pText, GUI__strlen(pText) + 1);
	_UpdatePositions(pObj);
}
void MULTIPAGE_SetBkColor(MultiPage_Obj *pObj, RGBC Color, unsigned Index) {
	if (!pObj)
		return;
	if ((int)Index >= MULTIPAGE_NUMCOLORS)
		return;
	pObj->aBkColor[Index] = Color;
	pObj->Invalidate();
}
void MULTIPAGE_SetTextColor(MultiPage_Obj *pObj, RGBC Color, unsigned Index) {
	if (!pObj)
		return;
	if ((int)Index >= MULTIPAGE_NUMCOLORS)
		return;
	pObj->aTextColor[Index] = Color;
	pObj->Invalidate();
}
void MULTIPAGE_SetFont(MultiPage_Obj *pObj, CFont *pFont) {
	if (!pObj)
		return;
	if (pFont)
		return;
	pObj->Font = pFont;
	_UpdatePositions(pObj);
}
void MULTIPAGE_SetAlign(MultiPage_Obj *pObj, unsigned Align) {
	if (!pObj)
		return;
	pObj->Align = Align;
	SRect rClient;
	_CalcClientRect(pObj, &rClient);
	pObj->pClient->Position(rClient.left_top() + pObj->rect.left_top());
	_UpdatePositions(pObj);
}
int MULTIPAGE_GetSelection(MultiPage_Obj *pObj) {
	if (!pObj)
		return 0;
	return pObj->Selection;
}
WObj *MULTIPAGE_GetWindow(MultiPage_Obj *pObj, unsigned Index) {
	if (!pObj)
		return 0;
	if ((int)Index >= pObj->Handles.NumItems)
		return 0;
	MULTIPAGE_PAGE *pPage = (MULTIPAGE_PAGE *)GUI_ARRAY_GetItem(&pObj->Handles, Index);
	return pPage->pWin;
}
int MULTIPAGE_IsPageEnabled(MultiPage_Obj *pObj, unsigned Index) {
	if (!pObj)
		return 0;
	return _GetEnable(pObj, Index);
}
unsigned MULTIPAGE_GetDefaultAlign(void) {
	return MULTIPAGE__DefaultAlign;
}
RGBC MULTIPAGE_GetDefaultBkColor(unsigned Index) {
	if (Index < GUI_COUNTOF(MULTIPAGE__DefaultBkColor))
		return MULTIPAGE__DefaultBkColor[Index];
	return GUI_INVALID_COLOR;
}
CFont *MULTIPAGE_GetDefaultFont(void) {
	return MULTIPAGE__pDefaultFont;
}
RGBC MULTIPAGE_GetDefaultTextColor(unsigned Index) {
	if (Index < GUI_COUNTOF(MULTIPAGE__DefaultTextColor))
		return MULTIPAGE__DefaultTextColor[Index];
	return GUI_INVALID_COLOR;
}
void MULTIPAGE_SetDefaultAlign(unsigned Align) {
	MULTIPAGE__DefaultAlign = Align;
}
void MULTIPAGE_SetDefaultBkColor(RGBC Color, unsigned Index) {
	if (Index < GUI_COUNTOF(MULTIPAGE__DefaultBkColor))
		MULTIPAGE__DefaultBkColor[Index] = Color;
}
void MULTIPAGE_SetDefaultFont(CFont *pFont) {
	MULTIPAGE__pDefaultFont = pFont;
}
void MULTIPAGE_SetDefaultTextColor(RGBC Color, unsigned Index) {
	if (Index < GUI_COUNTOF(MULTIPAGE__DefaultTextColor))
		MULTIPAGE__DefaultTextColor[Index] = Color;
}
