#include "SCROLLBAR.h"
#include "HEADER.h"
#define HEADER_FONT_DEFAULT      &GUI_Font13_1
#define HEADER_SUPPORT_DRAG      1
#define HEADER_BORDER_V_DEFAULT  0
#define HEADER_BORDER_H_DEFAULT  2
#define HEADER_BKCOLOR_DEFAULT   0xAAAAAA
#define HEADER_TEXTCOLOR_DEFAULT GUI_BLACK
#define HEADER_CURSOR_DEFAULT    &GUI_CursorHeaderM
static const Cursor *_pOldCursor;
static const Cursor *_pDefaultCursor = HEADER_CURSOR_DEFAULT;
static RGBC          _DefaultBkColor = HEADER_BKCOLOR_DEFAULT;
static RGBC          _DefaultTextColor = HEADER_TEXTCOLOR_DEFAULT;
static int           _DefaultBorderH = HEADER_BORDER_H_DEFAULT;
static int           _DefaultBorderV = HEADER_BORDER_V_DEFAULT;
static CFont*        _pDefaultFont = HEADER_FONT_DEFAULT;
static void _Paint(Header_Obj* pObj) {
	SRect rect;
	int xPos = -pObj->ScrollPos;
	int EffectSize = pObj->pEffect->EffectSize;
	GUI.BkColor(pObj->BkColor);
	GUI.Font(pObj->pFont);
	GUI_Clear();
	int i, NumItems = GUI_ARRAY_GetNumItems(&pObj->Columns);
	for (i = 0; i < NumItems; i++) {
		HEADER_COLUMN* pColumn = (HEADER_COLUMN*)GUI_ARRAY_GetItem(&pObj->Columns, i);
		WM_GetClientRect(&rect);
		rect.x0 = xPos;
		rect.x1 = rect.x0 + pColumn->Width;
		if (pColumn->pDrawObj) {
			int xOff = 0, yOff = 0;
			switch (pColumn->Align & GUI_TA_HORIZONTAL) {
			case GUI_TA_RIGHT:
				xOff = (pColumn->Width - GUI_DRAW__GetXSize(pColumn->pDrawObj));
				break;
			case GUI_TA_HCENTER:
				xOff = (pColumn->Width - GUI_DRAW__GetXSize(pColumn->pDrawObj)) / 2;
				break;
			}
			switch (pColumn->Align & GUI_TA_VERTICAL) {
			case GUI_TA_BOTTOM:
				yOff = ((rect.y1 - rect.y0 + 1) - GUI_DRAW__GetYSize(pColumn->pDrawObj));
				break;
			case GUI_TA_VCENTER:
				yOff = ((rect.y1 - rect.y0 + 1) - GUI_DRAW__GetYSize(pColumn->pDrawObj)) / 2;
				break;
			}
			WObj::SetUserClipRect(&rect);
			GUI_DRAW__Draw(pColumn->pDrawObj, xPos + xOff, yOff);
			WObj::SetUserClipRect(nullptr);
		}
		WIDGET__EFFECT_DrawUpRect(pObj, &rect);
		xPos += rect.x1 - rect.x0;
		rect.x0 += EffectSize + _DefaultBorderH;
		rect.x1 -= EffectSize + _DefaultBorderH;
		rect.y0 += EffectSize + _DefaultBorderV;
		rect.y1 -= EffectSize + _DefaultBorderV;
		GUI.PenColor(pObj->TextColor);
		GUI_DispStringInRect(pColumn->acText, &rect, pColumn->Align);
	}
	WM_GetClientRect(&rect);
	rect.x0 = xPos;
	rect.x1 = 0xfff;
	WIDGET__EFFECT_DrawUpRect(pObj, &rect);
}
static void _RestoreOldCursor(void) {
	if (!_pOldCursor)
		return;
	GUI_CURSOR_Select(_pOldCursor);
	_pOldCursor = 0;
}
static void _FreeAttached(Header_Obj* pObj) {
	
	int NumItems = GUI_ARRAY_GetNumItems(&pObj->Columns);
	
	int i;
	for (i = 0; i < NumItems; i++) {
		auto pColumn = (HEADER_COLUMN*)GUI_ARRAY_GetItem(&pObj->Columns, i);
		if (pColumn->pDrawObj)
			GUI_ALLOC_Free(pColumn->pDrawObj);
	}
	/* Delete attached objects (if any) */
	GUI_ARRAY_Delete(&pObj->Columns);
	_RestoreOldCursor();
}
#if (HEADER_SUPPORT_DRAG)
static int _GetItemIndex(Header_Obj *pObj, int x, int y) {
	if (!pObj)
		return -1;
	if (y < 0 || y >= pObj->SizeY())
		return -1;
	int xPos = 0;
	int Index, NumColumns = GUI_ARRAY_GetNumItems(&pObj->Columns);
	for (Index = 0; Index < NumColumns; Index++) {
		HEADER_COLUMN *pColumn = (HEADER_COLUMN *)GUI_ARRAY_GetItem(&pObj->Columns, Index);
		xPos += pColumn->Width;
		if (xPos < x - 4 || xPos > x + 4)
			continue;
		if (Index >= NumColumns - 1 || x >= xPos)
			continue;
		pColumn = (HEADER_COLUMN *)GUI_ARRAY_GetItem(&pObj->Columns, Index + 1);
		if (pColumn->Width == 0)
			return Index;
	}
	return -1;
}
#endif
#if (HEADER_SUPPORT_DRAG)
static void _HandlePID(Header_Obj *pObj, int x, int y, int Pressed) {
	int Hit = _GetItemIndex(pObj, x, y);
	/* set capture position () */
	if ((Pressed == 1) && (Hit >= 0) && (pObj->CapturePosX == -1)) {
		pObj->CapturePosX = x;
		pObj->CaptureItem = Hit;
	}
	/* set mouse cursor and capture () */
	if (Hit >= 0) {
		WM_SetCapture(pObj, 1);
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
			WM_ReleaseCapture();
		}
}
#endif
#if (HEADER_SUPPORT_DRAG)
static void _OnMouseOver(Header_Obj *pObj, WM_MESSAGE* pMsg) {
	const PidState* pState = (const PidState*)pMsg->Data;
	if (pState)
		_HandlePID(pObj, pState->x + pObj->ScrollPos, pState->y, -1);
}
#endif
#if (HEADER_SUPPORT_DRAG)
static void _OnTouch(Header_Obj *pObj, WM_MESSAGE *pMsg) {
	int Notification;
	const PidState *pState = (const PidState *)pMsg->Data;
	if (pState) {
		_HandlePID(pObj, pState->x + pObj->ScrollPos, pState->y, pState->Pressed);
		Notification = pState->Pressed ? WM_NOTIFICATION_CLICKED : WM_NOTIFICATION_RELEASED;
	}
	else
		Notification = WM_NOTIFICATION_MOVED_OUT;
	WM_NotifyParent(pObj, Notification);
}
#endif
static void _HEADER_Callback(WM_MESSAGE* pMsg) {
	auto pObj = (Header_Obj *)pMsg->pWin;
	/* Let widget handle the standard messages */
	if (WIDGET_HandleActive(pObj, pMsg) == 0)
		return;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		_Paint(pObj);
		break;
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		break;
	case WM_MOUSEOVER:
		_OnMouseOver(pObj, pMsg);
		break;
	case WM_DELETE:
		_FreeAttached(pObj); /* No return here ... WM_DefaultProc needs to be called */
	default:
		WM_DefaultProc(pMsg);
	}
}
Header_Obj *HEADER_CreateEx(
	int x0, int y0,
	int xsize, int ysize, WObj *pParent,
	int WinFlags, int ExFlags, int Id) {
	if (xsize == 0 && x0 == 0 && y0 == 0) {
		auto &&rect = pParent->InsideRect();
		xsize = rect.x1 - rect.x0 + 1;
		x0 = rect.x0; y0 = rect.y0;
	}
	if (ysize == 0) {
		const Widget::Effect* pEffect = Widget::DefaultEffect();
		ysize = GUI_GetYDistOfFont(_pDefaultFont);
		ysize += 2 * _DefaultBorderV;
		ysize += 2 * (unsigned)pEffect->EffectSize;
	}
	auto pObj = (Header_Obj *)WObj::Create(
		x0, y0, xsize, ysize,
		pParent, WinFlags | WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT,
		&_HEADER_Callback,
		sizeof(Header_Obj) - sizeof(WObj));
	if (!pObj) {
		return 0;
	}
	WIDGET__Init(pObj, Id, 0);
	pObj->BkColor = _DefaultBkColor;
	pObj->TextColor = _DefaultTextColor;
	pObj->pFont = _pDefaultFont;
	pObj->CapturePosX = -1;
	pObj->CaptureItem = -1;
	pObj->ScrollPos = 0;
	
	return pObj;
}
const Cursor * HEADER_SetDefaultCursor(const Cursor* pCursor) {
	const Cursor * pOldCursor = _pDefaultCursor;
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
const Cursor *HEADER_GetDefaultCursor(void) { return _pDefaultCursor; }
RGBC          HEADER_GetDefaultBkColor(void) { return _DefaultBkColor; }
RGBC          HEADER_GetDefaultTextColor(void) { return _DefaultTextColor; }
int           HEADER_GetDefaultBorderH(void) { return _DefaultBorderH; }
int           HEADER_GetDefaultBorderV(void) { return _DefaultBorderV; }
CFont*        HEADER_GetDefaultFont(void) { return _pDefaultFont; }
void HEADER_SetFont(Header_Obj *pObj, CFont *pFont) {
	if (!pObj)
		return;
	pObj->pFont = pFont;
	pObj->Invalidate();
}
void HEADER_SetHeight(Header_Obj *pObj, int Height) {
	if (!pObj)
		return;
	pObj->Size({ pObj->ClientRect().xsize(), Height });
	pObj->Parent()->Invalidate();
}
void HEADER_SetTextColor(Header_Obj *pObj, RGBC Color) {
	if (!pObj)
		return;
	pObj->TextColor = Color;
	pObj->Invalidate();
}
void HEADER_SetBkColor(Header_Obj *pObj, RGBC Color) {
	if (!pObj)
		return;
	pObj->BkColor = Color;
	pObj->Invalidate();
}
void HEADER_SetTextAlign(Header_Obj *pObj, unsigned int Index, int Align) {
	if (!pObj)
		return;
	if (Index > GUI_ARRAY_GetNumItems(&pObj->Columns))
		return;
	HEADER_COLUMN *pColumn = (HEADER_COLUMN *)GUI_ARRAY_GetItem(&pObj->Columns, Index);
	pColumn->Align = Align;
	pObj->Invalidate();
}
void HEADER_SetScrollPos(Header_Obj *pObj, int ScrollPos) {
	if (!pObj || ScrollPos < 0)
		return;
	if (ScrollPos == pObj->ScrollPos)
		return;
	pObj->ScrollPos = ScrollPos;
	pObj->Invalidate();
	pObj->Parent()->Invalidate();
}
void HEADER_AddItem(Header_Obj *pObj, int Width, const char* s, int Align) {
	if (!pObj)
		return;
	if (!Width) {	
		auto pFont = GUI.Font();
		GUI.Font(pObj->pFont);
		Width = GUI_GetStringDistX(s) + 2 * (pObj->pEffect->EffectSize + _DefaultBorderH);	
		GUI.Font(pFont);
	}
	
	HEADER_COLUMN Column;
	Column.Width = Width;
	Column.Align = Align;
	Column.pDrawObj = nullptr;
	
	int Index = GUI_ARRAY_GetNumItems(&pObj->Columns);
	if (GUI_ARRAY_AddItem(&pObj->Columns, &Column, sizeof(HEADER_COLUMN) + (int)GUI__strlen(s) + 1) == 0) {
		HEADER_COLUMN* pColumn = (HEADER_COLUMN*)GUI_ARRAY_GetItem(&pObj->Columns, Index);
		GUI__strcpy(pColumn->acText, s);
		pObj->Invalidate();
		pObj->Parent()->Invalidate();
	}
}
void HEADER_DeleteItem(Header_Obj *pObj, unsigned Index) {
	if (!pObj)
		return;
	if (Index >= GUI_ARRAY_GetNumItems(&pObj->Columns))
		return;
	GUI_ARRAY_DeleteItem(&pObj->Columns, Index);
	pObj->Invalidate();
	pObj->Parent()->Invalidate();
}
void HEADER_SetItemText(Header_Obj *pObj, unsigned int Index, const char *s) {
	if (!pObj)
		return;
	if (Index >= GUI_ARRAY_GetNumItems(&pObj->Columns))
		return;
	HEADER_COLUMN *pColumn = (HEADER_COLUMN *)GUI_ARRAY_ResizeItem(
		&pObj->Columns, Index, sizeof(HEADER_COLUMN) + (int)GUI__strlen(s));
	if (pColumn)
		GUI__strcpy(pColumn->acText, s);
}
void HEADER_SetItemWidth(Header_Obj *pObj, unsigned int Index, int Width) {
	if (!pObj)
		return;
	if (Width < 0)
		return;
	if (Index > GUI_ARRAY_GetNumItems(&pObj->Columns))
		return;
	HEADER_COLUMN *pColumn = (HEADER_COLUMN *)GUI_ARRAY_GetItem(&pObj->Columns, Index);
	if (!pColumn)
		return;
	pColumn->Width = Width;
	pObj->Invalidate();
	WM__SendMsgNoData(pObj->Parent(), WM_NOTIFY_CLIENTCHANGE);
	pObj->Parent()->Invalidate();
}
int HEADER_GetHeight(Header_Obj *pObj) {
	if (!pObj)
		return 0;
	auto rect = pObj->ClientRect() - pObj->Rect().left_top();
	return rect.y1 - rect.y0 + 1;
}
int HEADER_GetItemWidth(Header_Obj *pObj, unsigned int Index) {
	if (!pObj)
		return 0;
	if (Index > GUI_ARRAY_GetNumItems(&pObj->Columns))
		return 0;
	HEADER_COLUMN *pColumn = (HEADER_COLUMN *)GUI_ARRAY_GetItem(&pObj->Columns, Index);
	return pColumn->Width;
}
int HEADER_GetNumItems(Header_Obj *pObj) {
	if (!pObj)
		return 0;
	return GUI_ARRAY_GetNumItems(&pObj->Columns);
}
void HEADER__SetDrawObj(Header_Obj *pObj, unsigned Index, GUI_DRAW *pDrawObj) {
	if (!pObj)
		return;
	if (Index > GUI_ARRAY_GetNumItems(&pObj->Columns))
		return;
	HEADER_COLUMN *pColumn = (HEADER_COLUMN *)GUI_ARRAY_GetItem(&pObj->Columns, Index);
	if (!pColumn)
		return;
	GUI_ALLOC_Free(pColumn->pDrawObj);
	pColumn->pDrawObj = pDrawObj;
}
void HEADER_SetBitmapEx(Header_Obj *pObj, unsigned Index, const Bitmap* pBitmap, int x, int y) {
	HEADER__SetDrawObj(pObj, Index, GUI_DRAW_BITMAP_Create(pBitmap, x, y));
	pObj->Invalidate();
}
