#include "ScrollBar.h"
#include "ListBox.h"

#define LISTBOX_ITEM_SELECTED (1 << 0)
#define LISTBOX_ITEM_DISABLED (1 << 1)

ListBox::Property ListBox::DefaultProps;

static int _Tolower(int Key) {
	return Key >= 0x41 && Key <= 0x5a ?
		Key + 0x20 : Key;
}
static bool _IsAlphaNum(int Key) {
	Key = _Tolower(Key);
	if (Key >= 'a' && Key <= 'z')
		return true;
	if (Key >= '0' && Key <= '9')
		return true;
	return false;
}

int ListBox::_CallOwnerDraw(int Cmd, uint16_t ItemIndex) {
	WIDGET_ITEM_DRAW_INFO ItemInfo;
	ItemInfo.Cmd = Cmd;
	ItemInfo.pWin = this;
	ItemInfo.ItemIndex = ItemIndex;
	if (pfDrawItem)
		return pfDrawItem(&ItemInfo);
	return OwnerDrawProc(&ItemInfo);
}
int ListBox::_GetYSize() { return InsideRect().ysize(); }
int ListBox::_GetItemSize(uint16_t Index, uint8_t XY) {
	auto &item = ItemArray[Index];
	int size = XY == WIDGET_ITEM_GET_XSIZE ? item.xSize : item.ySize;
	if (size == 0) {
		auto pOldFont = GUI.Font();
		GUI.Font(Props.pFont);
		size = _CallOwnerDraw(XY, Index);
		GUI.Font(pOldFont);
	}
	if (XY == WIDGET_ITEM_GET_XSIZE)
		item.xSize = size;
	else
		item.ySize = size;
	return size;
}
int ListBox::_GetContentsSizeX() {
	int r = 0;
	for (int i = 0, NumItems = ItemArray.NumItems(); i < NumItems; ++i) {
		auto SizeX = _GetItemSize(i, WIDGET_ITEM_GET_XSIZE);
		if (r < SizeX)
			r = SizeX;
	}
	return r;
}
int ListBox::_GetItemPosY(uint16_t Index) {
	if (Index >= ItemArray.NumItems())
		return -1;
	if (Index < scrollStateV.v)
		return -1;
	int PosY = 0;
	for (auto i = scrollStateV.v; i < Index; ++i)
		PosY += _GetItemSize(i, WIDGET_ITEM_GET_YSIZE);
	return PosY;
}
bool ListBox::_IsPartiallyVis() {
	int Index = sel;
	if (Index >= ItemArray.NumItems())
		return false;
	if (Index < scrollStateV.v)
		return false;
	return _GetItemPosY(Index) +
		_GetItemSize(Index, WIDGET_ITEM_GET_YSIZE) > _GetYSize();
}
uint16_t ListBox::_GetNumVisItems() {
	int NumItems = ItemArray.NumItems();
	if (NumItems <= 1)
		return 1;
	int ySize = _GetYSize(), yDist = 0;
	for (auto i = NumItems - 1; i >= 0; --i) {
		yDist += _GetItemSize(i, WIDGET_ITEM_GET_YSIZE);
		if (yDist > ySize) {
			auto r = NumItems - 1 - i;
			return r < 1 ? 1 : r;
		}
	}
	return 1;
}
void ListBox::_NotifyOwner(int Notification) {
	auto pOwner = this->pOwner ? this->pOwner : Parent();
	WM_PARAM data;
	data = Notification;
	pOwner->SendMessage(WM_NOTIFY_PARENT, &data, this);
}
int ListBox::_UpdateScrollPos() {
	int PrevScrollStateV = scrollStateV.v;
	if (sel >= 0) {
		if (_IsPartiallyVis())
			scrollStateV.v = sel - scrollStateV.PageSize + 1;
		if (sel < scrollStateV.v)
			scrollStateV.v = sel;
	}
	scrollStateV.Bound();
	scrollStateH.Bound();
	ScrollStateH(scrollStateH);
	ScrollStateV(scrollStateV);
	return scrollStateV.v - PrevScrollStateV;
}
void ListBox::_InvalidateItemSize(uint16_t Index) {
	auto &item = ItemArray[Index];
	item.xSize = item.ySize = 0;
}
void ListBox::_InvalidateInsideArea() {
	Invalidate(InsideRect());
}
void ListBox::_InvalidateItem(int sel) {
	if (sel < 0)
		return;
	int ItemPosY = _GetItemPosY(sel);
	if (ItemPosY < 0)
		return;
	int ItemDistY = _GetItemSize(sel, WIDGET_ITEM_GET_YSIZE);
	auto &&rect = InsideRect();
	rect.y0 += ItemPosY;
	rect.y1 = rect.y0 + ItemDistY - 1;
	Invalidate(rect);
}
void ListBox::_InvalidateItemAndBelow(int sel) {
	if (sel < 0)
		return;
	int ItemPosY = _GetItemPosY(sel);
	if (ItemPosY < 0)
		return;
	auto &&rect = InsideRect();
	rect.y0 += ItemPosY;
	Invalidate(rect);
}
void ListBox::_SetScrollbarWidth() {
	int Width = ScrollbarWidth;
	if (Width == 0)
		Width = ScrollBar::DefaultWidth;
	if (auto pScroll = ScrollBarH())
		pScroll->Width(Width);
	if (auto pScroll = ScrollBarV())
		pScroll->Width(Width);
}
int ListBox::_CalcScrollParas() {
	scrollStateV.NumItems = ItemArray.NumItems();
	scrollStateV.PageSize = _GetNumVisItems();
	scrollStateH.NumItems = _GetContentsSizeX();
	scrollStateH.PageSize = InsideRect().xsize();
	return _UpdateScrollPos();
}
void ListBox::_ManageAutoScroll() {
	if (Flags & LISTBOX_CF_AUTOSCROLLBAR_V)
		ScrollBarV(_GetNumVisItems() < ItemArray.NumItems());
	if (Flags & LISTBOX_CF_AUTOSCROLLBAR_H)
		ScrollBarH(_GetContentsSizeX() > InsideRect().xsize());
	if (ScrollbarWidth)
		_SetScrollbarWidth();
}
int ListBox::_UpdateScrollers() {
	_ManageAutoScroll();
	return _CalcScrollParas();
}
void ListBox::_SelectByKey(int Key) {
	Key = _Tolower(Key);
	for (auto i = 0; i < ItemArray.NumItems(); ++i) {
		if (_Tolower(*ItemArray[i].Text) == Key) {
			Sel(i);
			break;
		}
	}
}

