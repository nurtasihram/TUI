#include "DropDown.h"

static void _DrawTriangle(int x, int y, int Size, int Inc) {
	for (; Size >= 0; --Size, y += Inc)
		GUI.DrawLineH(x - Size, y, x + Size);
}

void DropDown::_AdjustHeight() {
	SizeY(pListbox->OwnerDraw()(pListbox, WIDGET_ITEM_GET_YSIZE, Sel(), {}) + EffectSize() * 2);
}
SRect DropDown::_ListBoxRect() const {
	auto &&r = RectScreen();
	if (StatusEx & DROPDOWN_CF_UP)
		r.y0 -= ySizeEx;
	else {
		r.y0 = r.y1 + 1;
		r.y1 = r.y0 + ySizeEx;
	}
	return r;
}

void DropDown::_OnPaint() const {
	auto EffectSize = this->EffectSize();
	auto &&rClient = ClientRect() / EffectSize;
	auto BtnSize = rClient.ysize();
	auto ArrowSize = rClient.ysize() / 3,
		 ArrowOff = 3 + ArrowSize + ArrowSize / 3;
	rClient.x1 -= BtnSize;
	if (NumItems() > 0)
		pListbox->OwnerDraw()(pListbox, WIDGET_ITEM_DRAW, Sel(), rClient);
	else {
		GUI.BkColor(pListbox->BkColor(LISTBOX_CI_UNSEL));
		GUI.Clear(rClient);
	}
	DrawDown();
	rClient.x0 = rClient.x1;
	rClient.x1 += BtnSize;
	GUI.PenColor(pListbox->BkColor(LISTBOX_CI_UNSEL));
	GUI.Fill(rClient);
	GUI.PenColor(RGB_BLACK);
	bool bExpand = Expand();
	int yPos = bExpand ? rClient.y0 + ArrowOff : rClient.y1 - ArrowOff;
	_DrawTriangle((rClient.x0 + rClient.x1) / 2, yPos, ArrowSize, bExpand ? -1 : 1);
	DrawUp(rClient);
}
bool DropDown::_OnMouse(MOUSE_STATE State) {
	if (State) {
		if (State.Pressed)
			NotifyOwner(WN_CLICKED);
		else
			NotifyOwner(WN_RELEASED);
	}
	else
		NotifyOwner(WN_MOVED_OUT);
	return false;
}
bool DropDown::_OnKey(KEY_STATE State) {
	if (State.PressedCnt <= 0)
		return false;
	switch (State.Key) {
		case ' ':
			Expand(!Expand());
			return true;
		case GUI_KEY_DOWN:
			Inc();
			return true;
		case GUI_KEY_UP:
			Dec();
			return true;
	}
	return false;
}

WM_RESULT DropDown::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (DropDown *)pWin;
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
	case WM_MOUSE_CHANGED: {
		MOUSE_CHANGED_STATE State = Param;
		if (State.StatePrev)
			pObj->Expand(true);
		break;
	}
	case WM_DELETE:
		pObj->~DropDown();
		return 0;
	case WM_NOTIFY_CHILD:
		switch ((int)Param) {
			case WN_SCROLL_CHANGED:
				pObj->NotifyOwner(WN_SCROLL_CHANGED);
				break;
			case WN_CLICKED:
				pObj->Sel(pObj->pListbox->Sel());
			case WN_RELEASED:
				if (pObj->Expand()) {
					pObj->Expand(false);
					pObj->Focus();
				}
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
				   WM_CF Flags, DROPDOWN_CF FlagsEx,
				   GUI_PCSTR pItems) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE,
		   FlagsEx),
	pListbox(new ListBox(
		{},
		this, 0,
		WC_STAYONTOP | WC_POPUP, 0,
		pItems)),
	ySizeEx(rc.ysize()) {
	pListbox->Effect(&EffectItf::Simple);
	_AdjustHeight();
}

void DropDown::Expand(bool bExpand) {
	if (bExpand) {
		pListbox->Rect(_ListBoxRect());
		pListbox->AutoScroll(StatusEx, true);
		pListbox->Focus();
		pListbox->Capture(false);
		NotifyOwner(WN_CLICKED);
	}
	else if (pListbox->Captured())
		pListbox->CaptureRelease();
	pListbox->Visible(bExpand);
	Invalidate();
	if (!bExpand)
		Focus();
}

void DropDown::Sel(int16_t sel) {
	pListbox->Sel(sel);
	_AdjustHeight();
	Invalidate();
}
