#include "Menu.h"

bool Menu::_IsTopLevelMenu() {
	return !_SendMenuMessage(pOwner, MENU_IS_MENU);
}
bool Menu::_HasEffect() {
	if (!(Flags & MENU_CF_POPUP))
		if (_IsTopLevelMenu())
			return false;
	return true;
}
int Menu::_GetEffectSize() { return _HasEffect() ? EffectSize() : 0; }
int Menu::_CalcTextWidth(const char *pText) {
	int TextWidth = 0;
	if (pText) {
		auto pOldFont = GUI.Font();
		GUI.Font(Props.pFont);
		TextWidth = GUI.XDist(pText);
		GUI.Font(pOldFont);
	}
	return TextWidth;
}
int Menu::_GetItemWidth(unsigned Index) {
	if (Width && (Flags & MENU_CF_VERTICAL))
		return Width - _GetEffectSize() * 2;
	int ItemWidth;
	auto &Item = ItemArray[Index];
	if ((Flags & MENU_CF_VERTICAL) || !(Item.Flags & MENU_IF_SEPARATOR))
		ItemWidth = Item.TextWidth;
	else
		ItemWidth = 3;
	ItemWidth += Props.aBorder[MENU_BI_LEFT] + Props.aBorder[MENU_BI_RIGHT];
	return ItemWidth;
}
int Menu::_GetItemHeight(unsigned Index) {
	if (Height && !(Flags & MENU_CF_VERTICAL))
		return Height - _GetEffectSize() * 2;
	auto ItemHeight = Props.pFont->YDist;
	if (Flags & MENU_CF_VERTICAL)
		if (ItemArray[Index].Flags & MENU_IF_SEPARATOR)
			ItemHeight = 3;
	ItemHeight += Props.aBorder[MENU_BI_TOP] + Props.aBorder[MENU_BI_BOTTOM];
	return ItemHeight;
}
int Menu::_CalcMenuSizeX() {
	auto NumItems = this->NumItems();
	int xSize = 0;
	if (Flags & MENU_CF_VERTICAL)
		for (int i = 0; i < NumItems; ++i) {
			auto ItemWidth = _GetItemWidth(i);
			if (ItemWidth > xSize)
				xSize = ItemWidth;
		}
	else for (int i = 0; i < NumItems; ++i)
		xSize += _GetItemWidth(i);
	xSize += _GetEffectSize() * 2;
	return xSize;
}
int Menu::_CalcMenuSizeY() {
	auto NumItems = this->NumItems();
	int ySize = 0;
	if (Flags & MENU_CF_VERTICAL)
		for (int i = 0; i < NumItems; ++i)
			ySize += _GetItemHeight(i);
	else for (int i = 0; i < NumItems; ++i) {
		auto ItemHeight = _GetItemHeight(i);
		if (ySize < ItemHeight)
			ySize = ItemHeight;
	}
	ySize += _GetEffectSize() * 2;
	return ySize;
}
int Menu::_CalcWindowSizeX() { return Width ? Width : _CalcMenuSizeX(); }
int Menu::_CalcWindowSizeY() { return Height ? Height : _CalcMenuSizeY(); }
int Menu::_GetItemFromPos(Point Pos) {
	Point Size{ _CalcMenuSizeX(), _CalcMenuSizeY() };
	if (Size.y > Height && Height)
		Size.y = Height;
	if (Size.x > Width && Width)
		Size.x = Width;
	auto EffectSize = _GetEffectSize();
	Pos -= EffectSize;
	Size -= EffectSize * 2;
	if (SRect{ 0, Size - 1 } > Pos)
		return -1;
	auto NumItems = this->NumItems();
	if (Flags & MENU_CF_VERTICAL)
		for (int i = 0, yPos = 0; i < NumItems; ++i) {
			yPos += _GetItemHeight(i);
			if (Pos.y < yPos)
				return i;
		}
	else for (int i = 0, xPos = 0; i < NumItems; ++i) {
		xPos += _GetItemWidth(i);
		if (Pos.x < xPos)
			return i;
	}
	return -1;
}
Point Menu::_GetItemPos(unsigned Index) {
	Point Pos = _GetEffectSize();
	if (Flags & MENU_CF_VERTICAL)
		for (int i = 0; i < (int)Index; ++i)
			Pos.y += _GetItemHeight(i);
	else for (int i = 0; i < (int)Index; ++i)
		Pos.x += _GetItemWidth(i);
	return Pos;
}

