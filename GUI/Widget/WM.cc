#define WM_C
#include "WM.h"

uint8_t WM_IsActive;
PidState WM_PID__StateLast;

void WObj::_InsertWindowIntoList(WObj *pParent) {
	if (!pParent) return;
	pNext = nullptr;
	this->pParent = pParent;
	bool bOnTop = Status & WC_STAYONTOP;
	auto i = pParent->pFirstChild;
	if (!i) {
		pParent->pFirstChild = this;
		return;
	}
	if (!bOnTop) {
		if (i->Status & WC_STAYONTOP) {
			pNext = i;
			pParent->pFirstChild = this;
			return;
		}
	}
	for (;;) {
		auto pNext = i->pNext;
		if (!pNext) {
			i->pNext = this;
			break;
		}
		if (!bOnTop) {
			if (pNext->Status & WC_STAYONTOP) {
				i->pNext = this;
				this->pNext = pNext;
				break;
			}
		}
		i = pNext;
	}
}
void WObj::_RemoveWindowFromList() {
	if (!pParent) return;
	WObj *i = pParent->pFirstChild;
	if (i == this)
		pParent->pFirstChild = i->pNext;
	else for (; i; i = i->pNext)
		if (i->pNext == this) {
			i->pNext = pNext;
			break;
		}
}

WObj *WObj::pWinFirst = nullptr;
void WObj::_RemoveFromLinList() {
	auto pfWin = pWinFirst;
	while (pfWin) {
		auto pNext = pfWin->pNextLin;
		if (pNext == this) {
			pfWin->pNextLin = pNextLin;
			break;
		}
		pfWin = pNext;
	}
}
void WObj::_AddToLinList() {
	if (pWinFirst) {
		pNextLin = pWinFirst->pNextLin;
		pWinFirst->pNextLin = this;
	}
	else
		pWinFirst = this;
}

WObj::operator bool() const {
	for (auto i = pWinFirst; i; i = i->pNextLin)
		if (i == this)
			return true;
	return false;
}

WObj::CriticalHandles *WObj::CriticalHandles::pFirst = nullptr;
void WObj::CriticalHandles::Remove() {
	if (!pFirst)
		return;
	CriticalHandles *pLast = 0;
	for (auto pCH = pFirst; pCH; pCH = pCH->pNext) {
		if (pCH == this) {
			if (pLast)
				pLast->pNext = pCH->pNext;
			else if (pCH->pNext)
				pFirst = pCH->pNext;
			else
				pFirst = nullptr;
			break;
		}
		pLast = pCH;
	}
}
void WObj::CriticalHandles::Check(WObj *pWin) {
	for (auto pCH = this; pCH; pCH = pCH->pNext)
		if (pCH->pWin == pWin)
			pCH->pWin = nullptr;
}

void WObj::Detach() {
	if (!pParent) return;
	_RemoveWindowFromList();
	InvalidateArea(rect);
	Move(-pParent->rect.left_top());
	pParent = nullptr;
}

uint16_t WObj::nWindows = 0;
WObj *WObj::pWinActive = nullptr;

WObj::WObj(int x0, int y0, int width, int height,
		   WM_CB *cb, WObj *pParent, uint16_t Style) :
	cb(cb), Status(Style &WC_MASK) {
	if (!pParent)	
		if (nWindows)
			pParent = pWinDesktop;
	if (pParent) {
		x0 += pParent->rect.x0;
		y0 += pParent->rect.y0;
		if (width == 0)
			width = pParent->rect.x1 - pParent->rect.x0 + 1;
		if (height == 0)
			height = pParent->rect.y1 - pParent->rect.y0 + 1;
	}
	++nWindows;
	rect.x0 = x0;
	rect.y0 = y0;
	rect.x1 = x0 + width - 1;
	rect.y1 = y0 + height - 1;
	_AddToLinList();
	_InsertWindowIntoList(pParent);
	if (Style & WC_ACTIVATE)
		Select();
	if (Style & WC_BGND)
		BringToBottom();
	if (Style & WC_VISIBLE) {
		Status |= WC_VISIBLE;
		Invalidate();
	}
	SendMessage(WM_CREATE);
}

void WObj::Delete() {
	if (!*this) return;
	pWinNextDraw = nullptr;
	if (pWinFocus == this)
		pWinFocus = nullptr;
	if (pWinCapture == this)
		pWinCapture = nullptr;
	CriticalHandles::pFirst->Check(this);
	NotifyParent(WM_NOTIFICATION_CHILD_DELETED);
	for (auto pChild = pFirstChild; pChild; ) {
		auto pNext = pChild->pNext;
		pChild->Delete();
		pChild = pNext;
	}
	SendMessage(WM_DELETE);
	Detach();
	_RemoveFromLinList();
	if (Status & WC_ACTIVATE)
		--nInvalidWindows;
	--nWindows;
	GUI_MEM_Free(this);
	pWinFirst->Select();
}
void WObj::Select() {
	pWinActive = this;
	GUI.ClipRect();
	GUI.off = rect.left_top();
}

const SRect *WObj::SetUserClipRect(const SRect *pRect) {
	const SRect *pRectReturn = GUI.pUserClipRect;
	GUI.pUserClipRect = pRect;
	_ActivateClipRect();
	return pRectReturn;
}

#pragma region Invalidate & Validate
uint16_t WObj::nInvalidWindows = 0;
bool WObj::_ClipAtParentBorders(SRect &r) const {
	for (auto pWin = this; pWin->Status & WC_VISIBLE; pWin = pWin->pParent) {
		r &= pWin->rect;
		if (!pWin->pParent)
			return pWin = WObj::Desktop();
	}
	return false;
}
void WObj::_Invalidate1Abs(SRect r) {
	if (!(Status & WC_VISIBLE))
		return;
	r &= rect;
	if (!r) return;
	if (Status & WC_ACTIVATE)
		rInvalid |= r;
	else {
		rInvalid = r;
		Status |= WC_ACTIVATE;
		++nInvalidWindows;
	}
}
void WObj::InvalidateArea(const SRect &r) {
	for (auto i = pWinFirst; i; i = i->pNextLin)
		i->_Invalidate1Abs(r);
}
void WObj::_Invalidate(SRect r) {
	if (!(Status & WC_VISIBLE))
		return;
	if (_ClipAtParentBorders(r))
		_Invalidate1Abs(r);
}
void WObj::InvalidateDescs() {
	Invalidate();
	for (auto pChild = pFirstChild; pChild; pChild = pChild->pNext)
		pChild->InvalidateDescs();
}
void WObj::Validate() {
	if (Status & WC_ACTIVATE) {
		Status &= ~WC_ACTIVATE;
		--nInvalidWindows;
	}
}
#pragma endregion

