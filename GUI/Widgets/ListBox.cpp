
#include "ScrollBar.h"
#include "ListBox.h"

#define LISTBOX_ITEM_SELECTED (1 << 0)
#define LISTBOX_ITEM_DISABLED (1 << 1)

ListBox::Property ListBox::DefaultProps;

int ListBox::_CallOwnerDraw(WIDGET_ITEM_CMD Cmd, uint16_t ItemIndex, SRect rItem)
{ return OwnerDraw()(this, Cmd, ItemIndex, rItem); }
int ListBox::_GetYSize() { return WObj::InsideRect().ysize(); }
int ListBox::_GetItemSize(uint16_t Index, WIDGET_ITEM_CMD XY) {
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
	uint16_t i;
	for (i = 0; i < NumItems; ++i) {
		yDist += _GetItemSize(i, WIDGET_ITEM_GET_YSIZE);
		if (yDist > ySize)
			break;
	}
	return i;
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
	Invalidate(WObj::InsideRect());
}
void ListBox::_InvalidateItem(int16_t Index) {
	if (Index < 0)
		return;
	int ItemPosY = _GetItemPosY(Index);
	if (ItemPosY < 0)
		return;
	int ItemDistY = _GetItemSize(Index, WIDGET_ITEM_GET_YSIZE);
	auto &&rsWin = WObj::InsideRect();
	rsWin.y0 += ItemPosY;
	rsWin.y1 = rsWin.y0 + ItemDistY - 1;
	Invalidate(rsWin);
}
void ListBox::_InvalidateItemAndBelow(int sel) {
	if (sel < 0)
		return;
	int ItemPosY = _GetItemPosY(sel);
	if (ItemPosY < 0)
		return;
	auto &&rsWin = WObj::InsideRect();
	rsWin.y0 += ItemPosY;
	Invalidate(rsWin);
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
	scrollStateH.PageSize = WObj::InsideRect().xsize();
	return _UpdateScrollPos();
}
void ListBox::_ManageAutoScroll() {
	if (StatusEx & LISTBOX_CF_AUTOSCROLLBAR_V)
		ScrollBarV(_GetNumVisItems() < ItemArray.NumItems());
	if (StatusEx & LISTBOX_CF_AUTOSCROLLBAR_H)
		ScrollBarH(_GetContentsSizeX() > WObj::InsideRect().xsize());
	if (ScrollbarWidth)
		_SetScrollbarWidth();
}
int ListBox::_UpdateScrollers() {
	_ManageAutoScroll();
	return _CalcScrollParas();
}

void ListBox::_ToggleMultiSel(int sel) {
	if (!(StatusEx & LISTBOX_CF_MULTISEL))
		return;
	auto &item = ItemArray[sel];
	if (item.Status & LISTBOX_ITEM_DISABLED)
		return;
	item.Status ^= LISTBOX_ITEM_SELECTED;
	NotifyOwner(WN_SEL_CHANGED);
	_InvalidateItem(sel);
}
int ListBox::_GetItemFromPos(Point Pos) {
	auto &&rsWin = WObj::InsideRect();
	if (!(Pos <= rsWin))
		return -1;
	int sel = -1;
	int y0 = rsWin.y0;
	for (int i = scrollStateV.v, NumItems = ItemArray.NumItems();
		 i < NumItems; ++i) {
		if (Pos.y >= y0)
			sel = i;
		y0 += _GetItemSize(i, WIDGET_ITEM_GET_YSIZE);
	}
	return sel;
}

