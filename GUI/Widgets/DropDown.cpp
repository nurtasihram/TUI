#include "DropDown.h"

static int _Tolower(int Key) {
	return Key >= 0x41 && Key <= 0x5a ? Key + 0x20 : Key;
}
static void _DrawTriangleDown(int x, int y, int Size) {
	for (; Size >= 0; Size--, ++y)
		GUI.DrawLineH(x - Size, y, x + Size);
}

void DropDown::_AdjustHeight() {
	auto Height = textHeight;
	if (!Height)
		Height = Props.pFont->YDist;
	Height += EffectSize() + 2 * Props.TextBorderSize;
	SizeY(Height);
}
void DropDown::_SelectByKey(int Key) {
	Key = _Tolower(Key);
	for (int i = 0, NumItems = Handles.NumItems(); i < NumItems; ++i) {
		auto c = _Tolower(*Handles[i]);
		if (c == Key) {
			Sel(i);
			break;
		}
	}
}

void DropDown::_OnPaint() const {
	/* Do some initial calculations */
	auto Border = EffectSize();
	auto TextBorderSize = Props.TextBorderSize;
	GUI.Font(Props.pFont);
	auto ColorIndex = Focussed() ? 2 : 1;
	auto &&r = ClientRect() / Border;
	auto InnerSize = r.ysize();
	/* Draw the 3D effect (if configured) */
	DrawDown();
	/* Draw the outer Text frames */
	r.x1 -= InnerSize; /* Spare square area to the right */
	GUI.PenColor(Props.aBkColor[ColorIndex]);
	if (sel > 0) {
		/* Draw the Text */
		GUI.BkColor(Props.aBkColor[ColorIndex]);
		GUI.Fill(r);
		r.x0 += TextBorderSize;
		r.x1 -= TextBorderSize;
		GUI.PenColor(Props.aTextColor[ColorIndex]);
		GUI.TextAlign(Props.Align);
		GUI.DispString(Handles[sel], r);
	}
	/* Draw arrow */
	r = ClientRect() / Border;
	r.x0 = r.x1 + 1 - InnerSize;
	GUI.PenColor(RGBC_GRAY(0xc0));
	GUI.Fill(r);
	GUI.PenColor(RGB_BLACK);
	_DrawTriangleDown((r.x1 + r.x0) / 2, r.y0 + 5, (r.y1 - r.y0 - 8) / 2);
	DrawUp(r);
}
bool DropDown::_OnTouch(const PID_STATE *pState) {
	if (pState) {
		if (pState->Pressed)
			NotifyParent(WN_CLICKED);
		else
			NotifyParent(WN_RELEASED);
	}
	else
		NotifyParent(WN_MOVED_OUT);
	return false;
}
bool DropDown::_OnKey(const KEY_STATE *pKi) {
	if (pKi->PressedCnt > 0)
		return AddKey(pKi->Key);
	return false;
}

WM_RESULT DropDown::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (DropDown *)pWin;
	bool IsExpandedBeforeMsg = pObj->pListbox;
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
		case WM_PID_STATE_CHANGED:
			if (!IsExpandedBeforeMsg) /* Make sure we do not react a second time */
				if (const PID_CHANGED_STATE *pInfo = Param)
					if (pInfo->Pressed)
						pObj->Expand();
			break;
		case WM_DELETE:
			pObj->~DropDown();
			return 0;
		case WM_NOTIFY_PARENT:
			switch ((int)Param) {
			case WN_SCROLL_CHANGED:
				pObj->NotifyParent(WN_SCROLL_CHANGED);
				break;
			case WN_CLICKED:
				pObj->Sel(pObj->pListbox->Sel());
				pObj->pListbox->Focus();
				break;
			case LISTBOX_NOTIFICATION_LOST_FOCUS:
				pObj->Collapse();
				break;
			}
			break;
		case WM_GET_CLASS:
			return ClassNames[WCLS_DROPDOWN];
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

DropDown::DropDown(const SRect &rc,
				   PWObj pParent, uint16_t Id,
				   WM_CF Flags, DROPDOWN_CF FlagsEx) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE),
	Flags(FlagsEx),
	ySizeEx(rc.ysize())
{ _AdjustHeight(); }
DropDown::~DropDown() {
	Handles.Delete();
	if (pListbox) {
		pListbox->Destroy();
		pListbox = nullptr;
	}
}

