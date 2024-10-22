#include "SLIDER.h"
#define SLIDER_USE_3D           1
#define SLIDER_BKCOLOR0_DEFAULT 0xc0c0c0
#define SLIDER_BKCOLOR1_DEFAULT GUI_WHITE
#define SLIDER_COLOR0_DEFAULT   0xc0c0c0
#define SLIDER_COLOR1_DEFAULT   GUI_BLACK
static RGBC _DefaultBkColor = SLIDER_BKCOLOR0_DEFAULT;
static void _Paint(Slider_Obj *pObj) {
	SRect r, rFocus;
	WIDGET__GetClientRect(pObj, &rFocus);
	r = rFocus / 1;
	int NumTicks = pObj->NumTicks;
	int xsize = r.x1 - r.x0 + 1 - pObj->Width;
	int x0 = r.x0 + pObj->Width / 2;
	int Range = pObj->Max - pObj->Min;
	if (Range == 0)
		Range = 1;
	if (pObj->aBkColor[0] == GUI_INVALID_COLOR)
		GUI.BkColor(WIDGET__GetBkColor(pObj));
	else
		GUI.BkColor(pObj->aBkColor[0]);
	GUI_Clear();
	SRect rSlider = r;
	rSlider.y0 = 5;
	rSlider.x0 = x0 + (uint32_t)xsize * (uint32_t)(pObj->v - pObj->Min) / Range - pObj->Width / 2;
	rSlider.x1 = rSlider.x0 + pObj->Width;
	SRect rSlot;
	rSlot.x0 = x0;
	rSlot.x1 = x0 + xsize;
	rSlot.y0 = (rSlider.y0 + rSlider.y1) / 2 - 1;
	rSlot.y1 = rSlot.y0 + 3;
	WIDGET__EFFECT_DrawDownRect(pObj, &rSlot);
	if (NumTicks < 0) {
		NumTicks = Range + 1;
		if (NumTicks > (xsize / 5))
			NumTicks = 11;
	}
	if (NumTicks > 1) {
		GUI.PenColor(GUI_BLACK);
		int i;
		for (i = 0; i < NumTicks; i++)
			WIDGET__DrawVLine(pObj, x0 + xsize * i / (NumTicks - 1), 1, 3);
	}
	GUI.PenColor(pObj->aColor[0]);
	WIDGET__FillRectEx(pObj, &rSlider);
	GUI.PenColor(GUI_BLACK);
	WIDGET__EFFECT_DrawUpRect(pObj, &rSlider);
	if (pObj->State & WIDGET_STATE_FOCUS) {
		GUI.PenColor(GUI_BLACK);
		WIDGET__DrawFocusRect(pObj, &rFocus, 0);
	}
}
static void _SliderPressed(Slider_Obj *pObj) {
	WIDGET_OrState(pObj, SLIDER_STATE_PRESSED);
	if (pObj->Status & WC_VISIBLE)
		WM_NotifyParent(pObj, WM_NOTIFICATION_CLICKED);
}
static void _SliderReleased(Slider_Obj *pObj) {
	WIDGET_AndState(pObj, SLIDER_STATE_PRESSED);
	if (pObj->Status & WC_VISIBLE)
		WM_NotifyParent(pObj, WM_NOTIFICATION_RELEASED);
}
static void _OnTouch(Slider_Obj *pObj, WM_MESSAGE *pMsg) {
	const PidState *pState = (const PidState *)pMsg->Data;
	if (!pState)
		return;
	if (!pState->Pressed) {
		if (pObj->State & SLIDER_STATE_PRESSED)
			_SliderReleased(pObj);
		return;
	}
	int Range = (pObj->Max - pObj->Min);
	int x0 = 1 + pObj->Width / 2;  /* 1 pixel focus rectangle + width of actual slider */
	int x = ((pObj->State & WIDGET_STATE_VERTICAL) ? pState->y : pState->x) - x0;
	int xsize = WIDGET__GetWindowSizeX(pObj) - 2 * x0;
	int Sel;
	if (x <= 0)
		Sel = pObj->Min;
	else if (x >= xsize)
		Sel = pObj->Max;
	else {
		/* Make sure we do not divide by 0, even though xsize should never be 0 in this case anyhow */
		int Div = xsize ? xsize : 1;
		Sel = pObj->Min + ((uint32_t)Range * (uint32_t)x + Div / 2) / Div;
	}
	if (pObj->Focussable())
		pObj->Focus();
	WM_SetCapture(pObj, 1);
	SLIDER_SetValue(pObj, Sel);
	if ((pObj->State & SLIDER_STATE_PRESSED) == 0)
		_SliderPressed(pObj);
}
static void  _OnKey(Slider_Obj *pObj, WM_MESSAGE *pMsg) {
	const WM_KEY_INFO *pKeyInfo = (const WM_KEY_INFO *)pMsg->Data;
	int Key = pKeyInfo->Key;
	if (pKeyInfo->PressedCnt <= 0)
		return;
	switch (Key) {
	case GUI_KEY_RIGHT:
		SLIDER_Inc(pObj);
		break;
	case GUI_KEY_LEFT:
		SLIDER_Dec(pObj);
		break;
	default:
		return;
	}
}
static void _SLIDER_Callback(WM_MESSAGE *pMsg) {
	Slider_Obj *pObj = (Slider_Obj *)pMsg->pWin;
	if (WIDGET_HandleActive(pObj, pMsg) == 0)
		return;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		_Paint(pObj);
		return;
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		break;
	case WM_KEY:
		_OnKey(pObj, pMsg);
		break;
	}
	WM_DefaultProc(pMsg);
}
Slider_Obj *SLIDER_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent,
							  int WinFlags, int ExFlags, int Id) {
	auto pObj = (Slider_Obj *)WObj::Create(
		x0, y0, xsize, ysize, pParent, WinFlags,
		_SLIDER_Callback, sizeof(Slider_Obj) - sizeof(WObj));
	if (!pObj) 
		return 0;
	uint16_t InitState = WIDGET_STATE_FOCUSSABLE;
	if (ExFlags & SLIDER_CF_VERTICAL)
		InitState |= WIDGET_CF_VERTICAL;
	WIDGET__Init(pObj, Id, InitState);
	pObj->aBkColor[0] = _DefaultBkColor;
	pObj->aBkColor[1] = SLIDER_BKCOLOR1_DEFAULT;
	pObj->aColor[0] = SLIDER_COLOR0_DEFAULT;
	pObj->aColor[1] = SLIDER_COLOR1_DEFAULT;
	pObj->Width = 8;
	pObj->Max = 100;
	pObj->Min = 0;
	pObj->NumTicks = -1;
	return pObj;
}
Slider_Obj *SLIDER_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb) {
	return SLIDER_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0, pCreateInfo->xSize, pCreateInfo->ySize,
						   pParent, 0, pCreateInfo->Flags, pCreateInfo->Id);
}
void SLIDER_Dec(Slider_Obj *pObj) {
	if (!pObj)
		return;
	if (pObj->v <= pObj->Min)
		return;
	pObj->v--;
	pObj->Invalidate();
	WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
}
void SLIDER_Inc(Slider_Obj *pObj) {
	if (!pObj)
		return;
	if (pObj->v >= pObj->Max)
		return;
	pObj->v++;
	pObj->Invalidate();
	WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
}
void SLIDER_SetWidth(Slider_Obj *pObj, int Width) {
	if (!pObj)
		return;
	if (pObj->Width == Width)
		return;
	pObj->Width = Width;
	pObj->Invalidate();
}
void SLIDER_SetValue(Slider_Obj *pObj, int v) {
	if (!pObj)
		return;
	if (v < pObj->Min)
		v = pObj->Min;
	if (v > pObj->Max)
		v = pObj->Max;
	if (pObj->v == v)
		return;
	pObj->v = v;
	pObj->Invalidate();
	WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
}
void SLIDER_SetRange(Slider_Obj *pObj, int Min, int Max) {
	if (!pObj)
		return;
	if (Max < Min)
		Max = Min;
	pObj->Min = Min;
	pObj->Max = Max;
	if (pObj->v < Min)
		pObj->v = Min;
	if (pObj->v > Max)
		pObj->v = Max;
	pObj->Invalidate();
}
void SLIDER_SetNumTicks(Slider_Obj *pObj, int NumTicks) {
	if (!pObj)
		return;
	if (NumTicks < 0)
		return;
	pObj->NumTicks = NumTicks;
	pObj->Invalidate();
}
void SLIDER_SetBkColor(Slider_Obj *pObj, RGBC Color) {
	if (!pObj)
		return;
	pObj->aBkColor[0] = Color;
	pObj->Invalidate();
}
void SLIDER_SetDefaultBkColor(RGBC Color) {
	_DefaultBkColor = Color;
}
int SLIDER_GetValue(Slider_Obj *pObj) {
	if (!pObj)
		return 0;
	return pObj->v;
}