#pragma region Paint
uint8_t WObj::nPaintCallback = 0;
WObj *WObj::pWinNextDraw = nullptr;
void WObj::_Paint1() {
	if (!cb) return;
	if (!(Status & WC_VISIBLE))
		return;
	++nPaintCallback;
	auto rInvalidClient = rInvalid - Position();
	if (WObj::IVR_Init(&rInvalid)) do {
		WM_MSG msg;
		msg.pWin = this;
		msg.msgid = WM_PAINT;
		msg.data = (size_t)&rInvalidClient;
		GUI.Props = GUI.DefaultProps;
		SendMessage(&msg);
	} while (WObj::IVR_Next());
	--nPaintCallback;
}
bool WObj::_OnPaint() {
	if (!(Status & WC_ACTIVATE))
		return false;
	bool res = false;
	if (cb)
		if (_ClipAtParentBorders(rInvalid)) {
			Select();
			_Paint1();
			res = true;
		}
	Status &= ~WC_ACTIVATE;
	--nInvalidWindows;
	return res;
}
bool WObj::PaintNext() {
	if (!WM_IsActive || !nInvalidWindows)
		return false;
	int UpdateRem = 1;
	auto pWin = pWinNextDraw ? pWinNextDraw : pWinFirst;
	for (; pWin && UpdateRem; ) {
		if (pWin->_OnPaint())
			UpdateRem--;
		pWin = pWin->pNextLin;
	}
	pWinNextDraw = pWin;
	return true;
}
#pragma endregion

#pragma region IVR
WObj::ContextIVR WObj::_ClipContext;
void WObj::_ActivateClipRect() {
	auto &rSrc = WM_IsActive ? _ClipContext.CurRect : pWinActive->rect;
	GUI.ClipRect(
		GUI.pUserClipRect ?
		pWinActive->ClientToScreen(*GUI.pUserClipRect) & rSrc : rSrc);
}
bool WObj::_FindNextIVR() {
	static auto _Findy1 = [](WObj *pWin, SRect &r) {
		for (; pWin; pWin = pWin->pNext)
			if (pWin->Status & WC_VISIBLE) {
				SRect rWinClipped = pWin->rect;
				if (r && rWinClipped)
					if (pWin->rect.y0 > r.y0) {
						if (rWinClipped.y0 - 1 < r.y1)
							r.y1 = rWinClipped.y0 - 1; /* Check upper border of window */
					}
					else if (rWinClipped.y1 < r.y1)
						r.y1 = rWinClipped.y1; /* Check lower border of window */
			}
	};
	static auto _Findx0 = [](WObj *pWin, SRect &r) {
		bool t = false;
		for (; pWin; pWin = pWin->pNext)
			if (pWin->Status & WC_VISIBLE) {
				SRect rWinClipped = pWin->rect;
				if (r && rWinClipped) {
					r.x0 = rWinClipped.x1 + 1;
					t = true;
				}
			}
		return t;
	};
	static auto _Findx1 = [](WObj *pWin, SRect &r) {
		for (; pWin; pWin = pWin->pNext)
			if (pWin->Status & WC_VISIBLE) {
				SRect rWinClipped = pWin->rect;
				if (r && rWinClipped)
					r.x1 = rWinClipped.x0 - 1;
			}
	};
	auto r = _ClipContext.CurRect;
	if (_ClipContext.Cnt == 0)
		r.left_top(_ClipContext.rClient.left_top());
	else {
		r.x0 = _ClipContext.CurRect.x1 + 1;
		r.y0 = _ClipContext.CurRect.y0;
		if (r.x0 > _ClipContext.rClient.x1) {
		NextStripe:
			r.x0 = _ClipContext.rClient.x0;
			r.y0 = _ClipContext.CurRect.y1 + 1;
		}
	}
	if (r.y0 > _ClipContext.rClient.y1)
		return false;
	if (r.x0 == _ClipContext.rClient.x0) {
		r.right_bottom(_ClipContext.rClient.right_bottom());
		for (auto pParent = pWinActive; pParent; pParent = pParent->pParent)
			_Findy1(pParent->pNext, r);
		_Findy1(pWinActive->pFirstChild, r);
	}
Find_x0:
	r.x1 = r.x0;
	for (auto pParent = pWinActive; pParent; pParent = pParent->pParent)
		if (_Findx0(pParent->pNext, r))
			goto Find_x0;
	if (_Findx0(pWinActive->pFirstChild, r))
		goto Find_x0;
	r.x1 = _ClipContext.rClient.x1;
	if (r.x1 < r.x0) {
		_ClipContext.CurRect = r;
		goto NextStripe;
	}
	for (auto pParent = pWinActive; pParent; pParent = pParent->pParent)
		_Findx1(pParent->pNext, r);
	_Findx1(pWinActive->pFirstChild, r);
	if (_ClipContext.Cnt > 200)
		return false;
	_ClipContext.CurRect = r;
	return true;
}
bool WObj::IVR_Next() {
	static bool bCursorHidden = true;
	if (WM_IsActive == 0)
		return false;
	if (_ClipContext.EntranceCnt > 1) {
		_ClipContext.EntranceCnt--;
		return false;
	}
	if (bCursorHidden) {
		bCursorHidden = false;
		GUI.Cursor._TempShow();
	}
	++_ClipContext.Cnt;
	if (!_FindNextIVR()) {
		_ClipContext.EntranceCnt--;
		return false;
	}
	_ActivateClipRect();
	bCursorHidden = GUI.Cursor._TempHide(&_ClipContext.CurRect);
	return true;
}
bool WObj::IVR_Init(const SRect *pMaxRect) {
	if (WM_IsActive == 0) {
		_ActivateClipRect();
		return true;
	}
	if (++_ClipContext.EntranceCnt > 1)
		return true;
	_ClipContext.Cnt = -1;
	SRect r;
	if (nPaintCallback)
		r = pWinActive->rInvalid;
	else if (pWinActive->Status & WC_VISIBLE)
		r = pWinActive->rect;
	else {
		--_ClipContext.EntranceCnt;
		return false;
	}
	if (pMaxRect)
		r &= *pMaxRect;
	if (GUI.pUserClipRect)
		r &= pWinActive->ClientToScreen(*GUI.pUserClipRect);
	if (!pWinActive->_ClipAtParentBorders(r)) {
		--_ClipContext.EntranceCnt;
		return false;
	}
	_ClipContext.rClient = r;
	return IVR_Next();
}
#pragma endregion

