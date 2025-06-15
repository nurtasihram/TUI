#include "Slider.h"

Slider::Property Slider::DefaultProps;

void Slider::_SliderPressed() {
	OrState(SLIDER_CF__PRESSED);
	if (Status & WC_VISIBLE)
		NotifyOwner(WN_CLICKED);
}
void Slider::_Released() {
	MaskState(SLIDER_CF__PRESSED);
	if (Status & WC_VISIBLE)
		NotifyOwner(WN_RELEASED);
}

void Slider::_OnPaint() const {
	auto &&rFocus = ClientRect();
	auto &&r = rFocus / 1;
	int NumTicks = this->nTicks;
	int xsize = r.xsize() - width;
	int x0 = r.x0 + width / 2;
	int Range = Max - Min;
	if (Range == 0)
		Range = 1;
	GUI.BkColor(BkColorProp(Props.BkColor));
	GUI.Clear();
	SRect rSlider = r;
	rSlider.y0 = 5;
	rSlider.x0 = x0 + (uint32_t)xsize * (uint32_t)(v - Min) / Range - width / 2;
	rSlider.x1 = rSlider.x0 + width;
	SRect rSlot;
	rSlot.x0 = x0;
	rSlot.x1 = x0 + xsize;
	rSlot.y0 = (rSlider.y0 + rSlider.y1) / 2 - 1;
	rSlot.y1 = rSlot.y0 + 3;
	DrawDown(rSlot);
	if (NumTicks < 0) {
		NumTicks = Range + 1;
		if (NumTicks > xsize / 5)
			NumTicks = 11;
	}
	if (NumTicks > 1) {
		GUI.PenColor(RGB_BLACK);
		for (int i = 0; i < NumTicks; ++i)
			DrawVLine(x0 + xsize * i / (NumTicks - 1), 1, 3);
	}
	GUI.PenColor(Props.Color);
	Fill(rSlider);
	GUI.PenColor(RGB_BLACK);
	DrawUp(rSlider);
	if (Focussed()) {
		GUI.PenColor(RGB_BLACK);
		DrawFocus(rFocus);
	}
}
void Slider::_OnMouse(MOUSE_STATE State) {
	if (!State)
		return;
	if (!State.Pressed) {
		if (StatusEx & SLIDER_CF__PRESSED)
			_Released();
		return;
	}
	int Range = Max - Min;
	int x0 = 1 + width / 2;  /* 1 pixel focus rectangle + width of actual slider */
	int x = ((StatusEx & WC_EX_VERTICAL) ? State.y : State.x) - x0;
	int xsize = SizeX() - 2 * x0;
	int Sel;
	if (x <= 0)
		Sel = Min;
	else if (x >= xsize)
		Sel = Max;
	else {
		/* Make sure we do not divide by 0, even though xsize should never be 0 in this case anyhow */
		int Div = xsize ? xsize : 1;
		Sel = Min + ((uint32_t)Range * (uint32_t)x + Div / 2) / Div;
	}
	Focus();
	Capture(true);
	Value(Sel);
	if (!(StatusEx & SLIDER_CF__PRESSED))
		_SliderPressed();
}
bool Slider::_OnKey(KEY_STATE State) {
	if (State.PressedCnt <= 0)
		return false;
	switch (State.Key) {
	case GUI_KEY_RIGHT:
		Inc();
		return true;
	case GUI_KEY_LEFT:
		Dec();
		return true;
	}
	return false;
}

WM_RESULT Slider::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (Slider *)pWin;
	switch (MsgId) {
	case WM_PAINT:
		pObj->_OnPaint();
		return 0;
	case WM_MOUSE:
		pObj->_OnMouse(Param);
		return 0;
	case WM_KEY:
		if (pObj->_OnKey(Param))
			return true;
		break;
	case WM_GET_CLASS:
		return ClassNames[WCLS_SLIDER];
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

Slider::Slider(const SRect &rc,
			   PWObj pParent, uint16_t Id,
			   WM_CF Flags, uint16_t FlagsEx,
			   int16_t Min, int16_t Max, int16_t v, int16_t nTicks) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, FlagsEx),
	Min(Min), Max(Max), v(v), nTicks(nTicks) {}
