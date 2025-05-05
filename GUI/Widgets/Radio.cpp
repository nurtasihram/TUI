#include "Radio.h"

constexpr auto RM_GET_RADIOGROUP = WM_WIDGET(0);

Radio::Property Radio::DefaultProps;

static bool _IsInGroup(PWObj pWin, uint8_t GroupId) {
	if (!GroupId) return false;
	return (int)pWin->SendMessage(RM_GET_RADIOGROUP) == GroupId;
}
static PWObj _GetPrevInGroup(PWObj pWin, uint8_t GroupId) {
	for (pWin = pWin->PrevSibling(); pWin; pWin = pWin->PrevSibling())
		if (_IsInGroup(pWin, GroupId))
			return pWin;
	return nullptr;
}
static Radio *_GetNextInGroup(PWObj pWin, uint8_t GroupId) {
	for (; pWin; pWin = pWin->NextSibling())
		if (_IsInGroup(pWin, GroupId))
			return (Radio *)pWin;
	return nullptr;
}
void Radio::_ClearSelection(uint8_t GroupId) {
	for (auto pWin = FirstSibling(); pWin; pWin = pWin->NextSibling())
		if (pWin != this)
			if (_IsInGroup(pWin, GroupId))
				((Radio *)pWin)->_SetValue(-1);
}
void Radio::_HandleSetValue(int16_t v) {
	if (v < 0) {
		if (auto pWin = (Radio *)_GetPrevInGroup(this, groupId)) {
			pWin->Focus();
			_SetValue(0x7FFF);
			_SetValue(-1);
		}
	}
	else if (v >= NumItems()) {
		if (auto pWin = _GetNextInGroup(NextSibling(), groupId)) {
			pWin->Focus();
			pWin->_SetValue(0);
			_SetValue(-1);
		}
	}
	else if (sel != v) {
		_ClearSelection(groupId);
		_SetValue(v);
	}
}
void Radio::_SetValue(int v) {
	if (v >= NumItems())
		v = NumItems() - 1;
	if (v == sel)
		return;
	sel = v;
	Invalidate();
	NotifyParent(WN_VALUE_CHANGED);
}

void Radio::_OnPaint() const {
	auto &&rFocus = ClientRect();
	bool bFocussed = Focussed();
	auto pBmRadio = Props.apBm[Enable()],
		 pBmCheck = Props.apBm[2];
	rFocus.x1 = pBmRadio->Size.x + Props.Border * 2 - 1;
	rFocus.y1 = height + (NumItems() - 1) * spacing - 1;
	GUI.Font(Props.pFont);
	auto FontDistY = GUI.Font()->YDist;
	auto CHeight = Props.pFont->CHeight;
	auto SpaceAbove = Props.pFont->Baseline - CHeight;
	SRect rsWin;
	rsWin.x0 = pBmRadio->Size.x + Props.Border * 2 + 2;
	rsWin.y0 = (CHeight <= height) ? (height - CHeight) / 2 : 0;
	rsWin.y1 = rsWin.y0 + CHeight - 1;
	int FocusBorder = FontDistY <= 12 ? 2 : 3;
	if (FocusBorder > rsWin.y0)
		FocusBorder = (uint8_t)rsWin.y0;
	GUI.PenColor(Props.TextColor);
	GUI.BkColor(BkColorProp(Props.BkColor));
	GUI.Clear();
	for (int i = 0, nItems = NumItems(); i < nItems; ++i) {
		auto y = i * spacing;
		GUI.DrawBitmap(*pBmRadio, { Props.Border, Props.Border + y });
		if (sel == i)
			GUI.DrawBitmap(
				*pBmCheck,
				{ Props.Border + (pBmRadio->Size.x - pBmCheck->Size.x) / 2,
				  Props.Border + (pBmRadio->Size.y - pBmCheck->Size.y) / 2 + y });
		GUI_PCSTR pText = TextArray[i];
		if (!pText)
			continue;
		if (!*pText)
			continue;
		auto r = rsWin;
		r.x1 = r.x0 + GUI.XDist(pText) - 2;
		r.y0 -= SpaceAbove;
		r += Point(0, y);
		GUI.DrawStringIn(pText, r);
		if (bFocussed && sel == i) {
			r.y0 += SpaceAbove;
			rFocus = r * FocusBorder;
		}
	}
	if (bFocussed) {
		GUI.PenColor(RGB_BLACK);
		GUI.DrawFocus(rFocus);
	}
}
void Radio::_OnMouse(const MOUSE_STATE *pState) {
	int Notification;
	bool Hit = 0;
	if (pState) {
		if (pState->Pressed) {
			int y = pState->y,
				sel = y / spacing;
			y -= sel * spacing;
			if (y <= height)
				Sel(sel);
			if (Focussable())
				Focus();
			Notification = WN_CLICKED;
		}
		else {
			Hit = true;
			Notification = WN_RELEASED;
		}
	}
	else
		Notification = WN_MOVED_OUT;
	NotifyParent(Notification);
	if (Hit)
		GUI.Key(Id);
}
bool Radio::_OnKey(KEY_STATE State) {
	if (State.PressedCnt <= 0)
		return false;
	switch (State.Key) {
	case GUI_KEY_RIGHT:
	case GUI_KEY_DOWN:
		Inc();
		break;
	case GUI_KEY_LEFT:
	case GUI_KEY_UP:
		Dec();
		break;
	default:
		return false;
	}
	return true;
}

