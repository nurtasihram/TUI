#include "SCROLLBAR.h"

#include "LISTBOX.h"
#define LISTBOX_FONT_DEFAULT         &GUI_Font13_1
#define LISTBOX_USE_3D               1
#define LISTBOX_BKCOLOR0_DEFAULT     GUI_WHITE     /* Not selected */
#define LISTBOX_BKCOLOR1_DEFAULT     GUI_GRAY      /* Selected, no focus */
#define LISTBOX_BKCOLOR2_DEFAULT     GUI_BLUE      /* Selected, focus */
#define LISTBOX_BKCOLOR3_DEFAULT     0xC0C0C0      /* Disabled */
#define LISTBOX_TEXTCOLOR0_DEFAULT   GUI_BLACK     /* Not selected */
#define LISTBOX_TEXTCOLOR1_DEFAULT   GUI_WHITE     /* Selected, no focus */
#define LISTBOX_TEXTCOLOR2_DEFAULT   GUI_WHITE     /* Selected, focus */
#define LISTBOX_TEXTCOLOR3_DEFAULT   GUI_GRAY      /* Disabled */
#define LISTBOX_SCROLLSTEP_H_DEFAULT 10
#define LISTBOX_ITEM_SELECTED (1 << 0)
#define LISTBOX_ITEM_DISABLED (1 << 1)
LISTBOX_PROPS LISTBOX_DefaultProps = {
	LISTBOX_FONT_DEFAULT,
	LISTBOX_SCROLLSTEP_H_DEFAULT,
	LISTBOX_BKCOLOR0_DEFAULT,
	LISTBOX_BKCOLOR1_DEFAULT,
	LISTBOX_BKCOLOR2_DEFAULT,
	LISTBOX_BKCOLOR3_DEFAULT,
	LISTBOX_TEXTCOLOR0_DEFAULT,
	LISTBOX_TEXTCOLOR1_DEFAULT,
	LISTBOX_TEXTCOLOR2_DEFAULT,
	LISTBOX_TEXTCOLOR3_DEFAULT
};
static int _CallOwnerDraw(ListBox_Obj *pObj, int Cmd, int ItemIndex) {
	WIDGET_ITEM_DRAW_INFO ItemInfo;
	ItemInfo.Cmd = Cmd;
	ItemInfo.pWin = pObj;
	ItemInfo.ItemIndex = ItemIndex;
	if (pObj->pfDrawItem)
		return pObj->pfDrawItem(&ItemInfo);
	return LISTBOX_OwnerDraw(&ItemInfo);
}
unsigned LISTBOX__GetNumItems(const ListBox_Obj *pObj) {
	return GUI_ARRAY_GetNumItems(&pObj->ItemArray);
}
const char *LISTBOX__GetpString(const ListBox_Obj *pObj, int Index) {
	LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	if (pItem)
		return pItem->acText;
	return nullptr;
}
static int _GetYSize(ListBox_Obj *pObj) {
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	return rect.y1 - rect.y0 + 1;
}
static int _GetItemSize(ListBox_Obj *pObj, unsigned Index, uint8_t XY) {
	LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	int size = 0;
	if (pItem)
		size = XY == WIDGET_ITEM_GET_XSIZE ? pItem->xSize : pItem->ySize;
	if (size == 0) {
		auto pOldFont = GUI.Font();
		GUI.Font(pObj->Props.pFont);
		size = _CallOwnerDraw(pObj, XY, Index);
		GUI.Font(pOldFont);
	}
	if (pItem) {
		if (XY == WIDGET_ITEM_GET_XSIZE)
			pItem->xSize = size;
		else
			pItem->ySize = size;
	}
	return size;
}
static int _GetContentsSizeX(ListBox_Obj *pObj) {
	int i, NumItems = LISTBOX__GetNumItems(pObj), r = 0;
	for (i = 0; i < NumItems; i++) {
		int SizeX = _GetItemSize(pObj, i, WIDGET_ITEM_GET_XSIZE);
		if (r < SizeX)
			r = SizeX;
	}
	return r;
}
static int _GetItemPosY(ListBox_Obj *pObj, unsigned Index) {
	if (Index >= LISTBOX__GetNumItems(pObj))
		return -1;
	if ((int)Index < pObj->ScrollStateV.v)
		return -1;
	int PosY = 0;
	unsigned i;
	for (i = pObj->ScrollStateV.v; i < Index; i++)
		PosY += _GetItemSize(pObj, i, WIDGET_ITEM_GET_YSIZE);
	return PosY;
}
static int _IsPartiallyVis(ListBox_Obj *pObj) {
	int Index = pObj->Sel;
	if (Index >= (int)LISTBOX__GetNumItems(pObj))
		return 0;
	if (Index < pObj->ScrollStateV.v)
		return 0;
	return _GetItemPosY(pObj, Index) +
		_GetItemSize(pObj, Index, WIDGET_ITEM_GET_YSIZE) > _GetYSize(pObj);
}
static unsigned _GetNumVisItems(ListBox_Obj *pObj) {
	int NumItems = LISTBOX__GetNumItems(pObj);
	if (NumItems <= 1)
		return 1;
	int r = 1;
	int i, ySize = _GetYSize(pObj), DistY = 0;
	for (i = NumItems - 1; i >= 0; i--) {
		DistY += _GetItemSize(pObj, i, WIDGET_ITEM_GET_YSIZE);
		if (DistY > ySize)
			break;
	}
	r = NumItems - i - 1;
	return r < 1 ? 1 : r;
}
static void _NotifyOwner(ListBox_Obj *pObj, int Notification) {
	auto pOwner = pObj->pOwner ? pObj->pOwner : pObj->Parent();
	WM_MESSAGE msg = { 0 };
	msg.MsgId = WM_NOTIFY_PARENT;
	msg.Data = Notification;
	msg.pWinSrc = pObj;
	pOwner->SendMessage(&msg);
}
int LISTBOX_OwnerDraw(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo) {
	ListBox_Obj *pObj = (ListBox_Obj *)pDrawItemInfo->pWin;
	switch (pDrawItemInfo->Cmd) {
	case WIDGET_ITEM_GET_XSIZE:
	{
		auto pOldFont = GUI.Font();
		GUI.Font(pObj->Props.pFont);
		int DistX = GUI_GetStringDistX(LISTBOX__GetpString(pObj, pDrawItemInfo->ItemIndex));
		GUI.Font(pOldFont);
		return DistX;
	}
	case WIDGET_ITEM_GET_YSIZE:
		return GUI_GetYDistOfFont(pObj->Props.pFont) + pObj->ItemSpacing;
	case WIDGET_ITEM_DRAW:
	{
		int ItemIndex = pDrawItemInfo->ItemIndex;
		LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, ItemIndex);
		SRect r;
		WM_GetInsideRect(&r);
		int FontDistY = GUI_GetFontDistY();
		char IsDisabled = (pItem->Status & LISTBOX_ITEM_DISABLED) ? 1 : 0,
			IsSelected = (pItem->Status & LISTBOX_ITEM_SELECTED) ? 1 : 0;
		int ColorIndex = (pObj->Flags & LISTBOX_CF_MULTISEL) ?
			IsDisabled ? 3 :
			IsSelected ? 2 : 0 : IsDisabled ? 3 :
			ItemIndex == pObj->Sel ?
			(pObj->State & WIDGET_STATE_FOCUS) ? 2 : 1 : 0;
		GUI.BkColor(pObj->Props.aBackColor[ColorIndex]);
		GUI.PenColor(pObj->Props.aTextColor[ColorIndex]);
		const char *s = LISTBOX__GetpString(pObj, ItemIndex);
		GUI_SetTextMode(DRAWMODE_TRANS);
		GUI_Clear();
		GUI_DispStringAt(s, pDrawItemInfo->x0 + 1, pDrawItemInfo->y0);
		if (!(pObj->Flags & LISTBOX_CF_MULTISEL) || ItemIndex != pObj->Sel)
			return 0;
		SRect rFocus;
		rFocus.x0 = pDrawItemInfo->x0;
		rFocus.y0 = pDrawItemInfo->y0;
		rFocus.x1 = r.x1;
		rFocus.y1 = pDrawItemInfo->y0 + FontDistY - 1;
		GUI.PenColor(GUI_WHITE - pObj->Props.aBackColor[ColorIndex]);
		GUI_DrawFocusRect(&rFocus, 0);
	}
	}
	return 0;
}
static int _UpdateScrollPos(ListBox_Obj *pObj) {
	int PrevScrollStateV = pObj->ScrollStateV.v;
	if (pObj->Sel >= 0) {
		if (_IsPartiallyVis(pObj))
			pObj->ScrollStateV.v = pObj->Sel - (pObj->ScrollStateV.PageSize - 1);
		if (pObj->Sel < pObj->ScrollStateV.v)
			pObj->ScrollStateV.v = pObj->Sel;
	}
	WM_CheckScrollBounds(&pObj->ScrollStateV);
	WM_CheckScrollBounds(&pObj->ScrollStateH);
	WIDGET__SetScrollState(pObj, &pObj->ScrollStateV, &pObj->ScrollStateH);
	return pObj->ScrollStateV.v - PrevScrollStateV;
}
void LISTBOX__InvalidateItemSize(const ListBox_Obj *pObj, unsigned Index) {
	LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	if (!pItem)
		return;
	pItem->xSize = 0;
	pItem->ySize = 0;
}
void LISTBOX__InvalidateInsideArea(ListBox_Obj *pObj) {
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	pObj->Invalidate(&rect);
}
void LISTBOX__InvalidateItem(ListBox_Obj *pObj, int Sel) {
	if (Sel < 0)
		return;
	int ItemPosY = _GetItemPosY(pObj, Sel);
	if (ItemPosY < 0)
		return;
	int ItemDistY = _GetItemSize(pObj, Sel, WIDGET_ITEM_GET_YSIZE);
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	rect.y0 += ItemPosY;
	rect.y1 = rect.y0 + ItemDistY - 1;
	pObj->Invalidate(&rect);
}
void LISTBOX__InvalidateItemAndBelow(ListBox_Obj *pObj, int Sel) {
	if (Sel < 0)
		return;
	int ItemPosY = _GetItemPosY(pObj, Sel);
	if (ItemPosY < 0)
		return;
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	rect.y0 += ItemPosY;
	pObj->Invalidate(&rect);
}
void LISTBOX__SetScrollbarWidth(ListBox_Obj *pObj) {
	int Width = pObj->ScrollbarWidth;
	if (Width == 0)
		Width = SCROLLBAR_GetDefaultWidth();
	SCROLLBAR_SetWidth((ScrollBar_Obj *)WM_GetDialogItem(pObj, GUI_ID_HSCROLL), Width);
	SCROLLBAR_SetWidth((ScrollBar_Obj *)WM_GetDialogItem(pObj, GUI_ID_VSCROLL), Width);
}
static int _CalcScrollParas(ListBox_Obj *pObj) {
	pObj->ScrollStateV.NumItems = LISTBOX__GetNumItems(pObj);
	pObj->ScrollStateV.PageSize = _GetNumVisItems(pObj);
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	pObj->ScrollStateH.NumItems = _GetContentsSizeX(pObj);
	pObj->ScrollStateH.PageSize = rect.x1 - rect.x0 + 1;
	return _UpdateScrollPos(pObj);
}
static void _ManageAutoScroll(ListBox_Obj *pObj) {
	if (pObj->Flags & LISTBOX_CF_AUTOSCROLLBAR_V)
		WM_SetScrollbarV(pObj, _GetNumVisItems(pObj) < LISTBOX__GetNumItems(pObj));
	if (pObj->Flags & LISTBOX_CF_AUTOSCROLLBAR_H) {
		int xSizeContents = _GetContentsSizeX(pObj);
		SRect rect;
		WM_GetInsideRectExScrollbar(pObj, &rect);
		WM_SetScrollbarH(pObj, xSizeContents > rect.x1 - rect.x0 + 1);
	}
	if (pObj->ScrollbarWidth)
		LISTBOX__SetScrollbarWidth(pObj);
}
int LISTBOX_UpdateScrollers(ListBox_Obj *pObj) {
	_ManageAutoScroll(pObj);
	return _CalcScrollParas(pObj);
}
static int _Tolower(int Key) {
	return Key >= 0x41 && Key <= 0x5a ?
		Key + 0x20 : Key;
}
static int _IsAlphaNum(int Key) {
	Key = _Tolower(Key);
	if (Key >= 'a' && Key <= 'z')
		return 1;
	if (Key >= '0' && Key <= '9')
		return 1;
	return 0;
}
static void _SelectByKey(ListBox_Obj *pObj, int Key) {
	Key = _Tolower(Key);
	unsigned i;
	for (i = 0; i < LISTBOX__GetNumItems(pObj); i++) {
		const char *s = LISTBOX__GetpString(pObj, i);
		if (_Tolower(*s) == Key) {
			LISTBOX_SetSel(pObj, i);
			break;
		}
	}
}
static void _FreeAttached(ListBox_Obj *pObj) {
	GUI_ARRAY_Delete(&pObj->ItemArray);
}
static void _OnPaint(ListBox_Obj *pObj, WM_MESSAGE *pMsg) {
	GUI.Font(pObj->Props.pFont);
	SRect ClipRect = *(const SRect *)pMsg->Data - pObj->Rect().left_top();
	SRect RectInside;
	WM_GetInsideRectExScrollbar(pObj, &RectInside);
	ClipRect &= RectInside;
	SRect RectItem;
	RectItem.x0 = ClipRect.x0;
	RectItem.x1 = ClipRect.x1;
	WIDGET_ITEM_DRAW_INFO ItemInfo;
	ItemInfo.Cmd = WIDGET_ITEM_DRAW;
	ItemInfo.pWin = pObj;
	ItemInfo.x0 = RectInside.x0 - pObj->ScrollStateH.v;
	ItemInfo.y0 = RectInside.y0;
	int i, NumItems = LISTBOX__GetNumItems(pObj);
	for (i = pObj->ScrollStateV.v; i < NumItems; i++) {
		RectItem.y0 = ItemInfo.y0;
		if (RectItem.y0 > ClipRect.y1)
			break;
		int ItemDistY = _GetItemSize(pObj, i, WIDGET_ITEM_GET_YSIZE);
		RectItem.y1 = RectItem.y0 + ItemDistY - 1;
		if (RectItem.y1 >= ClipRect.y0) {
			WObj::SetUserClipRect(&RectItem);
			ItemInfo.ItemIndex = i;
			if (pObj->pfDrawItem)
				pObj->pfDrawItem(&ItemInfo);
			else
				LISTBOX_OwnerDraw(&ItemInfo);
		}
		ItemInfo.y0 += ItemDistY;
	}
	WObj::SetUserClipRect(nullptr);
	RectItem.y0 = ItemInfo.y0;
	RectItem.y1 = RectInside.y1;
	GUI.BkColor(pObj->Props.aBackColor[0]);
	GUI_ClearRectEx(&RectItem);
	WIDGET__EFFECT_DrawDown(pObj);
}
static void _ToggleMultiSel(ListBox_Obj *pObj, int Sel) {
	if (!(pObj->Flags & LISTBOX_CF_MULTISEL))
		return;
	LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Sel);
	if (!pItem)
		return;
	if (pItem->Status & LISTBOX_ITEM_DISABLED)
		return;
	pItem->Status ^= LISTBOX_ITEM_SELECTED;
	_NotifyOwner(pObj, WM_NOTIFICATION_SEL_CHANGED);
	LISTBOX__InvalidateItem(pObj, Sel);
}
static int _GetItemFromPos(ListBox_Obj *pObj, int x, int y) {
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	if (x < rect.x0 || y < rect.y0)
		return -1;
	if (x > rect.x1 || y > rect.y1)
		return 1;
	int Sel = -1;
	int i, NumItems = LISTBOX__GetNumItems(pObj), y0 = rect.y0;
	for (i = pObj->ScrollStateV.v; i < NumItems; i++) {
		if (y >= y0)
			Sel = i;
		y0 += _GetItemSize(pObj, i, WIDGET_ITEM_GET_YSIZE);
	}
	return Sel;
}
static void _OnTouch(ListBox_Obj *pObj, WM_MESSAGE *pMsg) {
	const PidState *pState = (const PidState *)pMsg->Data;
	if (pState) {
		if (pState->Pressed == 0)
			_NotifyOwner(pObj, WM_NOTIFICATION_RELEASED);
	}
	else
		_NotifyOwner(pObj, WM_NOTIFICATION_MOVED_OUT);
}
static void _OnMouseOver(ListBox_Obj *pObj, WM_MESSAGE *pMsg) {
	if (!pObj->pOwner)
		return;
	const PidState *pState = (const PidState *)pMsg->Data;
	if (!pState)
		return;
	int Sel = _GetItemFromPos(pObj, pState->x, pState->y);
	if (Sel < 0)
		return;
	if (Sel < (int)(pObj->ScrollStateV.v + _GetNumVisItems(pObj)))
		LISTBOX_SetSel(pObj, Sel);
}
static void _LISTBOX_Callback(WM_MESSAGE *pMsg) {
	auto pObj = (ListBox_Obj *)pMsg->pWin;
	if (WIDGET_HandleActive(pObj, pMsg) == 0) {
		if (pMsg->MsgId == WM_SET_FOCUS)
			if (pMsg->Data == 0)
				_NotifyOwner(pObj, LISTBOX_NOTIFICATION_LOST_FOCUS);
		return;
	}
	switch (pMsg->MsgId) {
	case WM_NOTIFY_PARENT:
		switch (pMsg->Data) {
		case WM_NOTIFICATION_VALUE_CHANGED:
		{
			WM_SCROLL_STATE ScrollState;
			if (pMsg->pWinSrc == WM_GetScrollbarV(pObj)) {
				WM_GetScrollState(pMsg->pWinSrc, &ScrollState);
				pObj->ScrollStateV.v = ScrollState.v;
				LISTBOX__InvalidateInsideArea(pObj);
				_NotifyOwner(pObj, WM_NOTIFICATION_SCROLL_CHANGED);
			}
			else if (pMsg->pWinSrc == WM_GetScrollbarH(pObj)) {
				WM_GetScrollState(pMsg->pWinSrc, &ScrollState);
				pObj->ScrollStateH.v = ScrollState.v;
				LISTBOX__InvalidateInsideArea(pObj);
				_NotifyOwner(pObj, WM_NOTIFICATION_SCROLL_CHANGED);
			}
			break;
		}
		case WM_NOTIFICATION_SCROLLBAR_ADDED:
			LISTBOX_UpdateScrollers(pObj);
			break;
		}
		break;
	case WM_PAINT:
		_OnPaint(pObj, pMsg);
		break;
	case WM_PID_STATE_CHANGED:
	{
		const WM_PID_STATE_CHANGED_INFO *pInfo = (const WM_PID_STATE_CHANGED_INFO *)pMsg->Data;
		if (!pInfo->State)
			break;
		int Sel = _GetItemFromPos(pObj, pInfo->x, pInfo->y);
		if (Sel >= 0) {
			_ToggleMultiSel(pObj, Sel);
			LISTBOX_SetSel(pObj, Sel);
		}
		_NotifyOwner(pObj, WM_NOTIFICATION_CLICKED);
		return;
	}
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		return;
	case WM_MOUSEOVER:
		_OnMouseOver(pObj, pMsg);
		return;
	case WM_DELETE:
		_FreeAttached(pObj);
		break;
	case WM_KEY:
	{
		const WM_KEY_INFO *ki = (const WM_KEY_INFO *)pMsg->Data;
		if (ki->PressedCnt > 0)
			if (LISTBOX_AddKey(pObj, ki->Key))
				return;
		break;
	}
	case WM_SIZE:
		LISTBOX_UpdateScrollers(pObj);
		pObj->Invalidate();
		break;
	}
	WM_DefaultProc(pMsg);
}
void LISTBOX_MoveSel(ListBox_Obj *pObj, int Dir) {
	int Index = LISTBOX_GetSel(pObj);
	int NumItems = LISTBOX__GetNumItems(pObj);
	int NewSel = -1;
	do {
		Index += Dir;
		if (Index < 0 || Index >= NumItems)
			break;
		LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
		if (!pItem)
			continue;
		if (!(pItem->Status & LISTBOX_ITEM_DISABLED))
			NewSel = Index;
	} while (NewSel < 0);
	if (NewSel >= 0)
		LISTBOX_SetSel(pObj, NewSel);
}
static int _AddKey(ListBox_Obj *pObj, int Key) {
	switch (Key) {
	case ' ':
		_ToggleMultiSel(pObj, pObj->Sel);
		return 1;
	case GUI_KEY_RIGHT:
		if (WM_SetScrollValue(&pObj->ScrollStateH, pObj->ScrollStateH.v + pObj->Props.ScrollStepH)) {
			LISTBOX_UpdateScrollers(pObj);
			LISTBOX__InvalidateInsideArea(pObj);
		}
		return 1;
	case GUI_KEY_LEFT:
		if (WM_SetScrollValue(&pObj->ScrollStateH, pObj->ScrollStateH.v - pObj->Props.ScrollStepH)) {
			LISTBOX_UpdateScrollers(pObj);
			LISTBOX__InvalidateInsideArea(pObj);
		}
		return 1;
	case GUI_KEY_DOWN:
		LISTBOX_IncSel(pObj);
		return 1;
	case GUI_KEY_UP:
		LISTBOX_DecSel(pObj);
		return 1;
	default:
		if (_IsAlphaNum(Key)) {
			_SelectByKey(pObj, Key);
			return 1;
		}
	}
	return 0;
}
ListBox_Obj *LISTBOX_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent,
								int WinFlags, int ExFlags, int Id, const char **ppText) {
	ListBox_Obj *pObj = (ListBox_Obj *)WObj::Create(
		x0, y0, xsize, ysize,
		pParent, WinFlags, _LISTBOX_Callback,
		sizeof(ListBox_Obj) - sizeof(WObj));
	if (!pObj)
		return nullptr;
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE);
	pObj->Props = LISTBOX_DefaultProps;
	if (ppText)
		LISTBOX_SetText(pObj, ppText);
	LISTBOX_UpdateScrollers(pObj);
	return pObj;
}
void LISTBOX_InvalidateItem(ListBox_Obj *pObj, int Index) {
	if (!pObj)
		return;
	int NumItems = LISTBOX__GetNumItems(pObj);
	if (Index >= NumItems)
		return;
	if (Index >= 0) {
		LISTBOX__InvalidateItemSize(pObj, Index);
		LISTBOX_UpdateScrollers(pObj);
		LISTBOX__InvalidateItemAndBelow(pObj, Index);
		return;
	}
	int i;
	for (i = 0; i < NumItems; i++)
		LISTBOX__InvalidateItemSize(pObj, i);
	LISTBOX_UpdateScrollers(pObj);
	LISTBOX__InvalidateInsideArea(pObj);
}
int LISTBOX_AddKey(ListBox_Obj *pObj, int Key) {
	return pObj ? _AddKey(pObj, Key) : 0;
}
void LISTBOX_AddString(ListBox_Obj *pObj, const char *s) {
	if (!pObj)
		return;
	if (!s)
		return;
	LISTBOX_ITEM Item = { 0, 0 };
	if (GUI_ARRAY_AddItem(&pObj->ItemArray, &Item, sizeof(LISTBOX_ITEM) + GUI__strlen(s)))
		return;
	unsigned ItemIndex = GUI_ARRAY_GetNumItems(&pObj->ItemArray) - 1;
	LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, ItemIndex);
	GUI__strcpy(pItem->acText, s);
	LISTBOX__InvalidateItemSize(pObj, ItemIndex);
	LISTBOX_UpdateScrollers(pObj);
	LISTBOX__InvalidateItem(pObj, ItemIndex);
}
void LISTBOX_SetText(ListBox_Obj *pObj, const char **ppText) {
	if (!pObj)
		return;
	if (ppText) {
		int i;
		const char *s;
		for (i = 0; (s = *(ppText + i)) != 0; i++)
			LISTBOX_AddString(pObj, s);
	}
	LISTBOX_InvalidateItem(pObj, LISTBOX_ALL_ITEMS);
}
void LISTBOX_SetSel(ListBox_Obj *pObj, int NewSel) {
	if (!pObj)
		return;
	int MaxSel = LISTBOX__GetNumItems(pObj);
	MaxSel = MaxSel ? MaxSel - 1 : 0;
	if (NewSel > MaxSel)
		NewSel = MaxSel;
	if (NewSel < 0)
		NewSel = -1;
	else {
		LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, NewSel);
		if (pItem)
			if (pItem->Status & LISTBOX_ITEM_DISABLED)
				NewSel = -1;
	}
	if (NewSel == pObj->Sel)
		return;
	int OldSel = pObj->Sel;
	pObj->Sel = NewSel;
	if (_UpdateScrollPos(pObj))
		LISTBOX__InvalidateInsideArea(pObj);
	else {
		LISTBOX__InvalidateItem(pObj, OldSel);
		LISTBOX__InvalidateItem(pObj, NewSel);
	}
	_NotifyOwner(pObj, WM_NOTIFICATION_SEL_CHANGED);
}
int LISTBOX_GetSel(ListBox_Obj *pObj) {
	return pObj ? pObj->Sel : -1;
}
void LISTBOX_SetMulti(ListBox_Obj *pObj, int Mode) {
	if (!pObj)
		return;
	if (Mode) {
		if (!(pObj->Flags & LISTBOX_CF_MULTISEL)) {
			pObj->Flags |= LISTBOX_CF_MULTISEL;
			LISTBOX__InvalidateInsideArea(pObj);
		}
	}
	else if (pObj->Flags & LISTBOX_CF_MULTISEL) {
		pObj->Flags &= ~LISTBOX_CF_MULTISEL;
		LISTBOX__InvalidateInsideArea(pObj);
	}
}
int LISTBOX_GetMulti(ListBox_Obj *pObj) {
	return pObj ? (pObj->Flags & LISTBOX_CF_MULTISEL) != 0 : 0;
}
int LISTBOX_GetItemSel(ListBox_Obj *pObj, unsigned Index) {
	if (!pObj)
		return 0;
	if (Index >= LISTBOX__GetNumItems(pObj))
		return 0;
	if (!(pObj->Flags & LISTBOX_CF_MULTISEL))
		return 0;
	LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	if (!pItem)
		return 0;
	return (pItem->Status & LISTBOX_ITEM_SELECTED) != 0;
}
void LISTBOX_SetItemSel(ListBox_Obj *pObj, unsigned Index, int OnOff) {
	if (!pObj)
		return;
	if (Index >= LISTBOX__GetNumItems(pObj))
		return;
	if (!(pObj->Flags & LISTBOX_CF_MULTISEL))
		return;
	LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	if (!pItem)
		return;
	if (OnOff) {
		if (!(pItem->Status & LISTBOX_ITEM_SELECTED)) {
			pItem->Status |= LISTBOX_ITEM_SELECTED;
			LISTBOX__InvalidateItem(pObj, Index);
		}
	}
	else if (pItem->Status & LISTBOX_ITEM_SELECTED) {
		pItem->Status &= ~LISTBOX_ITEM_SELECTED;
		LISTBOX__InvalidateItem(pObj, Index);
	}
}
void LISTBOX_SetScrollStepH(ListBox_Obj *pObj, int Value) {
	if (!pObj)
		return;
	pObj->Props.ScrollStepH = Value;
}
int LISTBOX_GetScrollStepH(ListBox_Obj *pObj) {
	return pObj ? pObj->Props.ScrollStepH : 0;
}
void LISTBOX_InsertString(ListBox_Obj *pObj, const char *s, unsigned int Index) {
	if (!pObj)
		return;
	if (!s)
		return;
	if (Index < LISTBOX__GetNumItems(pObj)) {
		LISTBOX_AddString(pObj, s);
		return;
	}
	LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_InsertItem(&pObj->ItemArray, Index, sizeof(LISTBOX_ITEM) + GUI__strlen(s));
	if (!pItem)
		return;
	pItem->Status = 0;
	GUI__strcpy(pItem->acText, s);
	LISTBOX_InvalidateItem(pObj, Index);
}
void LISTBOX_SetItemSpacing(ListBox_Obj *pObj, unsigned Value) {
	if (!pObj)
		return;
	pObj->ItemSpacing = Value;
	LISTBOX_InvalidateItem(pObj, LISTBOX_ALL_ITEMS);
}
unsigned LISTBOX_GetItemSpacing(ListBox_Obj *pObj) {
	return pObj ? pObj->ItemSpacing : 0;
}
void LISTBOX_SetAutoScroll(ListBox_Obj *pObj, int State, char H0V1) {
	if (!pObj)
		return;
	H0V1 = H0V1 ? LISTBOX_CF_AUTOSCROLLBAR_V : LISTBOX_CF_AUTOSCROLLBAR_H;
	char Flags = pObj->Flags & ~H0V1;
	if (State)
		Flags |= H0V1;
	if (pObj->Flags == Flags)
		return;
	pObj->Flags = Flags;
	LISTBOX_UpdateScrollers(pObj);
}
void LISTBOX_SetBkColor(ListBox_Obj *pObj, unsigned Index, RGBC color) {
	if (!pObj)
		return;
	if ((unsigned int)Index >= GUI_COUNTOF(pObj->Props.aBackColor))
		return;
	pObj->Props.aBackColor[Index] = color;
	LISTBOX__InvalidateInsideArea(pObj);
}
void LISTBOX_SetOwner(ListBox_Obj *pObj, WObj *pOwner) {
	if (!pObj)
		return;
	pObj->pOwner = pOwner;
	LISTBOX__InvalidateInsideArea(pObj);
}
void LISTBOX_SetOwnerDraw(ListBox_Obj *pObj, WIDGET_DRAW_ITEM_FUNC *pfDrawItem) {
	if (!pObj)
		return;
	pObj->pfDrawItem = pfDrawItem;
	LISTBOX_InvalidateItem(pObj, LISTBOX_ALL_ITEMS);
}
void LISTBOX_SetScrollbarWidth(ListBox_Obj *pObj, unsigned Width) {
	if (!pObj)
		return;
	if (Width == (unsigned)pObj->ScrollbarWidth)
		return;
	pObj->ScrollbarWidth = Width;
	LISTBOX__SetScrollbarWidth(pObj);
	LISTBOX_Invalidate(pObj);
}
void LISTBOX_SetString(ListBox_Obj *pObj, const char *s, unsigned int Index) {
	if (!pObj)
		return;
	if (Index >= (unsigned int)LISTBOX__GetNumItems(pObj))
		return;
	LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_ResizeItem(&pObj->ItemArray, Index, sizeof(LISTBOX_ITEM) + GUI__strlen(s));
	if (!pItem)
		return;
	GUI__strcpy(pItem->acText, s);
	LISTBOX__InvalidateItemSize(pObj, Index);
	LISTBOX_UpdateScrollers(pObj);
	LISTBOX__InvalidateItem(pObj, Index);
}
RGBC LISTBOX_SetTextColor(ListBox_Obj *pObj, unsigned int Index, RGBC Color) {
	if (!pObj)
		return GUI_INVALID_COLOR;
	if (Index >= GUI_COUNTOF(pObj->Props.aBackColor))
		return GUI_INVALID_COLOR;
	pObj->Props.aTextColor[Index] = Color;
	RGBC r = pObj->Props.aTextColor[Index];
	LISTBOX__InvalidateInsideArea(pObj);
	return r;
}
void LISTBOX_SetDefaultFont(CFont *pFont) {
	LISTBOX_DefaultProps.pFont = pFont;
}
CFont *LISTBOX_GetDefaultFont(void) {
	return LISTBOX_DefaultProps.pFont;
}
void LISTBOX_SetDefaultScrollStepH(int Value) {
	LISTBOX_DefaultProps.ScrollStepH = Value;
}
int LISTBOX_GetDefaultScrollStepH(void) {
	return LISTBOX_DefaultProps.ScrollStepH;
}
void LISTBOX_SetDefaultBkColor(unsigned Index, RGBC Color) {
	if (Index < GUI_COUNTOF(LISTBOX_DefaultProps.aBackColor))
		LISTBOX_DefaultProps.aBackColor[Index] = Color;
}
RGBC LISTBOX_GetDefaultBkColor(unsigned Index) {
	if (Index < GUI_COUNTOF(LISTBOX_DefaultProps.aBackColor))
		return LISTBOX_DefaultProps.aBackColor[Index];
	return GUI_INVALID_COLOR;
}
void LISTBOX_SetDefaultTextColor(unsigned Index, RGBC Color) {
	if (Index < GUI_COUNTOF(LISTBOX_DefaultProps.aTextColor))
		LISTBOX_DefaultProps.aTextColor[Index] = Color;
}
RGBC LISTBOX_GetDefaultTextColor(unsigned Index) {
	if (Index < GUI_COUNTOF(LISTBOX_DefaultProps.aTextColor))
		return LISTBOX_DefaultProps.aTextColor[Index];
	return GUI_INVALID_COLOR;
}
void LISTBOX_DeleteItem(ListBox_Obj *pObj, unsigned int Index) {
	if (!pObj)
		return;
	if (Index >= LISTBOX__GetNumItems(pObj))
		return;
	GUI_ARRAY_DeleteItem(&pObj->ItemArray, Index);
	int Sel = pObj->Sel;
	if (Sel >= 0) {
		if ((int)Index == Sel)
			pObj->Sel = -1;
		else if ((int)Index < Sel)
			pObj->Sel--;
	}
	if (LISTBOX_UpdateScrollers(pObj))
		LISTBOX__InvalidateInsideArea(pObj);
	else
		LISTBOX__InvalidateItemAndBelow(pObj, Index);
}
void LISTBOX_SetFont(ListBox_Obj *pObj, CFont *pFont) {
	if (!pObj)
		return;
	pObj->Props.pFont = pFont;
	LISTBOX_InvalidateItem(pObj, LISTBOX_ALL_ITEMS);
}
CFont *LISTBOX_GetFont(ListBox_Obj *pObj) {
	return pObj ? pObj->Props.pFont : nullptr;
}
void LISTBOX_GetItemText(ListBox_Obj *pObj, unsigned Index, char *pBuffer, int MaxSize) {
	if (!pObj)
		return;
	unsigned NumItems = LISTBOX__GetNumItems(pObj);
	if (Index >= NumItems)
		return;
	const char *pString = LISTBOX__GetpString(pObj, Index);
	int CopyLen = GUI__strlen(pString);
	if (CopyLen > (MaxSize - 1))
		CopyLen = MaxSize - 1;
	GUI__memcpy(pBuffer, pString, CopyLen);
	pBuffer[CopyLen] = 0;
}
unsigned LISTBOX_GetNumItems(ListBox_Obj *pObj) {
	return pObj ? LISTBOX__GetNumItems(pObj) : 0;
}
int LISTBOX_GetItemDisabled(ListBox_Obj *pObj, unsigned Index) {
	if (!pObj)
		return 0;
	unsigned NumItems = LISTBOX__GetNumItems(pObj);
	if (Index >= NumItems)
		return 0;
	auto pItem = (WObj *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	if (!pItem)
		return 0;
	return (pItem->Status & LISTBOX_ITEM_DISABLED) != 0;
}
void LISTBOX_SetItemDisabled(ListBox_Obj *pObj, unsigned Index, int OnOff) {
	if (!pObj)
		return;
	if (Index >= LISTBOX__GetNumItems(pObj))
		return;
	LISTBOX_ITEM *pItem = (LISTBOX_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	if (!pItem)
		return;
	if (OnOff) {
		if (!(pItem->Status & LISTBOX_ITEM_DISABLED)) {
			pItem->Status |= LISTBOX_ITEM_DISABLED;
			LISTBOX__InvalidateItem(pObj, Index);
		}
	}
	else if (pItem->Status & LISTBOX_ITEM_DISABLED) {
		pItem->Status &= ~LISTBOX_ITEM_DISABLED;
		LISTBOX__InvalidateItem(pObj, Index);
	}
}
