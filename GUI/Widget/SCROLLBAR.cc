#include "ScrollBar.h"

ScrollBar::Property ScrollBar::DefaultProps;

static void _WIDGET__RECT2VRECT(const Widget *pWidget, SRect *pRect) {
	int xSize = pWidget->Rect().dx();
	int x0 = pRect->x0, x1 = pRect->x1;
	pRect->x0 = pRect->y0;
	pRect->y0 = xSize - x1;
	pRect->x1 = pRect->y1;
	pRect->y1 = xSize - x0;
}

int ScrollBar::_GetArrowSize() const {
	auto size = Size();
	auto r = size.y / 2 + 5;
	return r > size.x - 5 ? size.x - 5 : r;
}
ScrollBar::Positions ScrollBar::_CalcPositions() const {
	Positions Pos;
	auto &&r = Rect();
	auto &&WndPos = r.left_top();
	Pos.x1 = (State & WIDGET_STATE_VERTICAL) ? r.y1 : r.x1;
	if (Id == GUI_ID_HSCROLL) {
		if (auto pScroll = Parent()->ScrollBarV()) {
			auto &&rSub = pScroll->Rect();
			if (r.x1 == rSub.x1)
				r.x1 = rSub.x0 - 1;
		}
	}
	if (Id == GUI_ID_VSCROLL) {
		if (auto pScroll = Parent()->ScrollBarH()) {
			auto &&rSub = pScroll->Rect();
			if (r.y1 == rSub.y1)
				r.y1 = rSub.y0 - 1;
		}
	}
	r -= WndPos;
	if (State & WIDGET_STATE_VERTICAL)
		_WIDGET__RECT2VRECT(this, &r);
	auto NumItems = state.NumItems;
	auto xSize = r.xsize(),
		xSizeArrow = _GetArrowSize(),
		xSizeThumbArea = xSize - 2 * xSizeArrow,
		ThumbSize = GUI__DivideRound(xSizeThumbArea * state.PageSize, NumItems);
	if (ThumbSize < 4)
		ThumbSize = 4;
	if (ThumbSize > xSizeThumbArea)
		ThumbSize = xSizeThumbArea;
	int xSizeMoveable = xSizeThumbArea - ThumbSize;
	Pos.x0_LeftArrow = r.x0;
	Pos.x1_LeftArrow = xSizeArrow - 1;
	Pos.x1_RightArrow = xSize - 1;
	Pos.x0_RightArrow = xSize - xSizeArrow;
	Pos.x0_Thumb = Pos.x1_LeftArrow + 1 + GUI__DivideRound(
		xSizeMoveable * state.v, NumItems - state.PageSize);
	Pos.x1_Thumb = Pos.x0_Thumb + ThumbSize - 1;
	Pos.xSizeMoveable = xSizeMoveable;
	Pos.ThumbSize = ThumbSize;
	return Pos;
}
void ScrollBar::_InvalidatePartner() {
	if (auto pAnotherScroll = WM_GetScrollPartner(this))
		pAnotherScroll->Invalidate();
	Parent()->SendMessage(WM_NOTIFY_CLIENTCHANGE);
}
void ScrollBar::_DrawTriangle(int x, int y, int Size, int Inc) const {
	if (State & WIDGET_STATE_VERTICAL)
		for (; Size >= 0; Size--, y += Inc)
			GUI.DrawLineH(x, y - Size, y + Size);
	else
		for (; Size >= 0; Size--, x += Inc)
			GUI.DrawLineV(x, y - Size, y + Size);
}
void ScrollBar::_ScrollbarPressed() {
	OrState(SCROLLBAR_STATE_PRESSED);
	if (Status & WC_VISIBLE)
		NotifyParent(WM_NOTIFICATION_CLICKED);
}
void ScrollBar::_ScrollbarReleased() {
	AndState(SCROLLBAR_STATE_PRESSED);
	if (Status & WC_VISIBLE)
		NotifyParent(WM_NOTIFICATION_RELEASED);
}
void ScrollBar::_OnPaint() {
	auto &&Pos = _CalcPositions();
	auto &&rClient = ClientRect();
	int ArrowSize = rClient.dy() / 3 - 1,
		ArrowOff = 3 + ArrowSize + ArrowSize / 3;
	GUI.PenColor(Props.aColor[0]);

	auto r = rClient;
	r.x0 = Pos.x0_LeftArrow;
	r.x1 = Pos.x1_LeftArrow;
	FillRect(r);
	GUI.PenColor(Props.aBkColor[1]);
	_DrawTriangle(r.x0 + ArrowOff, r.dy() >> 1, ArrowSize, -1);
	DrawUp(r);

	GUI.PenColor(Props.aBkColor[0]);
	r.x0 = Pos.x1_LeftArrow + 1;
	r.x1 = Pos.x0_Thumb - 1;
	FillRect(r);
	r = rClient;
	r.x0 = Pos.x1_Thumb + 1;
	r.x1 = Pos.x0_RightArrow - 1;
	FillRect(r);

	r = rClient;
	r.x0 = Pos.x0_Thumb;
	r.x1 = Pos.x1_Thumb;
	GUI.PenColor(Props.aColor[0]);
	FillRect(r);
	DrawUp(r);

	GUI.PenColor(Props.aColor[0]);
	r.x0 = Pos.x0_RightArrow;
	r.x1 = Pos.x1_RightArrow;
	FillRect(r);
	GUI.PenColor(Props.aBkColor[1]);
	_DrawTriangle(r.x1 - ArrowOff, r.dy() >> 1, ArrowSize, 1);
	DrawUp(r);

	if (Pos.x1_RightArrow == Pos.x1)
		return;
	r.x0 = Pos.x1_RightArrow + 1;
	r.x1 = Pos.x1;
	GUI.PenColor(Props.aColor[0]);
	FillRect(r);
}
void ScrollBar::_OnTouch(WM_MSG *pMsg) {
	auto pState = (PidState *)pMsg->data;
	if (!pState)
		return;
	if (!pState->Pressed) {
		if (State & SCROLLBAR_STATE_PRESSED)
			_ScrollbarReleased();
		return;
	}
	int Sel = state.v;
	auto &&Pos = _CalcPositions();
	int Range = state.NumItems - state.PageSize;
	if (State & WIDGET_STATE_VERTICAL) {
		int t = pState->x;
		pState->x = pState->y;
		pState->y = t;
	}
	int x = pState->x;
	if (x <= Pos.x1_LeftArrow)
		Sel--;
	else if (x < Pos.x0_Thumb)
		Sel -= state.PageSize;
	else if (x <= Pos.x1_Thumb) {
		if (Pos.xSizeMoveable > 0) {
			x = x - Pos.ThumbSize / 2 - Pos.x1_LeftArrow - 1;
			Sel = GUI__DivideRound(Range * x, Pos.xSizeMoveable);
		}
	}
	else if (x < Pos.x0_RightArrow)
		Sel += state.PageSize;
	else if (x <= Pos.x1_RightArrow)
		Sel++;
	Capture(true);
	Value(Sel);
	if (!(State & SCROLLBAR_STATE_PRESSED))
		_ScrollbarPressed();
}
void ScrollBar::_OnKey(WM_MSG *pMsg) {
	auto pKeyInfo = (const WM_KEY_INFO *)pMsg->data;
	int Key = pKeyInfo->Key;
	if (pKeyInfo->PressedCnt <= 0)
		return;
	switch (Key) {
	case GUI_KEY_RIGHT:
	case GUI_KEY_DOWN:
		Inc();
		break;
	case GUI_KEY_LEFT:
	case GUI_KEY_UP:
		Dec();
		break;
	default:
		return;
	}
}