bool WObj::Exec1() {
	if (HandlePID())
		return true;
	if (GUI_PollKeyMsg())
		return true;
	if (PaintNext())
		return true;
	return false;
}

void WM_Activate() {
	WM_IsActive = 1;
}
void WM_Deactivate() {
	WM_IsActive = 0;
	GUI.ClipRect();
}
void WObj::DefCallback(WM_MSG *pMsg) {
	auto pWin = pMsg->pWin;
	switch (pMsg->msgid) {
		case WM_GET_INSIDE_RECT:
			*(SRect *)pMsg->data = pWin->ClientRect();
			break;
		case WM_GET_CLIENT_WINDOW:
			pMsg->data = (size_t)pWin;
			return;
		case WM_KEY:
			pWin->SendToParent(pMsg);
			return;
		case WM_GET_BKCOLOR:
			pMsg->data = RGB_INVALID_COLOR;
			return;
		case WM_NOTIFY_ENABLE:
			pWin->Invalidate();
			return;
	}
	pMsg->data = 0;
}

WObj *WObj::pWinDesktop = nullptr;
RGBC WObj::aColorDesktop = RGB_GRAY;
void WObj::Init() {
	if (pWinDesktop) return;
	GUI.pUserClipRect = nullptr;
	pWinActive = pWinDesktop = new WObj(0, 0, GUI_XMAX, GUI_YMAX, [](WM_MSG *pMsg) {
		switch (pMsg->msgid) {
			case WM_KEY:
			{
				auto pKeyInfo = (const WM_KEY_INFO *)pMsg->data;
				if (pKeyInfo->PressedCnt == 1)
					GUI_StoreKey(pKeyInfo->Key);
				break;
			}
			case WM_PAINT:
				if (aColorDesktop != RGB_INVALID_COLOR) {
					GUI.BkColor(aColorDesktop);
					GUI.Clear();
				}
			default:
				DefCallback(pMsg);
		}
	}, nullptr, WC_VISIBLE);
	pWinDesktop->Invalidate();
	CriticalHandles::Modal.Add();
	CriticalHandles::Last.Add();
	pWinDesktop->Select();
	WM_Activate();
}

void WObj::Parent(WObj *pParent) {
	if (!pParent)
		return;
	if (pParent == this)
		return;
	if (this->pParent == pParent)
		return;
	Detach();
	_InsertWindowIntoList(pParent);
	Position(pParent->rect.left_top());
}
void WObj::Parent(WObj *pParent, Point p) {
	Detach();
	Position(p);
	Parent(pParent);
}

WObj *WObj::DialogItem(uint16_t Id) {
	for (auto pObj = pFirstChild; pObj; pObj = pObj->pNext)
		if (pObj->ID() == Id)
			return pObj;
		else if (auto pItem = pObj->DialogItem(Id))
			return pItem;
	return nullptr;
}

void WObj::BringToTop() {
	if (!pNext)
		return;
	if (!(Status & WC_STAYONTOP))
		if (pNext->Status & WC_STAYONTOP)
			return;
	_RemoveWindowFromList();
	_InsertWindowIntoList(pParent);
	InvalidateDescs();
}
void WObj::BringToBottom() {
	if (auto pPrev = PrevSibling()) {
		auto pParent = Parent();
		pPrev->pNext = pNext;
		pNext = pParent->pFirstChild;
		pParent->pFirstChild = this;
		InvalidateArea(rect);
	}
}

SRect WObj::InsideRectAbs() const {
	SRect rInsideRect;
	WM_MSG msg;
	msg.data = (size_t)&rInsideRect;
	msg.msgid = WM_GET_INSIDE_RECT;
	const_cast<WObj *>(this)->SendMessage(&msg);
	return rInsideRect;
}
SRect WObj::InsideRect() const {
	auto &&rInside = InsideRectAbs();
	if (auto pBarV = DialogItem(GUI_ID_VSCROLL)) {
		auto &&rScrollbar = pBarV->Rect();
		auto Flags = pBarV->Status;
		if ((Flags & WC_ANCHOR_RIGHT) && (Flags & WC_VISIBLE))
			rInside.x1 = rScrollbar.x0 - 1;
	}
	if (auto pBarH = DialogItem(GUI_ID_HSCROLL)) {
		auto &&rScrollbar = pBarH->Rect();
		auto Flags = pBarH->Status;
		if ((Flags & WC_ANCHOR_BOTTOM) && (Flags & WC_VISIBLE))
			rInside.y1 = rScrollbar.y0 - 1;
	}
	return rInside;
}

