#include "Button.h"
#include "Frame.h"

Frame::Property Frame::DefaultProps;

void Frame::_InvalidateButton(uint16_t Id) {
	if (auto pButton = DialogItem(Id))
		pButton->Invalidate();
}
void Frame::_RestoreMinimized() {
	if (!(StatusEx & FRAME_CF_MINIMIZED))
		return;
	StatusEx &= ~FRAME_CF_MINIMIZED;
	SizeY(rRestore.ysize());
	pClient->Visible(true);
	if (pMenu) pMenu->Visible(true);
	_UpdatePositions();
	_InvalidateButton(GUI_ID_MINIMIZE);
}
void Frame::_RestoreMaximized() {
	if (!(StatusEx & FRAME_CF_MAXIMIZED))
		return;
	StatusEx &= ~FRAME_CF_MAXIMIZED;
	Size(rRestore.size());
	PositionScreen(rRestore.left_top());
	_UpdatePositions();
	_InvalidateButton(GUI_ID_MAXIMIZE);
}

uint16_t Frame::_CalcTitleHeight() const {
	if (!(StatusEx & FRAME_CF_TITLEVIS))
		return 0;
	return Props.TitleHeight ?
		Props.TitleHeight :
		2 + Props.pFont->YDist;
}
Frame::Positions Frame::_CalcPositions() const {
	auto &&size = Size();
	uint16_t BorderSize = Props.BorderSize;
	uint16_t IBorderSize = Status & FRAME_CF_TITLEVIS ? Props.IBorderSize : 0;
	uint16_t TitleHeight = _CalcTitleHeight();
	uint16_t MenuHeight = pMenu ? pMenu->SizeY() : 0;
	Positions Pos{
		TitleHeight,
		MenuHeight, {
			{ BorderSize, BorderSize + IBorderSize + TitleHeight + MenuHeight },
			  size - Point(BorderSize + 1)
		}, {
			BorderSize,
			{ size.x - BorderSize - 1, BorderSize + TitleHeight - 1 }
		}
	};
	for (auto pChild = FirstChild(); pChild; pChild = pChild->NextSibling()) {
		auto &&r = pChild->Rect();
		if (r.y1 > TitleHeight)
			continue;
		if (pChild->Styles() & WC_ANCHOR_RIGHT) {
			if (Pos.rTitleText.x1 >= r.x0)
				Pos.rTitleText.x1 = r.x0 - 1;
		}
		else if (Pos.rTitleText.x0 <= r.x1)
			Pos.rTitleText.x0 = r.x1 + 1;
	}
	return Pos;
}
void Frame::_UpdatePositions() {
	if (!pClient && !pMenu)
		return;
	auto &&Pos = _CalcPositions();
	if (pClient) {
		pClient->Position(Pos.rClient.left_top());
		pClient->Size(Pos.rClient.size());
	}
	if (pMenu) {
		Pos.rClient.y0 -= Pos.MenuHeight;
		pMenu->Position(Pos.rClient.left_top());
	}
}