void ScrollBar::_Callback(WM_MSG *pMsg) {
	auto pObj = (ScrollBar *)pMsg->pWin;
	if (!pObj->HandleActive(pMsg))
		return;
	switch (pMsg->msgid) {
	case WM_DELETE:
		pObj->_InvalidatePartner();
		break;
	case WM_PAINT:
		pObj->_OnPaint();
		return;
	case WM_TOUCH:
		pObj->_OnTouch(pMsg);
		break;
	case WM_KEY:
		pObj->_OnKey(pMsg);
		break;
	}
	DefCallback(pMsg);
}

SRect ScrollBar::_AutoSize(WObj *pParent, uint16_t ExFlags) const {
	auto &&rect = pParent->InsideRect();
	if (ExFlags & SCROLLBAR_CF_VERTICAL)
		return{ rect.x1 - DefaultWidth, rect.y0, rect.x1, rect.y1 };
	return{ rect.x0, rect.y1 - DefaultWidth, rect.x1, rect.y1 };
}

ScrollBar::ScrollBar(
	int x0, int y0, int xsize, int ysize,
	WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags) :
	Widget((x0 | y0) ? SRect::left_top({ x0, y0 }, { xsize, ysize }) :
		   _AutoSize(pParent, ExFlags),
		   &_Callback, pParent, Id, Flags, ExFlags) {
	_InvalidatePartner();
}
ScrollBar::ScrollBar(WObj *pParent, int SpecialFlags) : ScrollBar(
	0, 0, 0, 0,
	pParent, SpecialFlags & SCROLLBAR_CF_VERTICAL ? GUI_ID_VSCROLL : GUI_ID_HSCROLL, 
	((SpecialFlags & SCROLLBAR_CF_VERTICAL) ? WC_ANCHOR_TOP : WC_ANCHOR_LEFT) |
	(WC_VISIBLE | WC_STAYONTOP | WC_ANCHOR_RIGHT | WC_ANCHOR_BOTTOM),
	 SpecialFlags)
{ NotifyParent(WM_NOTIFICATION_SCROLLBAR_ADDED); }
