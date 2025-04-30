#include "Button.h"

Button::Property Button::DefaultProps;

void Button::_Pressed() {
	if (StatusEx & _BUTTON_STATE_PRESSED)
		return;
	StatusEx |= _BUTTON_STATE_PRESSED;
	Invalidate();
	if (Status & WC_VISIBLE)
		NotifyParent(WN_CLICKED);
}
void Button::_Released(int Notification) {
	if (!(StatusEx & _BUTTON_STATE_PRESSED))
		return;
	StatusEx &= ~_BUTTON_STATE_PRESSED;
	Invalidate();
	if (Status & WC_VISIBLE)
		NotifyParent(Notification);
	if (Notification == WN_RELEASED)
		GUI_StoreKey(this->Id);
}

void Button::_OnPaint() const {
	bool bPressed = StatusEx & _BUTTON_STATE_PRESSED;
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
	WObj::UserClip(&rInside);
	GUI.Clear();
	aDrawObj[ColorIndex < 2 ?
		aDrawObj[BUTTON_BI_PRESSED] && bPressed ? BUTTON_BI_PRESSED : BUTTON_BI_UNPRESSED :
		aDrawObj[BUTTON_BI_DISABLED] ? BUTTON_BI_DISABLED : BUTTON_BI_UNPRESSED].Draw(InsideRect());
	if (text) {
		GUI.Font(Props.pFont);
		GUI.TextAlign(Props.Align);
		GUI.DrawStringIn(
			text,
			bPressed ? rInside + Point(EffectSize) : rInside);
	}
	if (Focussed()) {
		GUI.PenColor(RGB_BLACK);
		if (bPressed)
			rClient += EffectSize;
		GUI.DrawFocus(rClient, 2);
	}
	WObj::UserClip(nullptr);
}
void Button::_OnTouch(const PID_STATE *pState) {
	if (pState) {
		if (StatusEx & BUTTON_CF_SWITCH)
			return;
		if (pState->Pressed)
			_Pressed();
		else
			_Released(WN_RELEASED);
	}
	else
		_Released(WN_MOVED_OUT);
}
void Button::_OnPidStateChange(const PID_CHANGED_STATE *pState) {
	if (pState->StatePrev == 0 && pState->Pressed == 1)
		_Pressed();
	else if (pState->StatePrev == 1 && pState->Pressed == 0)
		_Released(WN_RELEASED);
}
bool Button::_OnKey(const KEY_STATE *pKi) {
	int PressedCnt = pKi->PressedCnt;
	int Key = pKi->Key;
	switch (Key) {
		case ' ':
			if (PressedCnt > 0)
				_Pressed();
			else
				_Released(WN_RELEASED);
			return true;
	}
	return false;
}

WM_RESULT Button::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (Button *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	switch (MsgId) {
		case WM_PAINT:
			pObj->_OnPaint();
			return 0;
		case WM_MOUSE_KEY:
			pObj->_OnTouch(Param);
			return 0;
		case WM_KEY:
			if (!pObj->_OnKey(Param))
				break;
			return 0;
		case WM_MOUSE_CHANGED:
			pObj->_OnPidStateChange(Param);
			return 0;
		case WM_DELETE:
			pObj->~Button();
			return 0;
		case WM_GET_CLASS:
			return ClassNames[WCLS_BUTTON];
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

Button::Button(const SRect &rc,
			   PWObj pParent, uint16_t Id,
			   WM_CF Flags, WC_EX FlagsEx,
			   const char *pText) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, FlagsEx),
	text(pText) {}

void Button::Pressed(bool bPressed) {
	auto StatusEx = this->StatusEx;
	if (bPressed)
		StatusEx |= _BUTTON_STATE_PRESSED;
	else
		StatusEx &= ~_BUTTON_STATE_PRESSED;
	if (this->StatusEx == StatusEx)
		return;
	this->StatusEx = StatusEx;
	Invalidate();
}