#pragma region OnResize
static Point _Capture;
static uint8_t _CaptureFlags = 0;
void Frame::_SetCapture(Point Pos, uint8_t Mode) {
	if (!(_CaptureFlags & SIZE_REPOS_X))
		_Capture.x = Pos.x;
	if (!(_CaptureFlags & SIZE_REPOS_Y))
		_Capture.y = Pos.y;
	if (!Mode) return;
	if (!Captured())
		Capture(false);
	if (auto pCursor = CursorCtl::ResizeCursor(Mode))
		GUI.Cursor(pCursor);
	if (Mode & SIZE_MOUSEOVER)
		Mode = 0;
	_CaptureFlags = Mode | SIZE_MOUSEOVER;
}
constexpr uint8_t FRAME_REACT_BORDER = 3;
constexpr uint8_t FRAME_MINSIZE_X = 20;
void Frame::_ChangeWindowPosSize(Point p) {
	Point d;
	if (_CaptureFlags & SIZE_RESIZE_X)
		d.x = (_CaptureFlags & SIZE_REPOS_X) ? _Capture.x - p.x : p.x - _Capture.x;
	if (_CaptureFlags & SIZE_RESIZE_Y)
		d.y = (_CaptureFlags & SIZE_REPOS_Y) ? _Capture.y - p.y : p.y - _Capture.y;
	auto &&ptEnd = ClientRect().right_bottom();
	auto xMin = FRAME_MINSIZE_X - ptEnd.x;
	if (d.x < xMin)
		d.x = xMin;
	auto yMin = _CalcTitleHeight() + EffectSize() * 2 - ptEnd.y;
	if (d.y < yMin)
		d.y = yMin;
	Move({ _CaptureFlags & SIZE_REPOS_X ? -d.x : 0,
		   _CaptureFlags & SIZE_REPOS_Y ? -d.y : 0 });
	Resize(d);
}
uint8_t Frame::_CheckReactBorder(Point Pos) {
	static auto _CheckBorderX = [](int x, int x1, int Border) {
		return x > x1 - Border ?
			SIZE_RESIZE_X :
			x < Border ?
			SIZE_RESIZE_X | SIZE_REPOS_X : 0;
	};
	static auto _CheckBorderY = [](int y, int y1, int Border) {
		return y > y1 - Border ?
			SIZE_RESIZE_Y :
			y < Border ?
			SIZE_RESIZE_Y | SIZE_REPOS_Y : 0;
	};
	auto &&r = ClientRect();
	if (Pos.x < 0 || Pos.y < 0)
		return 0;
	if (Pos.x > r.x1 || Pos.y > r.y1)
		return 0;
	uint8_t Mode = _CheckBorderX(Pos.x, r.x1, FRAME_REACT_BORDER);
	if (Mode)
		Mode |= _CheckBorderY(Pos.y, r.y1, 4 * FRAME_REACT_BORDER);
	else {
		Mode |= _CheckBorderY(Pos.y, r.y1, FRAME_REACT_BORDER);
		if (Mode)
			Mode |= _CheckBorderX(Pos.x, r.x1, 4 * FRAME_REACT_BORDER);
	}
	return Mode;
}
bool Frame::_OnTouchResize(const MOUSE_STATE *pState) {
	if (!pState) return false;
	Point Pos = *pState;
	auto Mode = _CheckReactBorder(Pos);
	if (pState->Pressed != 1) {
		if (!Captured())
			return false;
		_CaptureFlags &= ~SIZE_RESIZE;
		if (!Mode)
			CaptureRelease();
		return true;
	}
	if (_CaptureFlags & SIZE_RESIZE) {
		_ChangeWindowPosSize(Pos);
		_SetCapture(Pos, 0);
		return true;
	}
	if (Mode) {
		Focus();
		BringToTop();
		_SetCapture(Pos, Mode);
		return true;
	}
	if (_CaptureFlags) {
		CaptureRelease();
		return true;
	}
	return false;
}
bool Frame::_ForwardMouseOverMsg(int MsgId, MOUSE_STATE State) {
	State = Client2Screen(State);
	if (auto pBelow = WObj::FindOnScreen(State)) {
		if (pBelow == this) return false;
		State = pBelow->Screen2Client(State);
		pBelow->SendMessage(MsgId, &State);
		return true;
	}
	return false;
}
bool Frame::_HandleResize(int MsgId, const MOUSE_STATE *pState) {
	if (Captured() && _CaptureFlags == 0)
		return false;
	if (Minimized() || Maximized())
		return false;
	switch (MsgId) {
		case WM_MOUSE:
			return _OnTouchResize(pState);
		case WM_MOUSE_OVER:
			if (pState) {
				if (auto Mode = _CheckReactBorder(*pState)) {
					if (!_ForwardMouseOverMsg(MsgId, *pState))
						_SetCapture(*pState, Mode | SIZE_MOUSEOVER);
					return true;
				}
				if (!Captured())
					return true;
				if (_CaptureFlags & SIZE_RESIZE)
					return true;
				CaptureRelease();
				_ForwardMouseOverMsg(MsgId, *pState);
				return true;
			}
			return false;
		case WM_CAPTURE_RELEASED:
			_CaptureFlags = 0;
			return true;
	}
	return false;
}
#pragma endregion