#pragma region Properties
void WObj::StayOnTop(bool bEnable) {
	auto Status = this->Status;
	if (bEnable)
		Status |= WC_STAYONTOP;
	else
		Status &= ~WC_STAYONTOP;
	if (this->Status == Status)
		return;
	this->Status = Status;
	Parent(Parent());
}
void WObj::Enable(bool bEnable) {
	auto Status = this->Status;
	if (bEnable)
		Status &= ~WC_DISABLED;
	else
		Status |= WC_DISABLED;
	if (this->Status == Status)
		return;
	WM_MSG msg;
	msg.msgid = WM_NOTIFY_ENABLE;
	msg.data = bEnable;
	this->Status = Status;
	this->SendMessage(&msg);
}
void WObj::Visible(bool bVisible) {
	auto Status = this->Status;
	if (bVisible)
		Status |= WC_VISIBLE;
	else
		Status &= ~WC_VISIBLE;
	if (this->Status == Status)
		return;
	this->Status = Status;
	if (bVisible)
		InvalidateDescs();
	else
		InvalidateArea(rect);
}
#pragma endregion

#pragma region Focus
WObj *WObj::pWinFocus = nullptr;
WObj *WObj::Focus() {
	if (this == pWinFocus)
		return pWinFocus;
	if (Status & WC_NOACTIVATE)
		return nullptr;
	if (!Focussable())
		return nullptr;
	WM_NOTIFY_CHILD_HAS_FOCUS_INFO Info;
	Info.pOld = pWinFocus;
	Info.pNew = this;
	WM_MSG msg = { 0 };
	msg.msgid = WM_SET_FOCUS;
	msg.data = 0;
	if (pWinFocus)
		pWinFocus->SendMessage(&msg);
	msg.data = 1;
	pWinFocus = this;
	SendMessage(&msg);
	if (msg.data)
		return this;
	for (auto pWin = this; (pWin = pWin->Parent());) {
		msg.msgid = WM_NOTIFY_CHILD_HAS_FOCUS;
		msg.data = (size_t)&Info;
		pWin->SendMessage(&msg);
	}
	if (auto pWin = Info.pOld)
		if (*pWin)
			while ((pWin = pWin->Parent()) != nullptr) {
				msg.msgid = WM_NOTIFY_CHILD_HAS_FOCUS;
				msg.data = (size_t)&Info;
				pWin->SendMessage(&msg);
			}
	return nullptr;
}
WObj *WObj::FocusNextChild() {
	static auto _GetNextChild = [](WObj *pParent, WObj *pChild) {
		WObj *pObj = nullptr;
		if (pChild)
			pObj = pChild->pNext;
		if (!pObj)
			pObj = pParent->pFirstChild;
		return pObj != pChild ? pObj : nullptr;
	};
	auto pChild = _GetNextChild(this, FocussedChild());
	auto pWin = pChild;
	while (pWin) {
		if (pWin->Focussable())
			break;
		pWin = _GetNextChild(pParent, pWin);
		if (pWin == pChild)
			break;
	}
	return pWin ? pWin->Focus() : nullptr;
}
#pragma endregion

bool WObj::OnKey(int Key, int Pressed) {
	if (!pWinFocus)
		return false;
	WM_KEY_INFO Info;
	Info.Key = Key;
	Info.PressedCnt = Pressed;
	WM_MSG msg;
	msg.msgid = WM_KEY;
	msg.data = (size_t)&Info;
	pWinFocus->_SendMessage(&msg);
	return true;
}
void WObj::Paint() {
	Select();
	GUI.Props = GUI.DefaultProps;
	Invalidate();  /* Important ... Window procedure is informed about invalid rect and may optimize */
	_Paint1();
	Validate();
}
void WM_PID__GetPrevState(PidState *pPrevState) {
	*pPrevState = WM_PID__StateLast;
}

WObj *WObj::LastSibling() {
	for (auto pWin = this; pWin; pWin = pWin->pNext)
		if (!pWin->pNext)
			return pWin;
	return nullptr;
}
WObj *WObj::PrevSibling() {
	for (auto pObj = FirstSibling(); pObj; pObj = pObj->pNext)
		if (pObj == this)
			return nullptr;
		else if (pObj->pNext == this)
			return pObj;
	return nullptr;
}

static void _NotifyVisChanged(WObj *pWin, SRect *pRect) {
	for (pWin = pWin->FirstChild(); pWin; pWin = pWin->NextSibling()) {
		if (!(pWin->Styles() & WC_VISIBLE))
			continue;
		if (pWin->RectAbs() && pRect) {
			pWin->SendMessage(WM_NOTIFY_VIS_CHANGED);
			_NotifyVisChanged(pWin, pRect);
		}
	}
}
void WM__NotifyVisChanged(WObj *pWin, SRect *pRect) {
	auto pParent = pWin->Parent();
	if (pParent)
		_NotifyVisChanged(pParent, pRect);
}

bool WM__IsAncestor(WObj *pChild, WObj *pParent) {
	if (!pChild)
		return false;
	if (!pParent)
		return false;
	for (; pChild; pChild = pChild->Parent())
		if (pChild->Parent() == pParent)
			return true;
	return false;
}
bool WM__IsAncestorOrSelf(WObj *pChild, WObj *pParent) {
	return pChild == pParent ? true :
		WM__IsAncestor(pChild, pParent);
}

#pragma region Capture
WObj *WObj::pWinCapture = nullptr;
bool WObj::bCaptureAutoRelease = false;
Point WObj::CapturePoint;
void WObj::Capture(bool bAutoRelease) {
	if (pWinCapture != this)
		CaptureRelease();
	pWinCapture = this;
	bCaptureAutoRelease = bAutoRelease;
}
void WObj::CaptureRelease() {
	if (!pWinCapture)
		return;
	WM_MSG msg;
	msg.msgid = WM_CAPTURE_RELEASED;
	pWinCapture->SendMessage(&msg);
	pWinCapture = 0;
}
void WObj::CaptureMove(const PidState &pid, int MinVisibility) {
	if (!Captured()) {
		Capture(true);
		CapturePoint = pid;
		return;
	}
	auto &&d = pid - CapturePoint;
	if (MinVisibility == 0) {
		Move(d);
		return;
	}
	auto &&rCapture = RectAbs() + d,
		&&rParent = Parent()->RectAbs();
	rParent /= MinVisibility;
	if (rCapture && rParent)
		Move(d);
}
#pragma endregion

