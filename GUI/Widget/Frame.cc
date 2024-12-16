

#include "Button.h"
#include "Frame.h"

Frame::Property Frame::DefaultProps;

static int16_t FRAME__MinVisibility = 5;

void Frame::_OnTouch(WM_MSG *pMsg) {
	if (auto pState = (const PidState *)pMsg->data)
		if (pState->Pressed) {
			if (!(Flags & FRAME_CF_ACTIVE))
				Focus();
			BringToTop();
			if (Flags & FRAME_CF_MOVEABLE)
				CaptureMove(*pState, FRAME__MinVisibility);
		}
}
void Frame::_OnPaint() const {
	auto pWin = ActiveWindow();
	auto &&size = pWin->Size() - 1;
	auto BorderSize = Props.BorderSize;
	auto &&Pos = _CalcPositions();
	auto Index = Flags & FRAME_CF_ACTIVE ? 1 : 0;
	SRect r, rText;
	r.x0 = Pos.rClient.x0;
	r.x1 = Pos.rClient.x1;
	r.y0 = Pos.rTitleText.y0;
	r.y1 = Pos.rTitleText.y1;
	Pos.rTitleText.y0++;
	Pos.rTitleText.x0++;
	Pos.rTitleText.x1--;
	GUI.Font(Props.pFont);
	GUI__CalcTextRect(Title, &Pos.rTitleText, &rText, Props.Align);
	int y0 = Pos.TitleHeight + BorderSize;
	WObj::IVR([&] {
		GUI.BkColor(Props.aBarColor[Index]);
		GUI.PenColor(Props.aTextColor[Index]);
		WIDGET__FillStringInRect(Title, r, Pos.rTitleText, rText);
		GUI.PenColor(Props.FrameColor);
		GUI.Fill({ 0, 0, size.x, BorderSize - 1 });
		GUI.Fill({ 0, 0, Pos.rClient.x0 - 1, size.y });
		GUI.Fill({ Pos.rClient.x1 + 1, 0, size.x, size.y });
		GUI.Fill({ 0, Pos.rClient.y1 + 1, size.x, size.y });
		GUI.Fill({ 0, y0, size.x, y0 + Props.IBorderSize - 1 });
		if (Props.BorderSize >= 2)
			DrawUp();
	});
}
void Frame::_OnChildHasFocus(WM_MSG *pMsg) {
	auto pInfo = (const WM_NOTIFY_CHILD_HAS_FOCUS_INFO *)pMsg->data;
	if (!pInfo)
		return;
	if (WM__IsAncestorOrSelf(pInfo->pNew, this)) {
		Active(true);
		return;
	}
	Active(false);
	if (WM__IsAncestor(pInfo->pOld, this))
		pFocussedChild = pInfo->pOld;
}
void Frame::_Callback(WM_MSG *pMsg) {
	auto pObj = (Frame *)pMsg->pWin;
	if (pObj->State & FRAME_CF_RESIZEABLE)
		if (_OnResizeable(pMsg))
			return;
	switch (pMsg->msgid) {
	case WM_HANDLE_DIALOG_STATUS:
	{
		WM_DIALOG_STATUS *wds = (WM_DIALOG_STATUS *)pMsg->data;
		if (wds)
			pObj->pDialogStatus = wds;
		else
			pMsg->data = (size_t)pObj->pDialogStatus;
		return;
	}
	case WM_PAINT:
		pObj->_OnPaint();
		break;
	case WM_TOUCH:
		pObj->_OnTouch(pMsg);
		return;
	case WM_GET_INSIDE_RECT: {
		auto pRect = (SRect *)pMsg->data;
		auto &&Pos = pObj->_CalcPositions();
		*pRect = Pos.rClient;
		return;
	}
	case WM_GET_CLIENT_WINDOW:
		pMsg->data = (size_t)pObj->pClient;
		return;
	case WM_NOTIFY_PARENT:
		if (pMsg->data == WM_NOTIFICATION_RELEASED) {
			WM_MSG msg;
			msg.pWinSrc = pObj;
			msg.data = pMsg->data;
			msg.msgid = WM_NOTIFY_PARENT_REFLECTION;
			pObj->SendMessage(&msg);
			if (auto pButton = (WObj *)pMsg->data)
			switch (pMsg->pWinSrc->ID()) {
				case GUI_ID_CLOSE:
					pObj->Delete();
					return;
				case GUI_ID_MAXIMIZE:
					if (pObj->Flags & FRAME_CF_MAXIMIZED)
						pObj->Restore();
					else
						pObj->Maximize();
					break;
				case GUI_ID_MINIMIZE:
					if (pObj->Flags & FRAME_CF_MINIMIZED)
						pObj->Restore();
					else
						pObj->Minimize();
					break;
			}
		}
		return;
	case WM_SET_FOCUS:
		if (pMsg->data == 1) {
			if (IsWindow(pObj->pFocussedChild))
				pObj->pFocussedChild->Focus();
			else
				pObj->pFocussedChild = pObj->pClient->FocusNextChild();
			pObj->Active(true);
			pMsg->data = 0;
		}
		else
			pObj->Active(false);
		return;
	case WM_TOUCH_CHILD:
		if (!(pObj->Flags & FRAME_CF_ACTIVE)) {
			const WM_MSG *pMsgOrg = (const WM_MSG *)pMsg->data;
			const PidState *pState = (const PidState *)pMsgOrg->data;
			if (!pState)
				break;
			if (pState->Pressed)
				pObj->Focus();
		}
		break;
	case WM_NOTIFY_CHILD_HAS_FOCUS:
		pObj->_OnChildHasFocus(pMsg);
		break;
	case WM_DELETE:
		pObj->~Frame();
		break;
	}
	if (pObj->HandleActive(pMsg))
		DefCallback(pMsg);
}
void Frame::_cbClient(WM_MSG *pMsg) {
	WObj *pWin = pMsg->pWin;
	auto pObj = (Frame *)pWin->Parent();
	WM_CB *cb = pObj->cb;
	switch (pMsg->msgid) {
	case WM_PAINT:
		if (pObj->Props.ClientColor != RGB_INVALID_COLOR) {
			GUI.BkColor(pObj->Props.ClientColor);
			GUI.Clear();
		}
		if (cb) {
			WM_MSG msg = *pMsg;
			msg.pWin = pWin;
			cb(&msg);
		}
		return;
	case WM_SET_FOCUS:
		if (pMsg->data) {
			if (pObj->pFocussedChild && (pObj->pFocussedChild != pWin))
				pObj->pFocussedChild->Focus();
			else
				pObj->pFocussedChild = pWin->FocusNextChild();
			pMsg->data = 0;
		}
		return;
	case WM_GET_ACCEPT_FOCUS:
		pObj->HandleActive(pMsg);
		return;
	case WM_KEY:
	{
		const WM_KEY_INFO *ki = (const WM_KEY_INFO *)pMsg->data;
		if (ki->PressedCnt > 0) {
			switch (ki->Key) {
			case GUI_KEY_TAB:
				pObj->pFocussedChild = pWin->FocusNextChild();
				return;
			}
		}
		break;
	}
	case WM_GET_BKCOLOR:
		pMsg->data = pObj->Props.ClientColor;
		return;
	case WM_GET_INSIDE_RECT:
	case WM_GET_ID:
	case WM_GET_CLIENT_WINDOW:
		DefCallback(pMsg);
		return;
	}
	if (cb) {
		pMsg->pWin = pObj;
		cb(pMsg);
	}
	else
		DefCallback(pMsg);
}
int16_t Frame::_CalcTitleHeight() const {
	if (!(State & FRAME_CF_TITLEVIS))
		return 0;
	return Props.TitleHeight ?
		Props.TitleHeight :
		2 + Props.pFont->YSize;
}
Frame::Positions Frame::_CalcPositions() const {
	auto &&rect = Rect();
	auto &&size = Size();
	auto BorderSize = Props.BorderSize;
	auto TitleHeight = _CalcTitleHeight();
	auto IBorderSize = Status & FRAME_CF_TITLEVIS ? Props.IBorderSize : 0;
	int16_t MenuHeight = pMenu ? pMenu->SizeY() : 0;
	Positions Pos = {
		TitleHeight,
		MenuHeight, {
			{ BorderSize, BorderSize + IBorderSize + TitleHeight + MenuHeight },
			size - BorderSize - 1
		}, {
			BorderSize,
			{ size.x - BorderSize - 1, BorderSize + TitleHeight - 1 }
		}
	};
	for (auto pChild = FirstChild(); pChild; pChild = pChild->NextSibling()) {
		auto &&r = pChild->Rect();
		auto &&d = r - rect;
		if (d.y0 != BorderSize)
			continue;
		if (pChild->Styles() & WC_ANCHOR_RIGHT) {
			if (d.x0 <= Pos.rTitleText.x1)
				Pos.rTitleText.x1 = d.x0 - 1;
		}
		else if (d.x1 >= Pos.rTitleText.x0)
			Pos.rTitleText.x0 = d.x1 + 1;
	}
	return Pos;
}
void Frame::_UpdatePositions() {
	if (!pClient && !pMenu)
		return;
	auto &&Pos = _CalcPositions();
	if (pClient) {
		pClient->MoveChildTo(Pos.rClient.left_top());
		pClient->Size(Pos.rClient.size());
	}
	if (pMenu) {
		Pos.rClient.y0 -= Pos.MenuHeight;
		pMenu->MoveChildTo(Pos.rClient.left_top());
	}
}

