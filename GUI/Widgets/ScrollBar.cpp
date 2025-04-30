#include "ScrollBar.h"

ScrollBar::Property ScrollBar::DefaultProps;

static int _DivideRound(int a, int b) { return b ? (a + b / 2) / b : 0; }

int ScrollBar::_GetArrowSize() const {
	auto size = Size();
	if (StatusEx & SCROLLBAR_CF_VERTICAL)
		size = ~size;
	auto r = size.y / 2 + 5;
	return r > size.x - 5 ? size.x - 5 : r;
}
ScrollBar::Positions ScrollBar::_CalcPositions() const {
	Positions Pos;
	auto &&r = Rect();
	auto &&WndPos = r.left_top();
	Pos.x1 = (StatusEx & WC_EX_VERTICAL) ? r.y1 : r.x1;
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
	if (StatusEx & WC_EX_VERTICAL)
		r = r.rot270().ymove(Rect().dx());
	auto NumItems = state.NumItems;
	auto xSize = r.xsize(),
		xSizeArrow = _GetArrowSize(),
		xSizeThumbArea = xSize - 2 * xSizeArrow,
		ThumbSize = _DivideRound(xSizeThumbArea * state.PageSize, NumItems);
	if (ThumbSize < 4)
		ThumbSize = 4;
	if (ThumbSize > xSizeThumbArea)
		ThumbSize = xSizeThumbArea;
	int xSizeMoveable = xSizeThumbArea - ThumbSize;
	Pos.x0_LeftArrow = r.x0;
	Pos.x1_LeftArrow = xSizeArrow - 1;
	Pos.x1_RightArrow = xSize - 1;
	Pos.x0_RightArrow = xSize - xSizeArrow;
	Pos.x0_Thumb = Pos.x1_LeftArrow + 1 + _DivideRound(
		xSizeMoveable * state.v, NumItems - state.PageSize);
	Pos.x1_Thumb = Pos.x0_Thumb + ThumbSize - 1;
	Pos.xSizeMoveable = xSizeMoveable;
	Pos.ThumbSize = ThumbSize;
	return Pos;
}
void ScrollBar::_InvalidatePartner() {
	if (auto pAnotherScroll = Partner())
		pAnotherScroll->Invalidate();
	Parent()->SendMessage(WM_NOTIFY_CLIENT_CHANGE);
}
void ScrollBar::_DrawTriangle(int x, int y, int Size, int Inc) const {
	if (StatusEx & WC_EX_VERTICAL)
		for (; Size >= 0; Size--, x += Inc)
			GUI.DrawLineH(y - Size, x, y + Size);
	else
		for (; Size >= 0; Size--, x += Inc)
			GUI.DrawLineV(x, y - Size, y + Size);
}
void ScrollBar::_ScrollbarPressed() {
	OrState(SCROLLBAR_CF__PRESSED);
	if (Status & WC_VISIBLE)
		NotifyParent(WN_CLICKED);
}
void ScrollBar::_ScrollbarReleased() {
	MaskState(SCROLLBAR_CF__PRESSED);
	if (Status & WC_VISIBLE)
		NotifyParent(WN_RELEASED);
}

SRect ScrollBar::_AutoSize(PWObj pParent, uint16_t FlagsEx) const {
	auto &&rcInside = pParent->InsideRect();
	if (FlagsEx & SCROLLBAR_CF_VERTICAL)
		return{ rcInside.x1 - DefaultWidth, rcInside.y0, rcInside.x1, rcInside.y1 };
	return{ rcInside.x0, rcInside.y1 - DefaultWidth, rcInside.x1, rcInside.y1 };
}