constexpr int16_t FRAME__MinVisibility = 5;
void Frame::_OnMouse(const MOUSE_STATE *pState) {
	if (pState)
		if (pState->Pressed) {
			if (!(StatusEx & FRAME_CF_ACTIVE))
				Focus();
			BringToTop();
			if (!(StatusEx & FRAME_CF_UNMOVEABLE))
				CaptureMove(*pState, FRAME__MinVisibility);
		}
}
void Frame::_OnPaint() const {
	auto pWin = ActiveWindow();
	auto &&size = pWin->Size() - 1;
	auto BorderSize = Props.BorderSize;
	auto &&Pos = _CalcPositions();
	auto Index = StatusEx & FRAME_CF_ACTIVE ? 1 : 0;
	SRect r{
		Pos.rClient.x0,
		Pos.rTitleText.y0,
		Pos.rClient.x1,
		Pos.rTitleText.y1
	};
	++Pos.rTitleText.y0;
	++Pos.rTitleText.x0;
	--Pos.rTitleText.x1;
	GUI.Font(Props.pFont);
	auto y0 = Pos.TitleHeight + BorderSize;
	GUI.BkColor(Props.aBarColor[Index]);
	GUI.PenColor(Props.aTextColor[Index]);
	GUI.Clear(r);
	if (Props.Border && !Index)
		GUI.DrawLineH(r.x0, r.y1, r.x1);
	GUI.TextAlign(Props.Align);
	GUI.DrawStringIn(Title, Pos.rTitleText);
	GUI.PenColor(Props.FrameColor);
	GUI.Fill({ 0, 0, size.x, BorderSize - 1 });
	GUI.Fill({ 0, 0, Pos.rClient.x0 - 1, size.y });
	GUI.Fill({ Pos.rClient.x1 + 1, 0, size.x, size.y });
	GUI.Fill({ 0, Pos.rClient.y1 + 1, size.x, size.y });
	GUI.Fill({ 0, y0, size.x, y0 + Props.IBorderSize - 1 });
	if (Props.BorderSize >= 2)
		DrawUp();
}
void Frame::_OnChildHasFocus(const FOCUS_CHANGED_STATE *pInfo) {
	if (pInfo) {
		if (pInfo->pNew) {
			if (pInfo->pNew->IsAncestorOrSelf(this)) {
				Active(true);
				return;
			}
		}
		Active(false);
		if (pInfo->pOld)
			if (pInfo->pOld->IsAncestor(this))
				pFocussedChild = pInfo->pOld;
	}
}

