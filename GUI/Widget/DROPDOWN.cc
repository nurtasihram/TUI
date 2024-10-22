#include "DropDown.h"

static int _Tolower(int Key) {
	return Key >= 0x41 && Key <= 0x5a ? Key + 0x20 : Key;
}
static void _DrawTriangleDown(int x, int y, int Size) {
	for (; Size >= 0; Size--, ++y)
		GUI.DrawLineH(y, x - Size, x + Size);
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

DropDown::~DropDown() {
	Handles.Delete();
	if (pListbox) {
		delete pListbox;
		pListbox = nullptr;
	}
}

void DropDown::_OnPaint() const {
	/* Do some initial calculations */
	auto Border = EffectSize();
	auto TextBorderSize = Props.TextBorderSize;
	GUI.Font(Props.pFont);
	auto ColorIndex = (State & WIDGET_STATE_FOCUS) ? 2 : 1;
	auto s = Handles[sel];
	auto &&r = ClientRect() / Border;
	auto InnerSize = r.ysize();
	/* Draw the 3D effect (if configured) */
	DrawDown();
	/* Draw the outer text frames */
	r.x1 -= InnerSize; /* Spare square area to the right */
	GUI.PenColor(Props.aBkColor[ColorIndex]);
	/* Draw the text */
	GUI.BkColor(Props.aBkColor[ColorIndex]);
	GUI.FillRect(r);
	r.x0 += TextBorderSize;
	r.x1 -= TextBorderSize;
	GUI.PenColor(Props.aTextColor[ColorIndex]);
	GUI_DispStringInRect(s, r, Props.Align);
	/* Draw arrow */
	r = ClientRect() / Border;
	r.x0 = r.x1 + 1 - InnerSize;
	GUI.PenColor(RGBC_GRAY(0xc0));
	GUI.FillRect(r);
	GUI.PenColor(RGB_BLACK);
	_DrawTriangleDown((r.x1 + r.x0) / 2, r.y0 + 5, (r.y1 - r.y0 - 8) / 2);
	DrawUp(r);
}
bool DropDown::_OnTouch(const PidState *pState) {
	if (pState) {
		if (pState->Pressed)
			NotifyParent(WM_NOTIFICATION_CLICKED);
		else
			NotifyParent(WM_NOTIFICATION_RELEASED);
	}
	else
		NotifyParent(WM_NOTIFICATION_MOVED_OUT);
	return false;
}
bool DropDown::_OnKey(const WM_KEY_INFO *pKi) {
	if (pKi->PressedCnt > 0)
		return AddKey(pKi->Key);
	return false;
}

void DropDown::_AdjustHeight() {
	int Height = textHeight;
	if (!Height)
		Height = GUI_GetYDistOfFont(Props.pFont);
	Height += EffectSize() + 2 * Props.TextBorderSize;
	SizeY(Height);
}
void DropDown::_Callback(WM_MSG *pMsg) {
	auto pObj = (DropDown *)pMsg->pWin;
	auto IsExpandedBeforeMsg = pObj->pListbox;
	if (!pObj->HandleActive(pMsg))
		return;
	switch (pMsg->msgid) {
	case WM_NOTIFY_PARENT:
		switch (pMsg->data) {
		case WM_NOTIFICATION_SCROLL_CHANGED:
			pObj->NotifyParent(WM_NOTIFICATION_SCROLL_CHANGED);
			break;
		case WM_NOTIFICATION_CLICKED:
			pObj->Sel(pObj->pListbox->Sel());
			pObj->Focus();
			break;
		case LISTBOX_NOTIFICATION_LOST_FOCUS:
			pObj->Collapse();
			break;
		}
		break;
	case WM_PID_STATE_CHANGED:
		if (IsExpandedBeforeMsg == 0) { /* Make sure we do not react a second time */
			auto pInfo = (const WM_PID_STATE_CHANGED_INFO *)pMsg->data;
			if (pInfo->State)
				pObj->Expand();
		}
		break;
	case WM_TOUCH:
		if (!pObj->_OnTouch((const PidState *)pMsg->data))
			return;
		break;
	case WM_PAINT:
		pObj->_OnPaint();
		break;
	case WM_DELETE:
		pObj->~DropDown();
		break;
	case WM_KEY:
		if (!pObj->_OnKey((const WM_KEY_INFO *)pMsg->data))
			return;
		break;
	}
	DefCallback(pMsg);
}

DropDown::DropDown(
	int x0, int y0, int xsize, int ysize,
	WObj *pParent, uint16_t Id, uint16_t Flags, uint8_t ExFlags) : 
	Widget(x0, y0, xsize, -1,
		   _Callback,
		   pParent, Id, Flags, WIDGET_STATE_FOCUSSABLE),
	Flags(ExFlags), ySizeEx(ysize) { _AdjustHeight(); }

void DropDown::Collapse() {
	if (pListbox) {
		delete pListbox;
		pListbox = nullptr;
	}
}
void DropDown::Expand() {
	if (pListbox) return;
	auto pParent = Parent();
	int xSize = SizeX(),
		ySize = ySizeEx;
	/* Get coordinates of window in client coordiantes of parent */
	auto &&r = Rect() - pParent->Rect().left_top();
	if (Flags & DROPDOWN_CF_UP)
		r.y0 -= ySize;
	else
		r.y0 = r.y1;
	pListbox = new ListBox(
		r.x0, r.y0,
		xSize, ySize,
		Parent(), 0,
		WC_VISIBLE, 0, NumItems());
	if (Flags & DROPDOWN_CF_AUTOSCROLLBAR) {
		pListbox->ScrollBarWidth(ScrollbarWidth);
		pListbox->AutoScrollV(true);
	}
	for (int i = 0, NumItems = this->NumItems(); i < NumItems; ++i)
		pListbox->Add(Handles[i]);
	for (int i = 0; i < GUI_COUNTOF(Props.aBkColor); ++i)
		pListbox->BkColor(i, Props.aBkColor[i]);
	for (int i = 0; i < GUI_COUNTOF(Props.aTextColor); ++i)
		pListbox->TextColor(i, Props.aTextColor[i]);
	pListbox->Spacing(ItemSpacing);
	pListbox->Font(Props.pFont);
	pListbox->Focus();
	pListbox->Owner(this);
	pListbox->Sel(sel);
	NotifyParent(WM_NOTIFICATION_CLICKED);
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

void DropDown::InsertString(uint16_t Index, const char *s) {
	if (!s) return;
	auto NumItems = this->NumItems();
	if (Index >= NumItems) {
		Add(s);
		if (pListbox)
			pListbox->Add(s);
		return;
	}
	Handles.Insert(s, Index);
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
	NotifyParent(WM_NOTIFICATION_SEL_CHANGED);
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
