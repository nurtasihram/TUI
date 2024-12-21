#include "Menu.h"

#define MENU_CF_ACTIVE (1<<6)  /* Internal flag only */
#define MENU_CF_POPUP  (1<<7)  /* Internal flag only */

Menu::Property Menu::DefaultProps;

auto MENU__pDefaultEffect = &Widget::EffectItf::D3L1;

static char _IsTopLevelMenu(Menu *pObj) {
	if (MENU__SendMenuMessage(pObj, pObj->pOwner, MENU_IS_MENU, 0) == 0)
		return 1;
	return 0;
}
static int _HasEffect(Menu *pObj) {
	if (!(pObj->Flags & MENU_CF_POPUP))
		if (_IsTopLevelMenu(pObj))
			return 0;
	return 1;
}
static int _GetEffectSize(Menu *pObj) {
//	if (_HasEffect(pObj))
//		return pObj->EffectSize();
	return 0;
}
static int _CalcTextWidth(Menu *pObj, const char *sText) {
	if (!sText)
		return 0;
	auto pOldFont = GUI.Font();
	GUI.Font(pObj->Props.pFont);
	int TextWidth = GUI_GetStringDistX(sText);
	GUI.Font(pOldFont);
	return TextWidth;
}
static int _GetItemWidth(Menu *pObj, unsigned Index) {
	if (pObj->Width && (pObj->Flags & MENU_CF_VERTICAL))
		return pObj->Width - (_GetEffectSize(pObj) << 1);
	auto &item = pObj->ItemArray[Index];
	int ItemWidth = (pObj->Flags & MENU_CF_VERTICAL) || !(item.Flags & MENU_IF_SEPARATOR) ?
		item.TextWidth : 3;
	return ItemWidth + pObj->Props.aBorder[MENU_BI_LEFT] + pObj->Props.aBorder[MENU_BI_RIGHT];
}
static int _GetItemHeight(Menu *pObj, unsigned Index) {
	if (pObj->Height && !(pObj->Flags & MENU_CF_VERTICAL))
		return pObj->Height - (_GetEffectSize(pObj) << 1);
	int ItemHeight = pObj->Props.pFont->YDist;
	if (pObj->Flags & MENU_CF_VERTICAL) {
		auto &item = pObj->ItemArray[Index];
		if (item.Flags & MENU_IF_SEPARATOR)
			ItemHeight = 3;
	}
	return ItemHeight + pObj->Props.aBorder[MENU_BI_TOP] + pObj->Props.aBorder[MENU_BI_BOTTOM];
}
static int _CalcMenuSizeX(Menu *pObj) {
	unsigned i, NumItems = pObj->ItemArray.NumItems();
	int xSize = 0;
	if (pObj->Flags & MENU_CF_VERTICAL) {
		for (i = 0; i < NumItems; ++i) {
			int ItemWidth = _GetItemWidth(pObj, i);
			if (ItemWidth > xSize)
				xSize = ItemWidth;
		}
	}
	else for (i = 0; i < NumItems; ++i)
		xSize += _GetItemWidth(pObj, i);
	return xSize + (_GetEffectSize(pObj) << 1);
}
static int _CalcMenuSizeY(Menu *pObj) {
	unsigned i, NumItems = pObj->ItemArray.NumItems();
	int ySize = 0;
	if (pObj->Flags & MENU_CF_VERTICAL)
		for (i = 0; i < NumItems; ++i)
			ySize += _GetItemHeight(pObj, i);
	else for (i = 0; i < NumItems; ++i) {
		int ItemHeight = _GetItemHeight(pObj, i);
		if (ItemHeight > ySize)
			ySize = ItemHeight;
	}
	return ySize + (_GetEffectSize(pObj) << 1);
}
static int _CalcWindowSizeX(Menu *pObj) {
	int xSize = pObj->Width;
	if (xSize)
		return xSize;
	return _CalcMenuSizeX(pObj);
}
static int _CalcWindowSizeY(Menu *pObj) {
	int ySize = pObj->Height;
	if (ySize == 0)
		return ySize;
	return _CalcMenuSizeY(pObj);
}
static int _GetItemFromPos(Menu *pObj, Point Pos) {
	Point Size = { _CalcMenuSizeX(pObj), _CalcMenuSizeY(pObj) };
	if (pObj->Width && pObj->Width < Size.x)
		Size.x = pObj->Width;
	if (pObj->Height && pObj->Height < Size.y)
		Size.y = pObj->Height;
	auto EffectSize = _GetEffectSize(pObj);
	Pos -= EffectSize;
	Size -= (EffectSize << 1);
	if (Pos.x < 0 || Pos.y < 0)
		return -1;
	if (Pos.x >= Size.x || Pos.y >= Size.y)
		return -1;
	auto NumItems = pObj->ItemArray.NumItems();
	int pos = 0;
	if (pObj->Flags & MENU_CF_VERTICAL)
		for (unsigned i = 0; i < NumItems; ++i) {
			pos += _GetItemHeight(pObj, i);
			if (Pos.y < pos)
				return i;
		}
	else for (unsigned i = 0; i < NumItems; ++i) {
		pos += _GetItemWidth(pObj, i);
		if (Pos.x < pos)
			return i;
	}
	return -1;
}
static Point _GetItemPos(Menu *pObj, unsigned Index) {
	Point Pos = _GetEffectSize(pObj);
	if (pObj->Flags & MENU_CF_VERTICAL)
		for (int i = 0; i < (int)Index; ++i)
			Pos.y += _GetItemHeight(pObj, i);
	else
		for (int i = 0; i < (int)Index; ++i)
			Pos.x += _GetItemWidth(pObj, i);
	return Pos;
}
static void _SetCapture(Menu *pObj) {
	if (!pObj->bSubmenuActive)
		if (!pObj->Captured())
			pObj->Capture(false);
}
static void _ReleaseCapture(Menu *pObj) {
	if (pObj->Captured())
		if (_IsTopLevelMenu(pObj) && !(pObj->Flags & MENU_CF_POPUP))
			WObj::CaptureRelease();
}
static void _CloseSubmenu(Menu *pObj) {
	if (!(pObj->Flags & MENU_CF_ACTIVE))
		return;
	if (!pObj->bSubmenuActive)
		return;
	auto &item = pObj->ItemArray[pObj->Sel];
	MENU__SendMenuMessage(pObj, item.pSubmenu, MENU_ON_CLOSE, 0);
	item.pSubmenu->Detach();
	pObj->bSubmenuActive = false;
	_SetCapture(pObj);
	MENU__InvalidateItem(pObj, pObj->Sel);
}
static void _OpenSubmenu(Menu *pObj, unsigned Index) {
	if (!(pObj->Flags & MENU_CF_ACTIVE))
		return;
	auto PrevActiveSubmenu = pObj->bSubmenuActive;
	auto &item = pObj->ItemArray[Index];
	_CloseSubmenu(pObj);
	if (!item.pSubmenu)
		return;
	if (item.Flags & MENU_IF_DISABLED)
		return;
	auto EffectSize = _GetEffectSize(pObj);
	auto &&Pos = _GetItemPos(pObj, Index);
	if (pObj->Flags & MENU_CF_VERTICAL) {
		Pos.x += _CalcMenuSizeX(pObj) - (_GetEffectSize(pObj) << 1);
		Pos.y -= EffectSize;
	}
	else {
		Pos.y += _CalcMenuSizeY(pObj) - (_GetEffectSize(pObj) << 1);
		Pos.x -= EffectSize;
	}
	Pos += pObj->Position();
	if (PrevActiveSubmenu == 0)
		if (_IsTopLevelMenu(pObj))
			MENU__SendMenuMessage(pObj, pObj->pOwner, MENU_ON_INITMENU, 0);
	MENU__SendMenuMessage(pObj, pObj->pOwner, MENU_ON_INITSUBMENU, item.Id);
	MENU_SetOwner(item.pSubmenu, pObj);
	item.pSubmenu->Parent(WObj::Desktop(), Pos);
	MENU__SendMenuMessage(pObj, item.pSubmenu, MENU_ON_OPEN, 0);
	pObj->bSubmenuActive = true;
	MENU__InvalidateItem(pObj, Index);
}
static void _ClosePopup(Menu *pObj) {
	if (!(pObj->Flags & MENU_CF_POPUP))
		return;
	pObj->Flags &= ~(MENU_CF_POPUP);
	pObj->Detach();
	WObj::CaptureRelease();
}
static void _SetSelection(Menu *pObj, int Index) {
	if (Index == pObj->Sel)
		return;
	MENU__InvalidateItem(pObj, pObj->Sel); /* Invalidate previous selection */
	MENU__InvalidateItem(pObj, Index);     /* Invalidate new selection */
	pObj->Sel = Index;
}
static void _SelectItem(Menu *pObj, unsigned Index) {
	if (pObj->Sel == (int)Index)
		return;
	_SetCapture(pObj);
	_OpenSubmenu(pObj, Index);
	_SetSelection(pObj, Index);
}
static void _DeselectItem(Menu *pObj) {
	if (!pObj->bSubmenuActive)
		return;
	_SetSelection(pObj, -1);
	_ReleaseCapture(pObj);
}
static void _ActivateItem(Menu *pObj, unsigned Index) {
	auto &item = pObj->ItemArray[Index];
	if (item .pSubmenu)
		return;
	if (item.Flags & (MENU_IF_DISABLED | MENU_IF_SEPARATOR))
		return;
	_ClosePopup(pObj);
	MENU__SendMenuMessage(pObj, pObj->pOwner, MENU_ON_ITEMSELECT, item.Id);
}
static void _ActivateMenu(Menu *pObj, unsigned Index) {
	if (pObj->Flags & MENU_CF_OPEN_ON_POINTEROVER)
		return;
	auto &item = pObj->ItemArray[Index];
	if (!item.pSubmenu)
		return;
	if (item.Flags & MENU_IF_DISABLED)
		return;
	if ((pObj->Flags & MENU_CF_ACTIVE) == 0) {
		pObj->Flags |= MENU_CF_ACTIVE;
		_OpenSubmenu(pObj, Index);
		_SetSelection(pObj, Index);
	}
	else if (pObj->Flags & MENU_CF_CLOSE_ON_SECOND_CLICK) {
		if ((int)Index == pObj->Sel) {
			_CloseSubmenu(pObj);
			pObj->Flags &= ~MENU_CF_ACTIVE;
		}
	}
}
static void _DeactivateMenu(Menu *pObj) {
	_CloseSubmenu(pObj);
	if (pObj->Flags & MENU_CF_OPEN_ON_POINTEROVER)
		return;
	pObj->Flags &= ~MENU_CF_ACTIVE;
}
static bool _ForwardMouseOverMsg(Menu *pObj, Point Pos) {
	if (pObj->bSubmenuActive || (pObj->Flags & MENU_CF_POPUP))
		return false;
	if (!_IsTopLevelMenu(pObj))
		return false;
	Pos += pObj->Position();
	auto pBelow = WObj::ScreenToWin(Pos);
	if (!pBelow || pBelow == pObj)
		return 0;
	PID_STATE State = Pos - pBelow->Position();
	WM_PARAM data;
	data = (WM_PARAM)&State;
	pBelow->SendMessage(WM_MOUSEOVER, &data);
	return 1;
}
static bool _HandlePID(Menu *pObj, const PID_STATE &pid) {
	PID_STATE PrevState;
	WM_PID__GetPrevState(&PrevState);
	bool XYInWidget = false;
	if (pid.x >= 0 && pid.y >= 0) {
		auto &&r = pObj->ClientRect();
		XYInWidget = pid.x <= r.x1 && pid.y <= r.y1;
	}
	if (!XYInWidget) {
		if (pid.Pressed == 1 && PrevState.Pressed == 0) {
			_DeactivateMenu(pObj);
			_ClosePopup(pObj);
		}
		_DeselectItem(pObj);
		_ForwardMouseOverMsg(pObj, pid);
		return true;
	}
	int ItemIndex = _GetItemFromPos(pObj, pid);
	if (ItemIndex >= 0) {
		if (pid.Pressed == 1) {
			if (PrevState.Pressed == 0)
				_ActivateMenu(pObj, ItemIndex);
			_SelectItem(pObj, ItemIndex);
		}
		else if (pid.Pressed == 0 && PrevState.Pressed == 1)
			_ActivateItem(pObj, ItemIndex);
		else if (pid.Pressed < 0) {  /* Mouse moved */
			if (!_ForwardMouseOverMsg(pObj, pid))
				_SelectItem(pObj, ItemIndex);
			else
				_DeselectItem(pObj);
		}
	}
	else if (pid.Pressed == 1) {
		if (PrevState.Pressed == 0)
			_DeactivateMenu(pObj);
		_DeselectItem(pObj);
	}
	else if (pid.Pressed < 0)
		_DeselectItem(pObj);
	return false;
}
static void _ForwardPIDMsgToOwner(Menu *pObj, WM_PARAM *pData) {
	if (_IsTopLevelMenu(pObj))
		return;
	if (auto pOwner = pObj->pOwner ? pObj->pOwner : pObj->Parent()) {
		if (auto pState = (PID_STATE *)*pData)
			*pState += pObj->Position() - pOwner->Position();
//		pOwner->SendMessage(pData);
	}
}
static void _OnMenu(Menu *pObj, WM_PARAM *pData) {
	auto pMsgData = (const MENU_MSG_DATA *)*pData;
	if (!pMsgData)
		return;
	switch (pMsgData->MsgType) {
	case MENU_ON_ITEMSELECT:
		_DeactivateMenu(pObj);
		_DeselectItem(pObj);
		_ClosePopup(pObj);
	case MENU_ON_INITMENU:
	case MENU_ON_INITSUBMENU:
		if (auto pOwner = pObj->pOwner ?
			pObj->pOwner : pObj->Parent()) {
//			pData->pWinSrc = pObj;
//			pOwner->SendMessage(pData);
		}
		break;
	case MENU_ON_OPEN:
		pObj->Sel = -1;
		pObj->bSubmenuActive = false;
		pObj->Flags |= MENU_CF_ACTIVE | MENU_CF_OPEN_ON_POINTEROVER;
		_SetCapture(pObj);
		MENU__ResizeMenu(pObj);
		break;
	case MENU_ON_CLOSE:
		_CloseSubmenu(pObj);
		break;
	case MENU_IS_MENU:
		*pData = 1;
		break;
	}
}
static bool _OnTouch(Menu *pObj, WM_PARAM *pData) {
	if (auto pState = (const PID_STATE *)*pData)
		return _HandlePID(pObj, *pState);
	return _HandlePID(pObj, { -1, -1 });
}
static bool _OnMouseOver(Menu *pObj, WM_PARAM *pData) {
	if (auto pState = (const PID_STATE *)*pData)
		return _HandlePID(pObj, { *pState, -1 });
	return false;
}
static void _SetPaintColors(const Menu *pObj, const Menu::Item *pItem, int ItemIndex) {
	unsigned ColorIndex;
	char Selected = (ItemIndex == pObj->Sel) ? 1 : 0;
	if (pObj->bSubmenuActive && Selected)
		ColorIndex = MENU_CI_ACTIVE_SUBMENU;
	else if (pItem->Flags & MENU_IF_SEPARATOR)
		ColorIndex = MENU_CI_ENABLED;
	else {
		ColorIndex = Selected ? MENU_CI_SELECTED : MENU_CI_ENABLED;
		if (pItem->Flags & MENU_IF_DISABLED) {
			if (pObj->Flags & MENU_CF_HIDE_DISABLED_SEL)
				ColorIndex = MENU_CI_DISABLED;
			else
				ColorIndex += MENU_CI_DISABLED;
		}
	}
	GUI.BkColor(pObj->Props.aBkColor[ColorIndex]);
	GUI.PenColor(pObj->Props.aTextColor[ColorIndex]);
}
//static void _OnPaint(Menu *pObj) {
//	SRect rText;
//	uint8_t BorderLeft = pObj->Props.aBorder[MENU_BI_LEFT];
//	uint8_t BorderTop = pObj->Props.aBorder[MENU_BI_TOP];
//	int FontHeight = GUI_GetYDistOfFont(pObj->Props.pFont);
//	int EffectSize = _GetEffectSize(pObj);
//	auto NumItems = pObj->ItemArray.NumItems();
//	auto &&rFill = pObj->ClientRect() / EffectSize;
//	GUI.Font(pObj->Props.pFont);
//	if (pObj->Flags & MENU_CF_VERTICAL) {
//		int ItemHeight, xSize;
//		xSize = _CalcMenuSizeX(pObj);
//		rFill.x1 = xSize - EffectSize - 1;
//		rText.x0 = rFill.x0 + BorderLeft;
//		for (int i = 0; i < NumItems; ++i) {
//			auto &item = pObj->ItemArray[i];
//			ItemHeight = _GetItemHeight(pObj, i);
//			_SetPaintColors(pObj, &item, i);
//			rFill.y1 = rFill.y0 + ItemHeight - 1;
//			if (item.Flags & MENU_IF_SEPARATOR) {
//				GUI.Clear(rFill);
//				GUI.PenColor(RGBC_GRAY(0x7C));
//				GUI.DrawLineH(rFill.y0 + BorderTop + 1, rFill.x0 + 2, rFill.x1 - 2);
//			}
//			else {
//				rText.x1 = rText.x0 + item.TextWidth - 1;
//				rText.y0 = rFill.y0 + BorderTop;
//				rText.y1 = rText.y0 + FontHeight - 1;
//				WIDGET__FillStringInRect(item.pText, rFill, rText, rText);
//			}
//			rFill.y0 += ItemHeight;
//		}
//	}
//	else {
//		auto ySize = _CalcMenuSizeY(pObj);
//		rFill.y1 = ySize - EffectSize - 1;
//		rText.y0 = rFill.y0 + BorderTop;
//		rText.y1 = rText.y0 + FontHeight - 1;
//		for (int i = 0; i < NumItems; ++i) {
//			auto &item = pObj->ItemArray[i];
//			auto ItemWidth = _GetItemWidth(pObj, i);
//			_SetPaintColors(pObj, &item, i);
//			rFill.x1 = rFill.x0 + ItemWidth - 1;
//			if (item .Flags & MENU_IF_SEPARATOR) {
//				GUI.Clear(rFill);
//				GUI.PenColor(RGBC_GRAY(0x7C));
//				GUI.DrawLineV(rFill.x0 + BorderLeft + 1, rFill.y0 + 2, rFill.y1 - 2);
//			}
//			else {
//				rText.x0 = rFill.x0 + BorderLeft;
//				rText.x1 = rText.x0 + item.TextWidth - 1;
//				WIDGET__FillStringInRect(item.pText, rFill, rText, rText);
//			}
//			rFill.x0 += ItemWidth;
//		}
//	}
//	if (pObj->Width || pObj->Height) {
//		auto &&r = pObj->ClientRect() / EffectSize;
//		GUI.BkColor(pObj->Props.aBkColor[MENU_CI_ENABLED]);
//		GUI.Clear({ rFill.x1 + 1, EffectSize, r.x1, rFill.y1 });
//		GUI.Clear({ EffectSize, rFill.y1 + 1, r.x1, r.y1 });
//	}
//	if (_HasEffect(pObj))
//		pObj->DrawUp();
//}
//static void _MENU_Callback(int msgid, WM_PARAM *pData) {
//	auto pObj = (Menu *)pWin;
//	if (msgid != WM_PID_STATE_CHANGED)
//		if (!pObj->HandleActive(msgid, pData))
//			return;
//	switch (msgid) {
//	case WM_MENU:
//		_OnMenu(pObj, pData);
//		return;  /* Message handled, do not call DefCallback() here. */
//	case WM_TOUCH:
//		if (_OnTouch(pObj, pData))
//			_ForwardPIDMsgToOwner(pObj, pData);
//		break;
//	case WM_MOUSEOVER:
//		if (_OnMouseOver(pObj, pData))
//			_ForwardPIDMsgToOwner(pObj, pData);
//		break;
//	case WM_PAINT:
//		_OnPaint(pObj);
//		break;
//	case WM_DELETE:
//		pObj->ItemArray.Delete();
//		break; /* No return here ... DefCallback needs to be called */
//	}
//	DefCallback(pObj, msgid, pData, pWinSrc);
//}
//Menu *MENU_CreateEx(int x0, int y0, int xSize, int ySize, WObj *pParent, uint16_t Flags, uint16_t ExFlags, uint16_t Id) {
//	auto pObj = (Menu *)WObj::Create(
//		x0, y0,
//		xSize, ySize,
//		pParent, WC_VISIBLE | WC_STAYONTOP | Flags,
//		&_MENU_Callback, sizeof(Menu) - sizeof(WObj));
//	if (!pObj) 
//		return 0;
//	/* init widget specific variables */
//	pObj->Init(Id, WIDGET_STATE_FOCUSSABLE);
//	/* init member variables */
//	if (ExFlags & MENU_CF_OPEN_ON_POINTEROVER)
//		ExFlags |= MENU_CF_ACTIVE;
//	else
//		ExFlags &= ~(MENU_CF_ACTIVE);
//	pObj->Props = Menu::DefaultProps;
//	pObj->Flags = ExFlags;
//	pObj->Width = ((xSize > 0) ? xSize : 0);
//	pObj->Height = ((ySize > 0) ? ySize : 0);
//	pObj->Sel = -1;
//	pObj->pOwner = 0;
//	pObj->bSubmenuActive = false;
//	pObj->Effect(MENU__pDefaultEffect);
//	return pObj;
//}
void MENU__InvalidateItem(Menu *pObj, unsigned Index) {
	pObj->Invalidate();  /* Can be optimized, no need to invalidate all items */
}
void MENU__RecalcTextWidthOfItems(Menu *pObj) {
	auto pOldFont = GUI.Font();
	GUI.Font(pObj->Props.pFont);
	for (int i = 0, NumItems = pObj->ItemArray.NumItems(); i < NumItems; ++i) {
		auto &item = pObj->ItemArray[i];
		item.TextWidth = GUI_GetStringDistX(item.pText);
	}
	GUI.Font(pOldFont);
}
void MENU__ResizeMenu(Menu *pObj) {
	pObj->Size({ _CalcWindowSizeX(pObj), _CalcWindowSizeY(pObj) });
	pObj->Invalidate();
}
void MENU__SetItem(Menu *pObj, unsigned Index, const Menu::Item *pItemData) {
	const char *pText = pItemData->pText;
	if (!pText)
		pText = "";
	Menu::Item item;
	item.Id = pItemData->Id;
	item.Flags = pItemData->Flags;
	if (item.Flags & MENU_IF_SEPARATOR)
		item.pSubmenu = nullptr; /* Ensures that no separator is a submenu */
	else
		item.pSubmenu = pItemData->pSubmenu;
	GUI__strcpy(item.pText, pText);
	MENU_SetOwner(item.pSubmenu, pObj);
	item.TextWidth = _CalcTextWidth(pObj, pText);
	pObj->ItemArray[Index] = item;
}
void MENU__SetItemFlags(Menu *pObj, unsigned Index, uint16_t Mask, uint16_t Flags) {
	auto &item = pObj->ItemArray[Index];
	item.Flags &= ~Mask;
	item.Flags |= Flags;
}
size_t MENU__SendMenuMessage(Menu *pObj, WObj *pDestWin, uint16_t MsgType, uint16_t ItemId) {
	MENU_MSG_DATA MsgData;
	MsgData.MsgType = MsgType;
	MsgData.ItemId = ItemId;
	WM_PARAM data;
	data = (WM_PARAM)&MsgData;
//	msg.pWinSrc = pObj;
	if (!pDestWin)
		pDestWin = pObj->Parent();
	if (!pDestWin)
		return 0;
	pDestWin->SendMessage(WM_MENU, &data);
	return data;
}
void MENU_AddItem(Menu *pObj, const Menu::Item *pItemData) {
	if (!pObj)
		return;
	if (!pItemData)
		return;
	auto Index = pObj->ItemArray.NumItems() - 1;
	pObj->ItemArray.Add(Menu::Item());
	MENU__SetItem(pObj, Index, pItemData);
	MENU__ResizeMenu(pObj);
}
void MENU_SetOwner(Menu *pObj, WObj *pOwner) {
	if (!pObj)
		return;
	pObj->pOwner = pOwner;
}
void MENU_Attach(Menu *pObj, WObj *pDestWin, Point Pos, Point Size, uint16_t Flags) {
	if (!pObj)
		return;
	pObj->Width = Size.x > 0 ? Size.x : 0;
	pObj->Height = Size.y > 0 ? Size.y : 0;
	pObj->Parent(pDestWin, Pos);
	MENU__ResizeMenu(pObj);
}
void MENU_DeleteItem(Menu *pObj, uint16_t ItemId) {
	if (!pObj)
		return;
	int Index = MENU__FindItem(pObj, ItemId, &pObj);
	if (Index < 0)
		return;
	pObj->ItemArray.Delete(Index);
	MENU__ResizeMenu(pObj);
}
void MENU_DisableItem(Menu *pObj, uint16_t ItemId) {
	if (!pObj)
		return;
	int Index = MENU__FindItem(pObj, ItemId, &pObj);
	if (Index < 0)
		return;
	MENU__SetItemFlags(pObj, Index, MENU_IF_DISABLED, MENU_IF_DISABLED);
	MENU__InvalidateItem(pObj, Index);
}
void MENU_EnableItem(Menu *pObj, uint16_t ItemId) {
	if (!pObj)
		return;
	int Index = MENU__FindItem(pObj, ItemId, &pObj);
	if (Index < 0)
		return;
	MENU__SetItemFlags(pObj, Index, MENU_IF_DISABLED, 0);
	MENU__InvalidateItem(pObj, Index);
}
unsigned MENU_GetNumItems(Menu *pObj) {
	return pObj->ItemArray.NumItems();
}
void MENU_InsertItem(Menu *pObj, uint16_t ItemId, const Menu::Item *pItemData) {
	if (!pObj)
		return;
	if (!pItemData)
		return;
	int Index = MENU__FindItem(pObj, ItemId, &pObj);
	if (Index < 0)
		return;
	pObj->ItemArray.Insert(*pItemData, Index);
	MENU__ResizeMenu(pObj);
}
void MENU_Popup(Menu *pObj, WObj *pDestWin, Point Pos, Point Size, uint16_t Flags) {
	if (!pObj)
		return;
	if (!pDestWin)
		return;
	pObj->Flags |= MENU_CF_POPUP;
	pObj->Width = Size.x > 0 ? Size.x : 0;
	pObj->Height = Size.y > 0 ? Size.y : 0;
	Pos += pDestWin->Position();
	MENU_SetOwner(pObj, pDestWin);
	pObj->Parent(WObj::Desktop(), Pos);
	MENU__SendMenuMessage((Menu *)pDestWin, pObj, MENU_ON_OPEN, 0);
}
void MENU_SetBkColor(Menu *pObj, unsigned ColorIndex, RGBC Color) {
	if (!pObj)
		return;
	if (ColorIndex >= GUI_COUNTOF(pObj->Props.aBkColor))
		return;
	if (Color == pObj->Props.aBkColor[ColorIndex])
		return;
	pObj->Props.aBkColor[ColorIndex] = Color;
	pObj->Invalidate();
}
void MENU_SetBorderSize(Menu *pObj, unsigned BorderIndex, uint8_t BorderSize) {
	if (!pObj)
		return;
	if (BorderIndex >= GUI_COUNTOF(pObj->Props.aBorder))
		return;
	if (BorderSize == pObj->Props.aBorder[BorderIndex])
		return;
	pObj->Props.aBorder[BorderIndex] = BorderSize;
	MENU__ResizeMenu(pObj);
}
void MENU_SetFont(Menu *pObj, CFont *pFont) {
	if (!pObj)
		return;
	if (pFont == pObj->Props.pFont)
		return;
	pObj->Props.pFont = pFont;
	MENU__RecalcTextWidthOfItems(pObj);
	MENU__ResizeMenu(pObj);
}
void MENU_SetItem(Menu *pObj, uint16_t ItemId, const Menu::Item *pItemData) {
	if (!pObj)
		return;
	if (!pItemData)
		return;
	int Index = MENU__FindItem(pObj, ItemId, &pObj);
	if (Index)
		return;
	MENU__SetItem(pObj, Index, pItemData);
	MENU__ResizeMenu(pObj);
}
void MENU_SetTextColor(Menu *pObj, unsigned ColorIndex, RGBC Color) {
	if (!pObj)
		return;
	if (ColorIndex >= GUI_COUNTOF(pObj->Props.aTextColor))
		return;
	if (Color == pObj->Props.aTextColor[ColorIndex])
		return;
	pObj->Props.aTextColor[ColorIndex] = Color;
	pObj->Invalidate();
}
int MENU__FindItem(Menu *pObj, uint16_t ItemId, Menu **ppMenu) {
	int ItemIndex = -1;
	unsigned i, NumItems = pObj->ItemArray.NumItems();
	for (i = 0; i < NumItems && ItemIndex < 0; ++i) {
		auto &item = pObj->ItemArray[i];
		if (item.Id == ItemId) {
			*ppMenu = pObj;
			ItemIndex = i;
		}
		else if (item.pSubmenu)
			ItemIndex = MENU__FindItem(item.pSubmenu, ItemId, ppMenu);
	}
	return ItemIndex;
}
