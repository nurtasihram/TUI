#include "WM.h"
WM_CRITICAL_HANDLE WM__CHWinModal;
WM_CRITICAL_HANDLE WM__CHWinLast;
bool WM__IsInModalArea(WObj *pWin) {
	if (!pWin)
		return false;
	if (!WM__CHWinModal.pWin)
		return true;
	if (WM__CHWinModal.pWin == pWin)
		return true;
	return WM__IsAncestor(pWin, WM__CHWinModal.pWin);
}
void WM__SendPIDMessage(WObj *pWin, WM_MESSAGE* pMsg) {
	WM_MESSAGE msg = *pMsg;
	WM__SendMessageIfEnabled(pWin, &msg);
	msg.pWinSrc = pWin;
	msg.MsgId = WM_TOUCH_CHILD;
	while (*pWin) {
		pWin = pWin->Parent();
		if (!pWin)
			continue;
		msg.Data = (size_t)pMsg; /* Needs to be set for each window, as callback is allowed to modify it */
		WM__SendMessageIfEnabled(pWin, &msg); /* Send message to the ancestors */
	}
}
void WM__SendTouchMessage(WObj *pWin, WM_MESSAGE* pMsg) {
	if (auto pState = (PidState *)pMsg->Data)
		*pState -= pWin->rect.left_top();
	WM__SendPIDMessage(pWin, pMsg);
}

bool WObj::HandlePID() {
	bool r = false;
	
	WM_MESSAGE msg;
	WM_CRITICAL_HANDLE CHWin;
	PidState State, StateNew;
	GUI_PID_GetState(&StateNew);
	WM__AddCriticalHandle(&CHWin);
	
	if ((WM_PID__StateLast.x != StateNew.x) ||
		(WM_PID__StateLast.y != StateNew.y) ||
		(WM_PID__StateLast.Pressed != StateNew.Pressed)) {
		GUI_CURSOR_SetPosition(StateNew.x, StateNew.y);
		CHWin.pWin = WM__pCapture ? WM__pCapture : WObj::ScreenToWin(StateNew);
		
		if (WM__IsInModalArea(CHWin.pWin)) {
			if (WM_PID__StateLast.Pressed != StateNew.Pressed && CHWin.pWin) {
				WObj* pWin = CHWin.pWin;
				
				WM_PID_STATE_CHANGED_INFO Info;
				Info.State = StateNew.Pressed;
				Info.StatePrev = WM_PID__StateLast.Pressed;
				Info.x = StateNew.x - pWin->rect.x0;
				Info.y = StateNew.y - pWin->rect.y0;
				
				msg.Data = (size_t)&Info;
				msg.MsgId = WM_PID_STATE_CHANGED;
				
				WM__SendMessageIfEnabled(CHWin.pWin, &msg);
			}
			if (WM_PID__StateLast.Pressed | StateNew.Pressed) {    /* Only if pressed or just released */
				msg.MsgId = WM_TOUCH;
				r = true;
				if (WM__CHWinLast.pWin != CHWin.pWin)
					if (WM__CHWinLast.pWin != nullptr) {
						if (StateNew.Pressed)
							msg.Data = 0;
						else {
							State.x = WM_PID__StateLast.x;
							State.y = WM_PID__StateLast.y;
							State.Pressed = 0;
							msg.Data = (size_t)&State;
						}
						WM__SendTouchMessage(WM__CHWinLast.pWin, &msg);
						WM__CHWinLast.pWin = nullptr;
					}
				if (CHWin.pWin) {
					State = StateNew;
					if (State.Pressed)
						WM__CHWinLast.pWin = CHWin.pWin;
					else {
						if (WM__CaptureReleaseAuto)
							WM_ReleaseCapture();
						WM__CHWinLast.pWin = nullptr;
					}
					msg.Data = (size_t)&State;
					WM__SendTouchMessage(CHWin.pWin, &msg);
				}
			}
			else if (CHWin.pWin)
				if (CHWin.pWin->Enable()) {
					State = StateNew;
					msg.MsgId = WM_MOUSEOVER;
					msg.Data = (size_t)&State;
					WM__SendTouchMessage(CHWin.pWin, &msg);
				}
		}
		WM_PID__StateLast = StateNew;
	}
	
	WM__RemoveCriticalHandle(&CHWin);
	return r;
}