WObj *WObj::ScreenToWin(Point Pos, WObj *pStop, WObj *pWin) {
	if (!pWin->Visible()) return nullptr;
	if (pWin->RectAbs() > Pos) return nullptr;
	for (auto pChild = pWin->pFirstChild;
		 pChild && pChild != pStop;
		 pChild = pChild->pNext)
		if (auto pHit = ScreenToWin(Pos, pStop, pChild))
			pWin = pHit;
	return pWin;
}

#pragma region Resize & Move
void WObj::_MoveDescendents(Point d) {
	for (auto i = this; i; i = i->pNext) {
		i->rect += d;
		i->rInvalid += d;
		i->pFirstChild->_MoveDescendents(d);  /* Children need to be moved along ...*/
		i->SendMessage(WM_MOVE);
	}
}
void WObj::Move(Point dPos) {
	if (!dPos) return;
	auto rOld = rect;
	rect += dPos;
	rInvalid += dPos;
	pFirstChild->_MoveDescendents(dPos);
	if (Status & WC_VISIBLE) {
		InvalidateArea(rect);
		InvalidateArea(rOld);
	}
	SendMessage(WM_MOVE);
}

void WObj::_UpdateChildPositions(SRect d) {
	for (auto pChild = pFirstChild; pChild; pChild = pChild->pNext) {
		SRect rNew = pChild->rect;
		switch (pChild->Status & (WC_ANCHOR_RIGHT | WC_ANCHOR_LEFT)) {
			case WC_ANCHOR_RIGHT:
				rNew.x0 += d.x1;
				rNew.x1 += d.x1;
				break;
			case WC_ANCHOR_RIGHT | WC_ANCHOR_LEFT:
				rNew.x0 += d.x0;
				rNew.x1 += d.x1;
				break;
			default:
				rNew.x0 += d.x0;
				rNew.x1 += d.x0;
		}
		switch (pChild->Status & (WC_ANCHOR_TOP | WC_ANCHOR_BOTTOM)) {
			case WC_ANCHOR_BOTTOM:
				rNew.y0 += d.y1;
				rNew.y1 += d.y1;
				break;
			case WC_ANCHOR_BOTTOM | WC_ANCHOR_TOP:
				rNew.y0 += d.y0;
				rNew.y1 += d.y1;
				break;
			default:
				rNew.y0 += d.y0;
				rNew.y1 += d.y0;
		}
		pChild->RectAbs(rNew);
	}
}
void WObj::Resize(Point dSize) {
	if (!dSize) return;
	SRect rOld = rect, rNew = rOld;
	if (dSize.x) {
		if ((Status & WC_ANCHOR_RIGHT) && !(Status & WC_ANCHOR_LEFT))
			rNew.x0 -= dSize.x;
		else
			rNew.x1 += dSize.x;
	}
	if (dSize.y) {
		if ((Status & WC_ANCHOR_BOTTOM) && !(Status & WC_ANCHOR_TOP))
			rNew.y0 -= dSize.y;
		else
			rNew.y1 += dSize.y;
	}
	rect = rNew;
	InvalidateArea(rOld | rNew);
	rNew -= rOld;
	_UpdateChildPositions(rNew);
	rInvalid &= rect;
	SendMessage(WM_SIZE);
}
#pragma endregion

#pragma region ScrollBar
#include "ScrollBar.h"
WObj *WM_GetScrollPartner(WObj *pScroll) {
	uint16_t Id = pScroll->ID();
	if (Id == GUI_ID_HSCROLL)
		Id = GUI_ID_VSCROLL;
	else if (Id == GUI_ID_VSCROLL)
		Id = GUI_ID_HSCROLL;
	return pScroll->Parent()->DialogItem(Id);
}

static void _SetScrollbar(WObj *pWin, bool bEnable, uint16_t Id) {
	auto pBar = pWin->DialogItem(Id);
	if (bEnable) {
		if (!pBar)
			new ScrollBar(pWin, Id == GUI_ID_VSCROLL ? SCROLLBAR_CF_VERTICAL : 0);
	}
	else if (pBar) {
		pBar->Visible(false);
		pBar->Delete();
	}
}
void WObj::ScrollBarV(bool bEnable) { _SetScrollbar(this, bEnable, GUI_ID_VSCROLL); }
void WObj::ScrollBarH(bool bEnable) { _SetScrollbar(this, bEnable, GUI_ID_HSCROLL); }
#pragma endregion

#pragma region Touch

WObj::CriticalHandles WObj::CriticalHandles::Modal;
WObj::CriticalHandles WObj::CriticalHandles::Last;

bool WObj::_IsInModalArea() {
	if (!CriticalHandles::Modal.pWin)
		return true;
	if (CriticalHandles::Modal.pWin == this)
		return true;
	return WM__IsAncestor(this, CriticalHandles::Modal.pWin);
}

void WObj::_SendMessage(WM_MSG *pMsg) {
	pMsg->pWin = this;
	if (cb)
		cb(pMsg);
	else
		DefCallback(pMsg);
}
void WObj::_SendMessageIfEnabled(WM_MSG *pMsg) {
	if (Enable())
		_SendMessage(pMsg);
}
void WObj::SendMessage(int msgid) {
	if (!cb) return;
	WM_MSG msg = { 0 };
	msg.pWin = this;
	msg.msgid = msgid;
	cb(&msg);
}
void WObj::_SendPIDMessage(WM_MSG *pMsg) {
	WM_MSG msg = *pMsg;
	_SendMessageIfEnabled(&msg);
	msg.pWinSrc = this;
	msg.msgid = WM_TOUCH_CHILD;
	for (auto i = pParent; IsWindow(i); i = i->pParent) {
		msg.data = (size_t)pMsg;
		i->_SendMessageIfEnabled(&msg);
	}
}
void WObj::_SendTouchMessage(WM_MSG *pMsg) {
	if (auto pState = (PidState *)pMsg->data)
		*pState -= rect.left_top();
	_SendPIDMessage(pMsg);
}