WM_RESULT Frame::_cbClient(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pParent = (Frame *)pWin->Parent();
	auto cb = pParent->cb;
	switch (MsgId) {
		case WM_PAINT:
			if (pParent->Props.ClientColor != RGB_INVALID_COLOR) {
				GUI.BkColor(pParent->Props.ClientColor);
				GUI.Clear();
			}
			if (cb)
				cb(pWin, WM_PAINT, Param, nullptr);
			return 0;
		case WM_KEY: {
			KEY_STATE State = Param;
			if (State.PressedCnt <= 0)
				break;
			switch (State.Key) {
				case GUI_KEY_TAB:
					pParent->pFocussedChild = pWin->FocusNextChild();
					return true;
			}
			break;
		}
		case WM_FOCUS:
			return pParent->SendMessage(MsgId, Param, pSrc);
		case WM_FOCUSSABLE:
			pParent->HandleActive(MsgId, Param);
			return Param;
		case WM_GET_BKCOLOR:
			return pParent->Props.ClientColor;
		case WM_GET_INSIDE_RECT:
		case WM_GET_ID:
		case WM_GET_CLIENT_WINDOW:
			return DefCallback(pWin, MsgId, Param, pSrc);
	}
	if (cb)
		return cb(pWin, MsgId, Param, pSrc);
	return DefCallback(pWin, MsgId, Param, pSrc);
}
WM_RESULT Frame::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (Frame *)pWin;
	if (pObj->StatusEx & FRAME_CF_RESIZEABLE)
		if (pObj->_HandleResize(MsgId, Param))
			return Param;
	switch (MsgId) {
		case WM_PAINT:
			pObj->_OnPaint();
			return 0;
		case WM_MOUSE:
			pObj->_OnMouse(Param);
			return 0;
		case WM_MOUSE_CHILD:
			if (!(pObj->StatusEx & FRAME_CF_ACTIVE))
				if (const MOUSE_STATE *pState = Param)
					if (pState->Pressed)
						pObj->Focus();
			return 0;
		case WM_DELETE:
			pObj->~Frame();
			return 0;
		case WM_HANDLE_DIALOG_STATUS:
			if (DIALOG_STATE *wds = Param)
				pObj->pDialogStatus = wds;
			else
				return pObj->pDialogStatus;
			return 0;
		case WM_GET_INSIDE_RECT:
			return pObj->_CalcPositions().rClient;
		case WM_GET_CLIENT_WINDOW:
			return pObj->pClient;
		case WM_GET_SERVE_RECT:
			return pObj->pClient->ServeRect();
		case WM_NOTIFY_CHILD:
			switch ((int)Param) {
				case WN_RELEASED:
					pObj->SendMessage(WM_NOTIFY_CHILD_REFLECT, Param, pObj);
					if (PWObj pButton = Param)
						if (pSrc)
							switch (pSrc->ID()) {
								case GUI_ID_CLOSE:
									pObj->Destroy();
									return 0;
								case GUI_ID_MAXIMIZE:
									if (pObj->StatusEx & FRAME_CF_MAXIMIZED)
										pObj->Restore();
									else
										pObj->Maximize();
									break;
								case GUI_ID_MINIMIZE:
									if (pObj->StatusEx & FRAME_CF_MINIMIZED)
										pObj->Restore();
									else
										pObj->Minimize();
									break;
						}
					break;
			}
			return 0;
		case WM_FOCUS:
			if (Param) {
				if (IsWindow(pObj->pFocussedChild))
					pObj->pFocussedChild->Focus();
				else
					pObj->pFocussedChild = pObj->pClient->FocusNextChild();
				pObj->Active(true);
			}
			else
				pObj->Active(false);
			break;
		case WM_NOTIFY_CHILD_HAS_FOCUS:
			pObj->_OnChildHasFocus(Param);
			return 0;
		case WM_GET_CLASS:
			return ClassNames[WCLS_FRAME];
	}
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	return DefCallback(pObj, MsgId, Param, pSrc);
}

Frame::Frame(const SRect &rc,
			 PWObj pParent, uint16_t Id,
			 WM_CF Flags, FRAME_CF FlagsEx,
			 GUI_PCSTR pTitle, WM_CB cb) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, FlagsEx | FRAME_CF_TITLEVIS),
	cb(cb),
	pClient(new WObj(
		_CalcPositions().rClient,
		_cbClient,
		this, 0,
		WC_ANCHOR_MASK | WC_VISIBLE)) {
	Text(pTitle);
}