void Frame::_InvalidateButton(uint16_t Id) {
	for (auto pChild = pFirstChild; pChild; pChild = pChild->NextSibling())
		if (pChild->ID() == Id)
			pChild->Invalidate();
}
void Frame::_RestoreMinimized() {
	if (!(Flags & FRAME_CF_MINIMIZED))
		return;
	Flags &= ~FRAME_CF_MINIMIZED;
	SizeY(rRestore.ysize());
	pClient->Visible(true);
	if (pMenu) pMenu->Visible(true);
	_UpdatePositions();
	_InvalidateButton(GUI_ID_MINIMIZE);
}
void Frame::_RestoreMaximized() {
	if (!(Flags & FRAME_CF_MAXIMIZED))
		return;
	Flags &= ~FRAME_CF_MAXIMIZED;
	Position(rRestore.left_top());
	Size(rRestore.size());
	_UpdatePositions();
	_InvalidateButton(GUI_ID_MAXIMIZE);
}
void Frame::Minimize() {
	_RestoreMaximized();
	if (Flags & FRAME_CF_MINIMIZED)
		return;
	Flags |= FRAME_CF_MINIMIZED;
	rRestore = rect;
	pClient->Visible(false);
	if (pMenu) pMenu->Visible(false);
	SizeY(_CalcTitleHeight() + EffectSize() * 2 + 2);
	_UpdatePositions();
	_InvalidateButton(GUI_ID_MINIMIZE);
}
void Frame::Maximize() {
	_RestoreMinimized();
	if (Flags & FRAME_CF_MAXIMIZED)
		return;
	Flags |= FRAME_CF_MAXIMIZED;
	auto pParent = Parent();
	auto rParent = pParent->Rect();
	if (!pParent->Parent())
		rParent = LCD_Rect();
	rRestore = rect;
	Position(rParent.left_top());
	Size(rParent.size());
	_UpdatePositions();
	_InvalidateButton(GUI_ID_MAXIMIZE);
}