void ListBox::_OnPaint(SRect rClip) {
	GUI.Font(Props.pFont);
	auto &&rInside = WObj::InsideRect();
	rClip &= rInside;
	SRect rItem;
	rItem.x0 = rClip.x0 - scrollStateH.v;
	rItem.y0 = rInside.x0;
	rItem.x1 = rClip.x1;
	for (int i = scrollStateV.v,
		 NumItems = ItemArray.NumItems(); i < NumItems; ++i) {
		if (rItem.y0 > rClip.y1)
			break;
		int ItemDistY = _GetItemSize(i, WIDGET_ITEM_GET_YSIZE);
		rItem.y1 = rItem.y0 + ItemDistY - 1;
		if (rItem.y1 >= rClip.y0) {
			WObj::UserClip(&rItem);
			_CallOwnerDraw(WIDGET_ITEM_DRAW, i, rItem);
		}
		rItem.y0 += ItemDistY;
	}
	WObj::UserClip(nullptr);
	rItem.y1 = rInside.y1;
	GUI.BkColor(Props.aBkColor[0]);
	GUI.Clear(rItem);
	DrawDown();
}
void ListBox::_OnMouse(MOUSE_STATE State) {
	if (State) {
		if (!State.Pressed)
			NotifyOwner(WN_RELEASED);
	}
	else
		NotifyOwner(WN_MOVED_OUT);
}
bool ListBox::_OnKey(KEY_STATE State) {
	if (State.PressedCnt <= 0)
		return false;
	switch (auto Key = State.Key) {
		case ' ':
			if (StatusEx & LISTBOX_CF_MULTISEL) {
				_ToggleMultiSel(sel);
				return true;
			}
			break;
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
	}
	return false;
}
void ListBox::_OnMouseOver(MOUSE_STATE State) {
	if (State && Popup()) {
		auto sel = _GetItemFromPos(State);
		if (sel >= 0)
			if (sel < scrollStateV.v + _GetNumVisItems())
				Sel(sel);
	}
}