void Frame::Minimize() {
	_RestoreMaximized();
	if (StatusEx & FRAME_CF_MINIMIZED)
		return;
	StatusEx |= FRAME_CF_MINIMIZED;
	rRestore = Rect();
	pClient->Visible(false);
	if (pMenu) pMenu->Visible(false);
	SizeY(_CalcTitleHeight() + EffectSize() * 2 + 1);
	_UpdatePositions();
	_InvalidateButton(GUI_ID_MINIMIZE);
}
void Frame::Maximize() {
	_RestoreMinimized();
	if (StatusEx & FRAME_CF_MAXIMIZED)
		return;
	StatusEx |= FRAME_CF_MAXIMIZED;
	auto pParent = Parent();
	auto rParent = pParent->Rect();
	if (!pParent->Parent())
		rParent = GUI_X_LCD_Rect();
	rRestore = Rect();
	PositionScreen(rParent.left_top());
	Size(rParent.size());
	_UpdatePositions();
	_InvalidateButton(GUI_ID_MAXIMIZE);
}
void Frame::Restore() {
	_RestoreMinimized();
	_RestoreMaximized();
}

constexpr uint16_t BUTTON_BORDER_OFFSET = 1;
Button *Frame::AddButton(FRAME_BUTTON Flags, int Off, uint16_t Id) {
	if (auto pButton = DialogItem(Id)) return (Button *)pButton;
	auto &&Pos = _CalcPositions();
	auto Size = TitleHeight() - BUTTON_BORDER_OFFSET * 2;
	auto x = 0;
	if (Flags & FRAME_BUTTON_RIGHT)
		x = Pos.rTitleText.x1 - (Size - 1) - Off;
	else
		x = Pos.rTitleText.x0 + Off;
	auto pButton = new Button(
		SRect::left_top({ x, BorderSize() + BUTTON_BORDER_OFFSET }, Size),
		this, Id,
		Flags);
	pButton->Focussable(false);
	return pButton;
}
Button *Frame::AddMinButton(FRAME_BUTTON Flags, int Off) {
	auto pButton = AddButton(Flags, Off, GUI_ID_MINIMIZE);
	pButton->SelfDraw(BUTTON_BI_UNPRESSED, [](SRect r) {
		int Size = r.xsize() / 2, Size2 = Size / 2;
		auto pObj = (Frame *)ActiveWindow()->Parent();
		if (pObj->StatusEx & FRAME_CF_MINIMIZED)
			for (int i = 1; i < Size; ++i)
				GUI.DrawLineH(r.x0 + i, r.y0 + i + Size2, r.x1 - i);
		else
			for (int i = 1; i < Size; ++i)
				GUI.DrawLineH(r.x0 + i, r.y1 - i - Size2, r.x1 - i);
	});
	return pButton;
}
Button *Frame::AddMaxButton(FRAME_BUTTON Flags, int Off) {
	auto pButton = AddButton(Flags, Off, GUI_ID_MAXIMIZE);
	pButton->SelfDraw(BUTTON_BI_UNPRESSED, [](SRect r) {
		static auto _DrawMax = [](SRect r) {
			GUI.DrawLineH(r.x0 + 1, r.y0 + 1, r.x1 - 1);
			GUI.DrawLineH(r.x0 + 1, r.y0 + 2, r.x1 - 1);
			GUI.DrawLineH(r.x0 + 1, r.y1 - 1, r.x1 - 1);
			GUI.DrawLineV(r.x0 + 1, r.y0 + 1, r.y1 - 1);
			GUI.DrawLineV(r.x1 - 1, r.y0 + 1, r.y1 - 1);
		};
		static auto _DrawRestore = [](SRect r) {
			int Size = r.xsize() * 2 / 3;
			GUI.DrawLineH(r.x1 - Size, r.y0 + 1, r.x1 - 1);
			GUI.DrawLineH(r.x1 - Size, r.y0 + 2, r.x1 - 1);
			GUI.DrawLineH(r.x0 + Size, r.y0 + Size, r.x1 - 1);
			GUI.DrawLineV(r.x1 - Size, r.y0 + 1, r.y1 - Size);
			GUI.DrawLineV(r.x1 - 1, r.y0 + 1, r.y0 + Size);
			GUI.DrawLineH(r.x0 + 1, r.y1 - Size, r.x0 + Size);
			GUI.DrawLineH(r.x0 + 1, r.y1 - Size + 1, r.x0 + Size);
			GUI.DrawLineH(r.x0 + 1, r.y1 - 1, r.x0 + Size);
			GUI.DrawLineV(r.x0 + 1, r.y1 - Size, r.y1 - 1);
			GUI.DrawLineV(r.x0 + Size, r.y1 - Size, r.y1 - 1);
		};
		auto pObj = (Frame *)ActiveWindow()->Parent();
		if (pObj->StatusEx & FRAME_CF_MAXIMIZED)
			_DrawRestore(r);
		else
			_DrawMax(r);
	});
	return pButton;
}
Button *Frame::AddCloseButton(FRAME_BUTTON Flags, int Off) {
	auto pButton = AddButton(Flags, Off, GUI_ID_CLOSE);
	pButton->SelfDraw(BUTTON_BI_UNPRESSED, [](SRect r) {
		int Size = r.xsize() - 2;
		for (int i = 2; i < Size; ++i) {
			GUI.DrawLineH(r.x0 + i, r.y0 + i, r.x0 + i + 1);
			GUI.DrawLineH(r.x1 - i - 1, r.y0 + i, r.x1 - i);
		}
	});
	return pButton;
}