void DropDown::Collapse() {
	if (pListbox) {
		pListbox->Destroy();
		pListbox = nullptr;
	}
}
void DropDown::Expand() {
	if (pListbox) return;
	/* Get coordinates of window in client coordiantes of parent */
	auto &&r = Rect();
	if (Flags & DROPDOWN_CF_UP)
		r.y0 -= ySizeEx;
	else
		r.y0 = r.y1;
	//pListbox = new ListBox(
	//	r.x0, r.y0,
	//	SizeX(), ySizeEx,
	//	WM_UNATTACHED, 0,
	//	WC_VISIBLE | WC_STAYONTOP, 0, NumItems());
	if (Flags & DROPDOWN_CF_AUTOSCROLLBAR) {
		pListbox->ScrollBarWidth(ScrollbarWidth);
		pListbox->AutoScrollV(true);
	}
	for (int i = 0, NumItems = this->NumItems(); i < NumItems; ++i)
		pListbox->Add(Handles[i]);
	//for (int i = 0; i < GUI_COUNTOF(Props.aBkColor); ++i)
	//	pListbox->BkColor(i, Props.aBkColor[i]);
	//for (int i = 0; i < GUI_COUNTOF(Props.aTextColor); ++i)
	//	pListbox->TextColor(i, Props.aTextColor[i]);
	pListbox->Spacing(ItemSpacing);
	pListbox->Font(Props.pFont);
	pListbox->Focus();
	pListbox->Owner(this);
	pListbox->Sel(sel);
	NotifyParent(WN_CLICKED);
}

bool DropDown::AddKey(int Key) {
	switch (Key) {
		case GUI_KEY_DOWN:
			Inc();
			return true;
		case GUI_KEY_UP:
			Dec();
			return true;
		default:
			_SelectByKey(Key);
			return true;
	}
	return false;
}

void DropDown::Insert(uint16_t Index, const char *s) {
	if (!s) return;
	auto NumItems = this->NumItems();
	if (Index >= NumItems) {
		Add(s);
		if (pListbox)
			pListbox->Add(s);
		return;
	}
	Handles.Insert(Index) = s;
	Invalidate();
	if (pListbox)
		pListbox->Insert(Index, s);
}
void DropDown::Delete(uint16_t Index) {
	auto NumItems = this->NumItems();
	if (Index >= NumItems)
		return;
	Handles.Delete(Index);
	Invalidate();
	if (pListbox)
		pListbox->Delete(Index);
}

//void DROPDOWN_SetFont(DropDown *pObj, CFont *pfont) {
//	if (!pObj)
//		return;
//	int OldHeight = GUI_GetYDistOfFont(pObj->Props.pFont);
//	pObj->Props.pFont = pfont;
//	DROPDOWN__AdjustHeight(pObj);
//	pObj->Invalidate();
//	if (pObj->pListbox) {
//		if (OldHeight != GUI_GetYDistOfFont(pObj->Props.pFont)) {
//			DROPDOWN_Collapse(pObj);
//			DROPDOWN_Expand(pObj);
//		}
//		LISTBOX_SetFont(pObj->pListbox, pfont);
//	}
//}
//void DROPDOWN_SetBkColor(DropDown *pObj, unsigned int Index, RGBC color) {
//	if (!pObj)
//		return;
//	if (Index >= GUI_COUNTOF(pObj->Props.aBkColor))
//		return;
//	pObj->Props.aBkColor[Index] = color;
//	pObj->Invalidate();
//	if (pObj->pListbox)
//		LISTBOX_SetBkColor(pObj->pListbox, Index, color);
//}
//void DROPDOWN_SetTextColor(DropDown *pObj, unsigned int Index, RGBC color) {
//	if (!pObj)
//		return;
//	if (Index >= GUI_COUNTOF(pObj->Props.aBkColor))
//		return;
//	pObj->Props.aTextColor[Index] = color;
//	pObj->Invalidate();
//	if (pObj->pListbox)
//		LISTBOX_SetTextColor(pObj->pListbox, Index, color);
//}

void DropDown::Sel(int16_t sel) {
	int NumItems = Handles.NumItems();
	int MaxSel = NumItems ? NumItems - 1 : 0;
	if (sel > MaxSel)
		sel = MaxSel;
	if (this->sel == sel)
		return;
	this->sel = sel;
	Invalidate();
	NotifyParent(WN_SEL_CHANGED);
}

void DropDown::AutoScroll(bool bEnable) {
	auto Flags = this->Flags;
	if (bEnable)
		Flags |= DROPDOWN_CF_AUTOSCROLLBAR;
	else
		Flags &= ~DROPDOWN_CF_AUTOSCROLLBAR;
	if (this->Flags == Flags)
		return;
	this->Flags = Flags;
	if (pListbox)
		pListbox->AutoScrollV(bEnable);
}