void ListBox::_ToggleMultiSel(int sel) {
	if (!(Flags & LISTBOX_CF_MULTISEL))
		return;
	auto &item = ItemArray[sel];
	if (item.Status & LISTBOX_ITEM_DISABLED)
		return;
	item.Status ^= LISTBOX_ITEM_SELECTED;
	_NotifyOwner(WN_SEL_CHANGED);
	_InvalidateItem(sel);
}
int ListBox::_GetItemFromPos(int x, int y) {
	auto &&rect = InsideRect();
	if (x < rect.x0 || y < rect.y0)
		return -1;
	if (x > rect.x1 || y > rect.y1)
		return 1;
	int sel = -1;
	int y0 = rect.y0;
	for (int i = scrollStateV.v, NumItems = ItemArray.NumItems();
		 i < NumItems; ++i) {
		if (y >= y0)
			sel = i;
		y0 += _GetItemSize(i, WIDGET_ITEM_GET_YSIZE);
	}
	return sel;
}

void ListBox::_OnPaint(WM_PARAM *pData) {
	GUI.Font(Props.pFont);
	auto rClip = *(const SRect *)*pData;
	auto &&rInside = InsideRect();
	rClip &= rInside;
	SRect rItem;
	rItem.x0 = rClip.x0;
	rItem.x1 = rClip.x1;
	WIDGET_ITEM_DRAW_INFO ItemInfo;
	ItemInfo.Cmd = WIDGET_ITEM_DRAW;
	ItemInfo.pWin = this;
	ItemInfo.x0 = rInside.x0 - scrollStateH.v;
	ItemInfo.y0 = rInside.y0;
	for (int i = scrollStateV.v,
		 NumItems = ItemArray.NumItems(); i < NumItems; ++i) {
		rItem.y0 = ItemInfo.y0;
		if (rItem.y0 > rClip.y1)
			break;
		int ItemDistY = _GetItemSize(i, WIDGET_ITEM_GET_YSIZE);
		rItem.y1 = rItem.y0 + ItemDistY - 1;
		if (rItem.y1 >= rClip.y0) {
			WObj::UserClip(&rItem);
			ItemInfo.ItemIndex = i;
			if (pfDrawItem)
				pfDrawItem(&ItemInfo);
			else
				OwnerDrawProc(&ItemInfo);
		}
		ItemInfo.y0 += ItemDistY;
	}
	WObj::UserClip(nullptr);
	rItem.y0 = ItemInfo.y0;
	rItem.y1 = rInside.y1;
	GUI.BkColor(Props.aBkColor[0]);
	GUI.Clear(rItem);
	DrawDown();
}
void ListBox::_OnTouch(const PID_STATE *pState) {
	if (pState) {
		if (pState->Pressed == 0)
			_NotifyOwner(WN_RELEASED);
	}
	else
		_NotifyOwner(WN_MOVED_OUT);
}
void ListBox::_OnMouseOver(const PID_STATE *pState) {
	if (!pOwner)
		return;
	if (pState) {
		auto sel = _GetItemFromPos(pState->x, pState->y);
		if (sel >= 0)
			if (sel < scrollStateV.v + _GetNumVisItems())
				Sel(sel);
	}
}
void ListBox::_Callback(WObj *pWin, int msgid, WM_PARAM *pData, WObj *pWinSrc) {
	auto pObj = (ListBox *)pWin;
	if (!pObj->HandleActive(msgid, pData)) {
		if (msgid == WM_SET_FOCUS)
			if (*pData == 0)
				pObj->_NotifyOwner(LISTBOX_NOTIFICATION_LOST_FOCUS);
		return;
	}
	switch (msgid) {
	case WM_NOTIFY_PARENT:
		switch (*pData) {
		case WN_VALUE_CHANGED:
			if (pWinSrc == pObj->ScrollBarV()) {
				pObj->scrollStateV.v = ((ScrollBar *)pWinSrc)->ScrollState().v;
				pObj->_InvalidateInsideArea();
				pObj->_NotifyOwner(WN_SCROLL_CHANGED);
			}
			else if (pWinSrc == pObj->ScrollBarH()) {
				pObj->scrollStateH.v = ((ScrollBar *)pWinSrc)->ScrollState().v;
				pObj->_InvalidateInsideArea();
				pObj->_NotifyOwner(WN_SCROLL_CHANGED);
			}
			break;
		case WN_SCROLLBAR_ADDED:
			pObj->_UpdateScrollers();
			break;
		}
		break;
	case WM_PAINT:
		pObj->_OnPaint(pData);
		break;
	case WM_PID_STATE_CHANGED:
	{
		auto pInfo = (const PID_CHANGED_STATE *)*pData;
		if (!pInfo->State)
			break;
		int sel = pObj->_GetItemFromPos(pInfo->x, pInfo->y);
		if (sel >= 0) {
			pObj->_ToggleMultiSel(sel);
			pObj->Sel(sel);
		}
		pObj->_NotifyOwner(WN_CLICKED);
		return;
	}
	case WM_TOUCH:
		pObj->_OnTouch((const PID_STATE *)*pData);
		return;
	case WM_MOUSEOVER:
		pObj->_OnMouseOver((const PID_STATE *)*pData);
		return;
	case WM_DELETE:
		pObj->~ListBox();
		break;
	case WM_KEY:
	{
		auto ki = (const KEY_STATE *)*pData;
		if (ki->PressedCnt > 0)
			if (pObj->AddKey(ki->Key))
				return;
		break;
	}
	case WM_SIZE:
		pObj->_UpdateScrollers();
		pObj->Invalidate();
		break;
	}
	DefCallback(pObj, msgid, pData, pWinSrc);
}