bool WObj::HandlePID() {
	WM_MSG msg;
	CriticalHandles CHWin;
	PidState State, StateNew;
	GUI_PID_GetState(&StateNew);
	if (WM_PID__StateLast == StateNew)
		return false;
	bool r = false;
	CHWin.Add();
	GUI.Cursor.Position(StateNew);
	CHWin.pWin = pWinCapture ? pWinCapture : WObj::ScreenToWin(StateNew);
	if (CHWin.pWin->_IsInModalArea()) {
		if (WM_PID__StateLast.Pressed != StateNew.Pressed && CHWin.pWin) {
			auto pWin = CHWin.pWin;
			WM_PID_STATE_CHANGED_INFO Info;
			Info.State = StateNew.Pressed;
			Info.StatePrev = WM_PID__StateLast.Pressed;
			Info.x = StateNew.x - pWin->rect.x0;
			Info.y = StateNew.y - pWin->rect.y0;
			msg.data = (size_t)&Info;
			msg.msgid = WM_PID_STATE_CHANGED;
			CHWin.pWin->_SendMessageIfEnabled(&msg);
		}
		if (WM_PID__StateLast.Pressed | StateNew.Pressed) {    /* Only if pressed or just released */
			msg.msgid = WM_TOUCH;
			r = true;
			if (CriticalHandles::Last.pWin != CHWin.pWin)
				if (CriticalHandles::Last.pWin != nullptr) {
					if (StateNew.Pressed)
						msg.data = 0;
					else {
						State.x = WM_PID__StateLast.x;
						State.y = WM_PID__StateLast.y;
						State.Pressed = 0;
						msg.data = (size_t)&State;
					}
					CriticalHandles::Last.pWin->_SendTouchMessage(&msg);
					CriticalHandles::Last.pWin = nullptr;
				}
			if (CHWin.pWin) {
				State = StateNew;
				if (State.Pressed)
					CriticalHandles::Last.pWin = CHWin.pWin;
				else {
					if (bCaptureAutoRelease)
						CaptureRelease();
					CriticalHandles::Last.pWin = nullptr;
				}
				msg.data = (size_t)&State;
				CHWin.pWin->_SendTouchMessage(&msg);
			}
		}
		else if (CHWin.pWin)
			if (CHWin.pWin->Enable()) {
				State = StateNew;
				msg.msgid = WM_MOUSEOVER;
				msg.data = (size_t)&State;
				CHWin.pWin->_SendTouchMessage(&msg);
			}
	}
	WM_PID__StateLast = StateNew;
	CHWin.Remove();
	return r;
}
#pragma endregion

#pragma region Dialog
void WObj::DialogEnd(int r) {
	if (auto pStatus = DialogStatusPtr()) {
		pStatus->ReturnValue = r;
		pStatus->Done = 1;
	}
	Delete();
}
int WObj::DialogExec() {
	WM_DIALOG_STATUS Status;
	DialogStatusPtr(&Status);
	while (!Status.Done)
		WObj::Exec();
	return Status.ReturnValue;
}

#include "Button.h"
#include "CheckBox.h"
#include "DropDown.h"
#include "Edit.h"
#include "Header.h"
#include "ListBox.h"
#include "ListView.h"
#include "Menu.h"
#include "MultiEdit.h"
#include "MultiPage.h"
#include "ProgBar.h"
#include "Radio.h"
#include "ScrollBar.h"
#include "Slider.h"
#include "Static.h"
#include "Frame.h"

WObj *WM_CREATESTRUCT::Create() const {
	switch (Class) {
		case WCLS_BUTTON:
			return new Button(*this);
		case WCLS_CHECKBOX:
			return new CheckBox(*this);
		//case WCLS_DROPDOWN:
		//	return new Dropdown(*this.x, *this.y, *this.xsize, *this.ysize, *this.pParent, *this.Flags);
		//case WCLS_EDIT:
		//	return new Edit(*this.x, *this.y, *this.xsize, *this.ysize, *this.pParent, *this.Flags);
		//case WCLS_HEADER:
		//	return new Header(*this.x, *this.y, *this.xsize, *this.ysize, *this.pParent, *this.Flags);
		case WCLS_LISTBOX:
			return new ListBox(*this);
		//case WCLS_LISTVIEW:
		//	return new ListView(*this.x, *this.y, *this.xsize, *this.ysize, *this.pParent, *this.Flags);
		//case WCLS_MENU:
		//	return new Menu(*this.x, *this.y, *this.xsize, *this.ysize, *this.pParent, *this.Flags);
		//case WCLS_MULTIEDIT:
		//	return new MultiEdit(*this.x, *this.y, *this.xsize, *this.ysize, *this.pParent, *this.Flags);
		//case WCLS_MULTIPAGE:
		//	return new MultiPage(*this.x, *this.y, *this.xsize, *this.ysize, *this.pParent, *this.Flags);
		//case WCLS_PROGBAR:
		//	return new ProgBar(*this);
		case WCLS_RADIO:
			return new Radio(*this);
		case WCLS_SCROLLBAR:
			return new ScrollBar(*this);
		case WCLS_SLIDER:
			return new Slider(*this);
		case WCLS_STATIC:
			return new Static(*this);
		case WCLS_FRAME:
			return new Frame(*this);
		default:
			break;
	}
	return nullptr;
}

WObj *WM_CREATESTRUCT::CreateDialog(WM_CB *cb, WObj *pParent, int x0, int y0) const {
	auto wc0 = *this;
	wc0.pParent = pParent;
	wc0.Para = (uint64_t)cb;
	auto pDialog = wc0.Create();
	auto pDialogClient = pDialog->Client();
	for (auto pWC = this + 1; pWC->Class != WCLS_EOF; ++pWC) {
		auto wc = *pWC;
		wc.pParent = pDialogClient;
		if (auto pChild = wc.Create())
			pChild->Visible(true);
	}
	pDialog->Visible(true);
	pDialogClient->Visible(true);
	pDialog->Focus();
	pDialog->FocusNextChild();
	pDialogClient->SendMessage(WM_INIT_DIALOG);
	return pDialog;
}
int WM_CREATESTRUCT::DialogBox(WM_CB *cb, WObj *pParent, int x0, int y0) const {
	return CreateDialog(cb, pParent, x0, y0)->DialogExec();
}
#pragma endregion

