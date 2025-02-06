#include "CheckBox.h"

CheckBox::Property CheckBox::DefaultProps;

void CheckBox::_OnPaint() {
	int EffectSize = this->EffectSize();
	int ColorIndex = this->Enable();
	auto &&rClient = ClientRect();
	SRect rBox;
	rBox.right_bottom(Props.apBm[0]->Size - 1 + 2 * EffectSize).AlignTo(ALIGN_VCENTER, rClient);
	GUI.BkColor(BkColorProp(Props.BkColor));
	GUI.Clear();
	auto rText = rClient;
	rText.x0 += rBox.x1 + Props.Spacing;
	if (text) {
		GUI.BkColor(BkColorProp(Props.BkColor));
		GUI.PenColor(Props.TextColor);
		GUI.Font(Props.pFont);
		GUI.TextAlign(Props.Align);
		rText = GUI.DispString(text, rText);
		if (Focussed()) {
			GUI.PenColor(RGB_BLACK);
			GUI.DrawFocus(rText * 2);
		}
	}
	GUI.PenColor(Props.aBkColorBox[ColorIndex]);
	GUI.Fill(rBox);
	if (CurrentState) {
		auto pBm = Props.apBm[(CurrentState - 1) * 2 + ColorIndex];
		GUI.DrawBitmap(*pBm, SRect(0, pBm->Size).AlignTo(ALIGN_VCENTER, rClient).xmove(EffectSize).left_top());
	}
	DrawDown(rBox);
}
void CheckBox::_OnTouch(const PID_STATE *pState) {
	int Notification = 0;
	int Hit = 0;
	if (pState) {
		if (!Captured()) {
			if (pState->Pressed) {
				Capture(true);
				CurrentState = (CurrentState + 1) % Props.NumStates;
				Invalidate();
				Notification = WN_CLICKED;
			}
			else {
				Hit = 1;
				Notification = WN_RELEASED;
			}
		}
	}
	else
		Notification = WN_MOVED_OUT;
	NotifyParent(Notification);
	if (Hit == 1)
		GUI_StoreKey(Id);
}
bool CheckBox::_OnKey(const KEY_STATE *pKeyInfo) {
	if (!Enable()) return false;
	if (pKeyInfo->PressedCnt <= 0)
		return false;
	switch (pKeyInfo->Key) {
	case GUI_KEY_SPACE:
		CurrentState = (CurrentState + 1) % Props.NumStates;
		Invalidate();
		break;
	}
	return true;
}
WM_RESULT CheckBox::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (CheckBox *)pWin;
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
			if (!pObj->_OnKey(Param))
				return 0;
			break;
		case WM_DELETE:
			pObj->~CheckBox();
			return 0;
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

CheckBox::CheckBox(const SRect &rc,
				   PWObj pParent, uint16_t Id,
				   WM_CF Flags, WC_EX FlagsEx,
				   const char *pText) :
	Widget({ rc.left_top(), {
				rc.x1 >= rc.x0 ? rc.x1 : rc.x0 + DefaultProps.apBm[0]->Size.x + 2 * DefaultEffect()->EffectSize,
				rc.y1 >= rc.y0 ? rc.y1 : rc.x0 + DefaultProps.apBm[0]->Size.y + 2 * DefaultEffect()->EffectSize }
		   },
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, FlagsEx),
	text(pText) {}

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
/* Pixel data */
static const BPP1_DAT _acCheck3[]{
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
CBitmap CheckBox::abmCheck[4] {
	{
		/* Size */ { 11, 11 },
		/* BytesPerLine */ 2,
		/* Bits */ _acCheck,
		/* Palette */ &_PalCheckDisabled,
		/* Transparent */ false
	}, {
		/* Size */ { 11, 11 },
		/* BytesPerLine */ 2,
		/* Bits */ _acCheck,
		/* Palette */ &_PalCheckEnabled,
		/* Transparent */ false
	}, {
		/* Size */ { 11, 11 },
		/* BytesPerLine */ 2,
		/* Bits */_acCheck3,
		/* Palette */ &_PalCheckDisabled,
		/* Transparent */ false
	}, {
		/* Size */ { 11, 11 },
		/* BytesPerLine */ 2,
		/* Bits */_acCheck3,
		/* Palette */ &_PalCheckEnabled,
		/* Transparent */ false
	}
};
#pragma endregion
