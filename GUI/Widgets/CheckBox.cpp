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
	WObj::UserClip(&rBox);
	GUI.BkColor(Props.aBkColorBox[ColorIndex]);
	GUI.Clear();
	if (CurrentState) {
		auto pBm = Props.apBm[(CurrentState - 1) * 2 + ColorIndex];
		GUI.DrawBitmap(*pBm, SRect(EffectSize, pBm->Size + EffectSize).AlignTo(ALIGN_VCENTER, rClient).left_top());
	}
	DrawDown(rBox);
	WObj::UserClip(nullptr);
	if (text) {
		auto rText = rClient;
		rText.x0 += rBox.x1 + 1 + Props.Spacing;
		GUI.BkColor(BkColorProp(Props.BkColor));
		GUI.PenColor(Props.TextColor);
		GUI.Font(Props.pFont);
		GUI.TextAlign(Props.Align);
		GUI.DispString(text, rText);
		if (!Focussed())
			return;
		auto xSizeText = GUI.XDist(text);
		auto ySizeText = GUI.Font()->YSize;
		auto rFocus = rText;
		switch (Props.Align & ~TEXTALIGN_HORIZONTAL) {
			case TEXTALIGN_VCENTER:
				rFocus.y0 = (rText.y1 - ySizeText) / 2;
				break;
			case TEXTALIGN_BOTTOM:
				rFocus.y0 = rText.y1 - ySizeText;
				break;
		}
		switch (Props.Align & ~TEXTALIGN_VCENTER) {
			case TEXTALIGN_HCENTER:
				rFocus.x0 += (rText.dx() - xSizeText) / 2;
				break;
			case TEXTALIGN_RIGHT:
				rFocus.x0 += rText.dx() - xSizeText;
				break;
		}
		rFocus.x1 = rFocus.x0 + xSizeText;
		rFocus.y1 = rFocus.y0 + ySizeText;
		GUI.PenColor(RGB_BLACK);
		GUI.OutlineFocus(rFocus);
	}
}
void CheckBox::_OnTouch(const PID_STATE *pState) {
	int Notification = 0;
	int Hit = 0;
	if (pState) {
		if (!Captured()) {
			if (pState->Pressed) {
				Capture(true);
				CurrentState = (CurrentState + 1) % nStates;
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
		CurrentState = (CurrentState + 1) % nStates;
		Invalidate();
		break;
	}
	return true;
}
WM_RESULT CheckBox::_Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc) {
	auto pObj = (CheckBox *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	switch (MsgId) {
		case WM_TOUCH:
			pObj->_OnTouch(Param);
			return 0;
		case WM_KEY:
			if (!pObj->_OnKey(Param))
				return 0;
			break;
		case WM_PAINT:
			pObj->_OnPaint();
			return 0;
		case WM_DELETE:
			pObj->~CheckBox();
			return 0;
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

CheckBox::CheckBox(int x0, int y0, int xsize, int ysize,
				   WObj *pParent, uint16_t Id,
				   WM_CF Flags, const char *pText) :
	Widget(x0, y0,
		   xsize ? xsize : DefaultProps.apBm[0]->Size.x + 2 * DefaultEffect()->EffectSize,
		   ysize ? ysize : DefaultProps.apBm[0]->Size.y + 2 * DefaultEffect()->EffectSize,
		   _Callback,
		   pParent,Id,
		   Flags | WC_FOCUSSABLE),
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