#pragma region Widget

bool Widget::HandleActive(WM_MSG *pMsg) {
	switch (pMsg->msgid) {
		case WM_WIDGET_SET_EFFECT: {
			auto Diff = EffectSize();
			pEffect = (const EffectItf *)pMsg->data;
			Diff -= EffectSize();
			_UpdateChildPositions({ -Diff, Diff });
			Invalidate();
			return false;
		}
		case WM_PID_STATE_CHANGED:
			if (State & WIDGET_STATE_FOCUSSABLE) {
				auto pInfo = (const WM_PID_STATE_CHANGED_INFO *)pMsg->data;
				if (pInfo->State)
					Focus();
			}
			return true;
		case WM_TOUCH_CHILD:
			if (auto pMsgOrg = (const WM_MSG *)pMsg->data)
				if (auto pState = (const PidState *)pMsgOrg->data)
					if (pState->Pressed) {
						BringToTop();
						return false;
					}
			return true;
		case WM_SET_ID:
			Id = (int)pMsg->data;
			return false;
		case WM_GET_ID:
			pMsg->data = Id;
			return false;
		case WM_SET_FOCUS: {
			int Notification;
			if (pMsg->data) {
				State |= WIDGET_STATE_FOCUS;
				Notification = WM_NOTIFICATION_GOT_FOCUS;
			}
			else {
				State &= ~WIDGET_STATE_FOCUS;
				Notification = WM_NOTIFICATION_LOST_FOCUS;
			}
			Invalidate();
			NotifyParent(Notification);
			pMsg->data = 0;
			return false;
		}
		case WM_GET_ACCEPT_FOCUS:
			pMsg->data = bool(State & WIDGET_STATE_FOCUSSABLE);
			return false;
		case WM_GET_INSIDE_RECT:
			*(SRect *)pMsg->data = ClientRect() / pEffect->EffectSize;
			return false;
	}
	return true;
}

void WIDGET__FillStringInRect(const char *pText, const SRect &rFill, const SRect &rTextMax, const SRect &rTextAct) {
	if (GUI.pClipRect_HL) {
		SRect r = rFill + GUI.off;
		if (!(GUI.pClipRect_HL && r))
			return;
	}
	if (!pText) {
		GUI.Clear(rFill);
		return;
	}
	if (!*pText) {
		GUI.Clear(rFill);
		return;
	}
	auto &&rText = rTextMax & rTextAct;
	/* Top */	 GUI.Clear({ rFill.x0, rFill.y0, rFill.x1, rText.y0 - 1 });
	/* Left */	 GUI.Clear({ rFill.x0, rText.y0, rText.x0 - 1, rText.y1 });
	/* Right */	 GUI.Clear({ rText.x1 + 1, rText.y0, rFill.x1, rText.y1 });
	/* Bottom */ GUI.Clear({ rFill.x0, rText.y1 + 1, rFill.x1, rFill.y1 });
	auto pOldClipRect = WObj::SetUserClipRect(&rTextMax);
	GUI.TextMode(DRAWMODE_NORMAL);
	GUI_DispStringAt(pText, rTextAct.left_top());
	WObj::SetUserClipRect(pOldClipRect);
}

void Widget::DrawVLine(int x, int y0, int y1) const {
	SRect r = { x, y0, x, y1 };
	if (State & WIDGET_STATE_VERTICAL)
		r = r.rot90().xmove(WObj::SizeX() - 1);
	GUI.Fill(r);
}
void Widget::Fill(SRect r) const {
	if (State & WIDGET_STATE_VERTICAL)
		r = r.rot90().xmove(WObj::SizeX() - 1);
	GUI.Fill(r);
}
void Widget::OutlineFocus(SRect r, int Dist) const {
	if (State & WIDGET_STATE_VERTICAL)
		r = r.rot90().xmove(WObj::SizeX() - 1);
	GUI.OutlineFocus(r, Dist);
}

bool Widget::_EffectRequiresRedraw(const SRect &r) const {
	return r > ClientToScreen(this->rInvalid) / EffectSize();
}

void Widget::DrawDown(SRect r) const {
	if (!pEffect)
		return;
	if (State & WIDGET_STATE_VERTICAL)
		r = r.rot90().xmove(WObj::SizeX() - 1);
	if (_EffectRequiresRedraw(r))
		pEffect->DrawDown(r);
}
void Widget::DrawUp(SRect r) const {
	if (!pEffect)
		return;
	if (State & WIDGET_STATE_VERTICAL)
		r = r.rot90().xmove(WObj::SizeX() - 1);
	if (_EffectRequiresRedraw(r))
		pEffect->DrawUp(r);
}

void Widget::Effect(const EffectItf *pEffect) {
	WM_MSG msg;
	msg.pWinSrc = 0;
	msg.msgid = WM_WIDGET_SET_EFFECT;
	msg.data = (size_t)pEffect;
	SendMessage(&msg);
}

#include "ScrollBar.h"

void Widget::ScrollStateH(const WM_SCROLL_STATE &s) {
	if (auto pScroll = ScrollBarH())
		pScroll->ScrollState(s);
}
void Widget::ScrollStateV(const WM_SCROLL_STATE &s) {
	if (auto pScroll = ScrollBarV())
		pScroll->ScrollState(s);
}

#pragma region Effects
const Widget::EffectItf Widget::EffectItf::None;