void Menu::_SetCapture() {
	if (!bSubmenuActive)
		if (!Captured())
			Capture(false);
}
void Menu::_ReleaseCapture() {
	if (Captured())
		if (_IsTopLevelMenu() && !(Flags & MENU_CF_POPUP))
			CaptureRelease();
}

void Menu::_CloseSubmenu() {
	if (!(Flags & MENU_CF_ACTIVE))
		return;
	if (!bSubmenuActive)
		return;
	auto &Item = ItemArray[Sel];
	/* Inform submenu about its deactivation and detach it */
	_SendMenuMessage(Item.pSubmenu, MENU_ON_CLOSE);
	Item.pSubmenu->Detach();
	bSubmenuActive = false;
	/*
	* Keep capture in menu widget. The capture may only released
	* by clicking outside the menu or when mouse moved out.
	* And it may only released from a top level menu.
	*/
	_SetCapture();
	/* Invalidate menu item. This is needed because the appearance may have changed */
	_InvalidateItem(Sel);
}
void Menu::_OpenSubmenu(unsigned Index) {
	if (!(Flags & MENU_CF_ACTIVE))
		return;
	auto PrevActiveSubmenu = bSubmenuActive;
	/* Close previous submenu (if needed) */
	_CloseSubmenu();
	auto &Item = ItemArray[Index];
	if (!Item.pSubmenu)
		return;
	if (Flags & MENU_IF_DISABLED)
		return;
	/* Calculate position of submenu */
	auto EffectSize = _GetEffectSize();
	auto &&Pos = _GetItemPos(Index);
	if (Flags & MENU_CF_VERTICAL) {
		Pos.x += _CalcMenuSizeX() - _GetEffectSize() * 2;
		Pos.y -= EffectSize;
	}
	else {
		Pos.y += _CalcMenuSizeY() - _GetEffectSize() * 2;
		Pos.x -= EffectSize;
	}
	Pos += Position();
	/*
	* Notify owner window when for the first time open a menu (when no
	* other submenu was open), so it can initialize the menu items.
	*/
	if (!PrevActiveSubmenu)
		if (_IsTopLevelMenu())
			_SendMenuMessage(pOwner, MENU_ON_INITMENU);
	/* Notify owner window when a submenu opens, so it can initialize the menu items. */
	_SendMenuMessage(pOwner, MENU_ON_INITSUBMENU, Item.Id);
	/* Set active menu as owner of submenu. */
	Item.pSubmenu->Owner(this);
	/* Attach submenu and inform it about its activation. */
	Item.pSubmenu->Parent(Desktop(), Pos);
	_SendMenuMessage(Item.pSubmenu, MENU_ON_OPEN);
	bSubmenuActive = true;
	/* Invalidate menu item. This is needed because the appearance may have changed. */
	_InvalidateItem(Index);
}
void Menu::_ClosePopup() {
	if (Flags & MENU_CF_POPUP) {
		Flags &= ~MENU_CF_POPUP;
		Detach();
		CaptureRelease();
	}
}

void Menu::_SetSelection(int Index) {
	if (Index != Sel) {
		_InvalidateItem(Sel); /* Invalidate previous selection */
		_InvalidateItem(Index); /* Invalidate new selection */
		Sel = Index;
	}
}
void Menu::_SelectItem(unsigned Index) {
	if (Sel != (int)Index) {
		_SetCapture();
		_OpenSubmenu(Index);
		_SetSelection(Index);
	}
}
void Menu::_DeselectItem() {
	if (!bSubmenuActive) {
		_SetSelection(-1);
		_ReleaseCapture();
	}
}