void ScrollBar::_OnPaint() {
	auto &&Pos = _CalcPositions();
	auto &&rClient = ClientRect();
	int ArrowSize = rClient.dy() / 3 - 1,
		ArrowOff = 3 + ArrowSize + ArrowSize / 3;
	/* Left Arrow */
	auto r = rClient;
	r.x0 = Pos.x0_LeftArrow;
	r.x1 = Pos.x1_LeftArrow;
	GUI.PenColor(Props.aColor);
	Fill(r);
	GUI.PenColor(Props.aBkColor[1]);
	_DrawTriangle(r.x0 + ArrowOff, r.dy() / 2, ArrowSize, -1);
	DrawUp(r);
	/* Thumb Overlap */
	GUI.PenColor(Props.aBkColor[0]);
	r.x0 = Pos.x1_LeftArrow + 1;
	r.x1 = Pos.x0_Thumb - 1;
	Fill(r);
	r = rClient;
	r.x0 = Pos.x1_Thumb + 1;
	r.x1 = Pos.x0_RightArrow - 1;
	Fill(r);
	/* Thumb */
	r = rClient;
	r.x0 = Pos.x0_Thumb;
	r.x1 = Pos.x1_Thumb;
	GUI.PenColor(Props.aColor);
	Fill(r);
	DrawUp(r);
	/* Right Arrow */
	GUI.PenColor(Props.aColor);
	r.x0 = Pos.x0_RightArrow;
	r.x1 = Pos.x1_RightArrow;
	Fill(r);
	GUI.PenColor(Props.aBkColor[1]);
	_DrawTriangle(r.x1 - ArrowOff, r.dy() >> 1, ArrowSize, 1);
	DrawUp(r);
	/* Overlap Area */
	if (Pos.x1_RightArrow == Pos.x1)
		return;
	r.x0 = Pos.x1_RightArrow + 1;
	r.x1 = Pos.x1;
	GUI.PenColor(Props.aColor);
	Fill(r);
}
void ScrollBar::_OnTouch(const PID_STATE *pState) {
	if (!pState)
		return;
	if (!pState->Pressed) {
		if (StatusEx & SCROLLBAR_CF__PRESSED)
			_ScrollbarReleased();
		return;
	}
	int Sel = state.v;
	auto &&Pos = _CalcPositions();
	int Range = state.NumItems - state.PageSize;
	int x = StatusEx & WC_EX_VERTICAL ? pState->y : pState->x;
	if (x <= Pos.x1_LeftArrow)
		Sel--;
	else if (x < Pos.x0_Thumb)
		Sel -= state.PageSize;
	else if (x <= Pos.x1_Thumb) {
		if (Pos.xSizeMoveable > 0) {
			x -= Pos.ThumbSize / 2 + Pos.x1_LeftArrow + 1;
			Sel = _DivideRound(Range * x, Pos.xSizeMoveable);
		}
	}
	else if (x < Pos.x0_RightArrow)
		Sel += state.PageSize;
	else if (x <= Pos.x1_RightArrow)
		Sel++;
	Capture(true);
	Value(Sel);
	if (!(StatusEx & SCROLLBAR_CF__PRESSED))
		_ScrollbarPressed();
}
void ScrollBar::_OnKey(const KEY_STATE *pKeyInfo) {
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

WM_RESULT ScrollBar::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (ScrollBar *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	switch (MsgId) {
	case WM_PAINT:
		pObj->_OnPaint();
		return 0;
	case WM_MOUSE_KEY:
		pObj->_OnTouch(Param);
		return 0;
	case WM_KEY:
		pObj->_OnKey(Param);
		return 0;
	case WM_DELETE:
		pObj->_InvalidatePartner();
		return 0;
	case WM_GET_CLASS:
		return ClassNames[WCLS_SCROLLBAR];
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

ScrollBar::ScrollBar(const SRect &rc,
					 PWObj pParent, uint16_t Id,
					 WM_CF Flags, SCROLLBAR_CF FlagsEx,
					 SCROLL_STATE State) :
	Widget(rc ? rc : _AutoSize(pParent, FlagsEx),
		   _Callback,
		   pParent, Id,
		   Flags, FlagsEx),
	state(State)
{ _InvalidatePartner(); }
ScrollBar::ScrollBar(PWObj pParent, uint16_t Id) :
	ScrollBar({},
			  pParent, Id,
			  (Id == GUI_ID_VSCROLL ? WC_ANCHOR_TOP : WC_ANCHOR_LEFT) |
				(WC_STAYONTOP | WC_ANCHOR_RIGHT | WC_ANCHOR_BOTTOM),
			  (Id == GUI_ID_VSCROLL ? SCROLLBAR_CF_VERTICAL : 0))
{ NotifyParent(WN_SCROLLBAR_ADDED); }

ScrollBar *ScrollBar::Partner() {
	uint16_t Id = this->Id;
	if (Id == GUI_ID_HSCROLL)
		Id = GUI_ID_VSCROLL;
	else if (Id == GUI_ID_VSCROLL)
		Id = GUI_ID_HSCROLL;
	return (ScrollBar *)Parent()->DialogItem(Id);
}
