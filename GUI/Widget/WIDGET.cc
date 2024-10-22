#include "Widget.h"
static void _UpdateChildPostions(Widget *pWidget, int Diff) {
	WM__UpdateChildPositions(pWidget, -Diff, -Diff, Diff, Diff);
}
static int _EffectRequiresRedraw(const Widget *pWidget, const SRect *pRect) {
	int EffectSize = pWidget->pEffect->EffectSize;
	SRect rInvalid = pWidget->rInvalid;
	WM__Client2Screen(pWidget, &rInvalid);
	if (pRect->x0 + EffectSize > rInvalid.x0)
		return 1;
	if (pRect->x1 - EffectSize < rInvalid.x1)
		return 1;
	if (pRect->y0 + EffectSize > rInvalid.y0)
		return 1;
	if (pRect->y1 - EffectSize < rInvalid.y1)
		return 1;
	return 0;
}
void WIDGET__RotateRect90(Widget *pWidget, SRect *pDest, const SRect *pRect) {
	int x0 = pRect->x0,
		x1 = pRect->x1,
		XSize = pWidget->rect.x1 - pWidget->rect.x0;
	pDest->x0 = XSize - pRect->y1;
	pDest->x1 = XSize - pRect->y0;
	pDest->y0 = x0;
	pDest->y1 = x1;
}
void WIDGET__GetClientRect(Widget *pWidget, SRect *pRect) {
	if (pWidget->State & WIDGET_STATE_VERTICAL) {
		SRect rect;
		WM_GetClientRect(&rect);
		pRect->x0 = rect.y0;
		pRect->x1 = rect.y1;
		pRect->y0 = rect.x0;
		pRect->y1 = rect.x1;
	}
	else
		WM_GetClientRect(pRect);
}
RGBC WIDGET__GetBkColor(Widget *pWidget) {
	RGBC BkColor = WM_GetBkColor(pWidget->Parent());
	if (BkColor == GUI_INVALID_COLOR)
		BkColor = GUI_WHITE;
	return BkColor;
}
void WIDGET__GetInsideRect(Widget *pWidget, SRect *pRect) {
	WM__GetClientRectWin(pWidget, pRect);
	*pRect /= pWidget->pEffect->EffectSize;
}
int WIDGET__GetXSize(const Widget *pWidget) {
	return pWidget->State & WIDGET_STATE_VERTICAL ?
		pWidget->rect.y1 - pWidget->rect.y0 :
		pWidget->rect.x1 - pWidget->rect.x0;
}
int WIDGET__GetYSize(const Widget *pWidget) {
	return pWidget->State & WIDGET_STATE_VERTICAL ?
		pWidget->rect.x1 - pWidget->rect.x0 :
		pWidget->rect.y1 - pWidget->rect.y0;
}
int WIDGET__GetWindowSizeX(Widget *pWidget) {
	if (pWidget->State & WIDGET_STATE_VERTICAL)
		return pWidget->SizeY();
	return pWidget->SizeX();
}
void WIDGET_SetState(Widget *pWidget, int State) {
	if (State == pWidget->State)
		return;
	pWidget->State = State;
	pWidget->Invalidate();
}
int WIDGET_GetState(Widget *pWidget) {
	if (!pWidget)
		return 0;
	return pWidget->State;
}
void WIDGET_OrState(Widget *pWidget, int State) {
	if (!pWidget)
		return;
	if (State == (pWidget->State & State))
		return;
	pWidget->State |= State;
	pWidget->Invalidate();
}
void WIDGET_AndState(Widget *pWidget, int Mask) {
	if (!pWidget)
		return;
	uint16_t StateNew = pWidget->State & (~Mask);
	if (pWidget->State == StateNew)
		return;
	pWidget->State = StateNew;
	pWidget->Invalidate();
}
void WIDGET__Init(Widget *pWidget, int Id, uint16_t State) {
	pWidget->pEffect = Widget::pEffectDefault;
	pWidget->State = State;
	pWidget->Id = Id;
}
int WIDGET_HandleActive(Widget *pWidget, WM_MESSAGE *pMsg) {
	switch (pMsg->MsgId) {
	case WM_WIDGET_SET_EFFECT:
	{
		int Diff = pWidget->pEffect->EffectSize;
		pWidget->pEffect = (const Widget::Effect *)pMsg->Data;
		Diff -= pWidget->pEffect->EffectSize;
		_UpdateChildPostions(pWidget, Diff);
		pWidget->Invalidate();
		return 0;
	}
	case WM_GET_ID:
		pMsg->Data = pWidget->Id;
		return 0;
	case WM_PID_STATE_CHANGED:
		if (pWidget->State & WIDGET_STATE_FOCUSSABLE) {
			auto pInfo = (const WM_PID_STATE_CHANGED_INFO *)pMsg->Data;
			if (pInfo->State)
				pWidget->Focus();
		}
		break;
	case WM_TOUCH_CHILD:
	{
		const WM_MESSAGE *pMsgOrg = (const WM_MESSAGE *)pMsg->Data;
		const PidState *pState = (const PidState *)pMsgOrg->Data;
		if (!pState)
			break;
		if (!pState->Pressed)
			break;
		pWidget->BringToTop();
		return 0;
		
	}
	case WM_SET_ID:
		pWidget->Id = (int)pMsg->Data;
		return 0;
	case WM_SET_FOCUS:
	{
		int Notification;
		if (pMsg->Data == 1) {
			WIDGET_SetState(pWidget, pWidget->State | WIDGET_STATE_FOCUS);
			Notification = WM_NOTIFICATION_GOT_FOCUS;
		}
		else {
			WIDGET_SetState(pWidget, pWidget->State & ~WIDGET_STATE_FOCUS);
			Notification = WM_NOTIFICATION_LOST_FOCUS;
		}
		WM_NotifyParent(pWidget, Notification);
		pMsg->Data = 0;
		return 0;
	}
	case WM_GET_ACCEPT_FOCUS:
		pMsg->Data = (pWidget->State & WIDGET_STATE_FOCUSSABLE) ? 1 : 0;
		return 0;
	case WM_GET_INSIDE_RECT:
		WIDGET__GetInsideRect(pWidget, (SRect *)pMsg->Data);
		return 0;
	}
	return 1;
}
void WIDGET__SetScrollState(Widget *pWidget, const WM_SCROLL_STATE *pVState, const WM_SCROLL_STATE *pHState) {
	WM_SetScrollState(WM_GetDialogItem(pWidget, GUI_ID_VSCROLL), pVState);
	WM_SetScrollState(WM_GetDialogItem(pWidget, GUI_ID_HSCROLL), pHState);
}
void WIDGET__DrawFocusRect(Widget *pWidget, const SRect *pRect, int Dist) {
	if (pWidget->State & WIDGET_STATE_VERTICAL) {
		SRect rect;
		WIDGET__RotateRect90(pWidget, &rect, pRect);
		pRect = &rect;
	}
	GUI_DrawFocusRect(pRect, Dist);
}
void WIDGET__DrawVLine(Widget *pWidget, int x, int y0, int y1) {
	if (pWidget->State & WIDGET_STATE_VERTICAL) {
		SRect r0, r1;
		r0.x0 = x;
		r0.x1 = x;
		r0.y0 = y0;
		r0.y1 = y1;
		WIDGET__RotateRect90(pWidget, &r1, &r0);
		GUI_DrawHLine(r1.y0, r1.x0, r1.x1);
	}
	else
		GUI_DrawVLine(x, y0, y1);
}
void WIDGET__FillRectEx(Widget *pWidget, const SRect *pRect) {
	if (pWidget->State & WIDGET_STATE_VERTICAL) {
		SRect r;
		WIDGET__RotateRect90(pWidget, &r, pRect);
		pRect = &r;
	}
	GUI_FillRectEx(pRect);
}
void WIDGET__EFFECT_DrawDownRect(Widget *pWidget, SRect *pRect) {
	SRect rect;
	if (pRect == nullptr) {
		WM_GetClientRect(&rect);
		pRect = &rect;
	}
	if (pWidget->State & WIDGET_STATE_VERTICAL) {
		WIDGET__RotateRect90(pWidget, &rect, pRect);
		pRect = &rect;
	}
	if (_EffectRequiresRedraw(pWidget, pRect))
		pWidget->pEffect->DrawDownRect(*pRect);
}
void WIDGET__EFFECT_DrawDown(Widget *pWidget) {
	WIDGET__EFFECT_DrawDownRect(pWidget, nullptr);
}
void WIDGET__EFFECT_DrawUpRect(Widget *pWidget, SRect *pRect) {
	SRect rect;
	if (pWidget->State & WIDGET_STATE_VERTICAL) {
		WIDGET__RotateRect90(pWidget, &rect, pRect);
		pRect = &rect;
	}
	if (_EffectRequiresRedraw(pWidget, pRect))
		pWidget->pEffect->DrawUpRect(*pRect);
}
void WIDGET_SetWidth(Widget *pWidget, int Width) {
	if (pWidget->State & WIDGET_STATE_VERTICAL)
		pWidget->SizeX(Width);
	else
		pWidget->SizeY(Width);
}
void WIDGET__FillStringInRect(const char *pText, const SRect *pFillRect, const SRect *pTextRectMax, const SRect *pTextRectAct) {
	/* Check if we have anything to do at all ... */
	if (GUI.pClipRect_HL) {
		SRect r = *pFillRect;
		WM_ADDORG(r.x0, r.y0);
		WM_ADDORG(r.x1, r.y1);
		if (!(GUI.pClipRect_HL && r))
			return;
	}
	if (!pText) {
		GUI_ClearRectEx(pFillRect);
		return;
	}
	if (!*pText) {
		GUI_ClearRectEx(pFillRect);
		return;
	}
	auto &&rText = *pTextRectMax & *pTextRectAct;
	GUI_ClearRect(pFillRect->x0, pFillRect->y0, pFillRect->x1, rText.y0 - 1); /* Top */
	GUI_ClearRect(pFillRect->x0, rText.y0, rText.x0 - 1, rText.y1);     /* Left */
	GUI_ClearRect(rText.x1 + 1, rText.y0, pFillRect->x1, rText.y1);     /* Right */
	GUI_ClearRect(pFillRect->x0, rText.y1 + 1, pFillRect->x1, pFillRect->y1);/* Bottom */
	auto pOldClipRect = WObj::SetUserClipRect(pTextRectMax);
	GUI_SetTextMode(DRAWMODE_NORMAL);
	GUI_DispStringAt(pText, pTextRectAct->x0, pTextRectAct->y0);
	WObj::SetUserClipRect(pOldClipRect);
}
void WIDGET_SetEffect(Widget *pWidget, const Widget::Effect *pEffect) {
	WM_MESSAGE msg;
	msg.pWinSrc = 0;
	msg.MsgId = WM_WIDGET_SET_EFFECT;
	msg.Data = (size_t)pEffect;
	pWidget->SendMessage(&msg);
}