void Menu::_ActivateItem(unsigned Index) {
	auto &Item = ItemArray[Index];
	if (Item.pSubmenu)
		return;
	if (!(Item.Flags & (MENU_IF_DISABLED | MENU_IF_SEPARATOR))) {
		_ClosePopup();
		/* Send item select message to owner. */
		_SendMenuMessage(pOwner, MENU_ON_ITEMSELECT, Id);
	}
}
void Menu::_ActivateMenu(unsigned Index) {
	if (Flags & MENU_CF_OPEN_ON_POINTEROVER)
		return;
	auto &Item = ItemArray[Index];
	if (!Item.pSubmenu)
		return;
	if (Flags & MENU_IF_DISABLED)
		return;
	if (!(Flags & MENU_CF_ACTIVE)) {
		Flags |= MENU_CF_ACTIVE;
		_OpenSubmenu(Index);
		_SetSelection(Index);
	}
	else if (Flags & MENU_CF_CLOSE_ON_SECOND_CLICK) {
		if ((int)Index == Sel) {
			_CloseSubmenu();
			Flags &= ~MENU_CF_ACTIVE;
		}
	}
}
void Menu::_DeactivateMenu() {
	_CloseSubmenu();
	if (!(Flags & MENU_CF_OPEN_ON_POINTEROVER))
		Flags &= ~MENU_CF_ACTIVE;
}

bool Menu::_ForwardMouseOverMsg(Point Pos) {
	if (bSubmenuActive || (Flags & MENU_CF_POPUP))
		return false;
	if (!_IsTopLevelMenu())
		return false;
	auto pBelow = ScreenToWin(Pos + Position());
	if (!pBelow || pBelow == this)
		return false;
	PID_STATE State = Pos - pBelow->Position();
	pBelow->_SendMessage(WM_MOUSEOVER, &State);
	return true;
}
void Menu::_ForwardPIDMsgToOwner(int MsgId, PID_STATE *pState) {
	if (_IsTopLevelMenu())
		return;
	auto pOwner = this->pOwner ? this->pOwner : Parent();
	if (!pOwner)
		return;
	if (pState) {
		PID_STATE State = *pState;
		State += Position() - pOwner->Position();
		pOwner->_SendMessage(MsgId, &State);
	}
	else
		pOwner->_SendMessage(MsgId);
}
bool Menu::_HandlePID(const PID_STATE &State) {
	bool bInside = 0;
	auto &&PrevState = PrevPidState();
	/* Check if coordinates are inside the widget. */
	if (State.x >= 0 && State.y >= 0)
		if (WObj::ClientRect() <= State)
			bInside = true;
	if (!bInside) {
		/*
		 * Handle PID when coordinates are outside the widget.
		 */
		if (State.Pressed == 1 && PrevState.Pressed == 0) {
			/*
			 * User has clicked outside the menu. Close the active submenu.
			 * The widget itself must be closed (if needed) by the owner.
			 */
			_DeactivateMenu();
			_ClosePopup();
		}
		_DeselectItem();
		_ForwardMouseOverMsg(State);
		return true;   /* Coordinates are not in widget, we need to forward PID message to owner */
	}
	auto ItemIndex = _GetItemFromPos(State);
	/* Handle PID when coordinates are inside the widget. */
	if (ItemIndex < 0) {
		/* Coordinates are outside the menu but inside the widget. */
		if (State.Pressed == 1) {
			if (PrevState.Pressed == 0) { /* Clicked */
				/* User has clicked outside the menu. Close the active submenu.
					* The widget itself must be closed (if needed) by the owner. */
				_DeactivateMenu();
			}
			_DeselectItem();
		}
		else if (State.Pressed < 0) /* Moved out or mouse moved */
			_DeselectItem();
		return false;
	}
	/* Coordinates are inside the menu. */
	if (State.Pressed == 1) {
		if (PrevState.Pressed == 0)  /* Clicked */
			_ActivateMenu(ItemIndex);
		_SelectItem(ItemIndex);
	}
	else if (State.Pressed == 0 && PrevState.Pressed == 1) /* Released */
		_ActivateItem(ItemIndex);
	else if (State.Pressed < 0) { /* Mouse moved */
		if (!_ForwardMouseOverMsg(State))
			_SelectItem(ItemIndex);
		else
			_DeselectItem();
	}
	return false;
}