void Frame::TitleVis(bool bTitleVis) {
	int StatusEx = this->StatusEx;
	if (bTitleVis)
		StatusEx |= FRAME_CF_TITLEVIS;
	else
		StatusEx &= ~FRAME_CF_TITLEVIS;
	if (this->StatusEx == StatusEx)
		return;
	this->StatusEx = StatusEx;
	_UpdatePositions();
	auto TitleHeight = this->TitleHeight();
	for (auto pChild = FirstChild(); pChild; pChild = pChild->NextSibling())
		if (pChild->Rect().y0 <= TitleHeight)
			if (pChild != pClient)
				pChild->Visible(bTitleVis);
	if (this->StatusEx & FRAME_CF_MINIMIZED)
		Visible(bTitleVis);
	Invalidate();
}

void Frame::Active(bool bActive) {
	auto StatusEx = this->StatusEx;
	if (bActive)
		StatusEx |= FRAME_CF_ACTIVE;
	else
		StatusEx &= ~FRAME_CF_ACTIVE;
	if (this->StatusEx == StatusEx)
		return;
	this->StatusEx = StatusEx;
	Invalidate();
}
void Frame::BorderSize(int Size) {
	if (Props.BorderSize != Size)
		return;
	auto OldHeight = _CalcTitleHeight();
	auto OldSize = Props.BorderSize;
	auto Diff = Size - OldSize;
	for (auto pChild = FirstChild(); pChild; pChild = pChild->NextSibling()) {
		auto &&r = pChild->Rect();
		if (r.y0 > OldHeight)
			continue;
		if (r.ysize() == OldHeight) {
			if (pChild->Styles() & WC_ANCHOR_RIGHT)
				pChild->Move({ -Diff, Diff });
			else
				pChild->Move(Diff);
		}
	}
	Props.BorderSize = Size;
	_UpdatePositions();
	Invalidate();
}
void Frame::Menu(::Menu *pMenu) {
	auto IBorderSize = StatusEx & FRAME_CF_TITLEVIS ? Props.IBorderSize : 0;
	auto BorderSize = Props.BorderSize;
	this->pMenu = pMenu;
	if (cb)
		pMenu->Owner(pClient);
	auto xSize = SizeX() - BorderSize * 2;
	pMenu->Attach(this, { BorderSize, BorderSize + _CalcTitleHeight() + IBorderSize }, { xSize, 0 });
	pMenu->Anchor(WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT);
	_UpdatePositions();
	Invalidate();
}