static uint16_t _CountItems(const char **ppItems) {
	uint16_t nItems = 0;
	for (auto s = ppItems; *s; ++s)
		++nItems;
	return nItems;
}

ListBox::ListBox(int x0, int y0, int xsize, int ysize,
				 WObj *pParent, uint16_t Id, uint16_t Flags, uint8_t ExFlags,
				 int NumItems) :
	Widget(x0, y0, xsize, ysize,
		   _Callback,
		   pParent, Id, Flags, WIDGET_STATE_FOCUSSABLE), 
	ItemArray(NumItems), Flags(ExFlags) {}
ListBox::ListBox(int x0, int y0, int xsize, int ysize,
				 WObj *pParent, uint16_t Id, uint16_t Flags, uint8_t ExFlags,
				 const char *pItems) :
	ListBox(x0, y0, xsize, ysize,
			pParent, Id, Flags, ExFlags,
			GUI__countStrings(pItems)) {
	for (auto &t : ItemArray) {
		t.Text = pItems;
		pItems = GUI__nextString(pItems);
	}
	_UpdateScrollers();
}

void ListBox::InvalidateItem(int Index) {
	auto NumItems = ItemArray.NumItems();
	if (Index >= NumItems)
		return;
	if (Index >= 0) {
		_InvalidateItemSize(Index);
		_UpdateScrollers();
		_InvalidateItemAndBelow(Index);
		return;
	}
	for (int i = 0; i < NumItems; ++i)
		_InvalidateItemSize(i);
	_UpdateScrollers();
	_InvalidateInsideArea();
}

