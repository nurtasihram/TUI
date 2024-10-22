#include "GUI.h"
#include "CHECKBOX.h"
extern const Bitmap CHECKBOX__abmCheck[2];
#define CHECKBOX_FONT_DEFAULT      &GUI_Font13_1          /* Define default fonts */
#define CHECKBOX_IMAGE0_DEFAULT    &CHECKBOX__abmCheck[0] /* Define default images */
#define CHECKBOX_IMAGE1_DEFAULT    &CHECKBOX__abmCheck[1]
#define CHECKBOX_BKCOLOR_DEFAULT   0xC0C0C0               /* Text background color */
#define CHECKBOX_BKCOLOR0_DEFAULT  0x808080           /* Inactive color */
#define CHECKBOX_BKCOLOR1_DEFAULT  GUI_WHITE          /* Active color */
#define CHECKBOX_FGCOLOR0_DEFAULT  0x101010
#define CHECKBOX_FGCOLOR1_DEFAULT  GUI_BLACK
#define CHECKBOX_SPACING_DEFAULT   4
#define CHECKBOX_TEXTCOLOR_DEFAULT GUI_BLACK
#define CHECKBOX_TEXTALIGN_DEFAULT (GUI_TA_LEFT | GUI_TA_VCENTER)
CHECKBOX_PROPS CHECKBOX__DefaultProps = {
	CHECKBOX_FONT_DEFAULT,
	CHECKBOX_BKCOLOR0_DEFAULT,
	CHECKBOX_BKCOLOR1_DEFAULT,
	CHECKBOX_BKCOLOR_DEFAULT,
	CHECKBOX_TEXTCOLOR_DEFAULT,
	CHECKBOX_TEXTALIGN_DEFAULT,
	CHECKBOX_SPACING_DEFAULT,
	CHECKBOX_IMAGE0_DEFAULT,
	CHECKBOX_IMAGE1_DEFAULT
};
static void _Paint(CheckBox_Obj *pObj) {
	SRect RectBox;
	int EffectSize = pObj->pEffect->EffectSize,
		ColorIndex = pObj->Enable();
	if (pObj->Props.BkColor == GUI_INVALID_COLOR)
		GUI.BkColor(WIDGET__GetBkColor(pObj));
	else
		GUI.BkColor(pObj->Props.BkColor);
	GUI_Clear();
	RectBox.x1 = pObj->Props.apBm[0]->XSize - 1 + 2 * EffectSize;
	RectBox.y1 = pObj->Props.apBm[0]->YSize - 1 + 2 * EffectSize;
	WObj::SetUserClipRect(&RectBox);
	GUI.BkColor(pObj->Props.aBkColorBox[ColorIndex]);
	GUI_Clear();
	if (pObj->CurrentState) {
		int Index = (pObj->CurrentState - 1) * 2 + ColorIndex;
		GUI_DrawBitmap(pObj->Props.apBm[Index], EffectSize, EffectSize);
	}
	WIDGET__EFFECT_DrawDownRect(pObj, &RectBox);
	WObj::SetUserClipRect(nullptr);
	if (!pObj->pText)
		return;
	SRect RectText;
	const char *s = pObj->pText;
	WM_GetClientRect(&RectText);
	RectText.x0 += RectBox.x1 + 1 + pObj->Props.Spacing;
	GUI_SetTextMode(DRAWMODE_TRANS);
	GUI.PenColor(pObj->Props.TextColor);
	GUI.Font(pObj->Props.pFont);
	GUI_DispStringInRect(s, &RectText, pObj->Props.Align);
	if (!(pObj->State & WIDGET_STATE_FOCUS))
		return;
	int xSizeText = GUI_GetStringDistX(s);
	int ySizeText = GUI_GetFontSizeY();
	SRect RectFocus = RectText;
	
	switch (pObj->Props.Align & ~(GUI_TA_HORIZONTAL)) {
	
	case GUI_TA_VCENTER:
		RectFocus.y0 = (RectText.y1 - ySizeText) / 2;
		break;
	
	case GUI_TA_BOTTOM:
		RectFocus.y0 = RectText.y1 - ySizeText;
		break;
	
	}
	switch (pObj->Props.Align & ~(GUI_TA_VERTICAL)) {
	case GUI_TA_HCENTER:
		RectFocus.x0 += ((RectText.x1 - RectText.x0) - xSizeText) / 2;
		break;
	case GUI_TA_RIGHT:
		RectFocus.x0 += (RectText.x1 - RectText.x0) - xSizeText;
		break;
	}
	RectFocus.x1 = RectFocus.x0 + xSizeText;
	RectFocus.y1 = RectFocus.y0 + ySizeText;
	
	GUI.PenColor(GUI_BLACK);
	WIDGET__DrawFocusRect(pObj, &RectFocus, 0);
}
static void _OnTouch(CheckBox_Obj *pObj, WM_MESSAGE* pMsg) {
	int Notification = 0;
	int Hit = 0;
	const PidState *pState = (const PidState *)pMsg->Data;
	if (pState) {
		if (!WM_HasCaptured(pObj)) {
			if (pState->Pressed) {
				WM_SetCapture(pObj, 1);
				pObj->CurrentState = (pObj->CurrentState + 1) % pObj->NumStates;
				pObj->Invalidate();
				Notification = WM_NOTIFICATION_CLICKED;
			}
			else {
				Hit = 1;
				Notification = WM_NOTIFICATION_RELEASED;
			}
		}
	}
	else
		Notification = WM_NOTIFICATION_MOVED_OUT;
	
	WM_NotifyParent(pObj, Notification);
	
	if (Hit == 1) {
		GUI_StoreKey(pObj->Id);
	}
}
static void _OnKey(CheckBox_Obj *pObj, WM_MESSAGE* pMsg) {
	if (!pObj->Enable())
		return;
	const WM_KEY_INFO *pKeyInfo = (const WM_KEY_INFO *)pMsg->Data;
	if (pKeyInfo->PressedCnt <= 0)
		return;
	switch (pKeyInfo->Key) {
	case GUI_KEY_SPACE:
		pObj->CurrentState = (pObj->CurrentState + 1) % pObj->NumStates;
		pObj->Invalidate();
		break;
	}
}
static void _CHECKBOX_Callback(WM_MESSAGE* pMsg) {
	auto pObj = (CheckBox_Obj *)pMsg->pWin;
	if (!WIDGET_HandleActive(pObj, pMsg))
		return;
	switch (pMsg->MsgId) {
	case WM_KEY:
		_OnKey(pObj, pMsg);
		break;
	case WM_PAINT:
		_Paint(pObj);
		return;
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		break;
	}
	WM_DefaultProc(pMsg);
}
CheckBox_Obj *CHECKBOX_CreateEx(
	int x0, int y0, int xsize, int ysize, WObj *pParent,
	int WinFlags, int ExFlags, int Id) {
	if ((xsize | ysize) == 0) {
		int EffectSize = Widget::DefaultEffect()->EffectSize;
		if (xsize == 0)
			xsize = CHECKBOX__DefaultProps.apBm[0]->XSize + 2 * EffectSize;
		if (ysize == 0)
			ysize = CHECKBOX__DefaultProps.apBm[0]->YSize + 2 * EffectSize;
	}
	auto pObj = (CheckBox_Obj *)WObj::Create(
		x0, y0, xsize, ysize,
		pParent, WinFlags, _CHECKBOX_Callback,
		sizeof(CheckBox_Obj) - sizeof(WObj));
	if (!pObj) {
		return 0;
	}
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE);
	pObj->Props = CHECKBOX__DefaultProps;
	pObj->NumStates = 2; /* Default behaviour is 2 states: checked and unchecked */
	return pObj;
}
void CHECKBOX_SetFont(CheckBox_Obj *pObj, CFont* pFont) {
	if (!pObj)
		return;
	if (pObj->Props.pFont == pFont)
		return;
	pObj->Props.pFont = pFont;
	pObj->Invalidate();
}
void CHECKBOX_SetTextColor(CheckBox_Obj *pObj, RGBC Color) {
	if (!pObj)
		return;
	if (pObj->Props.TextColor == Color)
		return;
	pObj->Props.TextColor = Color;
	pObj->Invalidate();
}
void CHECKBOX_SetBkColor(CheckBox_Obj *pObj, RGBC Color) {
	if (!pObj)
		return;
	if (pObj->Props.BkColor == Color)
		return;
	pObj->Props.BkColor = Color;
	pObj->Invalidate();
}
void CHECKBOX_SetImage(CheckBox_Obj *pObj, const Bitmap *pBitmap, unsigned int Index) {
	if (!pObj)
		return;
	if (Index < GUI_COUNTOF(pObj->Props.apBm))
		pObj->Props.apBm[Index] = pBitmap;
}
void CHECKBOX_SetSpacing(CheckBox_Obj *pObj, unsigned Spacing) {
	if (!pObj)
		return;
	if (pObj->Props.Spacing == Spacing)
		return;
	pObj->Props.Spacing = Spacing;
	pObj->Invalidate();
}
void CHECKBOX_SetState(CheckBox_Obj *pObj, unsigned State) {
	if (!pObj)
		return;
	if (pObj->NumStates < State)
		return;
	pObj->CurrentState = State;
	pObj->Invalidate();
}
void CHECKBOX_SetText(CheckBox_Obj *pObj, const char *s) {
	if (!pObj || !s)
		return;
	if (GUI__SetText(&pObj->pText, s))
		pObj->Invalidate();
}
void CHECKBOX_SetTextAlign(CheckBox_Obj *pObj, int Align) {
	if (!pObj)
		return;
	if (pObj->Props.Align == Align)
		return;
	pObj->Props.Align = Align;
	pObj->Invalidate();
}
int CHECKBOX_GetState(CheckBox_Obj *pObj) {
	if (!pObj)
		return 0;
	return pObj->CurrentState;
}
bool CHECKBOX_IsChecked(CheckBox_Obj *pObj) {
	return CHECKBOX_GetState(pObj) == 1;
}
void CHECKBOX_SetDefaultImage(const Bitmap *pBitmap, unsigned int Index) {
	if (Index < GUI_COUNTOF(CHECKBOX__DefaultProps.apBm))
		CHECKBOX__DefaultProps.apBm[Index] = pBitmap;
}
void CHECKBOX_SetDefaultSpacing(int Spacing) { CHECKBOX__DefaultProps.Spacing = Spacing; }
void CHECKBOX_SetDefaultTextColor(RGBC Color) { CHECKBOX__DefaultProps.TextColor = Color; }
void CHECKBOX_SetDefaultBkColor(RGBC Color) { CHECKBOX__DefaultProps.BkColor = Color; }
void CHECKBOX_SetDefaultFont(CFont* pFont) { CHECKBOX__DefaultProps.pFont = pFont; }
void CHECKBOX_SetDefaultAlign(int Align) { CHECKBOX__DefaultProps.Align = Align; }
int CHECKBOX_GetDefaultSpacing(void) { return CHECKBOX__DefaultProps.Spacing; }
RGBC CHECKBOX_GetDefaultTextColor(void) { return CHECKBOX__DefaultProps.TextColor; }
RGBC CHECKBOX_GetDefaultBkColor(void) { return CHECKBOX__DefaultProps.BkColor; }
CFont* CHECKBOX_GetDefaultFont(void) { return CHECKBOX__DefaultProps.pFont; }
int CHECKBOX_GetDefaultAlign(void) { return CHECKBOX__DefaultProps.Align; }
#include "chars_pic.inl"
/* Colors */
static const RGBC _aColorDisabled[] = { CHECKBOX_FGCOLOR0_DEFAULT, CHECKBOX_BKCOLOR0_DEFAULT };
static const RGBC _aColorEnabled[]  = { CHECKBOX_FGCOLOR1_DEFAULT, CHECKBOX_BKCOLOR1_DEFAULT };
/* Palettes */
static const LogPalette _PalCheckDisabled = { 2, 0, _aColorDisabled };
static const LogPalette _PalCheckEnabled  = { 2, 0, _aColorEnabled  };
/* Pixel data */
static const uint8_t _acCheck[] = {
	XXXXXXXXXXXXXXXX,XXXXXX__________,
	XXXXXXXXXXXXXXXX,XXXXXX__________,
	XXXXXXXXXXXXXXXX,__XXXX__________,
	XXXXXXXXXXXXXX__,__XXXX__________,
	XXXX__XXXXXX____,__XXXX__________,
	XXXX____XX______,XXXXXX__________,
	XXXX__________XX,XXXXXX__________,
	XXXXXX______XXXX,XXXXXX__________,
	XXXXXXXX__XXXXXX,XXXXXX__________,
	XXXXXXXXXXXXXXXX,XXXXXX__________,
	XXXXXXXXXXXXXXXX,XXXXXX__________
};
/* Bitmaps */
const Bitmap CHECKBOX__abmCheck[2] = {
	{ 11, 11, 2, 1, _acCheck,  &_PalCheckDisabled },
	{ 11, 11, 2, 1, _acCheck,  &_PalCheckEnabled  }
};
/* Pixel data */
static const uint8_t _acCheckDis[] = {
	XXXXXXXXXXXXXXXX,XXXXXX__________,
	XXXXXXXXXXXXXXXX,XXXXXX__________,
	XXXXXXXXXXXXXXXX,__XXXX__________,
	XXXXXXXXXXXXXX__,XXXXXX__________,
	XXXX__XXXXXX__XX,__XXXX__________,
	XXXXXX__XX__XX__,XXXXXX__________,
	XXXX__XX__XX__XX,XXXXXX__________,
	XXXXXX__XX__XXXX,XXXXXX__________,
	XXXXXXXX__XXXXXX,XXXXXX__________,
	XXXXXXXXXXXXXXXX,XXXXXX__________,
	XXXXXXXXXXXXXXXX,XXXXXX__________
};
/* Bitmaps */
static const Bitmap _abmCheck[2] = {
	{ 11, 11, 2, 1, _acCheckDis, &_PalCheckDisabled },
	{ 11, 11, 2, 1, _acCheckDis, &_PalCheckEnabled  }
};
void CHECKBOX_SetNumStates(CheckBox_Obj *pObj, unsigned NumStates) {
	if (!CHECKBOX__DefaultProps.apBm[2])
		CHECKBOX_SetDefaultImage(&_abmCheck[0], 2);
	if (!CHECKBOX__DefaultProps.apBm[3])
		CHECKBOX_SetDefaultImage(&_abmCheck[1], 3);
	if (!pObj)
		return;
	if (NumStates != 2 && NumStates != 3)
		return;
	pObj->Props.apBm[2] = CHECKBOX__DefaultProps.apBm[2];
	pObj->Props.apBm[3] = CHECKBOX__DefaultProps.apBm[3];
	pObj->NumStates = NumStates;
}
