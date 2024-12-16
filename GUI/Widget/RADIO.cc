#include "Radio.h"

Radio::Property Radio::DefaultProps;

void Radio::_OnPaint() const {
	auto &&rFocus = ClientRect();
	bool bFocussed = State & WIDGET_STATE_FOCUS;
	auto pBmRadio = Props.apBmRadio[Enable()],
		 pBmCheck = Props.pBmCheck;
	rFocus.x1 = pBmRadio->Size.x + Props.Border * 2 - 1;
	rFocus.y1 = height + (NumItems() - 1) * spacing - 1;
	GUI.PenColor(Props.TextColor);
	GUI.Font(Props.pFont);
	GUI.TextMode(DRAWMODE_TRANS);
	auto FontDistY = GUI.Font()->YDist;
	auto CHeight = Props.pFont->CHeight;
	auto SpaceAbove = Props.pFont->Baseline - CHeight;
	SRect rect;
	rect.x0 = pBmRadio->Size.x + Props.Border * 2 + 2;
	rect.y0 = (CHeight <= height) ? (height - CHeight) / 2 : 0;
	rect.y1 = rect.y0 + CHeight - 1;
	int FocusBorder = FontDistY <= 12 ? 2 : 3;
	if (rect.y0 < FocusBorder)
		FocusBorder = (uint8_t)rect.y0;
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
		const char *pText = TextArray[i];
		if (!pText)
			continue;
		if (!*pText)
			continue;
		auto r = rect;
		r.x1 = r.x0 + GUI_GetStringDistX(pText) - 2;
		r += Point(0, y);
		GUI_DispStringAt(pText, { r.x0, r.y0 - SpaceAbove });
		if (bFocussed && (sel == i))
			rFocus = r * FocusBorder;
	}
	if (bFocussed) {
		GUI.PenColor(RGB_BLACK);
		OutlineFocus(rFocus);
	}
}
void Radio::_OnTouch(PidState *pState) {
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
			Notification = WM_NOTIFICATION_CLICKED;
		}
		else {
			Hit = true;
			Notification = WM_NOTIFICATION_RELEASED;
		}
	}
	else
		Notification = WM_NOTIFICATION_MOVED_OUT;
	NotifyParent(Notification);
	if (Hit)
		GUI_StoreKey(Id);
}
bool Radio::_OnKey(const WM_KEY_INFO *pKeyInfo) {
	if (pKeyInfo->PressedCnt <= 0)
		return false;
	switch (pKeyInfo->Key) {
	case GUI_KEY_RIGHT:
	case GUI_KEY_DOWN:
		Inc();
		break; /* Send to parent by not doing anything */
	case GUI_KEY_LEFT:
	case GUI_KEY_UP:
		Dec();
		break; /* Send to parent by not doing anything */
	default:
		return false;
	}
	return true;
}

void Radio::_Callback(WM_MSG *pMsg) {
	auto pObj = (Radio *)pMsg->pWin;
	if (!pObj->HandleActive(pMsg))
		return;
	switch (pMsg->msgid) {
	case WM_PAINT:
		pObj->_OnPaint();
		return;
	case WM_GET_RADIOGROUP:
		pMsg->data = pObj->groupId;
		return;
	case WM_TOUCH:
		pObj->_OnTouch((PidState *)pMsg->data);
		break;
	case WM_KEY:
		if (!pObj->_OnKey((const WM_KEY_INFO *)pMsg->data))
			return;
		break;
	case WM_DELETE:
		pObj->~Radio();
		break;
	}
	DefCallback(pMsg);
}

void Radio::_SetValue(int v) {
	if (v >= NumItems())
		v = NumItems() - 1;
	if (v == sel)
		return;
	sel = v;
	Invalidate();
	NotifyParent(WM_NOTIFICATION_VALUE_CHANGED);
}

Radio::Radio(int x0, int y0, int xSize, int ySize,
			 WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
			 int NumItems, int Spacing) :
	Widget(x0, y0, 
		  xSize ? xSize : DefaultProps.apBmRadio[0]->Size.x + DefaultProps.Border * 2,
		  ySize ? ySize : DefaultProps.apBmRadio[0]->Size.y + DefaultProps.Border * 2 + (NumItems - 1) * Spacing,
		  _Callback, pParent, Id, Flags, WIDGET_STATE_FOCUSSABLE | ExFlags),
	TextArray(NumItems),
	height(DefaultProps.apBmRadio[0]->Size.y + DefaultProps.Border * 2),
	spacing(Spacing <= 0 ? 20 : Spacing) {}

Radio::Radio(int x0, int y0, int xSize, int ySize,
			 WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
			 const char *pItems, int spacing) :
	Radio(x0, y0, xSize, ySize, pParent, Id, Flags, ExFlags, GUI__countStrings(pItems), spacing) {
	for (auto &s : TextArray) {
		s = pItems;
		pItems = GUI__nextString(pItems);
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
//		pObj->apBmRadio[Index] = pBitmap;
//		break;
//	case RADIO_BI_CHECK:
//		pObj->pBmCheck = pBitmap;
//		break;
//	}
//	pObj->Invalidate();
//}

static int _IsInGroup(WObj *pWin, uint8_t GroupId) {
	if (!GroupId) return 0;
	WM_MSG msg;
	msg.msgid = WM_GET_RADIOGROUP;
	pWin->SendMessage(&msg);
	return msg.data == GroupId;
}
static WObj *_GetPrevInGroup(WObj *pWin, uint8_t GroupId) {
	for (pWin = pWin->PrevSibling(); pWin; pWin = pWin->PrevSibling())
		if (_IsInGroup(pWin, GroupId))
			return pWin;
	return nullptr;
}
static Radio *_GetNextInGroup(WObj *pWin, uint8_t GroupId) {
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

void Radio::GroupId(uint8_t NewGroupId) {
	uint8_t OldGroupId = groupId;
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
	}
};
CBitmap Radio::bmCheck{
	/* Size */ { 4, 4 },
	/* BytesPerLine */ 1,
	/* Bits */ _acCheck,
	/* Palette */ &_PalCheck,
	/* Transparent */ true
};
#pragma endregion
