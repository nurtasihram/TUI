#include "CheckBox.h"

CheckBox::Property CheckBox::DefaultProps;

void CheckBox::_OnPaint() {
	int EffectSize = this->EffectSize();
	int ColorIndex = this->Enable();
	SRect rBox;
	rBox.right_bottom(Props.apBm[0]->Size - 1 + 2 * EffectSize);
	GUI.BkColor(BkColorProp(Props.BkColor));
	GUI.Clear();
	WObj::SetUserClipRect(&rBox);
	GUI.BkColor(Props.aBkColorBox[ColorIndex]);
	GUI.Clear();
	if (CurrentState)
		GUI.DrawBitmap(*Props.apBm[(CurrentState - 1) * 2 + ColorIndex], EffectSize);
	DrawDown(rBox);
	WObj::SetUserClipRect(nullptr);
	if (!text) return;
	auto &&rText = ClientRect();
	rText.x0 += rBox.x1 + 1 + Props.Spacing;
	GUI_SetTextMode(DRAWMODE_TRANS);
	GUI.PenColor(Props.TextColor);
	GUI.Font(Props.pFont);
	GUI_DispStringInRect(text, rText, Props.Align);
	if (!(State & WIDGET_STATE_FOCUS))
		return;
	auto xSizeText = GUI_GetStringDistX(text);
	auto ySizeText = GUI_GetFontSizeY();
	auto rFocus = rText;
	switch (Props.Align & ~GUI_TA_HORIZONTAL) {
	case GUI_TA_VCENTER:
		rFocus.y0 = (rText.y1 - ySizeText) / 2;
		break;
	case GUI_TA_BOTTOM:
		rFocus.y0 = rText.y1 - ySizeText;
		break;
	}
	switch (Props.Align & ~GUI_TA_VCENTER) {
	case GUI_TA_HCENTER:
		rFocus.x0 += (rText.dx() - xSizeText) / 2;
		break;
	case GUI_TA_RIGHT:
		rFocus.x0 +=  rText.dx() - xSizeText;
		break;
	}
	rFocus.x1 = rFocus.x0 + xSizeText;
	rFocus.y1 = rFocus.y0 + ySizeText;
	GUI.PenColor(RGB_BLACK);
	DrawFocus(rFocus);
}
void CheckBox::_OnTouch(WM_MSG* pMsg) {
	int Notification = 0;
	int Hit = 0;
	if (auto pState = (const PidState *)pMsg->data) {
		if (!Captured()) {
			if (pState->Pressed) {
				Capture(true);
				CurrentState = (CurrentState + 1) % nStates;
				Invalidate();
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
	NotifyParent(Notification);
	if (Hit == 1)
		GUI_StoreKey(Id);
}
bool CheckBox::_OnKey(WM_MSG* pMsg) {
	if (!Enable()) return false;
	auto pKeyInfo = (const WM_KEY_INFO *)pMsg->data;
	if (pKeyInfo->PressedCnt <= 0)
		return false;
	switch (pKeyInfo->Key) {
	case GUI_KEY_SPACE:
		CurrentState = (CurrentState + 1) % nStates;
		Invalidate();
		break;
	}
	return true;
}
void CheckBox::_Callback(WM_MSG* pMsg) {
	auto pObj = (CheckBox *)pMsg->pWin;
	if (!pObj->HandleActive(pMsg))
		return;
	switch (pMsg->msgid) {
	case WM_KEY:
		if (!pObj->_OnKey(pMsg))
			return;
		break;
	case WM_PAINT:
		pObj->_OnPaint();
		return;
	case WM_DELETE:
		pObj->~CheckBox();
		break;
	case WM_TOUCH:
		pObj->_OnTouch(pMsg);
		break;
	}
	DefCallback(pMsg);
}

CheckBox::CheckBox(int x0, int y0, int xsize, int ysize,
				   WObj *pParent, uint16_t Id, uint16_t Flags, const char *pText) :
	Widget(x0, y0,
		   xsize ? xsize : DefaultProps.apBm[0]->Size.x + 2 * DefaultEffect()->EffectSize,
		   ysize ? ysize : DefaultProps.apBm[0]->Size.y + 2 * DefaultEffect()->EffectSize,
		   _Callback, pParent, Id, Flags, WIDGET_STATE_FOCUSSABLE), text(pText) {}

#pragma region Images
/* Colors */
static const RGBC _aColorDisabled[]{ RGBC_GRAY(0x10), RGBC_GRAY(0x80) };
static const RGBC _aColorEnabled[]{ RGB_BLACK, RGB_WHITE };
/* Palettes */
static CPalette _PalCheckDisabled{ _aColorDisabled };
static CPalette _PalCheckEnabled{ _aColorEnabled };
/* Pixel data */
static const BPP1_DAT _acCheck[]{
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
CBitmap CheckBox::abmCheck[2] {
	{
		/* Size */ { 11, 11 },
		/* BytesPerLine */ 2,
		/* Bits */ _acCheck,
		/* Palette */ &_PalCheckDisabled,
		/* Transparent */ true
	}, {
		/* Size */ { 11, 11 },
		/* BytesPerLine */ 2,
		/* Bits */ _acCheck,
		/* Palette */ &_PalCheckEnabled,
		/* Transparent */ true
	}
};
/* Pixel data */
static const BPP1_DAT _acCheck3[] {
	XXXXXXXXXXXXXXXX,XXXXXX__________,
	XXXXXXXXXXXXXXXX,XXXXXX__________,
	XXXX____________,__XXXX__________,
	XXXX____________,__XXXX__________,
	XXXX____________,__XXXX__________,
	XXXX____________,__XXXX__________,
	XXXX____________,__XXXX__________,
	XXXX____________,__XXXX__________,
	XXXX____________,__XXXX__________,
	XXXXXXXXXXXXXXXX,XXXXXX__________,
	XXXXXXXXXXXXXXXX,XXXXXX__________
};
/* Bitmaps */
CBitmap CheckBox::abmCheck3[2] {
	{
		/* Size */ { 11, 11 },
		/* BytesPerLine */ 2,
		/* Bits */_acCheck3,
		/* Palette */ &_PalCheckDisabled,
		/* Transparent */ true
	}, {
		/* Size */ { 11, 11 },
		/* BytesPerLine */ 2,
		/* Bits */_acCheck3,
		/* Palette */ &_PalCheckEnabled,
		/* Transparent */ true
	}
};
#pragma endregion
