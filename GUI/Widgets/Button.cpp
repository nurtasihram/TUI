#include "Button.h"

Button::Property Button::DefaultProps;

#pragma region Callbacks
void Button::_Pressed() {
	if (!(StatusEx & BUTTON_STATE_PRESSED)) {
		StatusEx |= BUTTON_STATE_PRESSED;
		Invalidate();
	}
	if (Status & WC_VISIBLE)
		NotifyParent(WN_CLICKED);
}
void Button::_Released(int Notification) {
	if (StatusEx & BUTTON_STATE_PRESSED) {
		StatusEx &= ~BUTTON_STATE_PRESSED;
		Invalidate();
	}
	if (Status & WC_VISIBLE)
		NotifyParent(Notification);
	if (Notification == WN_RELEASED)
		GUI_StoreKey(this->Id);
}

void Button::_OnPaint() const {
	bool bPressed = StatusEx & BUTTON_STATE_PRESSED;
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
	if (auto pDraw =
		apDrawObj[ColorIndex < 2 ?
			apDrawObj[BUTTON_BI_PRESSED] && bPressed ? BUTTON_BI_PRESSED : BUTTON_BI_UNPRESSED :
			apDrawObj[BUTTON_BI_DISABLED] ? BUTTON_BI_DISABLED : BUTTON_BI_UNPRESSED
		])
		pDraw->Draw(InsideRectAbs());
	if (text) {
		GUI.Font(Props.pFont);
		GUI.TextAlign(Props.Align);
		GUI.DispString(
			text,
			bPressed ? rInside + Point(EffectSize) : rInside);
	}
	if (Focussed()) {
		GUI.PenColor(RGB_BLACK);
		if (bPressed)
			rClient += EffectSize;
		GUI.OutlineFocus(rClient, 2);
	}
	WObj::UserClip(nullptr);
}
void Button::_OnTouch(const PID_STATE *pState) {
	if (pState) {
		if (pState->Pressed) {
			if (!(StatusEx & BUTTON_STATE_PRESSED))
				_Pressed();
		}
		else if (StatusEx & BUTTON_STATE_PRESSED)
			_Released(WN_RELEASED);
	}
	else
		_Released(WN_MOVED_OUT);
}
void Button::_OnPidStateChange(const PID_CHANGED_STATE *pState) {
	if ((pState->StatePrev == 0) && (pState->Pressed == 1)) {
		if (!(StatusEx & BUTTON_STATE_PRESSED))
			_Pressed();
	}
	else if ((pState->StatePrev == 1) && (pState->Pressed == 0)) {
		if (StatusEx & BUTTON_STATE_PRESSED)
			_Released(WN_RELEASED);
	}
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

WM_RESULT Button::_Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc) {
	auto pObj = (Button *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	switch (MsgId) {
	case WM_PID_STATE_CHANGED:
		pObj->_OnPidStateChange(Param);
		return 0;
	case WM_TOUCH:
		pObj->_OnTouch(Param);
		return 0;
	case WM_PAINT:
		pObj->_OnPaint();
		return 0;
	case WM_DELETE:
		pObj->~Button();
		return 0;
	case WM_KEY:
		if (!pObj->_OnKey(Param))
			break;
		return 0;
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}
#pragma endregion

void Button::Pressed(bool bPressed) {
	auto StatusEx = this->StatusEx;
	if (bPressed)
		StatusEx |= BUTTON_STATE_PRESSED;
	else
		StatusEx &= ~BUTTON_STATE_PRESSED;
	if (this->StatusEx == StatusEx)
		return;
	this->StatusEx = StatusEx;
	Invalidate();
}
void Button::Focussable(bool bFocussable) {
	auto StatusEx = this->StatusEx;
	if (bFocussable)
		StatusEx |= WC_FOCUSSABLE;
	else
		StatusEx &= ~WC_FOCUSSABLE;
	if (this->StatusEx == StatusEx)
		return;
	this->StatusEx = StatusEx;
	Invalidate();
}

Button::Button(int x0, int y0, int xsize, int ysize,
			   WObj *pParent, uint16_t Id,
			   WM_CF Flags,
			   const char *pText) :
	Widget(x0, y0, xsize, ysize,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE),
	text(pText) {}

Button::~Button() {
	GUI_MEM_Free(apDrawObj[0]);
	apDrawObj[0] = nullptr;
	GUI_MEM_Free(apDrawObj[1]);
	apDrawObj[1] = nullptr;
	GUI_MEM_Free(apDrawObj[2]);
	apDrawObj[2] = nullptr;
}