void Frame::Restore() {
	_RestoreMinimized();
	_RestoreMaximized();
}

void Frame::Active(bool bActive) {
	if (bActive && !(Flags & FRAME_CF_ACTIVE)) {
		Flags |= FRAME_CF_ACTIVE;
		Invalidate();
	}
	else if (!bActive && (Flags & FRAME_CF_ACTIVE)) {
		Flags &= ~FRAME_CF_ACTIVE;
		Invalidate();
	}
}
void Frame::BorderSize(int Size) {
	int OldHeight = _CalcTitleHeight();
	int OldSize = Props.BorderSize;
	int Diff = Size - OldSize;
	for (auto pChild = FirstChild(); pChild; pChild = pChild->NextSibling()) {
		auto &&r = pChild->Rect() - rect.left_top();
		if (r.y0 == Props.BorderSize && r.y1 - r.y0 + 1 == OldHeight) {
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
void Frame::_UpdateButtons(int OldHeight) {
	int TitleHeight = _CalcTitleHeight();
	int Diff = TitleHeight - OldHeight;
	if (!Diff)
		return;
	int n = 0;
	WObj *pLeft, *pRight;
	do {
		pLeft = pRight = nullptr;
		WObj *pChild;
		for (pChild = FirstChild(); pChild; pChild = pChild->NextSibling()) {
			auto &&r = pChild->Rect() - rect.left_top();
			if (r.y0 != Props.BorderSize)
				continue;
			if (r.y1 - r.y0 + 1 != OldHeight)
				continue;
			int xLeft = GUI_XMAX, xRight = GUI_XMIN;
			if (pChild->Styles() & WC_ANCHOR_RIGHT) {
				if (r.x1 > xRight) {
					pRight = pChild;
					xRight = r.x0;
				}
			}
			else if (r.x0 < xLeft) {
				pLeft = pChild;
				xLeft = r.x0;
			}
		}
		if (pLeft) {
			pLeft->Resize(Diff);
			pLeft->Move({ n * Diff, 0 });
		}
		if (pRight) {
			pRight->Resize(Diff);
			pRight->Move({ -(n * Diff), 0 });
		}
		++n;
	} while (pLeft || pRight);
}
void Frame::Menu(::Menu *pMenu) {
	auto IBorderSize = State & FRAME_CF_TITLEVIS ? Props.IBorderSize : 0;
	auto BorderSize = Props.BorderSize;
	this->pMenu = pMenu;
	if (cb)
		MENU_SetOwner(pMenu, pClient);
	int xSize = SizeX() - BorderSize * 2;
	MENU_Attach(pMenu, this, { BorderSize, BorderSize + _CalcTitleHeight() + IBorderSize }, { xSize, 0 }, 0);
	pMenu->Anchor(WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT);
	_UpdatePositions();
	Invalidate();
}

Button *Frame::AddButton(uint16_t Flags, int Off, uint16_t Id) {
	if (auto pButton = DialogItem(Id)) return (Button *)pButton;
	auto &&Pos = _CalcPositions();
	int Size = this->TitleHeight();
	int BorderSize = this->BorderSize();
	int WinFlags, x;
	if (Flags & FRAME_BUTTON_RIGHT) {
		x = Pos.rTitleText.x1 - (Size - 1) - Off;
		WinFlags = WC_VISIBLE | WC_ANCHOR_RIGHT;
	}
	else {
		x = Pos.rTitleText.x0 + Off;
		WinFlags = WC_VISIBLE;
	}
	auto pButton = new Button(x, BorderSize, Size, Size, this, Id, WinFlags);
	pButton->Focussed();
	return pButton;
}
Button *Frame::AddMaxButton(uint16_t Flags, int Off) {
	auto pButton = AddButton(Flags, Off, GUI_ID_MAXIMIZE);
	pButton->SelfDraw(0, [](SRect r) {
		static auto _DrawMax = [](SRect r) {
			//GUI_LCD_DrawHLine(r.x0 + 1, r.y0 + 1, r.x1 - 1);
			//GUI_LCD_DrawHLine(r.x0 + 1, r.y0 + 2, r.x1 - 1);
			//GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - 1, r.x1 - 1);
			//GUI_LCD_DrawVLine(r.x0 + 1, r.y0 + 1, r.y1 - 1);
			//GUI_LCD_DrawVLine(r.x1 - 1, r.y0 + 1, r.y1 - 1);
		};
		static auto _DrawMaxRestore = [](SRect r) {
			//int Size = ((r.x1 - r.x0 + 1) << 1) / 3;
			//GUI_LCD_DrawHLine(r.x1 - Size, r.y0 + 1, r.x1 - 1);
			//GUI_LCD_DrawHLine(r.x1 - Size, r.y0 + 2, r.x1 - 1);
			//GUI_LCD_DrawHLine(r.x0 + Size, r.y0 + Size, r.x1 - 1);
			//GUI_LCD_DrawVLine(r.x1 - Size, r.y0 + 1, r.y1 - Size);
			//GUI_LCD_DrawVLine(r.x1 - 1, r.y0 + 1, r.y0 + Size);
			//GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - Size, r.x0 + Size);
			//GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - Size + 1, r.x0 + Size);
			//GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - 1, r.x0 + Size);
			//GUI_LCD_DrawVLine(r.x0 + 1, r.y1 - Size, r.y1 - 1);
			//GUI_LCD_DrawVLine(r.x0 + Size, r.y1 - Size, r.y1 - 1);
		};
		auto pObj = (Frame *)ActiveWindow()->Parent();
		if (pObj->Flags & FRAME_CF_MAXIMIZED)
			_DrawMaxRestore(r);
		else
			_DrawMax(r);
	});
	return pButton;
}
Button *Frame::AddCloseButton(uint16_t Flags, int Off) {
	auto pButton = AddButton(Flags, Off, GUI_ID_CLOSE);
	pButton->SelfDraw(0, [](SRect r) {
		int Size = r.x1 - r.x0 - 2;
		for (int i = 2; i < Size; ++i) {
		//	GUI_LCD_DrawHLine(r.x0 + i, r.y0 + i, r.x0 + i + 1);
		//	GUI_LCD_DrawHLine(r.x1 - i - 1, r.y0 + i, r.x1 - i);
		}
	});
	return pButton;
}
Button *Frame::AddMinButton(uint16_t Flags, int Off) {
	auto pButton = AddButton(Flags, Off, GUI_ID_MINIMIZE);
	pButton->SelfDraw(0, [](SRect r) {
		static auto _DrawMin = [](SRect r) {
			int Size = (r.x1 - r.x0 + 1) >> 1;
			for (int i = 1; i < Size; ++i);
			//	GUI_LCD_DrawHLine(r.x0 + i, r.y1 - i - (Size >> 1), r.x1 - i);
		};
		static auto _DrawRestore = [](SRect r) {
			int Size = (r.x1 - r.x0 + 1) >> 1;
			for (int i = 1; i < Size; ++i);
			//	GUI_LCD_DrawHLine(r.x0 + i, r.y0 + i + (Size >> 1), r.x1 - i);
		};
		auto pObj = (Frame *)ActiveWindow()->Parent();
		if (pObj->Flags & FRAME_CF_MINIMIZED)
			_DrawRestore(r);
		else
			_DrawMin(r);
	});
	return pButton;
}

void Frame::TitleVis(bool bTitleVis) {
	int State = this->State;
	if (bTitleVis)
		State |= FRAME_CF_TITLEVIS;
	else
		State &= ~FRAME_CF_TITLEVIS;
	if (this->State == State)
		return;
	this->State = State;
	_UpdatePositions();
	for (auto pChild = FirstChild(); pChild; pChild = pChild->NextSibling()) {
		int y0 = pChild->Rect().y0 - rect.y0;
		if (y0 != Props.BorderSize)
			continue;
		if (pChild == pClient)
			continue;
		pChild->Visible(bTitleVis);
	}
	if (this->Flags & FRAME_CF_MINIMIZED)
		Visible(bTitleVis);
	Invalidate();
}

Frame::Frame(int x0, int y0, int xsize, int ysize,
			 WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
			 const char *pTitle, WM_CB *cb) :
	Widget(x0, y0, xsize, ysize,
		  _Callback,
		   pParent, Id, Flags, WIDGET_STATE_FOCUSSABLE | FRAME_CF_TITLEVIS),
	cb(cb), Flags(ExFlags) {
	auto &&Pos = _CalcPositions();
	pClient = new WObj(
		Pos.rClient,
		_cbClient,
		this,
		WC_ANCHOR_RIGHT | WC_ANCHOR_LEFT | WC_ANCHOR_TOP
		| WC_ANCHOR_BOTTOM | WC_VISIBLE);
	Text(pTitle);
}

#pragma region OnResize

static constexpr uint8_t
	FRAME_RESIZE_X  = 1 << 0,
	FRAME_RESIZE_Y  = 1 << 1,
	FRAME_REPOS_X   = 1 << 2,
	FRAME_REPOS_Y   = 1 << 3,
	FRAME_MOUSEOVER = 1 << 4,
	FRAME_RESIZE    =
	FRAME_RESIZE_X | FRAME_RESIZE_Y |
	FRAME_REPOS_X | FRAME_REPOS_Y;

static void _SetResizeCursor(int Mode) {
	static CCursor *_pOldCursor = nullptr;
	static const BPP2_DAT _acResizeCursorH[]{
		________,________,________,________,________,
		________,________,________,________,________,
		________,________,________,________,________,
		________,________,________,________,________,
		________,XX______,________,XX______,________,
		______XX,XX______,________,XXXX____,________,
		____XXoo,XX______,________,XXooXX__,________,
		__XXoooo,XXXXXXXX,XXXXXXXX,XXooooXX,________,
		XXoooooo,oooooooo,oooooooo,oooooooo,XX______,
		__XXoooo,XXXXXXXX,XXXXXXXX,XXooooXX,________,
		____XXoo,XX______,________,XXooXX__,________,
		______XX,XX______,________,XXXX____,________,
		________,XX______,________,XX______,________,
		________,________,________,________,________,
		________,________,________,________,________,
		________,________,________,________,________,
		________,________,________,________,________,
	};
	static CCursor _ResizeCursorH{
		{
			/* Size */ { 17, 17 },
			/* BytesPerLine */ 5,
			/* Bits */ _acResizeCursorH,
			/* Palette */ &GUI_CursorPal,
			/* Transparent */ true
		}, { 8, 8 }
	};
	static const BPP2_DAT _acResizeCursorV[]{
		________,________,XX______,________,________,
		________,______XX,ooXX____,________,________,
		________,____XXoo,ooooXX__,________,________,
		________,__XXoooo,ooooooXX,________,________,
		________,XXXXXXXX,ooXXXXXX,XX______,________,
		________,______XX,ooXX____,________,________,
		________,______XX,ooXX____,________,________,
		________,______XX,ooXX____,________,________,
		________,______XX,ooXX____,________,________,
		________,______XX,ooXX____,________,________,
		________,______XX,ooXX____,________,________,
		________,______XX,ooXX____,________,________,
		________,XXXXXXXX,ooXXXXXX,XX______,________,
		________,__XXoooo,ooooooXX,________,________,
		________,____XXoo,ooooXX__,________,________,
		________,______XX,ooXX____,________,________,
		________,________,XX______,________,________,
	};
	static CCursor _ResizeCursorV{
		{
			/* Size */ { 17, 17 },
			/* BytesPerLine */ 5,
			/* Bits */ _acResizeCursorV,
			/* Palette */ &GUI_CursorPal,
			/* Transparent */ true
		}, { 8, 8 }
	};
	static const BPP2_DAT _acResizeCursorDD[]{
		________,________,________,________,________,
		________,________,________,________,________,
		____XXXX,XXXXXXXX,________,________,________,
		____XXoo,ooooXX__,________,________,________,
		____XXoo,ooXX____,________,________,________,
		____XXoo,XXooXX__,________,________,________,
		____XXXX,__XXooXX,________,________,________,
		____XX__,____XXoo,XX______,________,________,
		________,______XX,ooXX____,________,________,
		________,________,XXooXX__,____XX__,________,
		________,________,__XXooXX,__XXXX__,________,
		________,________,____XXoo,XXooXX__,________,
		________,________,______XX,ooooXX__,________,
		________,________,____XXoo,ooooXX__,________,
		________,________,__XXXXXX,XXXXXX__,________,
		________,________,________,________,________,
		________,________,________,________,________,
	};
	static CCursor _ResizeCursorDD{ {
			/* Size */ { 17, 17 },
			/* BytesPerLine */ 5,
			/* Bits */ _acResizeCursorDD,
			/* Palette */ &GUI_CursorPal,
			/* Transparent */ true
		}, { 8, 8 }
	};
	static const BPP2_DAT _acResizeCursorDU[]{
		________,________,________,________,________,
		________,________,________,________,________,
		________,________,__XXXXXX,XXXXXX__,________,
		________,________,____XXoo,ooooXX__,________,
		________,________,______XX,ooooXX__,________,
		________,________,____XXoo,XXooXX__,________,
		________,________,__XXooXX,__XXXX__,________,
		________,________,XXooXX__,____XX__,________,
		________,______XX,ooXX____,________,________,
		____XX__,____XXoo,XX______,________,________,
		____XXXX,__XXooXX,________,________,________,
		____XXoo,XXooXX__,________,________,________,
		____XXoo,ooXX____,________,________,________,
		____XXoo,ooooXX__,________,________,________,
		____XXXX,XXXXXXXX,________,________,________,
		________,________,________,________,________,
		________,________,________,________,________,
	};
	static CCursor _ResizeCursorDU{
		{
			/* Size */ { 17, 17 },
			/* BytesPerLine */ 5,
			/* Bits */ _acResizeCursorDU,
			/* Palette */ &GUI_CursorPal,
			/* Transparent */ true
		}, { 8, 8 }
	};
	CCursor *pNewCursor = nullptr;
	if (Mode) {
		int Direction = Mode & (FRAME_RESIZE_X | FRAME_RESIZE_Y);
		if (Direction == FRAME_RESIZE_X)
			pNewCursor = &_ResizeCursorH;
		else if (Direction == FRAME_RESIZE_Y)
			pNewCursor = &_ResizeCursorV;
		else {
			Direction = Mode & (FRAME_REPOS_X | FRAME_REPOS_Y);
			pNewCursor = Direction == (FRAME_REPOS_X | FRAME_REPOS_Y) || !Direction ?
				&_ResizeCursorDD : &_ResizeCursorDU;
		}
	}
	if (pNewCursor) {
		auto pOldCursor = GUI.Cursor.Select(pNewCursor);
		if (_pOldCursor == nullptr)
			_pOldCursor = pOldCursor;
	}
	else if (_pOldCursor) {
		GUI.Cursor.Select(_pOldCursor);
		_pOldCursor = nullptr;
	}
}
static Point _Capture;
static int _CaptureFlags;
void Frame::_SetCapture(Point Pos, int Mode) {
	if (!(_CaptureFlags & FRAME_REPOS_X))
		_Capture.x = Pos.x;
	if (!(_CaptureFlags & FRAME_REPOS_Y))
		_Capture.y = Pos.y;
	if (!Mode) return;
	if (!Captured())
		Capture(false);
	_SetResizeCursor(Mode);
	if (Mode & FRAME_MOUSEOVER)
		Mode = 0;
	_CaptureFlags = Mode | FRAME_MOUSEOVER;
}
constexpr uint8_t FRAME_REACT_BORDER = 3;
constexpr uint8_t FRAME_MINSIZE_X = 20, FRAME_MINSIZE_Y = 20;
void Frame::_ChangeWindowPosSize(Point &p) {
	Point d;
	auto &&rect = ClientRect();
	if (_CaptureFlags & FRAME_RESIZE_X)
		d.x = (_CaptureFlags & FRAME_REPOS_X) ? _Capture.x - p.x : p.x - _Capture.x;
	if (_CaptureFlags & FRAME_RESIZE_Y)
		d.y = (_CaptureFlags & FRAME_REPOS_Y) ? _Capture.y - p.y : p.y - _Capture.y;
	if (rect.x1 + d.x + 1 < FRAME_MINSIZE_X) {
		d.x = FRAME_MINSIZE_X - rect.x1 - 1;
		p.x = _Capture.x + d.x;
	}
	if (rect.y1 + d.y + 1 < FRAME_MINSIZE_Y) {
		d.y = FRAME_MINSIZE_Y - rect.y1 - 1;
		p.y = _Capture.y + d.y;
	}
	Move({ _CaptureFlags & FRAME_REPOS_X ? -d.x : 0,
		   _CaptureFlags & FRAME_REPOS_Y ? -d.y : 0 });
	Resize(d);
}
int Frame::_CheckReactBorder(Point Pos) {
	static auto _CheckBorderX = [](int x, int x1, int Border) {
		return x > x1 - Border ?
			FRAME_RESIZE_X :
			x < Border ?
			FRAME_RESIZE_X | FRAME_REPOS_X : 0;
	};
	static auto _CheckBorderY = [](int y, int y1, int Border) {
		return y > y1 - Border ?
			FRAME_RESIZE_Y :
			y < Border ?
			FRAME_RESIZE_Y | FRAME_REPOS_Y : 0;
	};
	auto &&r = ClientRect();
	if (Pos.x < 0 || Pos.y < 0)
		return 0;
	if (Pos.x > r.x1 || Pos.y > r.y1)
		return 0;
	int Mode = _CheckBorderX(Pos.x, r.x1, FRAME_REACT_BORDER);
	if (Mode)
		Mode |= _CheckBorderY(Pos.y, r.y1, 4 * FRAME_REACT_BORDER);
	else {
		Mode |= _CheckBorderY(Pos.y, r.y1, FRAME_REACT_BORDER);
		if (Mode)
			Mode |= _CheckBorderX(Pos.x, r.x1, 4 * FRAME_REACT_BORDER);
	}
	return Mode;
}
int Frame::_OnTouchResize(WM_MSG *pMsg) {
	auto pState = (const PidState *)pMsg->data;
	if (!pState) return false;
	Point Pos = *pState;
	auto Mode = _CheckReactBorder(Pos);
	if (pState->Pressed != 1) {
		if (!Captured())
			return false;
		_CaptureFlags &= ~(FRAME_RESIZE);
		if (!Mode)
			CaptureRelease();
		return true;
	}
	if (_CaptureFlags & FRAME_RESIZE) {
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
bool Frame::_ForwardMouseOverMsg(WM_MSG *pMsg) {
	auto pState = (PidState *)pMsg->data;
	*pState += Position();
	if (auto pBelow = WObj::ScreenToWin(*pState)) {
		if (pBelow == this) return false;
		*pState -= pBelow->Position();
		pBelow->SendMessage(pMsg);
		return true;
	}
	return false;
}
bool Frame::_OnResizeable(WM_MSG *pMsg) {
	auto pWin = (Frame *)pMsg->pWin;
	if (pWin->Captured() && _CaptureFlags == 0)
		return false;
	if (pWin->Minimized() || pWin->Maximized())
		return false;
	switch (pMsg->msgid) {
		case WM_TOUCH:
			return pWin->_OnTouchResize(pMsg);
		case WM_MOUSEOVER:
			if (auto pState = (const PidState *)pMsg->data) {
				if (auto Mode = pWin->_CheckReactBorder(*pState)) {
					if (!pWin->_ForwardMouseOverMsg(pMsg))
						pWin->_SetCapture(*pState, Mode | FRAME_MOUSEOVER);
					return true;
				}
				if (!pWin->Captured())
					return false;
				if (_CaptureFlags & FRAME_RESIZE)
					return true;
				CaptureRelease();
				pWin->_ForwardMouseOverMsg(pMsg);
				return true;
			}
			return false;
		case WM_CAPTURE_RELEASED:
			_SetResizeCursor(0);
			_CaptureFlags = 0;
			return true;
	}
	return false;
}
#pragma endregion
