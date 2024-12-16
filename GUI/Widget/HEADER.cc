#include "ScrollBar.h"
#include "Header.h"

#define HEADER_FONT_DEFAULT      &GUI_Font13_1
#define HEADER_SUPPORT_DRAG      1
#define HEADER_BORDER_V_DEFAULT  0
#define HEADER_BORDER_H_DEFAULT  2
#define HEADER_BKCOLOR_DEFAULT   0xAAAAAA
#define HEADER_TEXTCOLOR_DEFAULT RGB_BLACK
#define HEADER_CURSOR_DEFAULT    &GUI_CursorHeaderM
static CCursor *_pOldCursor;
static CCursor *_pDefaultCursor = HEADER_CURSOR_DEFAULT;
static RGBC          _DefaultBkColor = HEADER_BKCOLOR_DEFAULT;
static RGBC          _DefaultTextColor = HEADER_TEXTCOLOR_DEFAULT;
static int           _DefaultBorderH = HEADER_BORDER_H_DEFAULT;
static int           _DefaultBorderV = HEADER_BORDER_V_DEFAULT;
static CFont*        _pDefaultFont = HEADER_FONT_DEFAULT;
static void _OnPaint(Header* pObj) {
	SRect rect;
	int xPos = -pObj->ScrollPos;
	int EffectSize = pObj->EffectSize();
	GUI.BkColor(pObj->BkColor);
	GUI.Font(pObj->pFont);
	GUI.Clear();
	for (int i = 0, NumItems = pObj->Columns.NumItems(); i < NumItems; ++i) {
		auto &Col = pObj->Columns[i];
		WM_GetClientRect(&rect);
		rect.x0 = xPos;
		rect.x1 = rect.x0 + Col.Width;
		if (Col.pDrawObj) {
			Point Off;
			auto &&size = Col.pDrawObj->Size();
			switch (Col.Align & TEXTALIGN_HORIZONTAL) {
			case TEXTALIGN_RIGHT:
				Off.x =  Col.Width - size.x;
				break;
			case TEXTALIGN_HCENTER:
				Off.x = (Col.Width - size.x) / 2;
				break;
			}
			switch (Col.Align & TEXTALIGN_VCENTER) {
			case TEXTALIGN_BOTTOM:
				Off.y =  rect.ysize() - size.y;
				break;
			case TEXTALIGN_VCENTER:
				Off.y = (rect.ysize() - size.y) / 2;
				break;
			}
			Off.x += xPos;
			WObj::SetUserClipRect(&rect);
			Col.pDrawObj->Draw({ Off, 0 });
			WObj::SetUserClipRect(nullptr);
		}
		pObj->DrawUp(rect);
		xPos += rect.x1 - rect.x0;
		rect.x0 += EffectSize + _DefaultBorderH;
		rect.x1 -= EffectSize + _DefaultBorderH;
		rect.y0 += EffectSize + _DefaultBorderV;
		rect.y1 -= EffectSize + _DefaultBorderV;
		GUI.PenColor(pObj->TextColor);
		GUI_DispStringInRect(Col.pText, &rect, Col.Align);
	}
	WM_GetClientRect(&rect);
	rect.x0 = xPos;
	rect.x1 = 0xfff;
	pObj->DrawUp(rect);
}
static void _RestoreOldCursor() {
	if (!_pOldCursor)
		return;
	GUI_CURSOR_Select(_pOldCursor);
	_pOldCursor = 0;
}
static void _FreeAttached(Header* pObj) {
	pObj->Columns.Destruct();
	_RestoreOldCursor();
}
#if (HEADER_SUPPORT_DRAG)
static int _GetItemIndex(Header *pObj, int x, int y) {
	if (!pObj)
		return -1;
	if (y < 0 || y >= pObj->SizeY())
		return -1;
	int xPos = 0;
	for (int i = 0, NumColumns = pObj->Columns.NumItems(); i < NumColumns; ++i) {
		auto &Col = pObj->Columns[i];
		xPos += Col.Width;
		if (xPos < x - 4 || xPos > x + 4)
			continue;
		if (i >= NumColumns - 1 || x >= xPos)
			continue;
		if (pObj->Columns[i + 1].Width == 0)
			return i;
	}
	return -1;
}
#endif
#if (HEADER_SUPPORT_DRAG)
static void _HandlePID(Header *pObj, int x, int y, int Pressed) {
	int Hit = _GetItemIndex(pObj, x, y);
	/* set capture position () */
	if ((Pressed == 1) && (Hit >= 0) && (pObj->CapturePosX == -1)) {
		pObj->CapturePosX = x;
		pObj->CaptureItem = Hit;
	}
	/* set mouse cursor and capture () */
	if (Hit >= 0) {
		pObj->Capture(true);
		if (!_pOldCursor)
			_pOldCursor = GUI_CURSOR_Select(_pDefaultCursor);
	}
	/* modify header */
	if ((pObj->CapturePosX >= 0) && (x != pObj->CapturePosX) && (Pressed == 1)) {
		int NewSize = HEADER_GetItemWidth(pObj, pObj->CaptureItem) + x - pObj->CapturePosX;
		if (NewSize >= 0) {
			HEADER_SetItemWidth(pObj, pObj->CaptureItem, NewSize);
			pObj->CapturePosX = x;
		}
	}
	/* release capture & restore cursor */
	if (Pressed <= 0)
		if (Hit == -1) {
			_RestoreOldCursor();
			pObj->CapturePosX = -1;
			WObj::CaptureRelease();
		}
}
#endif
#if (HEADER_SUPPORT_DRAG)
static void _OnMouseOver(Header *pObj, WM_MSG* pMsg) {
	const PidState* pState = (const PidState*)pMsg->data;
	if (pState)
		_HandlePID(pObj, pState->x + pObj->ScrollPos, pState->y, -1);
}
#endif
#if (HEADER_SUPPORT_DRAG)
static void _OnTouch(Header *pObj, WM_MSG *pMsg) {
	int Notification;
	const PidState *pState = (const PidState *)pMsg->data;
	if (pState) {
		_HandlePID(pObj, pState->x + pObj->ScrollPos, pState->y, pState->Pressed);
		Notification = pState->Pressed ? WM_NOTIFICATION_CLICKED : WM_NOTIFICATION_RELEASED;
	}
	else
		Notification = WM_NOTIFICATION_MOVED_OUT;
	pObj->NotifyParent(Notification);
}
#endif
static void _HEADER_Callback(WM_MSG* pMsg) {
	auto pObj = (Header *)pMsg->pWin;
	if (!pObj->HandleActive(pMsg))
		return;
	switch (pMsg->msgid) {
	case WM_PAINT:
		_OnPaint(pObj);
		break;
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		break;
	case WM_MOUSEOVER:
		_OnMouseOver(pObj, pMsg);
		break;
	case WM_DELETE:
		_FreeAttached(pObj); /* No return here ... DefCallback needs to be called */
	default:
		DefCallback(pMsg);
	}
}
Header *HEADER_CreateEx(
	int x0, int y0,
	int xsize, int ysize, WObj *pParent,
	uint16_t Flags, uint16_t ExFlags, uint16_t Id) {
	//if (xsize == 0 && x0 == 0 && y0 == 0) {
	//	auto &&rect = pParent->InsideRect();
	//	xsize = rect.x1 - rect.x0 + 1;
	//	x0 = rect.x0; y0 = rect.y0;
	//}
	//if (ysize == 0) {
	//	auto pEffect = Widget::DefaultEffect();
	//	ysize = GUI_GetYDistOfFont(_pDefaultFont);
	//	ysize += 2 * _DefaultBorderV;
	//	ysize += 2 * pEffect->EffectSize;
	//}
	//auto pObj = (Header *)WObj::Create(
	//	x0, y0, xsize, ysize,
	//	pParent, Flags | WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT,
	//	&_HEADER_Callback,
	//	sizeof(Header) - sizeof(WObj));
	//if (!pObj) {
	//	return 0;
	//}
	//pObj->Init(Id);
	//pObj->BkColor = _DefaultBkColor;
	//pObj->TextColor = _DefaultTextColor;
	//pObj->pFont = _pDefaultFont;
	//pObj->CapturePosX = -1;
	//pObj->CaptureItem = -1;
	//pObj->ScrollPos = 0;
	//return pObj;
}
CCursor * HEADER_SetDefaultCursor(CCursor* pCursor) {
	CCursor * pOldCursor = _pDefaultCursor;
	_pDefaultCursor = pCursor;
	return pOldCursor;
}
RGBC HEADER_SetDefaultBkColor(RGBC Color) {
	RGBC OldColor = _DefaultBkColor;
	_DefaultBkColor = Color;
	return OldColor;
}
RGBC HEADER_SetDefaultTextColor(RGBC Color) {
	RGBC OldColor = _DefaultTextColor;
	_DefaultTextColor = Color;
	return OldColor;
}
int HEADER_SetDefaultBorderH(int Spacing) {
	int OldSpacing = _DefaultBorderH;
	_DefaultBorderH = Spacing;
	return OldSpacing;
}
int HEADER_SetDefaultBorderV(int Spacing) {
	int OldSpacing = _DefaultBorderV;
	_DefaultBorderV = Spacing;
	return OldSpacing;
}
CFont* HEADER_SetDefaultFont(CFont* pFont) {
	CFont* pOldFont = _pDefaultFont;
	_pDefaultFont = pFont;
	return pOldFont;
}
CCursor *HEADER_GetDefaultCursor() { return _pDefaultCursor; }
RGBC          HEADER_GetDefaultBkColor() { return _DefaultBkColor; }
RGBC          HEADER_GetDefaultTextColor() { return _DefaultTextColor; }
int           HEADER_GetDefaultBorderH() { return _DefaultBorderH; }
int           HEADER_GetDefaultBorderV() { return _DefaultBorderV; }
CFont*        HEADER_GetDefaultFont() { return _pDefaultFont; }
void HEADER_SetFont(Header *pObj, CFont *pFont) {
	if (!pObj)
		return;
	pObj->pFont = pFont;
	pObj->Invalidate();
}
void HEADER_SetHeight(Header *pObj, int Height) {
	if (!pObj)
		return;
	pObj->Size({ pObj->ClientRect().xsize(), Height });
	pObj->Parent()->Invalidate();
}
void HEADER_SetTextColor(Header *pObj, RGBC Color) {
	if (!pObj)
		return;
	pObj->TextColor = Color;
	pObj->Invalidate();
}
void HEADER_SetBkColor(Header *pObj, RGBC Color) {
	if (!pObj)
		return;
	pObj->BkColor = Color;
	pObj->Invalidate();
}
void HEADER_SetTextAlign(Header *pObj, unsigned int Index, int Align) {
	if (!pObj)
		return;
	if (Index > pObj->Columns.NumItems())
		return;
	pObj->Columns[Index].Align = Align;
	pObj->Invalidate();
}
void HEADER_SetScrollPos(Header *pObj, int ScrollPos) {
	if (!pObj || ScrollPos < 0)
		return;
	if (ScrollPos == pObj->ScrollPos)
		return;
	pObj->ScrollPos = ScrollPos;
	pObj->Invalidate();
	pObj->Parent()->Invalidate();
}
void HEADER_AddItem(Header *pObj, int Width, const char* s, int Align) {
	if (!pObj)
		return;
	if (!Width) {
		auto pFont = GUI.Font();
		GUI.Font(pObj->pFont);
		Width = GUI_GetStringDistX(s) + 2 * (pObj->EffectSize() + _DefaultBorderH);
		GUI.Font(pFont);
	}
	Header::Column Col;
	Col.Width = Width;
	Col.Align = Align;
	Col.pDrawObj = nullptr;
	GUI__SetText(&Col.pText, s);
	pObj->Columns.Add(Col);
	pObj->Invalidate();
	pObj->Parent()->Invalidate();
}
void HEADER_DeleteItem(Header *pObj, unsigned Index) {
	if (!pObj)
		return;
	if (Index >= pObj->Columns.NumItems())
		return;
	pObj->Columns.Delete(Index);
	pObj->Invalidate();
	pObj->Parent()->Invalidate();
}
void HEADER_SetItemText(Header *pObj, unsigned int Index, const char *s) {
	if (!pObj)
		return;
	if (Index >= pObj->Columns.NumItems())
		return;
	GUI__SetText(&pObj->Columns[Index].pText, s);
}
void HEADER_SetItemWidth(Header *pObj, unsigned int Index, int Width) {
	if (!pObj)
		return;
	if (Width < 0)
		return;
	if (Index >= pObj->Columns.NumItems())
		return;
	pObj->Columns[Index].Width = Width;
	pObj->Invalidate();
	pObj->Parent()->SendMessage(WM_NOTIFY_CLIENTCHANGE);
	pObj->Parent()->Invalidate();
}
int HEADER_GetHeight(Header *pObj) {
	if (!pObj)
		return 0;
	auto rect = pObj->ClientRect() - pObj->Rect().left_top();
	return rect.y1 - rect.y0 + 1;
}
int HEADER_GetItemWidth(Header *pObj, unsigned int Index) {
	if (!pObj)
		return 0;
	if (Index > pObj->Columns.NumItems())
		return 0;
	return pObj->Columns[Index].Width;
}
int HEADER_GetNumItems(Header *pObj) {
	if (!pObj)
		return 0;
	return pObj->Columns.NumItems();
}
void HEADER__SetDrawObj(Header *pObj, unsigned Index, GUI_DRAW_BASE *pDrawObj) {
	if (!pObj)
		return;
	if (Index >= pObj->Columns.NumItems())
		return;
	auto &Col = pObj->Columns[Index];
	GUI_MEM_Free(Col.pDrawObj);
	Col.pDrawObj = pDrawObj;
}
void HEADER_SetBitmapEx(Header *pObj, unsigned Index, CBitmap* pBitmap, int x, int y) {
	HEADER__SetDrawObj(pObj, Index, GUI_DRAW_BMP::Create(pBitmap));
	pObj->Invalidate();
}