WM_RESULT Menu::_OnMenu(const MENU_MSG_DATA *pMsgData) {
	if (!pMsgData)
		return 0;
	switch (pMsgData->MsgType) {
		case MENU_ON_ITEMSELECT:
			_DeactivateMenu();
			_DeselectItem();
			_ClosePopup();
			/* No break here. We need to forward message to owner. */
		case MENU_ON_INITMENU:
		case MENU_ON_INITSUBMENU: /* Forward message to owner. */
			if (auto pOwner = this->pOwner ? this->pOwner : Parent())
				return pOwner->_SendMessage(MENU_ON_INITSUBMENU, pMsgData, this);
			break;
		case MENU_ON_OPEN:
			Sel = -1;
			bSubmenuActive = false;
			Flags |= MENU_CF_ACTIVE | MENU_CF_OPEN_ON_POINTEROVER;
			_SetCapture();
			_ResizeMenu();
			break;
		case MENU_ON_CLOSE:
			_CloseSubmenu();
			break;
		case MENU_IS_MENU:
			return true;
	}
	return 0;
}

bool Menu::_OnTouch(const PID_STATE *pState) {
	if (pState)  /* Something happened in our area (pressed or released) */
		return _HandlePID(*pState);
	return _HandlePID({ -1, -1 }); /* Moved out */
}
bool Menu::_OnMouseOver(const PID_STATE *pState) {
	if (pState)
		return _HandlePID({ *pState, -1 });
	return false;
}

void Menu::_SetPaintColors(const Menu::Item &Item, int ItemIndex) const {
	unsigned ColorIndex;
	bool bSelected = ItemIndex == Sel;
	if (bSubmenuActive && bSelected)
		ColorIndex = MENU_CI_ACTIVE_SUBMENU;
	else if (Item.Flags & MENU_IF_SEPARATOR)
		ColorIndex = MENU_CI_ENABLED;
	else {
		ColorIndex = bSelected ? MENU_CI_SELECTED : MENU_CI_ENABLED;
		if (Item.Flags & MENU_IF_DISABLED) {
			if (Flags & MENU_CF_HIDE_DISABLED_SEL)
				ColorIndex = MENU_CI_DISABLED;
			else
				ColorIndex += MENU_CI_DISABLED;
		}
	}
	GUI.BkColor(Props.aBkColor[ColorIndex]);
	GUI.PenColor(Props.aTextColor[ColorIndex]);
}

void Menu::_OnPaint() {
	auto BorderLeft = Props.aBorder[MENU_BI_LEFT],
		 BorderTop = Props.aBorder[MENU_BI_TOP];
	auto FontHeight = Props.pFont->YDist;
	auto EffectSize = _GetEffectSize();
	auto NumItems = this->NumItems();
	auto &&rFill = WObj::ClientRect() / EffectSize;
	GUI.Font(Props.pFont);
	if (Flags & MENU_CF_VERTICAL) {
		int ItemHeight, xSize;
		xSize = _CalcMenuSizeX();
		rFill.x1 = xSize - EffectSize - 1;
		SRect rText;
		rText.x0 = rFill.x0 + BorderLeft;
		for (int i = 0; i < NumItems; ++i) {
			auto &Item = ItemArray[i];
			ItemHeight = _GetItemHeight(i);
			_SetPaintColors(Item, i);
			rFill.y1 = rFill.y0 + ItemHeight - 1;
			if (Item.Flags & MENU_IF_SEPARATOR) {
				GUI.Clear(rFill);
				GUI.PenColor(0x7C7C7C);
				GUI.DrawLineH(rFill.x0 + 2, rFill.y0 + BorderTop + 1, rFill.x1 - 2);
			}
			else {
				rText.x1 = rText.x0 + Item.TextWidth - 1;
				rText.y0 = rFill.y0 + BorderTop;
				rText.y1 = rText.y0 + FontHeight - 1;
				GUI.Clear(rFill);
				GUI.DispString(Item.text, rText);
			}
			rFill.y0 += ItemHeight;
		}
	}
	else {
		auto ySize = _CalcMenuSizeY();
		rFill.y1 = ySize - EffectSize - 1;
		SRect rText;
		rText.y0 = rFill.y0 + BorderTop;
		rText.y1 = rText.y0 + FontHeight - 1;
		for (int i = 0; i < NumItems; ++i) {
			auto &Item = ItemArray[i];
			auto ItemWidth = _GetItemWidth(i);
			_SetPaintColors(Item, i);
			rFill.x1 = rFill.x0 + ItemWidth - 1;
			if (Item.Flags & MENU_IF_SEPARATOR) {
				GUI.Clear(rFill);
				GUI.PenColor(0x7C7C7C);
				GUI.DrawLineV(rFill.x0 + BorderLeft + 1, rFill.y0 + 2, rFill.y1 - 2);
			}
			else {
				rText.x0 = rFill.x0 + BorderLeft;
				rText.x1 = rText.x0 + Item.TextWidth - 1;
				GUI.Clear(rFill);
				GUI.DispString(Item.text, rText);
			}
			rFill.x0 += ItemWidth;
		}
	}
	if (Width || Height) {
		auto &&r = WObj::ClientRect() / EffectSize;
		GUI.BkColor(Props.aBkColor[MENU_CI_ENABLED]);
		GUI.Clear({ rFill.x1 + 1, EffectSize, r.x1, rFill.y1 });
		GUI.Clear({ EffectSize, rFill.y1 + 1, r.x1, r.y1 });
	}
	/* Draw 3D effect (if configured) */
	if (_HasEffect())
		DrawUp();
}