WM_RESULT ListBox::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (ListBox *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	switch (MsgId) {
		case WM_PAINT:
			pObj->_OnPaint(Param);
			return 0;
		case WM_MOUSE:
			pObj->_OnMouse(Param);
			return 0;
		case WM_MOUSE_OVER:
			pObj->_OnMouseOver(Param);
			return 0;
		case WM_MOUSE_CHANGED: {
			MOUSE_CHANGED_STATE State = Param;
			if (!State.Pressed)
				break;
			int sel = pObj->_GetItemFromPos(State);
			if (sel >= 0) {
				pObj->_ToggleMultiSel(sel);
				pObj->Sel(sel);
			}
			pObj->NotifyOwner(WN_CLICKED);
			return 0;
		}
		case WM_KEY:
			if (pObj->_OnKey(Param))
				return true;
			break;
		case WM_DELETE:
			pObj->~ListBox();
			return 0;
		case WM_NOTIFY_CHILD:
			switch ((int)Param) {
				case WN_VALUE_CHANGED:
					if (pSrc == pObj->ScrollBarV()) {
						pObj->scrollStateV.v = ((ScrollBar *)pSrc)->ScrollState().v;
						pObj->_InvalidateInsideArea();
						pObj->NotifyOwner(WN_SCROLL_CHANGED);
					}
					else if (pSrc == pObj->ScrollBarH()) {
						pObj->scrollStateH.v = ((ScrollBar *)pSrc)->ScrollState().v;
						pObj->_InvalidateInsideArea();
						pObj->NotifyOwner(WN_SCROLL_CHANGED);
					}
					break;
				case WN_SCROLLBAR_ADDED:
					pObj->_UpdateScrollers();
					break;
			}
			break;
		case WM_SIZED:
			pObj->_UpdateScrollers();
			pObj->Invalidate();
			break;
		case WM_GET_CLASS:
			return ClassNames[WCLS_LISTBOX];
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

ListBox::ListBox(const SRect &rc,
				 PWObj pParent, uint16_t Id,
				 WM_CF Flags, LISTBOX_CF FlagsEx,
				 int NumItems) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, FlagsEx),
	ItemArray(NumItems) {}
ListBox::ListBox(const SRect &rc,
				 PWObj pParent, uint16_t Id,
				 WM_CF Flags, LISTBOX_CF FlagsEx,
				 GUI_PCSTR pItems) :
	ListBox(rc,
			pParent, Id,
			Flags, FlagsEx,
			GUI.NumTexts(pItems)) {
	for (auto &t : ItemArray) {
		t.Text = pItems;
		pItems = GUI.NextText(pItems);
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

void ListBox::Add(GUI_PCSTR s) {
	if (!s)
		return;
	auto Index = ItemArray.NumItems() - 1;
	ItemArray.Add().Text = s;
	_InvalidateItemSize(Index);
	_UpdateScrollers();
	_InvalidateItem(Index);
}
void ListBox::Insert(uint16_t Index, GUI_PCSTR s) {
	if (!s)
		return;
	if (Index >= ItemArray.NumItems())
		Add(s);
	else {
		ItemArray.Insert(Index).Text = s;
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

int ListBox::DefOwnerDraw(PWObj pWin, WIDGET_ITEM_CMD Cmd, int16_t ItemIndex, SRect rItem) {
	auto pObj = (ListBox *)pWin;
	switch (Cmd) {
		case WIDGET_ITEM_GET_XSIZE:
		{
			auto &text = pObj->ItemArray[ItemIndex].Text;
			return pObj->Props.pFont->XDist(text, text.Chars());
		}
		case WIDGET_ITEM_GET_YSIZE:
			return pObj->Props.pFont->YDist + pObj->ItemSpacing;
		case WIDGET_ITEM_DRAW:
		{
			auto &item = pObj->ItemArray[ItemIndex];
			bool bFocussed = pObj->Focussed();
			if (!pObj->Visible())
				if (auto pOwner = pObj->Owner())
					bFocussed = pOwner->Focussed();
			auto ColorIndex0 = bFocussed ? LISTBOX_CI_SELFOCUS : LISTBOX_CI_SEL;
			auto ColorIndex =
				item.Status & LISTBOX_ITEM_DISABLED ? LISTBOX_CI_DISABLED :
				pObj->MultiSel() ? item.Status & LISTBOX_ITEM_SELECTED ? ColorIndex0 : LISTBOX_CI_UNSEL :
				pObj->sel == ItemIndex ? ColorIndex0 : LISTBOX_CI_UNSEL;
			GUI.BkColor(pObj->Props.aBkColor[ColorIndex]);
			GUI.PenColor(pObj->Props.aTextColor[ColorIndex]);
			GUI.Clear();
			auto rText = rItem;
			rText.x0 += pObj->EffectSize();
			GUI.DrawStringIn(pObj->ItemArray[ItemIndex].Text, rText);
			if (!(pObj->StatusEx & LISTBOX_CF_MULTISEL) || ItemIndex != pObj->sel)
				return 0;
			GUI.PenColor(RGB_WHITE - pObj->Props.aBkColor[ColorIndex]);
			GUI.DrawFocus(rItem);
		}
	}
	return 0;
}

void ListBox::AutoScroll(LISTBOX_CF ScrollFlags, bool bEnabled) {
	auto StatusEx = this->StatusEx;
	if (bEnabled)
		StatusEx |= ScrollFlags;
	else
		StatusEx &= ~ScrollFlags;
	if (this->StatusEx == StatusEx)
		return;
	this->StatusEx = StatusEx;
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
	else if (0 <= NewSel && NewSel < MaxSel) {
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
	NotifyOwner(WN_SEL_CHANGED);
}

bool ListBox::ItemSel(uint16_t Index) const {
	if (Index >= ItemArray.NumItems())
		return false;
	if (!(StatusEx & LISTBOX_CF_MULTISEL))
		return false;
	return ItemArray[Index].Status & LISTBOX_ITEM_SELECTED;
}
void ListBox::ItemSel(uint16_t Index, bool bSel) {
	if (Index >= ItemArray.NumItems())
		return;
	if (!(StatusEx & LISTBOX_CF_MULTISEL))
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

void ListBox::ItemText(uint16_t Index, GUI_PCSTR s) {
	if (Index < ItemArray.NumItems()) {
		ItemArray[Index].Text = s;
		_InvalidateItemSize(Index);
		_UpdateScrollers();
		_InvalidateItem(Index);
	}
}
void ListBox::MultiSel(bool bEnabled) {
	auto StatusEx = this->StatusEx;
	if (bEnabled)
		StatusEx |= LISTBOX_CF_MULTISEL;
	else
		StatusEx &= ~LISTBOX_CF_MULTISEL;
	if (this->StatusEx == StatusEx)
		return;
	this->StatusEx = StatusEx;
	_InvalidateInsideArea();
}
