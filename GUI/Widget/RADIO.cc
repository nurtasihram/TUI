
#include "RADIO.h"
extern const Bitmap RADIO__abmRadio[];
extern const Bitmap RADIO__bmCheck;
#define RADIO_FONT_DEFAULT        &GUI_Font13_1
#define RADIO_IMAGE0_DEFAULT      &RADIO__abmRadio[0]
#define RADIO_IMAGE1_DEFAULT      &RADIO__abmRadio[1]
#define RADIO_IMAGE_CHECK_DEFAULT &RADIO__bmCheck
#define RADIO_DEFAULT_TEXT_COLOR  GUI_BLACK
#define RADIO_DEFAULT_BKCOLOR     0xC0C0C0
#define RADIO_BKCOLOR0_DEFAULT    0xc0c0c0           /* Inactive color */
#define RADIO_BKCOLOR1_DEFAULT    GUI_WHITE          /* Active color */
#define RADIO_BORDER              2
void (*RADIO__pfHandleSetValue)(Radio_Obj *pObj, int v);
RGBC   RADIO__DefaultTextColor = RADIO_DEFAULT_TEXT_COLOR;
CFont *RADIO__pDefaultFont = RADIO_FONT_DEFAULT;
const Bitmap *RADIO__apDefaultImage[] = { RADIO_IMAGE0_DEFAULT, RADIO_IMAGE1_DEFAULT };
const Bitmap *RADIO__pDefaultImageCheck = RADIO_IMAGE_CHECK_DEFAULT;
static void _ResizeRect(SRect *pDest, const SRect *pSrc, int Diff) {
	pDest->y0 = pSrc->y0 - Diff;
	pDest->y1 = pSrc->y1 + Diff;
	pDest->x0 = pSrc->x0 - Diff;
	pDest->x1 = pSrc->x1 + Diff;
}
static void _OnPaint(Radio_Obj *pObj) {
	FontInfo FontInfo;
	SRect rect, r, rFocus;
	WIDGET__GetClientRect(pObj, &rFocus);
	int HasFocus = (pObj->State & WIDGET_STATE_FOCUS) ? 1 : 0;
	const Bitmap *pBmRadio = pObj->apBmRadio[pObj->Enable()],
		*pBmCheck = pObj->pBmCheck;
	rFocus.x1 = pBmRadio->XSize + RADIO_BORDER * 2 - 1;
	rFocus.y1 = pObj->Height + ((pObj->NumItems - 1) * pObj->Spacing) - 1;
	GUI.PenColor(pObj->TextColor);
	GUI.Font(pObj->pFont);
	GUI_SetTextMode(DRAWMODE_TRANS);
	GUI_GetFontInfo(pObj->pFont, &FontInfo);
	int FontDistY = GUI_GetFontDistY();
	uint8_t CHeight = FontInfo.CHeight,
		SpaceAbove = FontInfo.Baseline - CHeight;
	rect.x0 = pBmRadio->XSize + RADIO_BORDER * 2 + 2;
	rect.y0 = (CHeight <= pObj->Height) ? ((pObj->Height - CHeight) / 2) : 0;
	rect.y1 = rect.y0 + CHeight - 1;
	uint8_t FocusBorder = FontDistY <= 12 ? 2 : 3;
	if (rect.y0 < FocusBorder)
		FocusBorder = (uint8_t)rect.y0;
	if (pObj->BkColor != GUI_INVALID_COLOR)
		GUI.BkColor(pObj->BkColor);
	else
		GUI.BkColor(RADIO_DEFAULT_BKCOLOR);
	GUI_Clear();
	int i;
	for (i = 0; i < pObj->NumItems; i++) {
		int y = i * pObj->Spacing;
		GUI_DrawBitmap(pBmRadio, RADIO_BORDER, RADIO_BORDER + y);
		if (pObj->Sel == i)
			GUI_DrawBitmap(pBmCheck, RADIO_BORDER + (pBmRadio->XSize - pBmCheck->XSize) / 2,
						   RADIO_BORDER + ((pBmRadio->YSize - pBmCheck->YSize) / 2) + y);
		const char *pText = (const char *)GUI_ARRAY_GetItem(&pObj->TextArray, i);
		if (!pText)
			continue;
		if (!*pText)
			continue;
		r = rect;
		r.x1 = r.x0 + GUI_GetStringDistX(pText) - 2;
		r += Point(0, y);
		GUI_DispStringAt(pText, r.x0, r.y0 - SpaceAbove);
		if (HasFocus && (pObj->Sel == i))
			_ResizeRect(&rFocus, &r, FocusBorder);
	}
	if (HasFocus) {
		GUI.PenColor(GUI_BLACK);
		WIDGET__DrawFocusRect(pObj, &rFocus, 0);
	}
}
static void _OnTouch(Radio_Obj *pObj, WM_MESSAGE *pMsg) {
	int Notification;
	int Hit = 0;
	PidState *pState = (PidState *)pMsg->Data;
	if (pState) {  /* Something happened in our area (pressed or released) */
		if (pState->Pressed) {
			int y = pState->y,
				Sel = y / pObj->Spacing;
			y -= Sel * pObj->Spacing;
			if (y <= pObj->Height)
				RADIO_SetValue(pObj, Sel);
			if (pObj->Focussable())
				pObj->Focus();
			Notification = WM_NOTIFICATION_CLICKED;
		}
		else {
			Hit = 1;
			Notification = WM_NOTIFICATION_RELEASED;
		}
	}
	else
		Notification = WM_NOTIFICATION_MOVED_OUT;
	WM_NotifyParent(pObj, Notification);
	if (Hit == 1) {
		GUI_StoreKey(pObj->Id);
	}
}
static void  _OnKey(Radio_Obj *pObj, WM_MESSAGE *pMsg) {
	const WM_KEY_INFO *pKeyInfo = (const WM_KEY_INFO *)pMsg->Data;
	if (pKeyInfo->PressedCnt <= 0)
		return;
	switch (pKeyInfo->Key) {
	case GUI_KEY_RIGHT:
	case GUI_KEY_DOWN:
		RADIO_Inc(pObj);
		break;                    /* Send to parent by not doing anything */
	case GUI_KEY_LEFT:
	case GUI_KEY_UP:
		RADIO_Dec(pObj);
		break;                    /* Send to parent by not doing anything */
	default:
		return;
	}
}
static void _RADIO_Callback(WM_MESSAGE *pMsg) {
	Radio_Obj *pObj = (Radio_Obj *)pMsg->pWin;
	if (WIDGET_HandleActive(pObj, pMsg) == 0)
		return;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		_OnPaint(pObj);
		return;
	case WM_GET_RADIOGROUP:
		pMsg->Data = pObj->GroupId;
		return;
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		break;
	case WM_KEY:
		_OnKey(pObj, pMsg);
		break;
	case WM_DELETE:
		GUI_ARRAY_Delete(&pObj->TextArray);
		break;
	}
	WM_DefaultProc(pMsg);
}
void RADIO__SetValue(Radio_Obj *pObj, int v) {
	if (v >= pObj->NumItems)
		v = (int)pObj->NumItems - 1;
	if (v == pObj->Sel)
		return;
	pObj->Sel = v;
	pObj->Invalidate();
	WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
}
Radio_Obj *RADIO_CreateEx(int x0, int y0, int xSize, int ySize, WObj *pParent,
							int WinFlags, int ExFlags, int Id, int NumItems, int Spacing) {
	int Height = RADIO__apDefaultImage[0]->YSize + RADIO_BORDER * 2;
	Spacing = Spacing <= 0 ? 20 : Spacing;
	NumItems = NumItems <= 0 ? 2 : NumItems;
	if (ySize == 0)
		ySize = Height + ((NumItems - 1) * Spacing);
	if (xSize == 0)
		xSize = RADIO__apDefaultImage[0]->XSize + RADIO_BORDER * 2;
	auto pObj = (Radio_Obj *)WObj::Create(x0, y0, xSize, ySize, pParent, WinFlags, _RADIO_Callback, sizeof(Radio_Obj) - sizeof(WObj));
	if (!pObj) {
		return 0;
	}
	int i;
	for (i = 0; i < NumItems; i++)
		GUI_ARRAY_AddItem(&pObj->TextArray, nullptr, 0);
	ExFlags &= RADIO_TEXTPOS_LEFT;
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE | ExFlags);
	pObj->apBmRadio[0] = RADIO__apDefaultImage[0];
	pObj->apBmRadio[1] = RADIO__apDefaultImage[1];
	pObj->pBmCheck = RADIO__pDefaultImageCheck;
	pObj->pFont = RADIO__pDefaultFont;
	pObj->TextColor = RADIO__DefaultTextColor;
	pObj->BkColor = WM_GetBkColor(pParent);
	pObj->NumItems = NumItems;
	pObj->Spacing = Spacing;
	pObj->Height = Height;
	return pObj;
}
Radio_Obj *RADIO_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb) {
	return RADIO_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0, pCreateInfo->xSize, pCreateInfo->ySize,
						  pParent, pCreateInfo->Flags, 0, pCreateInfo->Id,
						  (pCreateInfo->Para) & 0xFF, (pCreateInfo->Para >> 8) & 0xFF);
}
void RADIO_AddValue(Radio_Obj *pObj, int Add) {
	if (!pObj)
		return;
	RADIO_SetValue(pObj, pObj->Sel + Add);
}
void RADIO_Dec(Radio_Obj *pObj) {
	RADIO_AddValue(pObj, -1);
}
void RADIO_Inc(Radio_Obj *pObj) {
	RADIO_AddValue(pObj, 1);
}
void RADIO_SetValue(Radio_Obj *pObj, int v) {
	if (!pObj)
		return;
	if (pObj->GroupId && RADIO__pfHandleSetValue)
		(*RADIO__pfHandleSetValue)(pObj, v);
	else {
		if (v < 0)
			v = 0;
		RADIO__SetValue(pObj, v);
	}
}
int RADIO_GetValue(Radio_Obj *pObj) {
	if (!pObj)
		return 0;
	return pObj->Sel;
}
void RADIO_SetText(Radio_Obj *pObj, const char *pText, unsigned Index) {
	if (!pObj)
		return;
	if (Index >= (unsigned)pObj->NumItems)
		return;
	GUI_ARRAY_SetItem(&pObj->TextArray, Index, pText, pText ? (GUI__strlen(pText) + 1) : 0);
	pObj->Invalidate();
}
void RADIO_SetTextColor(Radio_Obj *pObj, RGBC Color) {
	if (!pObj)
		return;
	if (Color == pObj->TextColor)
		return;
	pObj->TextColor = Color;
	if (GUI_ARRAY_GetNumItems(&pObj->TextArray))
		pObj->Invalidate();
}
CFont *RADIO_GetDefaultFont(void) {
	return RADIO__pDefaultFont;
}
RGBC RADIO_GetDefaultTextColor(void) {
	return RADIO__DefaultTextColor;
}
void RADIO_SetDefaultFont(CFont *pFont) {
	RADIO__pDefaultFont = pFont;
}
void RADIO_SetDefaultTextColor(RGBC TextColor) {
	RADIO__DefaultTextColor = TextColor;
}
void RADIO_SetBkColor(Radio_Obj *pObj, RGBC Color) {
	if (!pObj)
		return;
	if (Color == pObj->BkColor)
		return;
	pObj->BkColor = Color;
	pObj->Invalidate();
}
void RADIO_SetDefaultImage(const Bitmap *pBitmap, unsigned int Index) {
	switch (Index) {
	case RADIO_BI_INACTIV:
	case RADIO_BI_ACTIV:
		RADIO__apDefaultImage[Index] = pBitmap;
		break;
	case RADIO_BI_CHECK:
		RADIO__pDefaultImageCheck = pBitmap;
		break;
	}
}
void RADIO_SetFont(Radio_Obj *pObj, CFont *pFont) {
	if (!pObj)
		return;
	if (pFont == pObj->pFont)
		return;
	pObj->pFont = pFont;
	if (GUI_ARRAY_GetNumItems(&pObj->TextArray))
		pObj->Invalidate();
}
void RADIO_SetImage(Radio_Obj *pObj, const Bitmap *pBitmap, unsigned int Index) {
	if (!pObj)
		return;
	switch (Index) {
	case RADIO_BI_INACTIV:
	case RADIO_BI_ACTIV:
		pObj->apBmRadio[Index] = pBitmap;
		break;
	case RADIO_BI_CHECK:
		pObj->pBmCheck = pBitmap;
		break;
	}
	pObj->Invalidate();
}
static void _SetValue(Radio_Obj *pObj, int v) {	RADIO__SetValue(pObj, v);
}
static int _IsInGroup(WObj *pWin, uint8_t GroupId) {
	if (!GroupId) return 0;
	WM_MESSAGE msg;
	msg.MsgId = WM_GET_RADIOGROUP;
	pWin->SendMessage(&msg);
	return msg.Data == GroupId;
}
static WObj *_GetPrevInGroup(WObj *pWin, uint8_t GroupId) {
	for (pWin = pWin->PrevSibling(); pWin; pWin = pWin->PrevSibling())
		if (_IsInGroup(pWin, GroupId))
			return pWin;
	return nullptr;
}
static WObj *_GetNextInGroup(WObj *pWin, uint8_t GroupId) {
	for (; pWin; pWin = pWin->NextSibling())
		if (_IsInGroup(pWin, GroupId))
			return pWin;
	return 0;
}
static void _ClearSelection(Radio_Obj *pObj, uint8_t GroupId) {
	for (auto pWin = pObj->FirstSibling(); pWin; pWin = pWin->pNext)
		if (pWin != pObj)
			if (_IsInGroup(pWin, GroupId))
				RADIO__SetValue((Radio_Obj *)pWin, -1);
}
static void _HandleSetValue(Radio_Obj *pObj, int v) {
	if (v < 0) {
		auto pWin = (Radio_Obj *)_GetPrevInGroup(pObj, pObj->GroupId);
		if (pWin) {
			pWin->Focus();
			_SetValue(pWin, 0x7FFF);
			RADIO__SetValue(pObj, -1);
		}
	}
	else if (v >= pObj->NumItems) {
		auto pWin = (Radio_Obj *)_GetNextInGroup(pObj->pNext, pObj->GroupId);
		if (pWin) {
			pWin->Focus();
			_SetValue(pWin, 0);
			RADIO__SetValue(pObj, -1);
		}
	}
	else if (pObj->Sel != v) {
		_ClearSelection(pObj, pObj->GroupId);
		RADIO__SetValue(pObj, v);
	}
}
void RADIO_SetGroupId(Radio_Obj *pObj, uint8_t NewGroupId) {
	if (!pObj)
		return;
	uint8_t OldGroupId = pObj->GroupId;
	if (NewGroupId == OldGroupId)
		return;
	WObj *hFirst = pObj->FirstSibling();
	if (NewGroupId && (RADIO__pfHandleSetValue == nullptr))
		RADIO__pfHandleSetValue = _HandleSetValue;
	if (OldGroupId && (pObj->Sel >= 0)) {
		auto pWin = (Radio_Obj *)_GetNextInGroup(hFirst, OldGroupId);
		pObj->GroupId = 0;
		if (pWin)
			_SetValue(pWin, 0);
	}
	if (_GetNextInGroup(hFirst, NewGroupId) != 0)
		RADIO__SetValue(pObj, -1);
	else if (pObj->Sel < 0)
		RADIO__SetValue(pObj, 0);
	pObj->GroupId = NewGroupId;
}
#include "chars_pic.inl"
/* Colors */
static const RGBC _aColorDisabled[] = { 0xC0C0C0, 0x808080, 0x000000, RADIO_BKCOLOR0_DEFAULT };
static const RGBC _aColorEnabled[] = { 0xC0C0C0, 0x808080, 0x000000, RADIO_BKCOLOR1_DEFAULT };
static const RGBC _aColorsCheck[] = { 0xFFFFFF, 0x000000 };
/* Palettes */
static const LogPalette _PalRadioDisabled = { 4, 1, _aColorDisabled };
static const LogPalette _PalRadioEnabled = { 4, 1, _aColorEnabled };
static const LogPalette _PalCheck = { 2, 1, _aColorsCheck };
/* Pixel data */
static const uint8_t _acRadio[] = {
________,XXXXXXXX,________,
____XXXX,oooooooo,XXXX____,
__XXoooo,dddddddd,oooodd__,
__XXoodd,dddddddd,dd__dd__,
XXoodddd,dddddddd,dddd__dd,
XXoodddd,dddddddd,dddd__dd,
XXoodddd,dddddddd,dddd__dd,
XXoodddd,dddddddd,dddd__dd,
__XXoodd,dddddddd,dd__dd__,
__XX____,dddddddd,____dd__,
____dddd,________,dddd____,
________,dddddddd,________
};
static const uint8_t _acCheck[] = {
  __XXXX__________,
  XXXXXXXX________,
  XXXXXXXX________,
  __XXXX__________
};
const Bitmap RADIO__abmRadio[] = {
  { 12, 12, 3, 2, _acRadio, &_PalRadioDisabled},
  { 12, 12, 3, 2, _acRadio, &_PalRadioEnabled}
};
const Bitmap RADIO__bmCheck =
{ 4, 4, 1, 1, _acCheck, &_PalCheck };
