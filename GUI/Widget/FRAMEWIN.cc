

#include "BUTTON.h"
#include "FRAMEWIN.h"
#define FRAMEWIN_DEFAULT_FONT              &GUI_Font8_1
#define FRAMEWIN_TITLEHEIGHT_DEFAULT       18
#define FRAMEWIN_BORDER_DEFAULT            3
#define FRAMEWIN_IBORDER_DEFAULT           1
#define FRAMEWIN_CLIENTCOLOR_DEFAULT       0xc0c0c0
#define FRAMEWIN_BARCOLOR_ACTIVE_DEFAULT   0xFF0000
#define FRAMEWIN_BARCOLOR_INACTIVE_DEFAULT 0x404040
#define FRAMEWIN_FRAMECOLOR_DEFAULT        0xAAAAAA
#define FRAMEWIN_TEXTCOLOR0_DEFAULT        GUI_WHITE
#define FRAMEWIN_TEXTCOLOR1_DEFAULT        GUI_WHITE
FRAMEWIN_PROPS FRAMEWIN__DefaultProps = {
	FRAMEWIN_DEFAULT_FONT,
	FRAMEWIN_BARCOLOR_INACTIVE_DEFAULT,
	FRAMEWIN_BARCOLOR_ACTIVE_DEFAULT,
	FRAMEWIN_TEXTCOLOR0_DEFAULT,
	FRAMEWIN_TEXTCOLOR1_DEFAULT,
	FRAMEWIN_CLIENTCOLOR_DEFAULT,
	FRAMEWIN_TITLEHEIGHT_DEFAULT,
	FRAMEWIN_BORDER_DEFAULT,
	FRAMEWIN_IBORDER_DEFAULT
};
static int16_t FRAMEWIN__MinVisibility = 5;
void Frame::_OnTouch(WM_MESSAGE *pMsg) {
	auto pState = (const PidState *)pMsg->Data;
	if (!pState)
		return;
	if (!pState->Pressed)
		return;
	if (!(Flags & FRAME_CF_ACTIVE))
		Focus();
	BringToTop();
	if (Flags & FRAME_CF_MOVEABLE)
		WM_SetCaptureMove(this, pState, FRAMEWIN__MinVisibility);
}
void Frame::_OnPaint() const {
	WObj *pWin = WM_GetActiveWindow();
	auto &&size = pWin->Size() - 1;
	auto BorderSize = Props.BorderSize;
	auto &&Pos = _CalcPositions();
	auto Index = Flags & FRAME_CF_ACTIVE ? 1 : 0;
	auto pText = this->pText;
	SRect r, rText;
	r.x0 = Pos.rClient.x0;
	r.x1 = Pos.rClient.x1;
	r.y0 = Pos.rTitleText.y0;
	r.y1 = Pos.rTitleText.y1;
	Pos.rTitleText.y0++;
	Pos.rTitleText.x0++;
	Pos.rTitleText.x1--;
	GUI.Font(Props.pFont);
	GUI__CalcTextRect(pText, &Pos.rTitleText, &rText, textAlign);
	int y0 = Pos.TitleHeight + BorderSize;
	WM_ITERATE_START() {
		GUI.BkColor(Props.aBarColor[Index]);
		GUI.PenColor(Props.aTextColor[Index]);
		WIDGET__FillStringInRect(pText, &r, &Pos.rTitleText, &rText);
		GUI.PenColor(FRAMEWIN_FRAMECOLOR_DEFAULT);
		GUI_FillRect(0, 0, size.x, BorderSize - 1);
		GUI_FillRect(0, 0, Pos.rClient.x0 - 1, size.y);
		GUI_FillRect(Pos.rClient.x1 + 1, 0, size.x, size.y);
		GUI_FillRect(0, Pos.rClient.y1 + 1, size.x, size.y);
		GUI_FillRect(0, y0, size.x, y0 + Props.IBorderSize - 1);
		if (Props.BorderSize >= 2)
			pEffect->DrawUp();
	} WM_ITERATE_END();
}
void Frame::_OnChildHasFocus(WM_MESSAGE *pMsg) {
	auto pInfo = (const WM_NOTIFY_CHILD_HAS_FOCUS_INFO *)pMsg->Data;
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
void Frame::_Callback(WM_MESSAGE *pMsg) {
	auto pObj = (Frame *)pMsg->pWin;
	if (pObj->State & FRAME_CF_RESIZEABLE)
		if (_OnResizeable(pMsg))
			return;
	switch (pMsg->MsgId) {
	case WM_HANDLE_DIALOG_STATUS:
	{
		WM_DIALOG_STATUS *wds = (WM_DIALOG_STATUS *)pMsg->Data;
		if (wds)
			pObj->pDialogStatus = wds;
		else
			pMsg->Data = (size_t)pObj->pDialogStatus;
		return;
	}
	case WM_PAINT:
		pObj->_OnPaint();
		break;
	case WM_TOUCH:
		pObj->_OnTouch(pMsg);
		return;
	case WM_GET_INSIDE_RECT: {
		auto pRect = (SRect *)pMsg->Data;
		auto &&Pos = pObj->_CalcPositions();
		*pRect = Pos.rClient;
		return;
	}
	case WM_GET_CLIENT_WINDOW:
		pMsg->Data = (size_t)pObj->pClient;
		return;
	case WM_NOTIFY_PARENT:
		if (pMsg->Data == WM_NOTIFICATION_RELEASED) {
			WM_MESSAGE msg;
			msg.pWinSrc = pObj;
			msg.Data = pMsg->Data;
			msg.MsgId = WM_NOTIFY_PARENT_REFLECTION;
			pMsg->pWinSrc->SendMessage(&msg);
		}
		return;
	case WM_SET_FOCUS:
		if (pMsg->Data == 1) {
			if (IsWindow(pObj->pFocussedChild))
				pObj->pFocussedChild->Focus();
			else
				pObj->pFocussedChild = WM_SetFocusOnNextChild(pObj->pClient);
			pObj->Active(true);
			pMsg->Data = 0;
		}
		else
			pObj->Active(false);
		return;
	case WM_TOUCH_CHILD:
		if (!(pObj->Flags & FRAME_CF_ACTIVE)) {
			const WM_MESSAGE *pMsgOrg = (const WM_MESSAGE *)pMsg->Data;
			const PidState *pState = (const PidState *)pMsgOrg->Data;
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
		GUI_ALLOC_Free(pObj->pText);
		pObj->pText = nullptr;
		break;
	}
	if (WIDGET_HandleActive(pObj, pMsg))
		WM_DefaultProc(pMsg);
}
void Frame::_cbClient(WM_MESSAGE *pMsg) {
	WObj *pWin = pMsg->pWin;
	auto pObj = (Frame *)pWin->Parent();
	WM_CALLBACK *cb = pObj->cb;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		if (pObj->Props.ClientColor != GUI_INVALID_COLOR) {
			GUI.BkColor(pObj->Props.ClientColor);
			GUI_Clear();
		}
		if (cb) {
			WM_MESSAGE msg = *pMsg;
			msg.pWin = pWin;
			cb(&msg);
		}
		return;
	case WM_SET_FOCUS:
		if (pMsg->Data) {
			if (pObj->pFocussedChild && (pObj->pFocussedChild != pWin))
				pObj->pFocussedChild->Focus();
			else
				pObj->pFocussedChild = WM_SetFocusOnNextChild(pWin);
			pMsg->Data = 0;
		}
		return;
	case WM_GET_ACCEPT_FOCUS:
		WIDGET_HandleActive(pObj, pMsg);
		return;
	case WM_KEY:
	{
		const WM_KEY_INFO *ki = (const WM_KEY_INFO *)pMsg->Data;
		if (ki->PressedCnt > 0) {
			switch (ki->Key) {
			case GUI_KEY_TAB:
				pObj->pFocussedChild = WM_SetFocusOnNextChild(pWin);
				return;
			}
		}
		break;
	}
	case WM_GET_BKCOLOR:
		pMsg->Data = pObj->Props.ClientColor;
		return;
	case WM_GET_INSIDE_RECT:
	case WM_GET_ID:
	case WM_GET_CLIENT_WINDOW:
		WM_DefaultProc(pMsg);
		return;
	}
	if (cb) {
		pMsg->pWin = pObj;
		cb(pMsg);
	}
	else
		WM_DefaultProc(pMsg);
}
int Frame::_CalcTitleHeight() const {
	if (!(State & FRAME_CF_TITLEVIS))
		return 0;
	return Props.TitleHeight ?
		Props.TitleHeight :
		2 + GUI_GetYSizeOfFont(Props.pFont);
}
Frame::Positions Frame::_CalcPositions() const {
	auto &&rect = Rect();
	auto &&size = Size();
	int BorderSize = Props.BorderSize;
	auto TitleHeight = _CalcTitleHeight();
	auto IBorderSize = Status & FRAME_CF_TITLEVIS ? Props.IBorderSize : 0;
	auto MenuHeight = pMenu ? pMenu->SizeY() : 0;
	Positions Pos = {
		TitleHeight,
		MenuHeight,
		{{BorderSize, BorderSize + IBorderSize + TitleHeight + MenuHeight},
		 size - BorderSize - 1},
		{BorderSize,
		 {size.x - BorderSize - 1, BorderSize + TitleHeight - 1}}};
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
		WM_MoveChildTo(pClient, Pos.rClient.x0, Pos.rClient.y0);
		pClient->Size(Pos.rClient.size());
	}
	if (pMenu)
		WM_MoveChildTo(pMenu, Pos.rClient.x0, Pos.rClient.y0 - Pos.MenuHeight);
}