WM_RESULT Menu::_Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc) {
	auto pObj = (Menu *)pWin;
	if (MsgId != WM_PID_STATE_CHANGED)
		/* Let widget handle the standard messages */
		if (!pObj->HandleActive(MsgId, Param))
			return Param;
	switch (MsgId) {
		case WM_PAINT:
			pObj->_OnPaint();
			return 0;
		case WM_DELETE:
			pObj->ItemArray.Delete();
			return 0;
		case WM_MENU:
			pObj->_OnMenu(Param);
			return 0;
		case WM_TOUCH:
			if (pObj->_OnTouch(Param))
				pObj->_ForwardPIDMsgToOwner(MsgId, Param);
			return 0;
		case WM_MOUSEOVER:
			if (pObj->_OnMouseOver(Param))
				pObj->_ForwardPIDMsgToOwner(MsgId, Param);
			return 0;
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

void Menu::_InvalidateItem(unsigned Index) {
	Invalidate(); /* Can be optimized, no need to invalidate all items */
}
void Menu::_RecalcTextWidthOfItems() {
	auto NumItems = this->NumItems();
	auto pOldFont = GUI.Font();
	GUI.Font(Props.pFont);
	for (int i = 0; i < NumItems; ++i) {
		auto &Item = ItemArray[i];
		Item.TextWidth = GUI.XDist(Item.text);
	}
	GUI.Font(pOldFont);
}
void Menu::_ResizeMenu() {
	Size({ _CalcWindowSizeX(), _CalcWindowSizeY() });
	Invalidate();
}

void Menu::_SetItem(unsigned Index, const Menu::Item &NewItem) {
	auto &Item = ItemArray[Index];
	Item = NewItem;
	Item.text = NewItem.text;
	Item.TextWidth = _CalcTextWidth(Item.text);
	if (Item.Flags & MENU_IF_SEPARATOR)
		Item.pSubmenu = nullptr;   /* Ensures that no separator is a submenu */
	else if (Item.pSubmenu)
		Item.pSubmenu->Owner(this);
}
void Menu::_SetItemFlags(unsigned Index, uint16_t Mask, uint16_t Flags) {
	auto &Item = ItemArray[Index];
	Item.Flags &= ~Mask;
	Item.Flags |= Flags;
}
int Menu::_FindItem(uint16_t ItemId, Menu **ppMenu) {
	int ItemIndex = -1;
	auto NumItems = this->NumItems();
	for (int i = 0; i < NumItems && ItemIndex < 0; ++i) {
		auto &Item = ItemArray[i];
		if (Item.Id == ItemId) {
			*ppMenu = this;
			ItemIndex = i;
		}
		else if (Item.pSubmenu)
			ItemIndex = Item.pSubmenu->_FindItem(ItemId, ppMenu);
	}
	return ItemIndex;
}
size_t Menu::_SendMenuMessage(WObj *pDestWin, MENU_MSGID MsgType, uint16_t ItemId) {
	if (!pDestWin)
		pDestWin = Parent();
	MENU_MSG_DATA MsgData{ MsgType, ItemId };
	if (pDestWin)
		return pDestWin->_SendMessage(WM_MENU, &MsgData, this);
	return 0;
}

Menu::Menu(int x0, int y0, int xsize, int ysize,
		   WObj *pParent, uint16_t Id,
		   WM_CF Flags, uint16_t ExFlags) :
	Widget(x0, y0, xsize, ysize,
		   _Callback,
		   pParent, Id,
		   Flags | WC_VISIBLE | WC_STAYONTOP | WC_FOCUSSABLE),
	Width(xsize > 0 ? xsize : 0), Height(ysize > 0 ? ysize : 0),
	Flags(ExFlags) {
	if (this->Flags & MENU_CF_OPEN_ON_POINTEROVER)
		this->Flags |= MENU_CF_ACTIVE;
	else
		this->Flags &= ~MENU_CF_ACTIVE;
}

void Menu::AddItem(const Menu::Item &Item) {
	auto &&NewItem = ItemArray.Add();
	NewItem.Flags = Item.Flags;
	NewItem.Id = Item.Id;
	NewItem.pSubmenu = NewItem.pSubmenu;
	NewItem.text = Item.text;
	NewItem.TextWidth = Item.TextWidth;
	_SetItem(NumItems() - 1, Item);
	_ResizeMenu();
}
void Menu::SetItem(uint16_t ItemId, const Menu::Item &Item) {
	Menu *pMenu;
	auto Index = _FindItem(ItemId, &pMenu);
	if (Index < 0)
		return;
	_SetItem(Index, Item);
	_ResizeMenu();
}

void Menu::DeleteItem(uint16_t ItemId) {
	Menu *pMenu;
	auto Index = _FindItem(ItemId, &pMenu);
	if (Index < 0)
		return;
	pMenu->ItemArray.Delete(Index);
	pMenu->_ResizeMenu();
}
void Menu::DisableItem(uint16_t ItemId) {
	Menu *pMenu;
	auto Index = _FindItem(ItemId, &pMenu);
	if (Index < 0)
		return;
	_SetItemFlags(Index, MENU_IF_DISABLED, MENU_IF_DISABLED);
	_InvalidateItem(Index);
}
void Menu::EnableItem(uint16_t ItemId) {
	Menu *pMenu;
	auto Index = _FindItem(ItemId, &pMenu);
	if (Index < 0)
		return;
	_SetItemFlags(Index, MENU_IF_DISABLED, 0);
	_InvalidateItem(Index);
}
void Menu::InsertItem(uint16_t ItemId, const Menu::Item &Item) {
	Menu *pMenu;
	auto Index = _FindItem(ItemId, &pMenu);
	if (Index < 0)
		return;
	auto &NewItem = ItemArray.Insert(Index);
	NewItem.Flags = Item.Flags;
	NewItem.Id = Item.Id;
	NewItem.pSubmenu = Item.pSubmenu;
	NewItem.text = Item.text;
	NewItem.TextWidth = Item.TextWidth;
	_SetItem(Index, Item);
	_ResizeMenu();
}
void Menu::Attach(WObj *pDestWin, Point Pos, Point Size) {
	Width = Size.x > 0 ? Size.x : 0;
	Height = Size.y > 0 ? Size.y : 0;
	Parent(pDestWin, Pos);
	_ResizeMenu();
}
void Menu::Popup(Menu *pParentMenu, Point Pos, Point Size, uint16_t Flags) {
	if (!pParentMenu)
		return;
	Flags |= MENU_CF_POPUP;
	Width = Size.x > 0 ? Size.x : 0;
	Height = Size.y > 0 ? Size.y : 0;
	Owner(pParentMenu);
	Parent(Desktop(), Pos + pParentMenu->Position());
	pParentMenu->_SendMenuMessage(this, MENU_ON_OPEN);
}