#pragma region Simple
class Effect_Simple : public Widget::EffectItf {
public:
	Effect_Simple() : Widget::EffectItf(1) {}
	void DrawUp(const SRect &r) const override {
		GUI.PenColor(RGB_BLACK);
		GUI.Outline(r); /* Draw rectangle around it */
	}
	void DrawDown(const SRect &r) const override {
		GUI.PenColor(RGB_BLACK);
		GUI.Outline(r); /* Draw rectangle around it */
	}
};
const Widget::EffectItf &&Widget::EffectItf::Simple = Effect_Simple();
#pragma endregion

#pragma region D3
class Effect_D3 : public Widget::EffectItf {
public:
	Effect_D3() : Widget::EffectItf(2) {}
	void DrawUp(const SRect &r) const override {
		GUI.PenColor(RGB_BLACK);
		GUI.Outline(r);
		GUI.PenColor(RGB_WHITE);
		GUI.DrawLineH(r.x0 + 1, r.y0 + 1, r.x1 - 2);
		GUI.DrawLineV(r.x0 + 1, r.y0 + 1, r.y1 - 2);
		GUI.PenColor(RGBC_GRAY(0x55));
		GUI.DrawLineH(r.x0 + 1, r.y1 - 1, r.x1 - 1);
		GUI.DrawLineV(r.x1 - 1, r.y0 + 1, r.y1 - 2);
	}
	void DrawDown(const SRect &r) const override {
		GUI.PenColor(RGBC_GRAY(0x80));
		GUI.DrawLineH(r.x0, r.y0, r.x1);
		GUI.DrawLineV(r.x0, r.y0 + 1, r.y1);
		GUI.PenColor(RGB_BLACK);
		GUI.DrawLineH(r.x0 + 1, r.y0 + 1, r.x1 - 1);
		GUI.DrawLineV(r.x0 + 1, r.y0 + 2, r.y1 - 1);
		GUI.PenColor(RGB_WHITE);
		GUI.DrawLineH(r.x0 + 1, r.y1, r.x1);
		GUI.DrawLineV(r.x1, r.y0 + 1, r.y1);
		GUI.PenColor(RGBC_GRAY(0xC0));
		GUI.DrawLineH(r.x0 + 2, r.y1 - 1, r.x1 - 1);
		GUI.DrawLineV(r.x1 - 1, r.y0 + 2, r.y1 - 1);
	}
};
const Widget::EffectItf &&Widget::EffectItf::D3 = Effect_D3();
#pragma endregion

#pragma region D3L1
class Effect_D3L1 : public Widget::EffectItf {
public:
	Effect_D3L1() : Widget::EffectItf(1) {}
	void DrawUp(const SRect &r) const override {
		GUI.PenColor(RGBC_GRAY(0xE7));
		GUI.DrawLineH(r.x0, r.y0, r.x1 - 1);
		GUI.DrawLineV(r.x0, r.y0 + 1, r.y1 - 1);
		GUI.PenColor(RGBC_GRAY(0x60));
		GUI.DrawLineH(r.x0, r.y1, r.x1);
		GUI.DrawLineV(r.x1, r.y0, r.y1 - 1);
	}
	void DrawDown(const SRect &r) const override {
		GUI.PenColor(RGBC_GRAY(0x60));
		GUI.DrawLineH(r.x0, r.y0, r.x1 - 1);
		GUI.DrawLineV(r.x0, r.y0 + 1, r.y1 - 1);
		GUI.PenColor(RGBC_GRAY(0xE7));
		GUI.DrawLineH(r.x0, r.y1, r.x1);
		GUI.DrawLineV(r.x1, r.y0, r.y1 - 1);
	}
};
const Widget::EffectItf &&Widget::EffectItf::D3L1 = Effect_D3L1();
#pragma endregion

#pragma region D3L2
class Effect_D3L2 : public Widget::EffectItf {
public:
	Effect_D3L2() : Widget::EffectItf(2) {}
	void DrawUp(const SRect &r) const override {
		GUI.PenColor(RGBC_GRAY(0xD0));
		GUI.DrawLineH(r.x0, r.y0, r.x1 - 1);
		GUI.DrawLineV(r.x0, r.y0 + 1, r.y1 - 1);
		GUI.PenColor(RGBC_GRAY(0xE7));
		GUI.DrawLineH(r.x0 + 1, r.y0 + 1, r.x1 - 2);
		GUI.DrawLineV(r.x0 + 1, r.y0 + 2, r.y1 - 2);
		GUI.PenColor(RGBC_GRAY(0x60));
		GUI.DrawLineH(r.x0, r.y1, r.x1);
		GUI.DrawLineV(r.x1, r.y0, r.y1 - 1);
		GUI.PenColor(RGBC_GRAY(0x9A));
		GUI.DrawLineH(r.x0 + 1, r.y1 - 1, r.x1 - 1);
		GUI.DrawLineV(r.x1 - 1, r.y0 + 1, r.y1 - 2);
	}
	void DrawDown(const SRect &r) const override {
		GUI.PenColor(RGBC_GRAY(0x9A));
		GUI.DrawLineH(r.x0, r.y0, r.x1 - 1);
		GUI.DrawLineV(r.x0, r.y0 + 1, r.y1 - 1);
		GUI.PenColor(RGBC_GRAY(0x60));
		GUI.DrawLineH(r.x0 + 1, r.y0 + 1, r.x1 - 2);
		GUI.DrawLineV(r.x0 + 1, r.y0 + 2, r.y1 - 2);
		GUI.PenColor(RGBC_GRAY(0xE7));
		GUI.DrawLineH(r.x0, r.y1, r.x1);
		GUI.DrawLineV(r.x1, r.y0, r.y1 - 1);
		GUI.PenColor(RGBC_GRAY(0xD0));
		GUI.DrawLineH(r.x0 + 1, r.y1 - 1, r.x1 - 1);
		GUI.DrawLineV(r.x1 - 1, r.y0 + 1, r.y1 - 2);
	}
};
const Widget::EffectItf &&Widget::EffectItf::D3L2 = Effect_D3L2();
#pragma endregion

const Widget::EffectItf *Widget::pEffectDefault = &Widget::EffectItf::D3L2;
#pragma endregion

#pragma endregion
