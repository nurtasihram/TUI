#include "Button.h"
#define BUTTON_FONT_DEFAULT       &GUI_Font13_1  /* Define default fonts */
#define BUTTON_3D_MOVE_X          1
#define BUTTON_3D_MOVE_Y          1
#define BUTTON_BKCOLOR0_DEFAULT   0xAAAAAA       /* Define colors */
#define BUTTON_BKCOLOR1_DEFAULT   GUI_WHITE
#define BUTTON_BKCOLOR2_DEFAULT   GUI_LIGHTGRAY
#define BUTTON_TEXTCOLOR0_DEFAULT GUI_BLACK
#define BUTTON_TEXTCOLOR1_DEFAULT GUI_BLACK
#define BUTTON_TEXTCOLOR2_DEFAULT GUI_DARKGRAY
#define BUTTON_REACT_ON_LEVEL     0
#define BUTTON_ALIGN_DEFAULT      GUI_TA_HCENTER | GUI_TA_VCENTER
BUTTON_PROPS BUTTON__DefaultProps = {
	BUTTON_BKCOLOR0_DEFAULT,
	BUTTON_BKCOLOR1_DEFAULT,
	BUTTON_BKCOLOR2_DEFAULT,
	BUTTON_TEXTCOLOR0_DEFAULT,
	BUTTON_TEXTCOLOR1_DEFAULT,
	BUTTON_TEXTCOLOR2_DEFAULT,
	BUTTON_FONT_DEFAULT,
	BUTTON_ALIGN_DEFAULT
};
static void _Paint(Button *pObj) {
	const char *s = nullptr;
	unsigned int Index;
	int State = pObj->State;
	int PressedState = (State & BUTTON_STATE_PRESSED) ? 1 : 0;
	int ColorIndex = pObj->Enable() ? PressedState : 2;
	GUI.Font(pObj->Props.pFont);
	if (pObj->pText)
		s = (const char *)pObj->pText;
	SRect rClient;
	WM_GetClientRect(&rClient);
	SRect rInside = rClient;
	int EffectSize;
	if (PressedState == 0) {
		pObj->pEffect->DrawUp();
		EffectSize = pObj->pEffect->EffectSize;
	}
	else {
		GUI.PenColor(0x000000);
		GUI_DrawRect(rClient.y0, rClient.x0, rClient.x1, rClient.y1);
		EffectSize = 1;
	}
	rInside /= EffectSize;
	GUI.BkColor(pObj->Props.aBkColor[ColorIndex]);
	GUI.PenColor(pObj->Props.aTextColor[ColorIndex]);
	WObj::SetUserClipRect(&rInside);
	GUI_Clear();
	Index = ColorIndex < 2 ?
		(pObj->apDrawObj[BUTTON_BI_PRESSED] && PressedState) ? BUTTON_BI_PRESSED : BUTTON_BI_UNPRESSED :
		pObj->apDrawObj[BUTTON_BI_DISABLED] ? BUTTON_BI_DISABLED : BUTTON_BI_UNPRESSED;
	GUI_DRAW__Draw(pObj->apDrawObj[Index], 0, 0);
	SRect r = rInside;
	if (PressedState)
		r += Point(BUTTON_3D_MOVE_X, BUTTON_3D_MOVE_Y);
	GUI_SetTextMode(DRAWMODE_TRANS);
	GUI_DispStringInRect(s, &r, pObj->Props.Align);
	if (State & BUTTON_STATE_FOCUS) {
		GUI.PenColor(GUI_BLACK);
		GUI_DrawFocusRect(&rClient, 2);
	}
	WObj::SetUserClipRect(nullptr);
}
static void _Delete(Button *pObj) {
	GUI_ALLOC_Free(pObj->pText);
	pObj->pText = nullptr;
	GUI_ALLOC_Free(pObj->apDrawObj[0]);
	pObj->apDrawObj[0] = nullptr;
	GUI_ALLOC_Free(pObj->apDrawObj[1]);
	pObj->apDrawObj[1] = nullptr;
}
static void _ButtonPressed(Button *pObj) {
	WIDGET_OrState(pObj, BUTTON_STATE_PRESSED);
	if (pObj->Status & WC_VISIBLE)
		WM_NotifyParent(pObj, WM_NOTIFICATION_CLICKED);
}
static void _ButtonReleased(Button *pObj, int Notification) {
	WIDGET_AndState(pObj, BUTTON_STATE_PRESSED);
	if (pObj->Status & WC_VISIBLE)
		WM_NotifyParent(pObj, Notification);
	if (Notification == WM_NOTIFICATION_RELEASED) {
		GUI_StoreKey(pObj->Id);
	}
}
static void _OnTouch(Button *pObj, WM_MESSAGE *pMsg) {
	const PidState *pState = (const PidState *)pMsg->Data;
#if BUTTON_REACT_ON_LEVEL
	if (!pState)
		_ButtonReleased(pObj, WM_NOTIFICATION_MOVED_OUT);
#else
	if (pState) {
		if (pState->Pressed) {
			if ((pObj->State & BUTTON_STATE_PRESSED) == 0)
				_ButtonPressed(pObj);
		}
		else if (pObj->State & BUTTON_STATE_PRESSED)
			_ButtonReleased(pObj, WM_NOTIFICATION_RELEASED);
	}
	else
		_ButtonReleased(pObj, WM_NOTIFICATION_MOVED_OUT);
#endif
}
#if BUTTON_REACT_ON_LEVEL
static void _OnPidStateChange(Button *pObj, WM_MESSAGE *pMsg) {
	const WM_PID_STATE_CHANGED_INFO *pState = (const WM_PID_STATE_CHANGED_INFO *)pMsg->Data;
	if ((pState->StatePrev == 0) && (pState->State == 1)) {
		if ((pObj->State & BUTTON_STATE_PRESSED) == 0)
			_ButtonPressed(pObj);
	}
	else if ((pState->StatePrev == 1) && (pState->State == 0)) {
		if (pObj->State & BUTTON_STATE_PRESSED)
			_ButtonReleased(pObj, WM_NOTIFICATION_RELEASED);
	}
}
#endif
void BUTTON_Callback(WM_MESSAGE *pMsg) {
	Button *pObj = (Button *)pMsg->pWin;
	/* Let widget handle the standard messages */
	if (!WIDGET_HandleActive(pObj, pMsg))
		return;
	switch (pMsg->MsgId) {
#if BUTTON_REACT_ON_LEVEL
	case WM_PID_STATE_CHANGED:
		_OnPidStateChange(pObj, pMsg);
		return;
#endif
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		return;
	case WM_PAINT:
		_Paint(pObj);
		return;
	case WM_DELETE:
		_Delete(pObj);
		break;
	case WM_KEY:
	{
		const WM_KEY_INFO *ki = (const WM_KEY_INFO *)pMsg->Data;
		int PressedCnt = ki->PressedCnt;
		int Key = ki->Key;
		switch (Key) {
		case ' ':
			if (PressedCnt > 0) /* Key pressed? */
				_ButtonPressed(pObj);
			else
				_ButtonReleased(pObj, WM_NOTIFICATION_RELEASED);
			return;
		}
		break;
	}
	}
	WM_DefaultProc(pMsg);
}
void BUTTON_SetText(Button *pObj, const char *s) {
	if (!pObj)
		return;
	if (GUI__SetText(&pObj->pText, s))
		BUTTON_Invalidate(pObj);
}
void BUTTON_SetFont(Button *pObj, CFont *pfont) {
	if (!pObj)
		return;
	pObj->Props.pFont = pfont;
	BUTTON_Invalidate(pObj);
}
void BUTTON_SetBkColor(Button *pObj, unsigned int Index, RGBC Color) {
	if (!pObj)
		return;
	if (Index > 2)
		return;
	pObj->Props.aBkColor[Index] = Color;
	BUTTON_Invalidate(pObj);
}
void BUTTON_SetTextColor(Button *pObj, unsigned int Index, RGBC Color) {
	if (!pObj)
		return;
	if (Index > 2)
		return;
	pObj->Props.aTextColor[Index] = Color;
	BUTTON_Invalidate(pObj);
}
RGBC BUTTON_GetTextColor(Button *pObj, unsigned int Index) {
	if (!pObj)
		return 0;
	if (Index > 2)
		return 0;
	return pObj->Props.aTextColor[Index];
}
void BUTTON_SetState(Button *pObj, int State) {
	WIDGET_SetState(pObj, State);
}
void BUTTON_SetPressed(Button *pObj, int State) {
	if (State)
		WIDGET_OrState(pObj, BUTTON_STATE_PRESSED);
	else
		WIDGET_AndState(pObj, BUTTON_STATE_PRESSED);
}
void BUTTON_SetFocussable(Button *pObj, int State) {
	if (State)
		WIDGET_OrState(pObj, WIDGET_STATE_FOCUSSABLE);
	else
		WIDGET_AndState(pObj, WIDGET_STATE_FOCUSSABLE);
}
void BUTTON_SetDrawObj(Button *pObj, int Index, GUI_DRAW *pDrawObj) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->apDrawObj))
		return;
	GUI_ALLOC_Free(pObj->apDrawObj[Index]);
	pObj->apDrawObj[Index] = pDrawObj;
	BUTTON_Invalidate(pObj);
}
void BUTTON_SetTextAlign(Button *pObj, int Align) {
	if (!pObj)
		return;
	pObj->Props.Align = Align;
	BUTTON_Invalidate(pObj);
}
Button *BUTTON_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id) {
	auto pObj = (Button *)WObj::Create(
		x0, y0, xsize, ysize,
		pParent, WinFlags, BUTTON_Callback,
		sizeof(Button) - sizeof(WObj));
	if (!pObj) {
		return 0;
	}
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE);
	pObj->Props = BUTTON__DefaultProps;
	return pObj;
}
Button *BUTTON_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb) {
	Button *hThis = BUTTON_CreateEx(
		pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,
		pCreateInfo->xSize, pCreateInfo->ySize,
		pParent, 0, pCreateInfo->Flags, pCreateInfo->Id);
	BUTTON_SetText(hThis, pCreateInfo->pName);
	return hThis;
}
void BUTTON_SetDefaultFont(CFont *pFont) {
	BUTTON__DefaultProps.pFont = pFont;
}
void BUTTON_SetDefaultTextColor(RGBC Color, unsigned Index) {
	if (Index < GUI_COUNTOF(BUTTON__DefaultProps.aTextColor))
		BUTTON__DefaultProps.aTextColor[Index] = Color;
}
void BUTTON_SetDefaultBkColor(RGBC Color, unsigned Index) {
	if (Index < GUI_COUNTOF(BUTTON__DefaultProps.aBkColor))
		BUTTON__DefaultProps.aBkColor[Index] = Color;
}
void BUTTON_SetDefaultTextAlign(int Align) {
	BUTTON__DefaultProps.Align = Align;
}
CFont *BUTTON_GetDefaultFont(void) {
	return BUTTON__DefaultProps.pFont;
}
RGBC BUTTON_GetDefaultTextColor(unsigned Index) {
	return Index < GUI_COUNTOF(BUTTON__DefaultProps.aTextColor) ?
		BUTTON__DefaultProps.aTextColor[Index] : GUI_INVALID_COLOR;
}
RGBC BUTTON_GetDefaultBkColor(unsigned Index) {
	return Index < GUI_COUNTOF(BUTTON__DefaultProps.aBkColor) ?
		BUTTON__DefaultProps.aBkColor[Index] : GUI_INVALID_COLOR;
}
int BUTTON_GetDefaultTextAlign(void) {
	return BUTTON__DefaultProps.Align;
}
RGBC BUTTON_GetBkColor(Button *pObj, unsigned int Index) {
	if (!pObj)
		return GUI_INVALID_COLOR;
	if (Index >= 2)
		return GUI_INVALID_COLOR;
	return pObj->Props.aBkColor[Index];
}
CFont *BUTTON_GetFont(Button *pObj) {
	if (!pObj)
		return nullptr;
	return pObj->Props.pFont;
}
void BUTTON_GetText(Button *pObj, char *pBuffer, int MaxLen) {
	if (!pObj)
		return;
	if (!pObj->pText) {
		pBuffer[0] = 0;
		return;
	}
	const char *pText = pObj->pText;
	int Len = (int)GUI__strlen(pText);
	if (Len > MaxLen - 1)
		Len = MaxLen - 1;
	GUI__memcpy((void *)pBuffer, (const void *)pText, Len);
	pBuffer[Len] = 0;
}
bool BUTTON_IsPressed(Button *pObj) {
	if (!pObj)
		return false;
	return (pObj->State & BUTTON_STATE_PRESSED) != 0;
}
