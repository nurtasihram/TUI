#include "ScrollBar.h"
#include "MultiPage.h"

MultiPage::Property MultiPage::DefaultProps;

void MultiPage::_ShowPage(unsigned Index) {
	auto pWin = Handles[Index].pWin;
	for (auto pChild = pClient->FirstChild(); pChild; pChild = pChild->NextSibling())
		if (pChild == pWin) {
			pChild->Visible(true);
			pChild->Focus();
		}
		else
			pChild->Visible(false);
}
SRect MultiPage::_CalcClientRect() {
	auto &&rInside = ClientRect();
	if (align & MULTIPAGE_ALIGN_BOTTOM)
		rInside.y1 -= Props.pFont->YSize + 6;
	else
		rInside.y0 += Props.pFont->YSize + 6;
	return rInside;
}
SRect MultiPage::_CalcBorderRect() {
	auto &&r = ClientRect();
	if (align & MULTIPAGE_ALIGN_BOTTOM)
		r.y1 -= Props.pFont->YSize + 6;
	else
		r.y0 += Props.pFont->YSize + 6;
	return r;
}
int MultiPage::_GetPageSizeX(unsigned Index) {
	return GUI.Font()->Size(Handles[Index].Text).x + 10;
}
int MultiPage::_GetPagePosX(unsigned Index) {
	int r = 0;
	for (unsigned i = 0; i < Index; ++i)
		r += _GetPageSizeX(i);
	return r;
}
int MultiPage::_GetTextWidth() {
	return _GetPagePosX(Handles.NumItems());
}
SRect MultiPage::_GetTextRect() {
	int Height = Props.pFont->YSize + 6;
	auto &&rBorder = _CalcBorderRect();
	SRect rText;
	rText.y0 = (align & MULTIPAGE_ALIGN_BOTTOM) ?
		rBorder.y1 : 0;
	rText.y1 = rText.y0 + Height;
	int Width = (StatusEx & MULTIPAGE_STATE_SCROLLMODE) ?
		rBorder.x1 - ((Height * 3) >> 1) - 3 :
		_GetTextWidth();
	if (align & MULTIPAGE_ALIGN_RIGHT) {
		rText.x0 = rBorder.x1 - Width;
		rText.x1 = rBorder.x1;
	}
	else {
		rText.x0 = 0;
		rText.x1 = Width;
	}
	return rText;
}
void MultiPage::_UpdatePositions() {
	auto Width = _GetTextWidth();
	auto &&rBorder = _CalcBorderRect();
	/* Set scrollmode according to the text width */
	if (Width > rBorder.x1) {
		auto &&rText = _GetTextRect();
		auto xText = rText.xsize();
		if (xText < 1)
			xText = 1;
		int16_t NumItems = 0;
		while (Width >= xText)
			Width -= _GetPageSizeX(NumItems++);
		Point size = ((Props.pFont->YSize + 6) * 3) / 2;
		size.y /= 2;
		Point pos = {
			(align & MULTIPAGE_ALIGN_RIGHT) ? rBorder.x0 : rBorder.x1 - 2 * size.y + 1,
			(align & MULTIPAGE_ALIGN_BOTTOM) ? rBorder.y1 : rBorder.y0 - size.y + 1
		};
		auto &&rcScroll = SRect::left_top(pos, size);
		SCROLL_STATE StateScroll{ NumItems, ScrollValue, 1 };
		auto pScroll = ScrollBarH();
		if (pScroll) {
			pScroll->Rect(rcScroll);
			pScroll->ScrollState(StateScroll);
		}
		else {
			pScroll = new ScrollBar(
				rcScroll,
				this, GUI_ID_HSCROLL,
				WC_VISIBLE, 0,
				StateScroll);
			pScroll->Effect(Effect());
		}
		if (ScrollValue > NumItems)
			ScrollValue = 0;
		StatusEx |= MULTIPAGE_STATE_SCROLLMODE;
	}
	else {
		ScrollBarH(false);
		StatusEx &= ~MULTIPAGE_STATE_SCROLLMODE;
	}
	/* Move and resize the client area to the updated positions */
	pClient->Rect(_CalcClientRect());
	Invalidate();
}
void MultiPage::_DrawTextItem(const char *pText, unsigned Index,
							  SRect r, int x0, int w, MULTIPAGE_CI ColorIndex) {
	r.x0 += x0;
	r.x1 = r.x0 + w;
	DrawUp(r);
	r /= EffectSize();
	GUI.PenColor(Props.aBkColor[ColorIndex]);
	Fill(r);
	GUI.BkColor(Props.aBkColor[ColorIndex]);
	GUI.PenColor(Props.aTextColor[ColorIndex]);
	GUI.DrawStringAt(pText, { r.x0 + 4, r.y0 + 3 });
	if (Sel == Index)
		GUI.DrawFocus(r);
}
bool MultiPage::_ClickedOnMultipage(Point Pos) {
	auto &&rText = _GetTextRect();
	if (Pos.y < rText.y0 || Pos.y > rText.y1)
		return true;
	if (Handles.NumItems() <= 0 || Pos.x < rText.x0 || Pos.x > rText.x1)
		return false;
	int w = 0, x0 = rText.x0;
	if (StatusEx & MULTIPAGE_STATE_SCROLLMODE)
		x0 -= _GetPagePosX(ScrollValue);
	for (int i = 0; i < Handles.NumItems(); ++i) {
		x0 += w;
		w = _GetPageSizeX(i);
		if (Pos.x >= x0 && Pos.x <= x0 + w - 1) {
			Select(i);
			NotifyParent(WN_VALUE_CHANGED);
			return true;
		}
	}
	return false;
}