WM_RESULT Radio::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (Radio *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
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
	case WM_DELETE:
		pObj->~Radio();
		return 0;
	case RM_GET_RADIOGROUP:
		return pObj->groupId;
	case WM_GET_CLASS:
		return ClassNames[WCLS_RADIO];
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

Radio::Radio(const SRect &rc,
			 PWObj pParent, uint16_t Id,
			 WM_CF Flags, uint16_t FlagsEx,
			 int NumItems, int Spacing) :
	Widget({ rc.left_top(), {
				rc.x1 >= rc.x0 ? rc.x1 : rc.x0 + DefaultProps.apBm[0]->Size.x + DefaultProps.Border * 2,
				rc.y1 >= rc.y0 ? rc.y1 : rc.x0 + DefaultProps.apBm[0]->Size.y + DefaultProps.Border * 2 + (NumItems - 1) * Spacing }
		   },
		  _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, FlagsEx),
	TextArray(NumItems),
	height(DefaultProps.apBm[0]->Size.y + DefaultProps.Border * 2),
	spacing(Spacing <= 0 ? 20 : Spacing) {}
Radio::Radio(const SRect &rc,
			 PWObj pParent, uint16_t Id,
			 WM_CF Flags, uint16_t FlagsEx,
			 GUI_PCSTR pItems, int Spacing) :
	Radio(rc,
		  pParent, Id,
		  Flags, FlagsEx,
		  GUI.NumTexts(pItems), Spacing) {
	for (auto &s : TextArray) {
		s = pItems;
		pItems = GUI.NextText(pItems);
	}
}

void Radio::Sel(int16_t v) {
	if (groupId)
		_HandleSetValue(v);
	else {
		if (v < 0)
			v = 0;
		_SetValue(v);
	}
}

//void RADIO_SetImage(Radio *pObj, CBitmap *pBitmap, unsigned int Index) {
//	if (!pObj)
//		return;
//	switch (Index) {
//	case RADIO_BI_INACTIV:
//	case RADIO_BI_ACTIV:
//		pObj->apBm[Index] = pBitmap;
//		break;
//	case RADIO_BI_CHECK:
//		pObj->pBmCheck = pBitmap;
//		break;
//	}
//	pObj->Invalidate();
//}

void Radio::GroupId(uint8_t NewGroupId) {
	auto OldGroupId = groupId;
	if (NewGroupId == OldGroupId)
		return;
	auto pFirst = FirstSibling();
	if (OldGroupId && sel >= 0) {
		auto pWin = _GetNextInGroup(pFirst, OldGroupId);
		groupId = 0;
		if (pWin)
			pWin->_SetValue(0);
	}
	if (!_GetNextInGroup(pFirst, NewGroupId))
		_SetValue(-1);
	else if (sel < 0)
		_SetValue(0);
	groupId = NewGroupId;
}

#pragma region Image
/* Colors */
static const RGBC _aColorDisabled[]{
	/* __ */ RGB_INVALID_COLOR,
	/* XX */ RGBC_GRAY(0x80),
	/* oo */ RGB_WHITE,
	/* dd */ RGBC_GRAY(0xC0)
};
static const RGBC _aColorEnabled[]{
	/* __ */ RGB_INVALID_COLOR,
	/* XX */ RGBC_GRAY(0x80),
	/* oo */ RGBC_GRAY(0xC0),
	/* dd */ RGB_WHITE
};
static const RGBC _aColorsCheck[]{
	RGB_WHITE,
	RGB_INVALID_COLOR
};
/* Palettes */
static CPalette _PalRadioDisabled{ _aColorDisabled };
static CPalette _PalRadioEnabled{ _aColorEnabled };
static CPalette _PalCheck{ _aColorsCheck };
/* Pixel data */
static const BPP2_DAT _acRadio[]{
	________,XXXXXXXX,________,
	____XXXX,oooooooo,XXXX____,
	__XXoooo,dddddddd,oooodd__,
	__XXoodd,dddddddd,dd__dd__,
	XXoodddd,dddddddd,dddd__dd,
	XXoodddd,dddddddd,dddd__dd,
	XXoodddd,dddddddd,dddd__dd,
	XXoodddd,dddddddd,dddd__dd,
	__XXoodd,dddddddd,dd__dd__,
	__XX____,dddddddd,____dd__,
	____dddd,________,dddd____,
	________,dddddddd,________
};
static const BPP1_DAT _acCheck[]{
	__XXXX__________,
	XXXXXXXX________,
	XXXXXXXX________,
	__XXXX__________
};
CBitmap Radio::abmRadio[]{
	{
		/* Size */ { 12, 12 },
		/* BytesPerLine */ 3,
		/* Bits */ _acRadio,
		/* Palette */ &_PalRadioDisabled,
		/* Transparent */ true
	}, {
		/* Size */ { 12, 12 },
		/* BytesPerLine */ 3,
		/* Bits */ _acRadio,
		/* Palette */ &_PalRadioEnabled,
		/* Transparent */ true
	}, {
		/* Size */ { 4, 4 },
		/* BytesPerLine */ 1,
		/* Bits */ _acCheck,
		/* Palette */ &_PalCheck,
		/* Transparent */ true
	}
};
#pragma endregion