bool ListBox::AddKey(int Key) {
	switch (Key) {
		case ' ':
			_ToggleMultiSel(sel);
			return true;
		case GUI_KEY_RIGHT:
			if (scrollStateH.Value(scrollStateH.v + Props.ScrollStepH)) {
				_UpdateScrollers();
				_InvalidateInsideArea();
			}
			return true;
		case GUI_KEY_LEFT:
			if (scrollStateH.Value(scrollStateH.v - Props.ScrollStepH)) {
				_UpdateScrollers();
				_InvalidateInsideArea();
			}
			return true;
		case GUI_KEY_DOWN:
			IncSel();
			return true;
		case GUI_KEY_UP:
			DecSel();
			return true;
		default:
			if (_IsAlphaNum(Key)) {
				_SelectByKey(Key);
				return true;
			}
	}
	return false;
}

void ListBox::Add(const char *s) {
	if (!s)
		return;
	auto Index = ItemArray.NumItems() - 1;
	ItemArray.Add(s);
	_InvalidateItemSize(Index);
	_UpdateScrollers();
	_InvalidateItem(Index);
}
void ListBox::Insert(uint16_t Index, const char *s) {
	if (!s)
		return;
	if (Index >= ItemArray.NumItems())
		Add(s);
	else {
		ItemArray.Insert((const char *)ItemArray[Index].Text, Index);
		InvalidateItem(Index);
	}
}

void ListBox::Delete(uint16_t Index) {
	if (Index >= ItemArray.NumItems())
		return;
	ItemArray.Delete(Index);
	if (sel >= 0) {
		if ((int)Index == sel)
			sel = -1;
		else if ((int)Index < sel)
			--sel;
	}
	if (_UpdateScrollers())
		_InvalidateInsideArea();
	else
		_InvalidateItemAndBelow(Index);
}

int ListBox::OwnerDrawProc(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo) {
	auto pObj = (ListBox *)pDrawItemInfo->pWin;
	switch (pDrawItemInfo->Cmd) {
		case WIDGET_ITEM_GET_XSIZE:
		{
			auto pOldFont = GUI.Font();
			GUI.Font(pObj->Props.pFont);
			auto DistX = GUI_GetStringDistX(pObj->ItemArray[pDrawItemInfo->ItemIndex].Text);
			GUI.Font(pOldFont);
			return DistX;
		}
		case WIDGET_ITEM_GET_YSIZE:
			return pObj->Props.pFont->YDist + pObj->ItemSpacing;
		case WIDGET_ITEM_DRAW:
		{
			auto ItemIndex = pDrawItemInfo->ItemIndex;
			auto &item = pObj->ItemArray[ItemIndex];
			int FontDistY = GUI.Font()->YDist;
			bool bDisabled = item.Status & LISTBOX_ITEM_DISABLED,
				bSelected = item.Status & LISTBOX_ITEM_SELECTED;
			int ColorIndex = (pObj->Flags & LISTBOX_CF_MULTISEL) ?
				bDisabled ? 3 :
				bSelected ? 2 : 0 : bDisabled ? 3 :
				ItemIndex == pObj->sel ?
				(pObj->State & WIDGET_STATE_FOCUS) ? 2 : 1 : 0;
			GUI.BkColor(pObj->Props.aBkColor[ColorIndex]);
			GUI.PenColor(pObj->Props.aTextColor[ColorIndex]);
			GUI.TextMode(DRAWMODE_TRANS);
			GUI.Clear();
			GUI_DispStringAt(pObj->ItemArray[ItemIndex].Text, { pDrawItemInfo->x0 + 1, pDrawItemInfo->y0 });
			if (!(pObj->Flags & LISTBOX_CF_MULTISEL) || ItemIndex != pObj->sel)
				return 0;
			SRect rFocus;
			rFocus.x0 = pDrawItemInfo->x0;
			rFocus.y0 = pDrawItemInfo->y0;
			rFocus.x1 = pObj->InsideRect().x1;
			rFocus.y1 = pDrawItemInfo->y0 + FontDistY - 1;
			GUI.PenColor(RGB_WHITE - pObj->Props.aBkColor[ColorIndex]);
			GUI.OutlineFocus(rFocus);
		}
	}
	return 0;
}