void MultiPage::_OnPaint() {
	DrawUp(_CalcBorderRect());
	if (Handles.NumItems() <= 0)
		return;
	int x0 = 0;
	if (StatusEx & MULTIPAGE_STATE_SCROLLMODE)
		x0 = (align & MULTIPAGE_ALIGN_RIGHT) ?
		-_GetPagePosX(ScrollValue) :
		-_GetPagePosX(ScrollValue);
	auto &&rText = _GetTextRect();
	auto Width = rText.xsize();
	auto rClip = rText;
	rClip.y0 = rText.y0 - 1;
	rClip.y1 = rText.y1 + 1;
	GUI.BkColor(Parent()->BkColor());
	GUI.Clear();
	WObj::UserClip(&rClip);
	GUI.Font(Props.pFont);
	int w = 0;
	for (int i = 0; i < Handles.NumItems(); ++i) {
		auto &page = Handles[i];
		x0 += w;
		w = Props.pFont->Size(page.Text).x + 10;
		if (w >= Width) break;
		_DrawTextItem(page.Text, i, rText, x0, w,
					  (page.Status & MULTIPAGE_STATE_ENABLED) ?
						  MULTIPAGE_CI_ENABLED :
						  MULTIPAGE_CI_DISABLED);
	}
	WObj::UserClip(nullptr);
}
void MultiPage::_OnTouch(PID_STATE *pState) {
	int Notification;
	if (pState) {
		if (pState->Pressed) {
			Point Pos = *pState;
			if (_ClickedOnMultipage(Pos))
				BringToTop();
			else {
				Pos = Client2Screen(Pos);
				if (auto pBelow = WObj::FindOnScreen(Pos, this)) {
					*pState = pBelow->Screen2Client(Pos);
					pBelow->Callback()(pBelow, WM_MOUSE_KEY, pState, nullptr);
				}
			}
			Notification = WN_CLICKED;
		}
		else
			Notification = WN_RELEASED;
	}
	else
		Notification = WN_MOVED_OUT;
	NotifyParent(Notification);
}

WM_PARAM MultiPage::_ClientCb(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = pWin;
	auto pParent = (MultiPage *)pObj->Parent();
	switch (MsgId) {
		case WM_PAINT:
			GUI.BkColor(pObj->BkColor());
			GUI.Clear();
			return 0;
		case WM_MOUSE_KEY:
			pParent->Focus();
			pParent->BringToTop();
			return 0;
		case WM_GET_BKCOLOR:
			return pParent->Props.aBkColor[
				pParent->Enable(pParent->Select()) ?
					MULTIPAGE_CI_ENABLED :
					MULTIPAGE_CI_DISABLED
			];
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}
WM_PARAM MultiPage::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (MultiPage *)pWin;
	bool Handled = pObj->HandleActive(MsgId, Param);
	switch (MsgId) {
	case WM_PAINT:
		pObj->_OnPaint();
		return 0;
	case WM_MOUSE_KEY:
		pObj->_OnTouch(Param);
		return 0;
	case WM_NOTIFY_CHILD:
		switch ((int)Param) {
			case WN_VALUE_CHANGED:
				if (pSrc->ID() == GUI_ID_HSCROLL) {
					pObj->ScrollValue = ((ScrollBar *)pSrc)->Value();
					pObj->Invalidate();
				}
				break;
		}
		break;
	case WM_GET_CLIENT_WINDOW:
		return pObj->pClient;
	case WM_GET_INSIDE_RECT:
		return pObj->_CalcClientRect();
	case WM_GET_SERVE_RECT:
		return pObj->pClient->ServeRect();
	case WM_WIDGET_SET_EFFECT:
		if (auto pScroll = pObj->ScrollBarH())
			pScroll->Effect(Param);
		break;
	case WM_SIZE:
		pObj->_UpdatePositions();
		return 0;
	case WM_DELETE:
		pObj->Handles.Delete();
		return 0;
	case WM_GET_CLASS:
		return ClassNames[WCLS_MULTIPAGE];
	}
	if (Handled)
		return DefCallback(pObj, MsgId, Param, pSrc);
	return 0;
}

MultiPage::MultiPage(
	const SRect &rc,
	PWObj pParent, uint16_t Id,
	WM_CF Flags, WC_EX FlagsEx) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, FlagsEx),
	pClient(new WObj(
		_CalcClientRect(),
		_ClientCb,
		this, 0,
		WC_VISIBLE | WC_ANCHOR_MASK))
{ _UpdatePositions(); }

void MultiPage::Add(const char *pText, PWObj pWin) {
	if (!pWin)
		pWin = new WObj(
			PageRect(),
			DefCallback,
			pClient, 0,
			WC_VISIBLE | WC_ANCHOR_MASK);
	else
		pWin->Parent(pClient);
	if (pWin) {
		auto &&page = Handles.Add();
		page.pWin = pWin;
		page.Status = MULTIPAGE_STATE_ENABLED;
		page.Text = pText;
		Select(Handles.NumItems() - 1);
	}
}
void MultiPage::Delete(unsigned Index, int Delete) {
	if (Index >= Handles.NumItems())
		return;
	auto &page = Handles[Index];
	auto pWin = page.pWin;
	if (Index == Sel) {
		if (Index == Handles.NumItems() - 1) {
			_ShowPage(Index - 1);
			--Sel;
		}
		else
			_ShowPage(Index + 1);
	}
	else if (Index < Sel)
		--Sel;
	Handles.Delete(Index);
	_UpdatePositions();
	if (Delete)
		pWin->Destroy();
}
void MultiPage::Select(unsigned Index) {
	if (Index >= Handles.NumItems())
		return;
	if (Index == Sel && Enable(Index))
		return;
	_ShowPage(Index);
	Sel = Index;
	_UpdatePositions();
}
