#include "ScrollBar.h"
#include "MultiPage.h"

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
static void _AddScrollbar(MultiPage *pObj, int x, int y, int w, int h) {
	if (auto pScroll = pObj->ScrollBarH()) {
		pScroll->MoveChildTo({ x, y });
		pScroll->Size({ w, h });
	}
	else {
		pScroll = ScrollBar::Create(
			x, y, w, h,
			pObj, GUI_ID_HSCROLL, WC_VISIBLE, 0);
		pScroll->Effect(pObj->Effect());
	}
	pObj->State |= MULTIPAGE_STATE_SCROLLMODE;
}
static void _SetScrollbar(MultiPage *pObj, int NumItems) {
	auto pScroll = pObj->ScrollBarH();
	pScroll->NumItems(NumItems);
	pScroll->PageSize(1);
	if (pObj->ScrollState >= NumItems)
		pObj->ScrollState = 0;
	pScroll->Value(pObj->ScrollState);
}
static void _DeleteScrollbar(MultiPage *pObj) {
	pObj->ScrollBarH()->Delete();
	pObj->State &= ~MULTIPAGE_STATE_SCROLLMODE;
}
static void _ShowPage(MultiPage *pObj, unsigned Index) {
	WObj *pWin = nullptr;
	if ((int)Index < pObj->Handles.NumItems())
		pWin = pObj->Handles[Index].pWin;
	WObj *pClient = pObj->pClient;
	for (auto pChild = pClient->pFirstChild; pChild; pChild = pChild->NextSibling())
		if (pChild == pWin) {
			pChild->Visible(true);
			pChild->Focus();
		}
		else
			pChild->Visible(false);
}
static void _SetEnable(MultiPage *pObj, unsigned Index, int State) {
	if (Index >= pObj->Handles.NumItems())
		return;
	auto &page = pObj->Handles[Index];
	if (State)
		page.Status |= MULTIPAGE_STATE_ENABLED;
	else
		page.Status &= ~MULTIPAGE_STATE_ENABLED;
}
static bool _GetEnable(MultiPage *pObj, unsigned Index) {
	if (Index >= pObj->Handles.NumItems())
		return false;
	auto &page = pObj->Handles[Index];
	return page.Status & MULTIPAGE_STATE_ENABLED;
}
static SRect _CalcClientRect(MultiPage *pObj) {
	auto &&rInside = pObj->InsideRect();
	if (pObj->Align & MULTIPAGE_ALIGN_BOTTOM)
		rInside.y1 -= GUI_GetYSizeOfFont(pObj->Font) + 6;
	else
		rInside.y0 += GUI_GetYSizeOfFont(pObj->Font) + 6;
	return rInside;
}
static SRect _CalcBorderRect(MultiPage *pObj) {
	auto &&r = pObj->ClientRect();
	if (pObj->Align & MULTIPAGE_ALIGN_BOTTOM)
		r.y1 -= GUI_GetYSizeOfFont(pObj->Font) + 6;
	else
		r.y0 += GUI_GetYSizeOfFont(pObj->Font) + 6;
	return r;
}
static int _GetPageSizeX(MultiPage *pObj, unsigned Index) {
	if ((int)Index >= pObj->Handles.NumItems())
		return 0;
	auto &page = pObj->Handles[Index];
	GUI.Font(pObj->Font);
	return GUI_GetStringDistX(page.pText) + 10;
}
static int _GetPagePosX(MultiPage *pObj, unsigned Index) {
	unsigned i, r = 0;
	for (i = 0; i < Index; ++i)
		r += _GetPageSizeX(pObj, i);
	return r;
}
static int _GetTextWidth(MultiPage *pObj) {
	return _GetPagePosX(pObj, pObj->Handles.NumItems());
}
static void _GetTextRect(MultiPage *pObj, SRect *pRect) {
	int Height = GUI_GetYSizeOfFont(pObj->Font) + 6;
	auto &&rBorder = _CalcBorderRect(pObj);
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
static void _UpdatePositions(MultiPage *pObj) {
	auto Width = _GetTextWidth(pObj);
	auto &&rBorder = _CalcBorderRect(pObj);
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
	rBorder = _CalcClientRect(pObj);
	pObj->pClient->MoveChildTo(rBorder.left_top());
	pObj->pClient->Size(rBorder.size());
	pObj->Invalidate();
}
static void _DrawTextItem(MultiPage *pObj, const char *pText, unsigned Index,
						  const SRect *pRect, int x0, int w, int ColorIndex) {
	SRect r = *pRect;
	r.x0 += x0;
	r.x1 = r.x0 + w;
	pObj->DrawUp(r);
	r /= pObj->EffectSize();
	if (pObj->Selection == Index) {
		if (pObj->Align & MULTIPAGE_ALIGN_BOTTOM) {
			r.y0 -= pObj->EffectSize() + 1;
			if (pObj->EffectSize() > 1) {
				GUI.PenColor(RGB_WHITE);
				GUI.DrawLineV(r.x0 - 1, r.y0, r.y0 + 1);
				GUI.PenColor(0x555555);
				GUI.DrawLineV(r.x1 + 1, r.y0, r.y0 + 1);
			}
		}
		else {
			r.y1 += pObj->EffectSize() + 1;
			if (pObj->EffectSize() > 1) {
				GUI.PenColor(RGB_WHITE);
				GUI.DrawLineV(r.x0 - 1, r.y1 - 2, r.y1 - 1);
				GUI.PenColor(0x555555);
				GUI.DrawLineV(r.x1 + 1, r.y1 - 2, r.y1 - 1);
			}
		}
	}
	GUI.PenColor(pObj->aBkColor[ColorIndex]);
	pObj->Fill(r);
	GUI.BkColor(pObj->aBkColor[ColorIndex]);
	GUI.PenColor(pObj->aTextColor[ColorIndex]);
	GUI_DispStringAt(pText, { r.x0 + 4, pRect->y0 + 3 });
}
static void _OnPaint(MultiPage *pObj) {
	auto &&rBorder = _CalcBorderRect(pObj);
	pObj->DrawUp(rBorder);
	if (pObj->Handles.NumItems() <= 0)
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
	for (i = 0; i < pObj->Handles.NumItems(); ++i) {
		auto &page = pObj->Handles[i];
		x0 += w;
		w = GUI_GetStringDistX(page.pText) + 10;
		_DrawTextItem(pObj, page.pText, i, &rText, x0, w, (page.Status & MULTIPAGE_STATE_ENABLED) ? 1 : 0);
	}
	WObj::SetUserClipRect(nullptr);
}
static bool _ClickedOnMultipage(MultiPage *pObj, Point Pos) {
	SRect rText;
	_GetTextRect(pObj, &rText);
	if (Pos.y < rText.y0 || Pos.y > rText.y1)
		return true;
	if (pObj->Handles.NumItems() <= 0 || Pos.x < rText.x0 || Pos.x > rText.x1)
		return false;
	int w = 0, x0 = rText.x0;
	if (pObj->State & MULTIPAGE_STATE_SCROLLMODE)
		x0 -= _GetPagePosX(pObj, pObj->ScrollState);
	for (int i = 0; i < pObj->Handles.NumItems(); ++i) {
		x0 += w;
		w = _GetPageSizeX(pObj, i);
		if (Pos.x >= x0 && Pos.x <= x0 + w - 1) {
			MULTIPAGE_SelectPage(pObj, i);
			pObj->NotifyParent(WN_VALUE_CHANGED);
			return 1;
		}
	}
	return 0;
}
static void _OnTouch(MultiPage *pObj, WM_PARAM *pData) {
	int Notification;
	if (auto pState = (PidState *)*pData) {
		if (pState->Pressed) {
			Point Pos = *pState;
			if (_ClickedOnMultipage(pObj, Pos))
				pObj->BringToTop();
			else {
				Pos += pObj->Position();
				if (auto pBelow = WObj::ScreenToWin(Pos, pObj)) {
					*pState = Pos - pBelow->Position();
					pWin = pBelow;
					pBelow->cb(pData);
				}
			}
			Notification = WN_CLICKED;
		}
		else
			Notification = WN_RELEASED;
	}
	else
		Notification = WN_MOVED_OUT;
	pObj->NotifyParent(Notification);
}
static void _Callback(int msgid, WM_PARAM *pData) {
	auto pObj = (MultiPage *)pWin;
	bool Handled = pObj->HandleActive(msgid, pData);
	switch (msgid) {
	case WM_PAINT:
		_OnPaint(pObj);
		break;
	case WM_TOUCH:
		_OnTouch(pObj, pData);
		break;
	case WM_NOTIFY_PARENT:
		if (*pData == WN_VALUE_CHANGED)
			if (pData->pWinSrc->ID() == GUI_ID_HSCROLL) {
				pObj->ScrollState = ((ScrollBar *)pData->pWinSrc)->Value();
				pObj->Invalidate();
			}
		break;
	case WM_GET_CLIENT_WINDOW:
		*pData = (WM_PARAM)pObj->pClient;
		break;
	case WM_GET_INSIDE_RECT:
		*(SRect *)*pData = _CalcClientRect(pObj);
		break;
	case WM_WIDGET_SET_EFFECT:
		if (auto pScroll = pObj->ScrollBarH())
			pScroll->Effect((const Widget::EffectItf *)*pData);
		break;
	case WM_SIZE:
		_UpdatePositions(pObj);
		break;
	case WM_DELETE:
		pObj->Handles.Delete();
		break;
	}
	if (Handled)
		DefCallback(pObj, msgid, pData, pWinSrc);
}
static void _ClientCallback(int msgid, WM_PARAM *pData) {
	auto pObj = pWin;
	auto pParent = (MultiPage *)pObj->Parent();
	switch (msgid) {
	case WM_PAINT:
		GUI.BkColor(pParent->aBkColor[1]);
		GUI.Clear();
		break;
	case WM_TOUCH:
		pParent->Focus();
		pParent->BringToTop();
		break;
	case WM_GET_CLIENT_WINDOW:
		*pData = (WM_PARAM)pObj;
		break;
	case WM_GET_INSIDE_RECT:
		DefCallback(pObj, msgid, pData, pWinSrc);
	}
}
MultiPage *MULTIPAGE_CreateEx(
	int x0, int y0, int xsize, int ysize, WObj *pParent,
	uint16_t Flags, uint16_t ExFlags, uint16_t Id) {
	//auto pObj = (MultiPage *)WObj::Create(
	//	x0, y0, xsize, ysize, pParent, Flags, &_Callback,
	//	sizeof(MultiPage) - sizeof(WObj));
	//if (!pObj) 
	//	return 0;
	//pObj->Init(Id, WIDGET_STATE_FOCUSSABLE);
	//pObj->aBkColor[0] = MULTIPAGE__DefaultBkColor[0];
	//pObj->aBkColor[1] = MULTIPAGE__DefaultBkColor[1];
	//pObj->aTextColor[0] = MULTIPAGE__DefaultTextColor[0];
	//pObj->aTextColor[1] = MULTIPAGE__DefaultTextColor[1];
	//pObj->Font = MULTIPAGE__pDefaultFont;
	//pObj->Align = MULTIPAGE__DefaultAlign;
	//pObj->Selection = 0xffff;
	//pObj->ScrollState = 0;
	//pObj->State = 0;
	//auto &&rClient = _CalcClientRect(pObj);
	//pObj->pClient = WObj::Create(
	//	rClient.x0, rClient.y0,
	//	rClient.x1 - rClient.x0 + 1,
	//	rClient.y1 - rClient.y0 + 1,
	//	pObj, WC_VISIBLE |
	//	WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT |
	//	WC_ANCHOR_TOP | WC_ANCHOR_BOTTOM
	//	, &_ClientCallback, 0);
	//_UpdatePositions(pObj);
	//return pObj;
}
void MULTIPAGE_AddPage(MultiPage *pObj, WObj *pWin, const char *pText) {
	if (!pObj)
		return;
	if (!pWin) {
		WObj *pClient = pObj->pClient;
		for (auto pChild = pClient->pFirstChild; pChild && !pWin; pChild = pChild->NextSibling()) {
			pWin = pChild;
			for (int i = 0; i < pObj->Handles.NumItems(); ++i) {
				auto &page = pObj->Handles[i];
				if (page.pWin == pChild)
					return;
			}
		}
	}
	else
		pWin->Parent(pObj->pClient, 0);
	if (pWin) {
		Page page;
		char NullByte = 0;
		if (!pText)
			pText = &NullByte;
		page.pWin = pWin;
		page.Status = MULTIPAGE_STATE_ENABLED;
		GUI__SetText(&page.pText, pText);
		pObj->Handles.Add(page);
		MULTIPAGE_SelectPage(pObj, pObj->Handles.NumItems() - 1);
	}
}
void MULTIPAGE_DeletePage(MultiPage *pObj, unsigned Index, int Delete) {
	if (!pObj)
		return;
	if (Index >= pObj->Handles.NumItems())
		return;
	auto &page = pObj->Handles[Index];
	auto pWin = page.pWin;
	if (Index == pObj->Selection) {
		if (Index == pObj->Handles.NumItems() - 1) {
			_ShowPage(pObj, Index - 1);
			pObj->Selection--;
		}
		else
			_ShowPage(pObj, Index + 1);
	}
	else if (Index < pObj->Selection)
		pObj->Selection--;
	pObj->Handles.Delete(Index);
	_UpdatePositions(pObj);
	if (Delete)
		pWin->Delete();
}
void MULTIPAGE_SelectPage(MultiPage *pObj, unsigned Index) {
	if (!pObj)
		return;
	if (Index >= pObj->Handles.NumItems())
		return;
	if (Index == pObj->Selection && _GetEnable(pObj, Index))
		return;
	_ShowPage(pObj, Index);
	pObj->Selection = Index;
	_UpdatePositions(pObj);
}
void MULTIPAGE_DisablePage(MultiPage *pObj, unsigned Index) {
	if (!pObj)
		return;
	_SetEnable(pObj, Index, 0);
	pObj->Invalidate();
}
void MULTIPAGE_EnablePage(MultiPage *pObj, unsigned Index) {
	if (!pObj)
		return;
	_SetEnable(pObj, Index, 1);
	pObj->Invalidate();
}
void MULTIPAGE_SetText(MultiPage *pObj, const char *pText, unsigned Index) {
	if (!pObj)
		return;
	if (!pText)
		return;
	if (Index >= pObj->Handles.NumItems())
		return;
	GUI__SetText(&pObj->Handles[Index].pText, pText);
	_UpdatePositions(pObj);
}
void MULTIPAGE_SetBkColor(MultiPage *pObj, RGBC Color, unsigned Index) {
	if (!pObj)
		return;
	if (Index >= MULTIPAGE_NUMCOLORS)
		return;
	pObj->aBkColor[Index] = Color;
	pObj->Invalidate();
}
void MULTIPAGE_SetTextColor(MultiPage *pObj, RGBC Color, unsigned Index) {
	if (!pObj)
		return;
	if (Index >= MULTIPAGE_NUMCOLORS)
		return;
	pObj->aTextColor[Index] = Color;
	pObj->Invalidate();
}
void MULTIPAGE_SetFont(MultiPage *pObj, CFont *pFont) {
	if (!pObj)
		return;
	if (pFont)
		return;
	pObj->Font = pFont;
	_UpdatePositions(pObj);
}
void MULTIPAGE_SetAlign(MultiPage *pObj, unsigned Align) {
	if (!pObj)
		return;
	pObj->Align = Align;
	auto &&rClient = _CalcClientRect(pObj);
	pObj->pClient->PositionScreen(rClient.left_top() + pObj->rect.left_top());
	_UpdatePositions(pObj);
}
int MULTIPAGE_GetSelection(MultiPage *pObj) {
	if (!pObj)
		return 0;
	return pObj->Selection;
}
WObj *MULTIPAGE_GetWindow(MultiPage *pObj, unsigned Index) {
	if (!pObj)
		return nullptr;
	if (Index >= pObj->Handles.NumItems())
		return nullptr;
	return pObj->Handles[Index].pWin;
}
int MULTIPAGE_IsPageEnabled(MultiPage *pObj, unsigned Index) {
	if (!pObj)
		return 0;
	return _GetEnable(pObj, Index);
}
