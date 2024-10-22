
#include "MENU.h"
#define MENU_FONT_DEFAULT         &GUI_Font13_1
#define MENU_EFFECT_DEFAULT       &Widget::Effect::D3L1
#define MENU_TEXTCOLOR0_DEFAULT   GUI_BLACK
#define MENU_BKCOLOR0_DEFAULT     GUI_LIGHTGRAY
#define MENU_TEXTCOLOR1_DEFAULT   GUI_WHITE
#define MENU_BKCOLOR1_DEFAULT     0x980000
#define MENU_TEXTCOLOR2_DEFAULT   0x7C7C7C
#define MENU_BKCOLOR2_DEFAULT     GUI_LIGHTGRAY
#define MENU_TEXTCOLOR3_DEFAULT   GUI_LIGHTGRAY
#define MENU_BKCOLOR3_DEFAULT     0x980000
#define MENU_TEXTCOLOR4_DEFAULT   GUI_WHITE
#define MENU_BKCOLOR4_DEFAULT     0x7C7C7C
#define MENU_BORDER_LEFT_DEFAULT    4
#define MENU_BORDER_RIGHT_DEFAULT   4
#define MENU_BORDER_TOP_DEFAULT     2
#define MENU_BORDER_BOTTOM_DEFAULT  2
#define MENU_CF_ACTIVE            (1<<6)  /* Internal flag only */
#define MENU_CF_POPUP             (1<<7)  /* Internal flag only */
MENU_PROPS MENU__DefaultProps = {
	MENU_TEXTCOLOR0_DEFAULT,
	MENU_TEXTCOLOR1_DEFAULT,
	MENU_TEXTCOLOR2_DEFAULT,
	MENU_TEXTCOLOR3_DEFAULT,
	MENU_TEXTCOLOR4_DEFAULT,
	MENU_BKCOLOR0_DEFAULT,
	MENU_BKCOLOR1_DEFAULT,
	MENU_BKCOLOR2_DEFAULT,
	MENU_BKCOLOR3_DEFAULT,
	MENU_BKCOLOR4_DEFAULT,
	MENU_BORDER_LEFT_DEFAULT,
	MENU_BORDER_RIGHT_DEFAULT,
	MENU_BORDER_TOP_DEFAULT,
	MENU_BORDER_BOTTOM_DEFAULT,
	MENU_FONT_DEFAULT
};
const Widget::Effect *MENU__pDefaultEffect = MENU_EFFECT_DEFAULT;
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
	if (_HasEffect(pObj))
		return pObj->pEffect->EffectSize;
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
	MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	int ItemWidth = (pObj->Flags & MENU_CF_VERTICAL) || !(pItem->Flags & MENU_IF_SEPARATOR) ?
		pItem->TextWidth : 3;
	return ItemWidth + pObj->Props.aBorder[MENU_BI_LEFT] + pObj->Props.aBorder[MENU_BI_RIGHT];
}
static int _GetItemHeight(Menu *pObj, unsigned Index) {
	if (pObj->Height && !(pObj->Flags & MENU_CF_VERTICAL))
		return pObj->Height - (_GetEffectSize(pObj) << 1);
	int ItemHeight = GUI_GetYDistOfFont(pObj->Props.pFont);
	if (pObj->Flags & MENU_CF_VERTICAL) {
		MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
		if (pItem->Flags & MENU_IF_SEPARATOR)
			ItemHeight = 3;
	}
	return ItemHeight + pObj->Props.aBorder[MENU_BI_TOP] + pObj->Props.aBorder[MENU_BI_BOTTOM];
}
static int _CalcMenuSizeX(Menu *pObj) {
	unsigned i, NumItems = MENU__GetNumItems(pObj);
	int xSize = 0;
	if (pObj->Flags & MENU_CF_VERTICAL) {
		for (i = 0; i < NumItems; i++) {
			int ItemWidth = _GetItemWidth(pObj, i);
			if (ItemWidth > xSize)
				xSize = ItemWidth;
		}
	}
	else for (i = 0; i < NumItems; i++)
		xSize += _GetItemWidth(pObj, i);
	return xSize + (_GetEffectSize(pObj) << 1);
}
static int _CalcMenuSizeY(Menu *pObj) {
	unsigned i, NumItems = MENU__GetNumItems(pObj);
	int ySize = 0;
	if (pObj->Flags & MENU_CF_VERTICAL)
		for (i = 0; i < NumItems; i++)
			ySize += _GetItemHeight(pObj, i);
	else for (i = 0; i < NumItems; i++) {
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
	auto NumItems = MENU__GetNumItems(pObj);
	int pos = 0;
	if (pObj->Flags & MENU_CF_VERTICAL)
		for (unsigned i = 0; i < NumItems; i++) {
			pos += _GetItemHeight(pObj, i);
			if (Pos.y < pos)
				return i;
		}
	else for (unsigned i = 0; i < NumItems; i++) {
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
	if (pObj->IsSubmenuActive == 0)
		if (WM_HasCaptured(pObj) == 0)
			WM_SetCapture(pObj, 0);
}
static void _ReleaseCapture(Menu *pObj) {
	if (WM_HasCaptured(pObj))
		if (_IsTopLevelMenu(pObj) && !(pObj->Flags & MENU_CF_POPUP))
			WM_ReleaseCapture();
}
static void _CloseSubmenu(Menu *pObj) {
	if (!(pObj->Flags & MENU_CF_ACTIVE))
		return;
	if (!pObj->IsSubmenuActive)
		return;
	MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, pObj->Sel);
	MENU__SendMenuMessage(pObj, pItem->pSubmenu, MENU_ON_CLOSE, 0);
	pItem->pSubmenu->Detach();
	pObj->IsSubmenuActive = 0;
	_SetCapture(pObj);
	MENU__InvalidateItem(pObj, pObj->Sel);
}
static void _OpenSubmenu(Menu *pObj, unsigned Index) {
	if (!(pObj->Flags & MENU_CF_ACTIVE))
		return;
	char PrevActiveSubmenu = pObj->IsSubmenuActive;
	MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	_CloseSubmenu(pObj);
	if (!pItem->pSubmenu)
		return;
	if (pItem->Flags & MENU_IF_DISABLED)
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
	MENU__SendMenuMessage(pObj, pObj->pOwner, MENU_ON_INITSUBMENU, pItem->Id);
	MENU_SetOwner(pItem->pSubmenu, pObj);
	pItem->pSubmenu->Parent(WM_GetDesktopWindow(), Pos);
	MENU__SendMenuMessage(pObj, pItem->pSubmenu, MENU_ON_OPEN, 0);
	pObj->IsSubmenuActive = 1;
	MENU__InvalidateItem(pObj, Index);
}
static void _ClosePopup(Menu *pObj) {
	if (!(pObj->Flags & MENU_CF_POPUP))
		return;
	pObj->Flags &= ~(MENU_CF_POPUP);
	pObj->Detach();
	WM_ReleaseCapture();
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
	if (pObj->IsSubmenuActive != 0)
		return;
	_SetSelection(pObj, -1);
	_ReleaseCapture(pObj);
}
static void _ActivateItem(Menu *pObj, unsigned Index) {
	MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	if (pItem->pSubmenu)
		return;
	if (pItem->Flags & (MENU_IF_DISABLED | MENU_IF_SEPARATOR))
		return;
	_ClosePopup(pObj);
	MENU__SendMenuMessage(pObj, pObj->pOwner, MENU_ON_ITEMSELECT, pItem->Id);
}
static void _ActivateMenu(Menu *pObj, unsigned Index) {
	if (pObj->Flags & MENU_CF_OPEN_ON_POINTEROVER)
		return;
	MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	if (!pItem->pSubmenu)
		return;
	if (pItem->Flags & MENU_IF_DISABLED)
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
	if (pObj->IsSubmenuActive || (pObj->Flags & MENU_CF_POPUP))
		return false;
	if (!_IsTopLevelMenu(pObj))
		return false;
	Pos += pObj->Position();
	auto pBelow = WObj::ScreenToWin(Pos);
	if (!pBelow || pBelow == pObj)
		return 0;
	PidState State = Pos - pBelow->Position();
	WM_MESSAGE msg;
	msg.Data = (size_t)&State;
	msg.MsgId = WM_MOUSEOVER;
	pBelow->SendMessage(&msg);
	return 1;
}
static bool _HandlePID(Menu *pObj, const PidState &pid) {
	PidState PrevState;
	WM_PID__GetPrevState(&PrevState);
	bool XYInWidget = false;
	if (pid.x >= 0 && pid.y >= 0) {
		SRect r;
		WM__GetClientRectWin(pObj, &r);
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
static void _ForwardPIDMsgToOwner(Menu *pObj, WM_MESSAGE *pMsg) {
	if (_IsTopLevelMenu(pObj))
		return;
	if (auto pOwner = pObj->pOwner ? pObj->pOwner : pObj->Parent()) {
		if (auto pState = (PidState *)pMsg->Data)
			*pState += pObj->Position() - pOwner->Position();
		pOwner->SendMessage(pMsg);
	}
}
static void _OnMenu(Menu *pObj, WM_MESSAGE *pMsg) {
	auto pData = (const MENU_MSG_DATA *)pMsg->Data;
	if (!pData)
		return;
	switch (pData->MsgType) {
	case MENU_ON_ITEMSELECT:
		_DeactivateMenu(pObj);
		_DeselectItem(pObj);
		_ClosePopup(pObj);
	case MENU_ON_INITMENU:
	case MENU_ON_INITSUBMENU:
	{
		WObj *pOwner = pObj->pOwner ?
			pObj->pOwner : pObj->Parent();
		if (pOwner) {
			pMsg->pWinSrc = pObj;
			pOwner->SendMessage(pMsg);
		}
		break;
	}
	case MENU_ON_OPEN:
		pObj->Sel = -1;
		pObj->IsSubmenuActive = 0;
		pObj->Flags |= MENU_CF_ACTIVE | MENU_CF_OPEN_ON_POINTEROVER;
		_SetCapture(pObj);
		MENU__ResizeMenu(pObj);
		break;
	case MENU_ON_CLOSE:
		_CloseSubmenu(pObj);
		break;
	case MENU_IS_MENU:
		pMsg->Data = 1;
		break;
	}
}
static bool _OnTouch(Menu *pObj, WM_MESSAGE *pMsg) {
	if (auto pState = (const PidState *)pMsg->Data)
		return _HandlePID(pObj, *pState);
	return _HandlePID(pObj, { -1, -1 });
}
static bool _OnMouseOver(Menu *pObj, WM_MESSAGE *pMsg) {
	if (auto pState = (const PidState *)pMsg->Data)
		return _HandlePID(pObj, { *pState, -1 });
	return false;
}
static void _SetPaintColors(const Menu *pObj, const MENU_ITEM *pItem, int ItemIndex) {
	unsigned ColorIndex;
	char Selected = (ItemIndex == pObj->Sel) ? 1 : 0;
	if (pObj->IsSubmenuActive && Selected)
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
static void _OnPaint(Menu *pObj) {
	SRect FillRect, TextRect;
	MENU_ITEM *pItem;
	unsigned TextWidth, NumItems, i;
	uint8_t BorderLeft = pObj->Props.aBorder[MENU_BI_LEFT];
	uint8_t BorderTop = pObj->Props.aBorder[MENU_BI_TOP];
	int FontHeight = GUI_GetYDistOfFont(pObj->Props.pFont);
	int EffectSize = _GetEffectSize(pObj);
	NumItems = MENU__GetNumItems(pObj);
	WM__GetClientRectWin(pObj, &FillRect);
	FillRect /= EffectSize;
	GUI.Font(pObj->Props.pFont);
	if (pObj->Flags & MENU_CF_VERTICAL) {
		int ItemHeight, xSize;
		xSize = _CalcMenuSizeX(pObj);
		FillRect.x1 = xSize - EffectSize - 1;
		TextRect.x0 = FillRect.x0 + BorderLeft;
		for (i = 0; i < NumItems; i++) {
			pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, i);
			ItemHeight = _GetItemHeight(pObj, i);
			_SetPaintColors(pObj, pItem, i);
			FillRect.y1 = FillRect.y0 + ItemHeight - 1;
			if (pItem->Flags & MENU_IF_SEPARATOR) {
				GUI_ClearRectEx(&FillRect);
				GUI.PenColor(0x7C7C7C);
				GUI_DrawHLine(FillRect.y0 + BorderTop + 1, FillRect.x0 + 2, FillRect.x1 - 2);
			}
			else {
				TextWidth = pItem->TextWidth;
				TextRect.x1 = TextRect.x0 + TextWidth - 1;
				TextRect.y0 = FillRect.y0 + BorderTop;
				TextRect.y1 = TextRect.y0 + FontHeight - 1;
				WIDGET__FillStringInRect(pItem->acText, &FillRect, &TextRect, &TextRect);
			}
			FillRect.y0 += ItemHeight;
		}
	}
	else {
		int ItemWidth, ySize;
		ySize = _CalcMenuSizeY(pObj);
		FillRect.y1 = ySize - EffectSize - 1;
		TextRect.y0 = FillRect.y0 + BorderTop;
		TextRect.y1 = TextRect.y0 + FontHeight - 1;
		for (i = 0; i < NumItems; i++) {
			pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, i);
			ItemWidth = _GetItemWidth(pObj, i);
			_SetPaintColors(pObj, pItem, i);
			FillRect.x1 = FillRect.x0 + ItemWidth - 1;
			if (pItem->Flags & MENU_IF_SEPARATOR) {
				GUI_ClearRectEx(&FillRect);
				GUI.PenColor(0x7C7C7C);
				GUI_DrawVLine(FillRect.x0 + BorderLeft + 1, FillRect.y0 + 2, FillRect.y1 - 2);
			}
			else {
				TextWidth = pItem->TextWidth;
				TextRect.x0 = FillRect.x0 + BorderLeft;
				TextRect.x1 = TextRect.x0 + TextWidth - 1;
				WIDGET__FillStringInRect(pItem->acText, &FillRect, &TextRect, &TextRect);
			}
			FillRect.x0 += ItemWidth;
		}
	}
	if (pObj->Width || pObj->Height) {
		SRect r;
		WM__GetClientRectWin(pObj, &r);
		r /= EffectSize;
		GUI.BkColor(pObj->Props.aBkColor[MENU_CI_ENABLED]);
		GUI_ClearRect(FillRect.x1 + 1, EffectSize, r.x1, FillRect.y1);
		GUI_ClearRect(EffectSize, FillRect.y1 + 1, r.x1, r.y1);
	}
	if (_HasEffect(pObj))
		pObj->pEffect->DrawUp();
}
static void _MENU_Callback(WM_MESSAGE *pMsg) {
	Menu *pObj = (Menu *)pMsg->pWin;
	if (pMsg->MsgId != WM_PID_STATE_CHANGED)
		/* Let widget handle the standard messages */
		if (WIDGET_HandleActive(pObj, pMsg) == 0)
			return;
	switch (pMsg->MsgId) {
	case WM_MENU:
		_OnMenu(pObj, pMsg);
		return;     /* Message handled, do not call WM_DefaultProc() here. */
	case WM_TOUCH:
		if (_OnTouch(pObj, pMsg))
			_ForwardPIDMsgToOwner(pObj, pMsg);
		break;
	case WM_MOUSEOVER:
		if (_OnMouseOver(pObj, pMsg))
			_ForwardPIDMsgToOwner(pObj, pMsg);
		break;
	case WM_PAINT:
		_OnPaint(pObj);
		break;
	case WM_DELETE:
		GUI_ARRAY_Delete(&pObj->ItemArray);
		break;      /* No return here ... WM_DefaultProc needs to be called */
	}
	WM_DefaultProc(pMsg);
}
Menu *MENU_CreateEx(int x0, int y0, int xSize, int ySize, WObj *pParent, int WinFlags, int ExFlags, int Id) {
	auto pObj = (Menu *)WObj::Create(
		x0, y0,
		xSize, ySize,
		pParent, WC_VISIBLE | WC_STAYONTOP | WinFlags,
		&_MENU_Callback, sizeof(Menu) - sizeof(WObj));
	if (!pObj) 
		return 0;
	/* init widget specific variables */
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE);
	/* init member variables */
	if (ExFlags & MENU_CF_OPEN_ON_POINTEROVER)
		ExFlags |= MENU_CF_ACTIVE;
	else
		ExFlags &= ~(MENU_CF_ACTIVE);
	pObj->Props = MENU__DefaultProps;
	pObj->Flags = ExFlags;
	pObj->Width = ((xSize > 0) ? xSize : 0);
	pObj->Height = ((ySize > 0) ? ySize : 0);
	pObj->Sel = -1;
	pObj->pOwner = 0;
	pObj->IsSubmenuActive = 0;
	WIDGET_SetEffect(pObj, MENU__pDefaultEffect);
	return pObj;
}
unsigned MENU__GetNumItems(Menu *pObj) {
	return GUI_ARRAY_GetNumItems(&pObj->ItemArray);
}
void MENU__InvalidateItem(Menu *pObj, unsigned Index) {
	pObj->Invalidate();  /* Can be optimized, no need to invalidate all items */
}
void MENU__RecalcTextWidthOfItems(Menu *pObj) {
	auto pOldFont = GUI.Font();
	GUI.Font(pObj->Props.pFont);
	unsigned i, NumItems = MENU__GetNumItems(pObj);
	for (i = 0; i < NumItems; i++) {
		MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, i);
		pItem->TextWidth = GUI_GetStringDistX(pItem->acText);
	}
	GUI.Font(pOldFont);
}
void MENU__ResizeMenu(Menu *pObj) {
	pObj->Size({ _CalcWindowSizeX(pObj),
				 _CalcWindowSizeY(pObj) });
	pObj->Invalidate();
}
char MENU__SetItem(Menu *pObj, unsigned Index, const MENU_ITEM_DATA *pItemData) {
	MENU_ITEM Item = { 0 };
	const char *pText = pItemData->pText;
	if (!pText)
		pText = "";
	Item.Id = pItemData->Id;
	Item.Flags = pItemData->Flags;
	Item.pSubmenu = pItemData->pSubmenu;
	Item.TextWidth = _CalcTextWidth(pObj, pText);
	if (Item.Flags & MENU_IF_SEPARATOR)
		Item.pSubmenu = 0;   /* Ensures that no separator is a submenu */
	if (GUI_ARRAY_SetItem(&pObj->ItemArray, Index, &Item, (int)sizeof(MENU_ITEM) + (int)GUI__strlen(pText)))
		return 0;
	MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	GUI__strcpy(pItem->acText, pText);
	MENU_SetOwner(Item.pSubmenu, pObj);
	return 1;
}
void MENU__SetItemFlags(Menu *pObj, unsigned Index, uint16_t Mask, uint16_t Flags) {
	MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	pItem->Flags &= ~Mask;
	pItem->Flags |= Flags;
}
size_t MENU__SendMenuMessage(Menu *pObj, WObj *pDestWin, uint16_t MsgType, uint16_t ItemId) {
	MENU_MSG_DATA MsgData;
	MsgData.MsgType = MsgType;
	MsgData.ItemId = ItemId;
	WM_MESSAGE msg = { 0 };
	msg.MsgId = WM_MENU;
	msg.Data = (size_t)&MsgData;
	msg.pWinSrc = pObj;
	if (!pDestWin)
		pDestWin = pObj->Parent();
	if (!pDestWin)
		return 0;
	pDestWin->SendMessage(&msg);
	return msg.Data;
}
void MENU_AddItem(Menu *pObj, const MENU_ITEM_DATA *pItemData) {
	if (!pObj)
		return;
	if (!pItemData)
		return;
	if (GUI_ARRAY_AddItem(&pObj->ItemArray, nullptr, 0))
		return;
	unsigned Index = MENU__GetNumItems(pObj) - 1;
	if (MENU__SetItem(pObj, Index, pItemData))
		MENU__ResizeMenu(pObj);
	else
		GUI_ARRAY_DeleteItem(&pObj->ItemArray, Index);
}
void MENU_SetOwner(Menu *pObj, WObj *pOwner) {
	if (!pObj)
		return;
	pObj->pOwner = pOwner;
}
void MENU_Attach(Menu *pObj, WObj *pDestWin, Point Pos, Point Size, int Flags) {
	if (!pObj)
		return;
	pObj->Width = Size.x > 0 ? Size.x : 0;
	pObj->Height = Size.y > 0 ? Size.y : 0;
	pObj->Parent(pDestWin, Pos);
	MENU__ResizeMenu(pObj);
}
Menu *MENU_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb) {
	return MENU_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,
						 pCreateInfo->xSize, pCreateInfo->ySize,
						 pParent, 0, pCreateInfo->Flags, pCreateInfo->Id);
}
void MENU_SetDefaultTextColor(unsigned ColorIndex, RGBC Color) {
	if (ColorIndex < GUI_COUNTOF(MENU__DefaultProps.aTextColor))
		MENU__DefaultProps.aTextColor[ColorIndex] = Color;
}
void MENU_SetDefaultBkColor(unsigned ColorIndex, RGBC Color) {
	if (ColorIndex < GUI_COUNTOF(MENU__DefaultProps.aBkColor))
		MENU__DefaultProps.aBkColor[ColorIndex] = Color;
}
void MENU_SetDefaultBorderSize(unsigned BorderIndex, uint8_t BorderSize) {
	if (BorderIndex < GUI_COUNTOF(MENU__DefaultProps.aBorder))
		MENU__DefaultProps.aBorder[BorderIndex] = BorderSize;
}
void MENU_SetDefaultEffect(const Widget::Effect *pEffect) {
	MENU__pDefaultEffect = pEffect;
}
void MENU_SetDefaultFont(CFont *pFont) {
	MENU__DefaultProps.pFont = pFont;
}
RGBC MENU_GetDefaultTextColor(unsigned ColorIndex) {
	if (ColorIndex < GUI_COUNTOF(MENU__DefaultProps.aTextColor))
		return MENU__DefaultProps.aTextColor[ColorIndex];
	return GUI_INVALID_COLOR;
}
RGBC MENU_GetDefaultBkColor(unsigned ColorIndex) {
	if (ColorIndex < GUI_COUNTOF(MENU__DefaultProps.aBkColor))
		return MENU__DefaultProps.aBkColor[ColorIndex];
	return GUI_INVALID_COLOR;
}
uint8_t MENU_GetDefaultBorderSize(unsigned BorderIndex) {
	if (BorderIndex < GUI_COUNTOF(MENU__DefaultProps.aBorder))
		return MENU__DefaultProps.aBorder[BorderIndex];
	return 0;
}
const Widget::Effect *MENU_GetDefaultEffect(void) {
	return MENU__pDefaultEffect;
}
CFont *MENU_GetDefaultFont(void) {
	return MENU__DefaultProps.pFont;
}
void MENU_DeleteItem(Menu *pObj, uint16_t ItemId) {
	if (!pObj)
		return;
	int Index = MENU__FindItem(pObj, ItemId, &pObj);
	if (Index < 0)
		return;
	GUI_ARRAY_DeleteItem(&pObj->ItemArray, Index);
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
void MENU_GetItem(Menu *pObj, uint16_t ItemId, MENU_ITEM_DATA *pItemData) {
	if (!pObj)
		return;
	if (!pItemData)
		return;
	int Index = MENU__FindItem(pObj, ItemId, &pObj);
	if (Index < 0)
		return;
	MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	pItemData->Flags = pItem->Flags;
	pItemData->Id = pItem->Id;
	pItemData->pSubmenu = pItem->pSubmenu;
	pItemData->pText = 0;
}
void MENU_GetItemText(Menu *pObj, uint16_t ItemId, char *pBuffer, unsigned BufferSize) {
	if (!pObj)
		return;
	if (!pBuffer)
		return;
	int Index = MENU__FindItem(pObj, ItemId, &pObj);
	if (Index < 0)
		return;
	MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, Index);
	GUI__memcpy(pBuffer, pItem->acText, BufferSize);
	pBuffer[BufferSize - 1] = 0;
}
unsigned MENU_GetNumItems(Menu *pObj) {
	return MENU__GetNumItems(pObj);
}
void MENU_InsertItem(Menu *pObj, uint16_t ItemId, const MENU_ITEM_DATA *pItemData) {
	if (!pObj)
		return;
	if (!pItemData)
		return;
	int Index = MENU__FindItem(pObj, ItemId, &pObj);
	if (Index < 0)
		return;
	if (!GUI_ARRAY_InsertBlankItem(&pObj->ItemArray, Index))
		return;
	if (MENU__SetItem(pObj, Index, pItemData))
		MENU__ResizeMenu(pObj);
	else
		GUI_ARRAY_DeleteItem(&pObj->ItemArray, Index);
}
void MENU_Popup(Menu *pObj, WObj *pDestWin, Point Pos, Point Size, int Flags) {
	if (!pObj)
		return;
	if (!pDestWin)
		return;
	pObj->Flags |= MENU_CF_POPUP;
	pObj->Width = Size.x > 0 ? Size.x : 0;
	pObj->Height = Size.y > 0 ? Size.y : 0;
	Pos += pDestWin->Position();
	MENU_SetOwner(pObj, pDestWin);
	pObj->Parent(WM_GetDesktopWindow(), Pos);
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
void MENU_SetItem(Menu *pObj, uint16_t ItemId, const MENU_ITEM_DATA *pItemData) {
	if (!pObj)
		return;
	if (!pItemData)
		return;
	int Index = MENU__FindItem(pObj, ItemId, &pObj);
	if (Index)
		return;
	if (!MENU__SetItem(pObj, Index, pItemData))
		return;
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
	unsigned i, NumItems = MENU__GetNumItems(pObj);
	for (i = 0; i < NumItems && ItemIndex < 0; i++) {
		MENU_ITEM *pItem = (MENU_ITEM *)GUI_ARRAY_GetItem(&pObj->ItemArray, i);
		if (pItem->Id == ItemId) {
			*ppMenu = pObj;
			ItemIndex = i;
		}
		else if (pItem->pSubmenu)
			ItemIndex = MENU__FindItem(pItem->pSubmenu, ItemId, ppMenu);
	}
	return ItemIndex;
}