void ListBox::AutoScroll(bool bEnabled, bool H0V1) {
	auto s = H0V1 ? LISTBOX_CF_AUTOSCROLLBAR_V : LISTBOX_CF_AUTOSCROLLBAR_H;
	char Flags = this->Flags;
	if (bEnabled)
		Flags |= s;
	else
		Flags &= ~s;
	if (this->Flags == Flags)
		return;
	this->Flags = Flags;
	_UpdateScrollers();
}

void ListBox::MoveSel(int Dir) {
	int Index = Sel();
	int NumItems = ItemArray.NumItems();
	int NewSel = -1;
	do {
		Index += Dir;
		if (Index < 0 || Index >= NumItems)
			break;
		auto &item = ItemArray[Index];
		if (!(item.Status & LISTBOX_ITEM_DISABLED))
			NewSel = Index;
	} while (NewSel < 0);
	if (NewSel >= 0)
		Sel(NewSel);
}
void ListBox::Sel(int NewSel) {
	auto MaxSel = ItemArray.NumItems();
	MaxSel = MaxSel ? MaxSel - 1 : 0;
	if (NewSel > MaxSel)
		NewSel = MaxSel;
	if (NewSel < 0)
		NewSel = -1;
	else {
		auto &item = ItemArray[NewSel];
		if (item.Status & LISTBOX_ITEM_DISABLED)
			NewSel = -1;
}
	if (NewSel == sel)
		return;
	auto OldSel = sel;
	sel = NewSel;
	if (_UpdateScrollPos())
		_InvalidateInsideArea();
	else {
		_InvalidateItem(OldSel);
		_InvalidateItem(NewSel);
	}
	_NotifyOwner(WN_SEL_CHANGED);
}

bool ListBox::ItemSel(uint16_t Index) const {
	if (Index >= ItemArray.NumItems())
		return false;
	if (!(Flags & LISTBOX_CF_MULTISEL))
		return false;
	return ItemArray[Index].Status & LISTBOX_ITEM_SELECTED;
}
void ListBox::ItemSel(uint16_t Index, bool bSel) {
	if (Index >= ItemArray.NumItems())
		return;
	if (!(Flags & LISTBOX_CF_MULTISEL))
		return;
	auto &item = ItemArray[Index];
	auto Status = item.Status;
	if (bSel)
		Status |= LISTBOX_ITEM_SELECTED;
	else
		Status &= ~LISTBOX_ITEM_SELECTED;
	if (item.Status == Status)
		return;
	item.Status = Status;
	_InvalidateItem(Index);
}

bool ListBox::ItemEnabled(uint16_t Index) const {
	if (Index >= ItemArray.NumItems())
		return false;
	return !(ItemArray[Index].Status & LISTBOX_ITEM_DISABLED);
}
void ListBox::ItemEnabled(uint16_t Index, bool bEnabled) {
	if (Index >= ItemArray.NumItems())
		return;
	auto &item = ItemArray[Index];
	auto oldStatus = item.Status;
	if (bEnabled) 
		oldStatus &= ~LISTBOX_ITEM_DISABLED;
	else
		oldStatus |= LISTBOX_ITEM_DISABLED;
	if (item.Status == oldStatus)
		return;
	item.Status = oldStatus;
	_InvalidateItem(Index);
}

void ListBox::ItemText(uint16_t Index, const char *s) {
	if (Index < ItemArray.NumItems()) {
		ItemArray[Index] = s;
		_InvalidateItemSize(Index);
		_UpdateScrollers();
		_InvalidateItem(Index);
	}
}
