#include "Slider.h"

Slider::Property Slider::DefaultProps;

void Slider::_SliderPressed() {
	OrState(SLIDER_STATE_PRESSED);
	if (Status & WC_VISIBLE)
		NotifyParent(WN_CLICKED);
}
void Slider::_Released() {
	AndState(SLIDER_STATE_PRESSED);
	if (Status & WC_VISIBLE)
		NotifyParent(WN_RELEASED);
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
		OutlineFocus(rFocus);
	}
}
void Slider::_OnTouch(const PID_STATE *pState) {
	if (!pState)
		return;
	if (!pState->Pressed) {
		if (StatusEx & SLIDER_STATE_PRESSED)
			_Released();
		return;
	}
	int Range = Max - Min;
	int x0 = 1 + width / 2;  /* 1 pixel focus rectangle + width of actual slider */
	int x = ((StatusEx & WC_EX_VERTICAL) ? pState->y : pState->x) - x0;
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
	if (!(StatusEx & SLIDER_STATE_PRESSED))
		_SliderPressed();
}
void Slider::_OnKey(const KEY_STATE *pKeyInfo) {
	int Key = pKeyInfo->Key;
	if (pKeyInfo->PressedCnt <= 0)
		return;
	switch (Key) {
	case GUI_KEY_RIGHT:
		Inc();
		break;
	case GUI_KEY_LEFT:
		Dec();
		break;
	default:
		return;
	}
}

WM_RESULT Slider::_Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc) {
	auto pObj = (Slider *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	switch (MsgId) {
	case WM_PAINT:
		pObj->_OnPaint();
		return 0;
	case WM_TOUCH:
		pObj->_OnTouch(Param);
		return 0;
	case WM_KEY:
		pObj->_OnKey(Param);
		break;
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

Slider::Slider(int x0, int y0, int xsize, int ysize,
			   WObj *pParent, uint16_t Id,
			   WM_CF Flags, uint16_t ExFlags,
			   int16_t Min, int16_t Max, int16_t v, int16_t nTicks) :
	Widget(x0, y0, xsize, ysize,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, ExFlags),
	Min(Min), Max(Max), v(v), nTicks(nTicks) {}