void Frame::_InvalidateButton(int Id) {
	for (auto pChild = pFirstChild; pChild; pChild = pChild->pNext)
		if (pChild->ID() == Id)
			pChild->Invalidate();
}
void Frame::_RestoreMinimized() {
	if (!(Flags & FRAME_CF_MINIMIZED))
		return;
	/* When window was minimized, restore it */
	SizeY(rRestore.ysize());
	pClient->Visible(true);
	if (pMenu) pMenu->Visible(true);
	_UpdatePositions();
	Flags &= ~FRAME_CF_MINIMIZED;
	_InvalidateButton(GUI_ID_MINIMIZE);
}
void Frame::_RestoreMaximized() {
	/* When window was maximized, restore it */
	if (!(Flags & FRAME_CF_MAXIMIZED))
		return;
	Position(rRestore.left_top());
	Size(rRestore.size());
	_UpdatePositions();
	Flags &= ~FRAME_CF_MAXIMIZED;
	_InvalidateButton(GUI_ID_MAXIMIZE);
}

void Frame::Minimize() {
	_RestoreMaximized();
	if (Flags & FRAME_CF_MINIMIZED)
		return;
	rRestore = rect;
	pClient->Visible(true);
	if (pMenu) pMenu->Visible(true);
	SizeY(_CalcTitleHeight() + pEffect->EffectSize * 2 + 2);
	_UpdatePositions();
	Flags |= FRAME_CF_MINIMIZED;
	_InvalidateButton(GUI_ID_MINIMIZE);
}
void Frame::Maximize() {
	_RestoreMinimized();
	if (!(Flags & FRAME_CF_MAXIMIZED))
		return;
	auto pParent = Parent();
	auto rParent = pParent->rect;
	if (!pParent->pParent)
		rParent = LCD_Rect();
	rRestore = rect;
	Position(rParent.left_top());
	Size(rParent.size());
	_UpdatePositions();
	Flags |= FRAME_CF_MAXIMIZED;
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
		auto &&r = pChild->rect - rect.left_top();
		if (r.y0 == Props.BorderSize && r.y1 - r.y0 + 1 == OldHeight) {
			if (pChild->Status & WC_ANCHOR_RIGHT)
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
			auto &&r = pChild->rect - rect.left_top();
			if (r.y0 != Props.BorderSize)
				continue;
			if (r.y1 - r.y0 + 1 != OldHeight)
				continue;
			int xLeft = GUI_XMAX, xRight = GUI_XMIN;
			if (pChild->Status & WC_ANCHOR_RIGHT) {
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
	WM_SetAnchor(pMenu, WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT);
	_UpdatePositions();
	Invalidate();
}

Button *Frame::AddButton(int Flags, int Off, int Id) {
	auto &&Pos = _CalcPositions();
	int Size = this->TitleHeight();
	int BorderSize = this->BorderSize();
	int WinFlags, x;
	if (Flags & FRAMEWIN_BUTTON_RIGHT) {
		x = Pos.rTitleText.x1 - (Size - 1) - Off;
		WinFlags = WC_VISIBLE | WC_ANCHOR_RIGHT;
	}
	else {
		x = Pos.rTitleText.x0 + Off;
		WinFlags = WC_VISIBLE;
	}
	auto pButton = BUTTON_CreateAsChild(x, BorderSize, Size, Size, this, Id, WinFlags);
	BUTTON_SetFocussable(pButton, 0);
	return pButton;
}
Button *Frame::AddMaxButton(int Flags, int Off) {
	static auto _DrawMax = [] {
		SRect r;
		WM_GetInsideRect(&r);
		WM_ADDORG(r.x0, r.y0);
		WM_ADDORG(r.x1, r.y1);
		WM_ITERATE_START(&r) {
			GUI_LCD_DrawHLine(r.x0 + 1, r.y0 + 1, r.x1 - 1);
			GUI_LCD_DrawHLine(r.x0 + 1, r.y0 + 2, r.x1 - 1);
			GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - 1, r.x1 - 1);
			GUI_LCD_DrawVLine(r.x0 + 1, r.y0 + 1, r.y1 - 1);
			GUI_LCD_DrawVLine(r.x1 - 1, r.y0 + 1, r.y1 - 1);
		} WM_ITERATE_END();
	};
	static auto _DrawMaxRestore = [] {
		SRect r;
		WM_GetInsideRect(&r);
		WM_ADDORG(r.x0, r.y0);
		WM_ADDORG(r.x1, r.y1);
		int Size = ((r.x1 - r.x0 + 1) << 1) / 3;
		WM_ITERATE_START(&r) {
			GUI_LCD_DrawHLine(r.x1 - Size, r.y0 + 1, r.x1 - 1);
			GUI_LCD_DrawHLine(r.x1 - Size, r.y0 + 2, r.x1 - 1);
			GUI_LCD_DrawHLine(r.x0 + Size, r.y0 + Size, r.x1 - 1);
			GUI_LCD_DrawVLine(r.x1 - Size, r.y0 + 1, r.y1 - Size);
			GUI_LCD_DrawVLine(r.x1 - 1, r.y0 + 1, r.y0 + Size);
			GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - Size, r.x0 + Size);
			GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - Size + 1, r.x0 + Size);
			GUI_LCD_DrawHLine(r.x0 + 1, r.y1 - 1, r.x0 + Size);
			GUI_LCD_DrawVLine(r.x0 + 1, r.y1 - Size, r.y1 - 1);
			GUI_LCD_DrawVLine(r.x0 + Size, r.y1 - Size, r.y1 - 1);
		} WM_ITERATE_END();
	};
	static auto _Draw = [] {
		auto pObj = (Frame *)WM_GetActiveWindow()->Parent();
		if (pObj->Flags & FRAME_CF_MAXIMIZED)
			_DrawMaxRestore();
		else
			_DrawMax();
	};
	auto pButton = AddButton(Flags, Off, GUI_ID_MAXIMIZE);
	BUTTON_SetSelfDraw(pButton, 0, _Draw);
	WM_SetCallback(pButton, [](WM_MESSAGE *pMsg) {
		if (pMsg->MsgId != WM_NOTIFY_PARENT_REFLECTION) {
			BUTTON_Callback(pMsg);
			return;
		}
		auto pWin = (Frame *)pMsg->pWinSrc;
		if (pWin->Flags & FRAME_CF_MAXIMIZED)
			pWin->Restore();
		else
			pWin->Maximize();
	});
	return pButton;
}Button *Frame::AddCloseButton(int Flags, int Off) {
	auto pButton = AddButton(Flags, Off, GUI_ID_CLOSE);
	static auto _Draw = [] {
		SRect r;
		WM_GetInsideRect(&r);
		WM_ADDORG(r.x0, r.y0);
		WM_ADDORG(r.x1, r.y1);
		int Size = r.x1 - r.x0 - 2;
		WM_ITERATE_START(&r); {
			int i;
			for (i = 2; i < Size; i++) {
				GUI_LCD_DrawHLine(r.x0 + i, r.y0 + i, r.x0 + i + 1);
				GUI_LCD_DrawHLine(r.x1 - i - 1, r.y0 + i, r.x1 - i);
			}
		} WM_ITERATE_END();
	};
	BUTTON_SetSelfDraw(pButton, 0, _Draw);
	WM_SetCallback(pButton, [](WM_MESSAGE *pMsg) {
		if (pMsg->MsgId == WM_NOTIFY_PARENT_REFLECTION)
			pMsg->pWinSrc->Delete();
		else
			BUTTON_Callback(pMsg);
	});
	return pButton;
}
Button *Frame::AddMinButton(int Flags, int Off) {
	static auto _DrawMin = [] {
		SRect r;
		WM_GetInsideRect(&r);
		WM_ADDORG(r.x0, r.y0);
		WM_ADDORG(r.x1, r.y1);
		int Size = (r.x1 - r.x0 + 1) >> 1;
		WM_ITERATE_START(&r) {
			int i;
			for (i = 1; i < Size; i++)
				GUI_LCD_DrawHLine(r.x0 + i, r.y1 - i - (Size >> 1), r.x1 - i);
		} WM_ITERATE_END();
	};
	static auto _DrawRestore = [] {
		SRect r;
		WM_GetInsideRect(&r);
		WM_ADDORG(r.x0, r.y0);
		WM_ADDORG(r.x1, r.y1);
		int Size = (r.x1 - r.x0 + 1) >> 1;
		WM_ITERATE_START(&r); {
			int i;
			for (i = 1; i < Size; i++)
				GUI_LCD_DrawHLine(r.x0 + i, r.y0 + i + (Size >> 1), r.x1 - i);
		} WM_ITERATE_END();
	};
	auto pButton = AddButton(Flags, Off, GUI_ID_MINIMIZE);
	static auto _Draw = [] {
		auto pObj = (Frame *)WM_GetActiveWindow()->Parent();
		if (pObj->Flags & FRAME_CF_MINIMIZED)
			_DrawRestore();
		else
			_DrawMin();
	};
	BUTTON_SetSelfDraw(pButton, 0, _Draw);
	WM_SetCallback(pButton, [](WM_MESSAGE *pMsg) {
		if (pMsg->MsgId == WM_NOTIFY_PARENT_REFLECTION) {
			auto pObj = (Frame *)pMsg->pWinSrc;
			if (pObj->Flags & FRAME_CF_MINIMIZED)
				pObj->Restore();
			else
				pObj->Minimize();
		}
		else
			BUTTON_Callback(pMsg);
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
		int y0 = pChild->rect.y0 - rect.y0;
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
Frame *Frame::Create(
	int x0, int y0, int xsize, int ysize, WObj *pParent,
	int WinFlags, int ExFlags, int Id, const char *pTitle, WM_CALLBACK *cb) {
	auto pObj = (Frame *)WObj::Create(
		x0, y0, xsize, ysize, pParent,
		WinFlags | WC_LATE_CLIP, _Callback,
		sizeof(Frame) - sizeof(WObj));
	if (!pObj) return nullptr;
	/* init widget specific variables */
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE | FRAME_CF_TITLEVIS);
	/* init member variables */
	pObj->Props = FRAMEWIN__DefaultProps;
	pObj->textAlign = GUI_TA_LEFT;
	pObj->cb = cb;
	pObj->Flags = ExFlags;
	pObj->pFocussedChild = 0;
	pObj->pMenu = 0;
	auto &&Pos = pObj->_CalcPositions();
	pObj->pClient = WObj::Create(
		Pos.rClient.x0, Pos.rClient.y0,
		Pos.rClient.x1 - Pos.rClient.x0 + 1,
		Pos.rClient.y1 - Pos.rClient.y0 + 1,
		pObj, WC_ANCHOR_RIGHT | WC_ANCHOR_LEFT | WC_ANCHOR_TOP
		| WC_ANCHOR_BOTTOM | WC_VISIBLE | WC_LATE_CLIP,
		_cbClient, 0);
	pObj->Text(pTitle);
	return pObj;
}
