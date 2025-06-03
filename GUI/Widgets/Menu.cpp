#include "Menu.h"

Menu::Property Menu::DefaultProps;

int Menu::_SendMenuMessage(WObj *pDest, MENU_MSGID MsgType, uint16_t ItemId) {
	if (!pDest)
		pDest = Parent();
	if (pDest)
		return pDest->SendMessage(WM_MENU, MSG_DAT{ MsgType, ItemId }, this);
	return 0;
}
bool Menu::_IsTopLevelMenu() {
	return !_SendMenuMessage(Owner(), MENU_IS_MENU, 0);
}
bool Menu::_HasEffect() {
	if (!(StatusEx & MENU_CF_POPUP))
		if (_IsTopLevelMenu())
			return false;
	return true;
}
int Menu::_GetEffectSize() {
	if (_HasEffect())
		return EffectSize();
	return 0;
}
int Menu::_GetItemWidth(uint16_t Index) {
	if (Width && (StatusEx & MENU_CF_VERTICAL))
		return Width - (_GetEffectSize() << 1);
	auto &Item = ItemArray[Index];
	auto Border = Props.aBorder[MENU_BI_LEFT] + Props.aBorder[MENU_BI_RIGHT];
	if ((StatusEx & MENU_CF_VERTICAL) || !(Item.Flags & MENU_IF_SEPARATOR))
		return Border + Item.Size.x;
	return Border + 3;
}
int Menu::_GetItemHeight(uint16_t Index) {
	if (Height && !(StatusEx & MENU_CF_VERTICAL))
		return Height - (_GetEffectSize() << 1);
	auto &Item = ItemArray[Index];
	auto Border = Props.aBorder[MENU_BI_TOP] + Props.aBorder[MENU_BI_BOTTOM];
	if ((StatusEx & MENU_CF_VERTICAL) && (Item.Flags & MENU_IF_SEPARATOR))
		return Border + 3;
	return Border + Item.Size.y;
}
int Menu::_CalcMenuSizeX() {
	unsigned i, NumItems = this->NumItems();
	int xSize = 0;
	if (StatusEx & MENU_CF_VERTICAL) {
		int ItemWidth;
		for (i = 0; i < NumItems; i++) {
			ItemWidth = _GetItemWidth(i);
			if (ItemWidth > xSize)
				xSize = ItemWidth;
		}
	}
	else for (i = 0; i < NumItems; i++)
		xSize += _GetItemWidth(i);
	xSize += (_GetEffectSize() << 1);
	return xSize;
}
int Menu::_CalcMenuSizeY() {
	auto NumItems = this->NumItems();
	int ySize = 0;
	if (StatusEx & MENU_CF_VERTICAL) {
		for (int i = 0; i < NumItems; i++)
			ySize += _GetItemHeight(i);
	}
	else {
		int ItemHeight;
		for (int i = 0; i < NumItems; i++) {
			ItemHeight = _GetItemHeight(i);
			if (ySize < ItemHeight)
				ySize = ItemHeight;
		}
	}
	ySize += (_GetEffectSize() << 1);
	return ySize;
}
int Menu::_CalcWindowSizeX() {
	int xSize = Width;
	if (xSize == 0) {
		xSize = _CalcMenuSizeX();
	}
	return xSize;
}
int Menu::_CalcWindowSizeY() {
	int ySize = Height;
	if (ySize == 0)
		ySize = _CalcMenuSizeY();
	return ySize;
}
int Menu::_GetItemFromPos(Point Pos) {
	int xSize, ySize, EffectSize, r = -1;
	ySize = _CalcMenuSizeY();
	if (Height && Height < ySize)
		ySize = Height;
	xSize = _CalcMenuSizeX();
	if (Width && Width < xSize)
		xSize = Width;
	EffectSize = _GetEffectSize();
	Pos -= EffectSize;
	xSize -= (EffectSize << 1);
	ySize -= (EffectSize << 1);
	if ((Pos.x >= 0) && (Pos.y >= 0) && (Pos.x < xSize) && (Pos.y < ySize)) {
		auto NumItems = this->NumItems();
		if (StatusEx & MENU_CF_VERTICAL) {
			int yPos = 0;
			for (int i = 0; i < NumItems; i++) {
				yPos += _GetItemHeight(i);
				if (Pos.y < yPos) {
					r = i;
					break;
				}
			}
		}
		else {
			int xPos = 0;
			for (int i = 0; i < NumItems; i++) {
				xPos += _GetItemWidth(i);
				if (Pos.x < xPos) {
					r = i;
					break;
				}
			}
		}
	}
	return r;
}
Point Menu::_GetItemPos(uint16_t Index) {
	auto EffectSize = _GetEffectSize();
	if (StatusEx & MENU_CF_VERTICAL) {
		int yPos = 0;
		for (int i = 0; i < Index; ++i)
			yPos += _GetItemHeight(i);
		return { EffectSize, EffectSize + yPos };
	}
	else {
		int xPos = 0;
		for (int i = 0; i < Index; ++i)
			xPos += _GetItemWidth(i);
		return { EffectSize + xPos, EffectSize };
	}
}
void Menu::_SetCapture() {
	if (!bSubmenuActive)
		if (!Captured())
			Capture(false);
}
void Menu::_ReleaseCapture() {
	if (Captured())
		if (_IsTopLevelMenu() && !(StatusEx & MENU_CF_POPUP))
			CaptureRelease();
}
void Menu::_CloseSubmenu() {
	if (StatusEx & MENU_CF__ACTIVE) {
		if (bSubmenuActive) {
			auto &Item = ItemArray[Sel];
			_SendMenuMessage(Item.pSubmenu, MENU_ON_CLOSE, 0);
			Item.pSubmenu->Visible(false);
			bSubmenuActive = false;
			_SetCapture();
			_InvalidateItem(Sel);
		}
	}
}
void Menu::_OpenSubmenu(uint16_t Index) {
	if (StatusEx & MENU_CF__ACTIVE) {
		auto PrevActiveSubmenu = bSubmenuActive;
		_CloseSubmenu();
		auto &Item = ItemArray[Index];
		if (Item.pSubmenu) {
			if (!(Item.Flags & MENU_IF_DISABLED)) {
				auto Pos =  _GetItemPos(Index);
				auto EffectSize = _GetEffectSize();
				if (StatusEx & MENU_CF_VERTICAL) {
					Pos.x += _CalcMenuSizeX() - (_GetEffectSize() << 1);
					Pos.y -= EffectSize;
				}
				else {
					Pos.y += _CalcMenuSizeY() - (_GetEffectSize() << 1);
					Pos.x -= EffectSize;
				}
				Pos += PositionScreen();
				if (!PrevActiveSubmenu)
					if (_IsTopLevelMenu())
						_SendMenuMessage(Owner(), MENU_ON_INITMENU, 0);
				_SendMenuMessage(Owner(), MENU_ON_INITSUBMENU, Item.Id);
				Item.pSubmenu->Popup(this, Pos);
				Item.pSubmenu->Visible(true);
				_SendMenuMessage(Item.pSubmenu, MENU_ON_OPEN, 0);
				bSubmenuActive = true;
				_InvalidateItem(Index);
			}
		}
	}
}
void Menu::_ClosePopup() {
	if (StatusEx & MENU_CF_POPUP) {
		StatusEx &= ~MENU_CF_POPUP;
		Visible(false);
		CaptureRelease();
	}
}
void Menu::_SetSelection(int16_t Index) {
	if (Index != Sel) {
		_InvalidateItem(Sel);
		_InvalidateItem(Index);
		Sel = Index;
	}
}
void Menu::_SelectItem(uint16_t Index) {
	if (Sel != Index) {
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
void Menu::_ActivateItem(uint16_t Index) {
	auto&Item = ItemArray[Index];
	if (Item.pSubmenu) return;
	if (Item.Flags & (MENU_IF_DISABLED | MENU_IF_SEPARATOR))
		return;
	_ClosePopup();
	_SendMenuMessage(Owner(), MENU_ON_ITEMSELECT, Id);
}
void Menu::_ActivateMenu(uint16_t Index) {
	if (!(StatusEx & MENU_CF_OPEN_ON_POINTEROVER)) {
		auto&Item = ItemArray[Index];
		if (Item.pSubmenu) {
			if (!(Item.Flags & MENU_IF_DISABLED)) {
				if (!(StatusEx & MENU_CF__ACTIVE)) {
					StatusEx |= MENU_CF__ACTIVE;
					_OpenSubmenu(Index);
					_SetSelection(Index);
				}
				else if (StatusEx & MENU_CF_CLOSE_ON_SECOND_CLICK) {
					if (Index == Sel) {
						_CloseSubmenu();
						StatusEx &= ~MENU_CF__ACTIVE;
					}
				}
			}
		}
	}
}
void Menu::_DeactivateMenu() {
	_CloseSubmenu();
	if (!(StatusEx & MENU_CF_OPEN_ON_POINTEROVER))
		StatusEx &= ~MENU_CF__ACTIVE;
}
bool Menu::_ForwardMouseOverMsg(Point Pos) {
	if (!bSubmenuActive && !(StatusEx & MENU_CF_POPUP)) {
		if (_IsTopLevelMenu()) {
			Pos = Client2Screen(Pos);
			auto pBelow = FindOnScreen(Pos);
			if (pBelow && pBelow != this) {
				Pos = pBelow->Screen2Client(Pos);
				pBelow->SendMessage(WM_MOUSE_OVER, MOUSE_STATE{ Pos, 0 });
				return true;
			}
		}
	}
	return false;
}
bool Menu::_HandleMouse(MOUSE_STATE State) {
	MOUSE_STATE PrevState = GUI.MousePrev();
	char XYInWidget = 0;
	if (State.x >= 0 && State.y >= 0) {
		auto &&r = ClientRect();
		if (State.x <= r.x1 && State.y <= r.y1)
			XYInWidget = 1;
	}
	if (XYInWidget) {
		int ItemIndex;
		ItemIndex = _GetItemFromPos(State);
		if (ItemIndex >= 0) {
			if (State.Pressed == 1) {
				if (PrevState.Pressed == 0)
					_ActivateMenu(ItemIndex);
				_SelectItem(ItemIndex);
			}
			else if (State.Pressed == 0 && PrevState.Pressed == 1)
				_ActivateItem(ItemIndex);
			else if (State.Pressed < 0) {
				if (!_ForwardMouseOverMsg(State))
					_SelectItem(ItemIndex);
				else
					_DeselectItem();
			}
		}
		else {
			if (State.Pressed == 1) {
				if (PrevState.Pressed == 0)
					_DeactivateMenu();
				_DeselectItem();
			}
			else if (State.Pressed < 0)
				_DeselectItem();
		}
		return false;
	}
	else {
		if (State.Pressed == 1 && PrevState.Pressed == 0) {
			_DeactivateMenu();
			_ClosePopup();
		}
		_DeselectItem();
		_ForwardMouseOverMsg(State);
	}
	return true;
}
void Menu::_ForwardMouseMsgToOwner(uint16_t MsgId, MOUSE_STATE State) {
	if (_IsTopLevelMenu())
		return;
	(Point &)State = Owner()->Screen2Client(Client2Screen(State));
	SendOwnerMessage(MsgId, State);
}
WM_RESULT Menu::_OnMenu(MSG_DAT MsgDat) {
	switch (MsgDat.MsgType) {
		case MENU_ON_ITEMSELECT:
			_DeactivateMenu();
			_DeselectItem();
			_ClosePopup();
		case MENU_ON_INITMENU:
		case MENU_ON_INITSUBMENU: 
			SendOwnerMessage(WM_MENU, MsgDat);
			break;
		case MENU_ON_OPEN:
			Sel = -1;
			bSubmenuActive = false;
			StatusEx |= MENU_CF__ACTIVE | MENU_CF_OPEN_ON_POINTEROVER;
			_SetCapture();
			_ResizeMenu();
			break;
		case MENU_ON_CLOSE:
			_CloseSubmenu();
			break;
		case MENU_IS_MENU:
			return 1;
	}
	return 0;
}
bool Menu::_OnMouse(MOUSE_STATE State) {
	if (State)
		return _HandleMouse(State);
	return _HandleMouse({ -1, -1 });
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
			if (StatusEx & MENU_CF_HIDE_DISABLED_SEL)
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
	auto EffectSize = this->EffectSize();
	auto NumItems = this->NumItems();
	auto &&rFill = WObj::ClientRect() / EffectSize;
	GUI.Font(Props.pFont);
	if (StatusEx & MENU_CF_VERTICAL) {
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
				rText.x1 = rText.x0 + Item.Size.x - 1;
				rText.y0 = rFill.y0 + BorderTop;
				rText.y1 = rText.y0 + FontHeight - 1;
				GUI.Clear(rFill);
				GUI.DrawStringIn(Item.Text, rText);
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
				rText.x1 = rText.x0 + Item.Size.x - 1;
				GUI.Clear(rFill);
				GUI.DrawStringIn(Item.Text, rText);
			}
			rFill.x0 += ItemWidth;
		}
	}
	if (Width | Height) {
		auto &&r = WObj::ClientRect() / EffectSize;
		GUI.BkColor(Props.aBkColor[MENU_CI_ENABLED]);
		GUI.Clear({ rFill.x1 + 1, EffectSize, r.x1, rFill.y1 });
		GUI.Clear({ EffectSize, rFill.y1 + 1, r.x1, r.y1 });
	}
	if (_HasEffect())
		DrawUp();
}
WM_RESULT Menu::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (Menu *)pWin;
	if (MsgId != WM_MOUSE_CHANGED)
		if (!pObj->HandleActive(MsgId, Param))
			return Param;
	switch (MsgId) {
		case WM_PAINT:
			pObj->_OnPaint();
			return 0;
		case WM_MOUSE:
			if (pObj->_OnMouse(Param))
				pObj->_ForwardMouseMsgToOwner(MsgId, Param);
			return 0;
		case WM_MOUSE_OVER:
			if (pObj->_HandleMouse({ Param, -1 }))
				pObj->_ForwardMouseMsgToOwner(MsgId, Param);
			return 0;
		case WM_DELETE:
			pObj->~Menu();
			return 0;
		case WM_MENU:
			return pObj->_OnMenu(Param);
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}
void Menu::_InvalidateItem(int16_t Index) {
	(void)Index;
	Invalidate();
}
void Menu::_RecalcTextWidthOfItems() {
	auto NumItems = this->NumItems();
	for (int i = 0; i < NumItems; i++) {
		auto&Item = ItemArray[i];
		Item.Size = Props.pFont->Size(Item.Text);
	}
}
void Menu::_ResizeMenu() {
	Size({ _CalcWindowSizeX(), _CalcWindowSizeY() });
	Invalidate();
}
void Menu::_SetItem(uint16_t Index, Item &Item) {
	auto &&NewItem = ItemArray[Index];
	NewItem = Item;
	NewItem.Size = Props.pFont->Size(NewItem.Text);
	if (NewItem.Flags & MENU_IF_SEPARATOR)
		NewItem.pSubmenu = nullptr;
	else if (NewItem.pSubmenu)
		NewItem.pSubmenu->Owner(this);
}
Menu::Menu(const SRect &rc,
		   PWObj pParent, uint16_t Id,
		   WM_CF Flags, MENU_CF FlagsEx) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | (WC_STAYONTOP | WC_FOCUSSABLE) | (FlagsEx & MENU_CF_POPUP ? WC_POPUP : WC_HIDE),
		   FlagsEx),
	Width(rc.x0 <= rc.x1 ? rc.xsize() : 0), Height(rc.y0 <= rc.y1 ? rc.ysize() : 0) {
	Effect(Status & WC_POPUP ? &EffectItf::D3L1 : nullptr);
	if (StatusEx & MENU_CF_OPEN_ON_POINTEROVER)
		StatusEx |= MENU_CF__ACTIVE;
	else
		StatusEx &= ~MENU_CF__ACTIVE;
}
void Menu::Add(Item MenuItem) {
	ItemArray.Add();
	_SetItem(NumItems() - 1, MenuItem);
	_ResizeMenu();
}
void Menu::Popup(Menu *pOwnerMenu, Point Pos) {
	if (pOwnerMenu) {
		StatusEx |= MENU_CF_POPUP;
		Owner(pOwnerMenu);
		Position(Pos);
		pOwnerMenu->_SendMenuMessage(this, MENU_ON_OPEN, 0);
	}
}
void Menu::Attach(PWObj pOwner, Point Pos, Point Size) {
	Width = Size.x > 0 ? Size.x : 0;
	Height = Size.y > 0 ? Size.y : 0;
	WObj::Owner(nullptr);
	Parent(pOwner, Pos);
	_ResizeMenu();
}
