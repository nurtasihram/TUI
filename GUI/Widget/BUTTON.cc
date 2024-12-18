#include "Button.h"

Button::Property Button::DefaultProps;

#pragma region Callbacks
void Button::_Pressed() {
	if (!(State & BUTTON_STATE_PRESSED)) {
		State |= BUTTON_STATE_PRESSED;
		Invalidate();
	}
	if (Status & WC_VISIBLE)
		NotifyParent(WM_NOTIFICATION_CLICKED);
}
void Button::_Released(int Notification) {
	if (State & BUTTON_STATE_PRESSED) {
		State &= ~BUTTON_STATE_PRESSED;
		Invalidate();
	}
	if (Status & WC_VISIBLE)
		NotifyParent(Notification);
	if (Notification == WM_NOTIFICATION_RELEASED)
		GUI_StoreKey(this->Id);
}

void Button::_OnPaint() const {
	bool bPressed = State & BUTTON_STATE_PRESSED;
	auto &&rClient = ClientRect();
	int EffectSize = 1;
	if (bPressed)
		DrawDown();
	else {
		DrawUp();
		EffectSize = this->EffectSize();
	}
	auto ColorIndex = Enable() ? bPressed : 2;
	GUI.BkColor(Props.aBkColor[ColorIndex]);
	GUI.PenColor(Props.aTextColor[ColorIndex]);
	auto &&rInside = rClient / EffectSize;
	WObj::SetUserClipRect(&rInside);
	GUI.Clear();
	if (auto pDraw =
		apDrawObj[ColorIndex < 2 ?
			apDrawObj[BUTTON_BI_PRESSED] && bPressed ? BUTTON_BI_PRESSED : BUTTON_BI_UNPRESSED :
			apDrawObj[BUTTON_BI_DISABLED] ? BUTTON_BI_DISABLED : BUTTON_BI_UNPRESSED
		])
		pDraw->Paint(InsideRectAbs());
	if (text) {
		GUI.Font(Props.pFont);
		GUI.TextMode(DRAWMODE_TRANS);
		GUI_DispStringInRect(
			text,
			bPressed ? rInside + Point(EffectSize) : rInside,
			Props.Align);
	}
	if (State & BUTTON_STATE_FOCUS) {
		GUI.PenColor(RGB_BLACK);
		GUI.OutlineFocus(rClient, 2);
	}
	WObj::SetUserClipRect(nullptr);
}
void Button::_OnTouch(const PidState *pState) {
	if (pState) {
		if (pState->Pressed) {
			if (!(State & BUTTON_STATE_PRESSED))
				_Pressed();
		}
		else if (State & BUTTON_STATE_PRESSED)
			_Released(WM_NOTIFICATION_RELEASED);
	}
	else
		_Released(WM_NOTIFICATION_MOVED_OUT);
}
void Button::_OnPidStateChange(const WM_PID_STATE_CHANGED_INFO *pState) {
	if ((pState->StatePrev == 0) && (pState->State == 1)) {
		if (!(State & BUTTON_STATE_PRESSED))
			_Pressed();
	}
	else if ((pState->StatePrev == 1) && (pState->State == 0)) {
		if (State & BUTTON_STATE_PRESSED)
			_Released(WM_NOTIFICATION_RELEASED);
	}
}
bool Button::_OnKey(const WM_KEY_INFO *pKi) {
	int PressedCnt = pKi->PressedCnt;
	int Key = pKi->Key;
	switch (Key) {
		case ' ':
			if (PressedCnt > 0)
				_Pressed();
			else
				_Released(WM_NOTIFICATION_RELEASED);
			return true;
	}
	return false;
}

void Button::_Callback(WM_MSG *pMsg) {
	auto pObj = (Button *)pMsg->pWin;
	if (!pObj->HandleActive(pMsg))
		return;
	switch (pMsg->msgid) {
	case WM_PID_STATE_CHANGED:
		pObj->_OnPidStateChange((const WM_PID_STATE_CHANGED_INFO *)pMsg->data);
		return;
	case WM_TOUCH:
		pObj->_OnTouch((const PidState *)pMsg->data);
		return;
	case WM_PAINT:
		pObj->_OnPaint();
		return;
	case WM_DELETE:
		pObj->~Button();
		break;
	case WM_KEY:
		if (!pObj->_OnKey((const WM_KEY_INFO *)pMsg->data))
			break;
	}
	DefCallback(pMsg);
}
#pragma endregion

void Button::Pressed(bool bPressed) {
	auto State = this->State;
	if (bPressed)
		State |= BUTTON_STATE_PRESSED;
	else
		State &= ~BUTTON_STATE_PRESSED;
	if (this->State == State)
		return;
	this->State = State;
	Invalidate();
}
void Button::Focussable(bool bFocussable) {
	auto State = this->State;
	if (bFocussable)
		State |= WIDGET_STATE_FOCUSSABLE;
	else
		State &= ~WIDGET_STATE_FOCUSSABLE;
	if (this->State == State)
		return;
	this->State = State;
	Invalidate();
}

Button::Button(int x0, int y0, int xsize, int ysize,
			   WObj *pParent, uint16_t Id, uint16_t Flags,
			   const char *pText) :
	Widget(x0, y0, xsize, ysize,
		   _Callback, pParent, Id, Flags, WIDGET_STATE_FOCUSSABLE),
	text(pText) {}

Button::~Button() {
	GUI_MEM_Free(apDrawObj[0]);
	apDrawObj[0] = nullptr;
	GUI_MEM_Free(apDrawObj[1]);
	apDrawObj[1] = nullptr;
	GUI_MEM_Free(apDrawObj[2]);
	apDrawObj[2] = nullptr;
}
