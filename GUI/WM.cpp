#include "WM.h"

#pragma region SCROLL_STATE
void SCROLL_STATE::Bound() {
	int Max = NumItems - PageSize;
	if (Max < 0) Max = 0;
	if (v < 0) v = 0;
	if (v > Max) v = Max;
}
int SCROLL_STATE::Pos(int Pos, int LowerDist, int UpperDist) {
	int vOld = v;
	auto PageSize = this->PageSize - 1;
	if (v < Pos - PageSize)
		v = Pos - PageSize + UpperDist;
	if (v > Pos)
		v = Pos - LowerDist;
	Bound();
	return v - vOld;
}
int SCROLL_STATE::Value(int v) {
	auto vOld = v;
	this->v = v;
	Bound();
	return v - vOld;
}
#pragma endregion

#pragma region WObj Basic
PWObj WObj::pWinFirst = nullptr;
uint16_t WObj::nWindows = 0;
PWObj WObj::pWinActive = nullptr;
uint16_t WObj::nInvalidWindows = 0;

#pragma region Window Chain
void WObj::_InsertWindowIntoList(PWObj pParent) {
	this->pParent = pParent;
	if (Popup()) pParent = Desktop();
	if (!pParent) return;
	pNext = nullptr;
	bool bStayOnTop = Status & WC_STAYONTOP;
	auto i = pParent->pFirstChild;
	if (!i) {
		pParent->pFirstChild = this;
		return;
	}
	if (!bStayOnTop) {
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
		if (!bStayOnTop) {
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
	if (auto pParent = Parent()) {
		auto i = pParent->pFirstChild;
		if (i == this)
			pParent->pFirstChild = i->pNext;
		else for (; i; i = i->pNext)
			if (i->pNext == this) {
				i->pNext = pNext;
				break;
			}
	}
}

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
#pragma endregion

WObj::WObj(const SRect &rc,
		   WM_CB cb,
		   PWObj pParent, uint16_t Id,
		   WM_CF Style, uint16_t ExStyles) :
	cb(cb),
	Status(Style), StatusEx(ExStyles),
	Id(Id) {
	if (!pParent)
		if (nWindows)
			pParent = pDesktop;
	if (pParent && !Popup()) {
		rsWin = pParent->Client2Screen(rc);
		if (rsWin.x1 <= rsWin.x0)
			rsWin.x1 = pParent->rsWin.x1;
		if (rsWin.y1 <= rsWin.y0)
			rsWin.y1 = pParent->rsWin.y1;
	}
	else
		rsWin = rc;
	++nWindows;
	_AddToLinList();
	_InsertWindowIntoList(pParent);
	if (Style & WC_ACTIVATE)
		Select();
	if (Style & WC_VISIBLE) {
		Status |= WC_VISIBLE;
		Invalidate();
	}
	SendMessage(WM_CREATE);
}

void WObj::Destroy() {
	if (!*this) return;
	pWinNextDraw = nullptr;
	if (pWinFocus == this)
		pWinFocus = nullptr;
	CaptureRelease();
	if (CriticalHandles::pFirst)
		CriticalHandles::pFirst->Check(this);
	NotifyOwner(WN_CHILD_DELETED);
	while (pFirstChild) {
		auto pNext = pFirstChild->pNext;
		pFirstChild->Destroy();
		pFirstChild = pNext;
	}
	SendMessage(WM_DELETE);
	_RemoveWindowFromList();
	_RemoveFromLinList();
	InvalidateArea(rsWin);
	if (Status & WC_ACTIVATE)
		--nInvalidWindows;
	GUI_MEM_Free(this);
	if (--nWindows) {
		pWinFirst->Select();
		return;
	}
	pWinFirst = pWinActive = pWinFocus = CaptureHandles::First.pWin = pDesktop = nullptr;
}
void WObj::Select() {
	pWinActive = this;
	GUI.ClipRect();
	GUI.Off(rsWin.left_top());
}
void WObj::BringToTop() {
	if (!pNext)
		return;
	if (!(Status & WC_STAYONTOP))
		if (pNext->Status & WC_STAYONTOP)
			return;
	_RemoveWindowFromList();
	_InsertWindowIntoList(Parent());
	InvalidateDescs();
}
void WObj::BringToBottom() {
	if (auto pPrev = PrevSibling()) {
		auto pParent = Parent();
		pPrev->pNext = pNext;
		pNext = pParent->pFirstChild;
		pParent->pFirstChild = this;
		InvalidateArea(rsWin);
	}
}

bool WObj::IsAncestor(PWObj pParent) const {
	if (!pParent)
		return false;
	for (auto iWin = this; iWin; iWin = iWin->Parent())
		if (iWin->Parent() == pParent)
			return true;
	return false;
}

#pragma region Paint
#pragma region Invalidate 
bool WObj::_ClipAtParentBorders(SRect &r) const {
	for (auto pWin = this; pWin->Status & WC_VISIBLE; pWin = pWin->Parent()) {
		r &= pWin->rsWin;
		if (!pWin->Parent())
			return pWin == WObj::Desktop();
	}
	return false;
}
void WObj::_Invalidate1Abs(SRect r) {
	if (!(Status & WC_VISIBLE))
		return;
	r &= rsWin;
	if (!r) return;
	if (Status & WC_ACTIVATE)
		rsInvalid |= r;
	else {
		rsInvalid = r;
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
#pragma endregion
uint8_t WObj::nPaintCallback = 0;
PWObj WObj::pWinNextDraw = nullptr;
void WObj::_Paint1() {
	if (!cb) return;
	if (!(Status & WC_VISIBLE))
		return;
	++nPaintCallback;
	auto rInvalidClient = Screen2Client(rsInvalid);
	auto GUI_Saved = GUI;
	if (WObj::IVR_Init(&rsInvalid)) do {
		SendMessage(WM_PAINT, rInvalidClient);
		GUI = GUI_Saved;
	} while (WObj::IVR_Next());
	--nPaintCallback;
}
bool WObj::_OnPaint() {
	if (!(Status & WC_ACTIVATE))
		return false;
	bool res = false;
	if (cb)
		if (_ClipAtParentBorders(rsInvalid)) {
			Select();
			_Paint1();
			res = true;
		}
	Status &= ~WC_ACTIVATE;
	--nInvalidWindows;
	return res;
}
bool WObj::PaintNext() {
	if (!nInvalidWindows)
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

#pragma region Resize & Move
void WObj::_MoveDescendents(Point d) {
	for (auto i = this; i; i = i->pNext) {
		i->rsWin += d;
		i->rsInvalid += d;
		if (auto pChild = i->pFirstChild)
			pChild->_MoveDescendents(d);
		i->SendMessage(WM_MOVED);
	}
}
SRect WObj::RectScreenAbs(const SRect &rsNew) {
	auto rsOld = rsWin;
	rsWin = rsNew;
	rsInvalid = rsNew;
	if (Status & WC_VISIBLE) {
		InvalidateArea(rsWin);
		InvalidateArea(rsOld);
	}
	return rsOld;
}
void WObj::RectScreen(const SRect &rsNew) {
	auto &&dRect = rsNew - RectScreenAbs(rsNew);
	if (auto dPos = dRect.left_top()) {
		if (pFirstChild)
			pFirstChild->_MoveDescendents(dRect.left_top());
		SendMessage(WM_MOVED);
	}
	if (dRect.size())
		SendMessage(WM_SIZED);
}
void WObj::Move(Point dPos) {
	if (!dPos) return;
	auto rOld = rsWin;
	rsWin += dPos;
	rsInvalid += dPos;
	if (pFirstChild)
		pFirstChild->_MoveDescendents(dPos);
	if (Status & WC_VISIBLE) {
		InvalidateArea(rsWin);
		InvalidateArea(rOld);
	}
	SendMessage(WM_MOVED);
}

void WObj::_UpdateChildPositions(SRect d) {
	for (auto pChild = pFirstChild; pChild; pChild = pChild->pNext) {
		SRect rNew = pChild->rsWin;
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
		pChild->Size(rNew.size());
		pChild->PositionScreen(rNew.left_top());
	}
}
void WObj::Resize(Point dSize) {
	if (!dSize) return;
	SRect rOld = rsWin, rNew = rOld;
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
	rsWin = rNew;
	InvalidateArea(rOld | rNew);
	rNew -= rOld;
	_UpdateChildPositions(rNew);
	rsInvalid &= rsWin;
	SendMessage(WM_SIZED);
}
#pragma endregion

#pragma region IVR
WObj::ContextIVR WObj::_ClipContext;
const SRect *WObj::pUserClip = nullptr;
const SRect *WObj::UserClip(const SRect *pRect) {
	auto pRectReturn = pUserClip;
	pUserClip = pRect;
	_ActivateClipRect();
	return pRectReturn;
}
void WObj::_ActivateClipRect() {
	GUI.ClipRect(
		pUserClip ?
		pWinActive->Client2Screen(*pUserClip) & _ClipContext.CurRect :
		_ClipContext.CurRect);
}
bool WObj::_FindNextIVR() {
	static auto _Findy1 = [](PWObj pWin, SRect &r) {
		for (; pWin; pWin = pWin->pNext)
			if (pWin->Status & WC_VISIBLE) {
				auto rWinClipped = pWin->rsWin;
				if (r && rWinClipped) {
					if (pWin->rsWin.y0 > r.y0) {
						if (rWinClipped.y0 - 1 < r.y1)
							r.y1 = rWinClipped.y0 - 1; /* Check upper border of window */
					}
					else if (rWinClipped.y1 < r.y1)
						r.y1 = rWinClipped.y1; /* Check lower border of window */
				}
			}
	};
	static auto _Findx0 = [](PWObj pWin, SRect &r) {
		bool t = false;
		for (; pWin; pWin = pWin->pNext)
			if (pWin->Status & WC_VISIBLE) {
				auto rWinClipped = pWin->rsWin;
				if (r && rWinClipped) {
					r.x0 = rWinClipped.x1 + 1;
					t = true;
				}
			}
		return t;
	};
	static auto _Findx1 = [](PWObj pWin, SRect &r) {
		for (; pWin; pWin = pWin->pNext)
			if (pWin->Status & WC_VISIBLE) {
				auto rWinClipped = pWin->rsWin;
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
		for (auto pParent = pWinActive; pParent; pParent = pParent->Parent())
			_Findy1(pParent->pNext, r);
		_Findy1(pWinActive->pFirstChild, r);
	}
Find_x0:
	r.x1 = r.x0;
	for (auto pParent = pWinActive; pParent; pParent = pParent->Parent())
		if (_Findx0(pParent->pNext, r))
			goto Find_x0;
	if (_Findx0(pWinActive->pFirstChild, r))
		goto Find_x0;
	r.x1 = _ClipContext.rClient.x1;
	if (r.x1 < r.x0) {
		_ClipContext.CurRect = r;
		goto NextStripe;
	}
	for (auto pParent = pWinActive; pParent; pParent = pParent->Parent())
		_Findx1(pParent->pNext, r);
	_Findx1(pWinActive->pFirstChild, r);
	if (_ClipContext.Cnt > 200)
		return false;
	_ClipContext.CurRect = r;
	return true;
}
bool WObj::IVR_Next() {
	static bool bCursorHidden = true;
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
	if (++_ClipContext.EntranceCnt > 1)
		return true;
	_ClipContext.Cnt = -1;
	SRect r;
	if (nPaintCallback)
		r = pWinActive->rsInvalid;
	else if (pWinActive->Status & WC_VISIBLE)
		r = pWinActive->rsWin;
	else {
		--_ClipContext.EntranceCnt;
		return false;
	}
	if (pMaxRect)
		r &= *pMaxRect;
	if (pUserClip)
		r &= pWinActive->Client2Screen(*pUserClip);
	if (!pWinActive->_ClipAtParentBorders(r)) {
		--_ClipContext.EntranceCnt;
		return false;
	}
	_ClipContext.rClient = r;
	return IVR_Next();
}
#pragma endregion

PWObj WObj::FindOnScreen(Point Pos, PWObj pStop, PWObj pWin) {
	if (!pWin->Visible()) return nullptr;
	if (pWin->RectScreen() > Pos) return nullptr;
	for (auto pChild = pWin->pFirstChild;
		 pChild && pChild != pStop;
		 pChild = pChild->pNext)
		if (auto pHit = FindOnScreen(Pos, pStop, pChild))
			pWin = pHit;
	return pWin;
}

void WObj::Init() {
	if (pDesktop) return;
	pWinActive = pDesktop = new WObj(
		GUI.Rect(),
		DesktopCallback,
		nullptr, 0,
		WC_VISIBLE);
	pDesktop->Invalidate();
	CriticalHandles::Modal.Add();
	CriticalHandles::Last.Add();
	pDesktop->Select();
}
void WObj::Deinit() {
	if (!pDesktop) return;
	pDesktop->Destroy();
}

bool WObj::Exec() {
	if (HandleMouse())
		return true;
	if (HandleKey())
		return true;
	if (HandleRect())
		return true;
	if (PaintNext())
		return true;
	return false;
}

WM_RESULT WObj::DefCallback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	switch (MsgId) {
		case WM_PAINT:
			GUI.BkColor(pWin->BkColorProp(pWin->BkColor()));
			GUI.Clear();
			break;
		case WM_SET_ID:
			pWin->Id = Param;
			return true;
		case WM_GET_ID:
			return pWin->Id;
		case WM_FOCUSSABLE:
			return pWin->Status & WC_FOCUSSABLE;
		case WM_GET_SERVE_RECT:
			return pWin->InsideRect();
		case WM_GET_INSIDE_RECT:
			return pWin->ClientRect();
		case WM_GET_CLIENT_WINDOW:
			return pWin;
		case WM_KEY:
			if (!pWin->Parent())
				break;
			if (!pWin->SendOwnerMessage(MsgId, Param)) {
				KEY_STATE State = Param;
				if (State.PressedCnt <= 0)
					break;
				switch (State.Key) {
					case GUI_KEY_TAB:
						if (auto pParent = pWin->Parent())
							pParent->FocusNextChild();
						break;
				}
			}
			break;
		case WM_GET_BKCOLOR:
			return RGB_INVALID_COLOR;
		case WM_NOTIFY_ENABLE:
			pWin->Invalidate();
			return 0;
		case WM_GET_CLASS:
			return "Window";
	}
	return 0;
}

bool WObj::OnKey(KEY_STATE State) {
	if (pWinFocus)
		return pWinFocus->_SendMessage(WM_KEY, State);
	return true;
}

PWObj WObj::pDesktop = nullptr;
RGBC WObj::clDesktop = RGB_GRAY;
WM_RESULT WObj::DesktopCallback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	switch (MsgId) {
		case WM_KEY:
			return false;
		case WM_PAINT:
			if (clDesktop != RGB_INVALID_COLOR) {
				GUI.BkColor(clDesktop);
				GUI.Clear();
			}
			return 0;
		case WM_DELETE:
			pDesktop = nullptr;
			return 0;
		case WM_GET_BKCOLOR:
			return clDesktop;
	}
	return DefCallback(pWin, MsgId, Param, pSrc);
}

#pragma region Focus
PWObj WObj::pWinFocus = nullptr;
PWObj WObj::Focus() {
	if (this == pWinFocus)
		return pWinFocus;
	if (!Focussable())
		return nullptr;
	if (pWinFocus)
		pWinFocus->SendMessage(WM_FOCUS, false);
	pWinFocus = this;
	if (SendMessage(WM_FOCUS, true))
		return this;
	FOCUS_CHANGED_STATE Info{ pWinFocus, this };
	for (auto pWin = this; (pWin = pWin->Parent());)
		pWin->SendMessage(WM_NOTIFY_CHILD_HAS_FOCUS, &Info);
	if (auto pWin = Info.pOld)
		if (*pWin)
			while ((pWin = pWin->Parent()) != nullptr)
				pWin->SendMessage(WM_NOTIFY_CHILD_HAS_FOCUS, &Info);
	return this;
}
PWObj WObj::FocussedChild() {
	if (pWinFocus)
		if (pWinFocus->IsChildOf(this))
			return pWinFocus;
	return nullptr;
}

PWObj WObj::NextChild(PWObj pChild) {
	PWObj pObj = nullptr;
	if (pChild)
		pObj = pChild->pNext;
	if (!pObj)
		pObj = pFirstChild;
	return pObj != pChild ? pObj : nullptr;
}
PWObj WObj::FocusNextChild() {
	auto pChild = NextChild(FocussedChild());
	auto pWin = pChild;
	while (pWin) {
		if (pWin->Focussable())
			break;
		pWin = NextChild(pWin);
		if (pWin == pChild)
			break;
	}
	return pWin ? pWin->Focus() : nullptr;
}
#pragma endregion

#pragma region Capture
WObj::CaptureHandles WObj::CaptureHandles::First;
void WObj::Capture(bool bAutoRelease) {
	for (auto i = &CaptureHandles::First; i; i = i->pPrev)
		if (i->pWin == this) {
			i->bAutoRelease = bAutoRelease;
			return;
		}
	CaptureHandles *pPrev = nullptr;
	if (CaptureHandles::First)
		pPrev = CaptureHandles::First.pPrev = new CaptureHandles(CaptureHandles::First);
	CaptureHandles::First = CaptureHandles{ this, GUI.Cursor, bAutoRelease, pPrev };
}
bool WObj::CaptureRelease() {
	CaptureHandles *pPrev, *pThis;
	for (pPrev = nullptr, 
		 pThis = &CaptureHandles::First;
		 pThis;
		 pPrev = pThis,
		 pThis = pThis->pPrev)
		if (pThis->pWin == this)
			break;
	if (!pThis) return false;
	if (pThis->pCursor)
		GUI.Cursor(pThis->pCursor);
	if (pThis->pWin)
		pThis->pWin->SendMessage(WM_CAPTURE_RELEASED);
	if (!pPrev) {
		CaptureHandles::First.pWin = nullptr;
		if (auto pPrev = CaptureHandles::First.pPrev) {
			CaptureHandles::First = *pPrev;
			delete pPrev;
		}
	}
	else {
		pPrev = pPrev->pPrev;

	}
	return true;
}
void WObj::CaptureMove(Point Pos, int MinVisibility) {
	CaptureHandles *i;
	for (i = &CaptureHandles::First; i; i = i->pPrev)
		if (i->pWin == this)
			break;
	if (!i) {
		Capture(true);
		CaptureHandles::First.Pos = Pos;
	}
	auto &&d = Pos - CaptureHandles::First.Pos;
	if (!MinVisibility) {
		Move(d);
		return;
	}
	auto &&rCapture = RectScreen() + d,
		&&rParent = Parent()->RectScreen();
	rParent /= MinVisibility;
	if (rCapture && rParent)
		Move(d);
}
bool WObj::Captured() const {
	for (auto i = &CaptureHandles::First; i; i = i->pPrev)
		if (i->pWin == this)
			return true;
	return false;
}
PWObj WObj::CapturedWindow() {
	return CaptureHandles::First.pWin;
}
#pragma endregion

#pragma region Mouse
bool WObj::_IsInModalArea() {
	if (!CriticalHandles::Modal.pWin)
		return true;
	if (CriticalHandles::Modal.pWin == this)
		return true;
	return IsAncestor(CriticalHandles::Modal.pWin);
}

WM_RESULT WObj::_SendMessage(int MsgId, WM_PARAM Param, PWObj pSrc) {
	if (cb)
		return cb(this, MsgId, Param, pSrc);
	return DefCallback(this, MsgId, Param, pSrc);
}
void WObj::_SendMessageIfEnabled(int MsgId, WM_PARAM Param, PWObj pSrc) {
	if (Enable())
		_SendMessage(MsgId, Param, pSrc);
}
void WObj::_SendMouseMessage(int MsgId, MOUSE_STATE State) {
	if (State) State -= rsWin.left_top();
	_SendMessageIfEnabled(MsgId, State);
	for (auto i = Parent(); i; i = i->Parent())
		i->_SendMessageIfEnabled(WM_MOUSE_CHILD, State, this);
}

WObj::CriticalHandles
*WObj::CriticalHandles::pFirst = nullptr,
WObj::CriticalHandles::Last,
WObj::CriticalHandles::Modal;
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
void WObj::CriticalHandles::Check(PWObj pWin) {
	for (auto pCH = this; pCH; pCH = pCH->pNext)
		if (pCH->pWin == pWin)
			pCH->pWin = nullptr;
}

#include "GUI_X.h"

WM_RESULT WObj::SendMessage(int MsgId, WM_PARAM Param, PWObj pSrc) {
	if (cb)
		return cb(this, MsgId, Param, pSrc);
	return 0;
}
WM_RESULT WObj::SendOwnerMessage(int MsgId, WM_PARAM Param) {
	if (auto pOwner = Owner())
		return pOwner->SendMessage(MsgId, Param, this);
	return 0;
}

bool WObj::HandleRect() {
	if (GUI.RectNA())
		return false;
	if (pDesktop)
		pDesktop->RectScreen(GUI.RectN());
	return true;
}
bool WObj::HandleKey() {
	if (GUI.KeyNA())
		return false;
	WObj::OnKey(GUI.Key());
	GUI.KeyNext();
	return true;
}
bool WObj::HandleMouse() {
	static auto _HandleMouse = []() {
		auto StateOld = GUI.MousePrev(), StateNow = GUI.Mouse();
		GUI.Cursor.Position(StateNow);
		CriticalHandles CHWin = CaptureHandles::First.pWin ? CaptureHandles::First.pWin : WObj::FindOnScreen(StateNow);
		if (!CHWin.pWin) return false;
		if (!CHWin.pWin->_IsInModalArea()) return false;
		if (StateOld.Pressed != StateNow.Pressed && CHWin.pWin)
			CHWin.pWin->_SendMessageIfEnabled(
				WM_MOUSE_CHANGED,
				MOUSE_CHANGED_STATE{
					CHWin.pWin->Screen2Client(StateNow),
					StateNow.Pressed, StateOld.Pressed });
		if (StateOld.Pressed | StateNow.Pressed) { /* Only if pressed or just released */
			MOUSE_STATE State;
			if (CriticalHandles::Last.pWin != CHWin.pWin)
				if (CriticalHandles::Last.pWin) {
					MOUSE_STATE State;
					if (!StateNow.Pressed)
						State = MOUSE_STATE{ StateOld, 0 };
					CriticalHandles::Last.pWin->_SendMouseMessage(WM_MOUSE, State);
					CriticalHandles::Last.pWin = nullptr;
				}
			if (CHWin.pWin) {
				State = StateNow;
				if (State.Pressed)
					CriticalHandles::Last.pWin = CHWin.pWin;
				else if (CaptureHandles::First)
					if (CaptureHandles::First.bAutoRelease)
						CaptureHandles::First.pWin->CaptureRelease();
				CHWin.pWin->_SendMouseMessage(WM_MOUSE, State);
			}
			return true;
		}
		if (CHWin.pWin->Enable())
			CHWin.pWin->_SendMouseMessage(WM_MOUSE_OVER, StateNow);
		return false;
	};
	if (GUI.MouseNA())
		return false;
	auto res = _HandleMouse();
	GUI.MouseNext();
	return res;
}

#pragma endregion

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
	this->Status = Status;
	this->SendMessage(WM_NOTIFY_ENABLE, bEnable);
}
void WObj::Focussable(bool bFocussable) {
	auto Status = this->Status;
	if (bFocussable)
		Status |= WC_FOCUSSABLE;
	else
		Status &= ~WC_FOCUSSABLE;
	if (this->Status == Status)
		return;
	this->Status = Status;
	Invalidate();
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
		InvalidateArea(rsWin);
}
void WObj::Parent(PWObj pParent, Point ptcPosition) {
	if (pParent == this)
		return;
	if (pParent == this->Parent())
		return;
	/* Detach */
	_RemoveWindowFromList();
	InvalidateArea(rsWin);
	/* Attach */
	_InsertWindowIntoList(pParent);
	Position(ptcPosition);
}
void WObj::Owner(PWObj pOwner) {
	_RemoveWindowFromList();
	auto Status = this->Status;
	if (pOwner)
		Status |= WC_POPUP;
	else
		Status &= ~WC_POPUP;
	this->Status = Status;
	Parent(pOwner);
}

PWObj WObj::LastSibling() {
	for (auto pWin = this; pWin; pWin = pWin->pNext)
		if (!pWin->pNext)
			return pWin;
	return nullptr;
}
PWObj WObj::PrevSibling() {
	for (auto pObj = FirstSibling(); pObj; pObj = pObj->pNext)
		if (pObj == this)
			return nullptr;
		else if (pObj->pNext == this)
			return pObj;
	return nullptr;
}
#pragma endregion

#pragma region Dialog
void WObj::DialogEnd(int r) {
	if (auto pStatus = DialogStatusPtr()) {
		pStatus->ReturnValue = r;
		pStatus->Done = 1;
	}
}
int WObj::DialogExec() {
	DIALOG_STATE Status;
	DialogStatusPtr(&Status);
	while (!Status.Done)
		WObj::Exec();
	Destroy();
	return Status.ReturnValue;
}

#include "Widgets/Widgets.inl"

GUI_PCSTR ClassNames[]{
	"",
	"BUTTON",
	"CHECKBOX",
	"DROPDOWN",
	"EDIT",
	"HEADER",
	"LISTBOX",
	"LISTVIEW",
	"MENU",
	"MULTIEDIT",
	"MULTIPAGE",
	"PROGBAR",
	"RADIO",
	"SCROLLBAR",
	"SLIDER",
	"STATIC",
	"FRAME"
};

PWObj WM_CREATESTRUCT::Create() const {
	switch (Class) {
		case WCLS_BUTTON:
			return new Button(*this);
		case WCLS_CHECKBOX:
			return new CheckBox(*this);
		case WCLS_DROPDOWN:
			return new DropDown(*this);
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
		case WCLS_PROGBAR:
			return new ProgBar(*this);
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
PWObj WM_CREATESTRUCT::CreateDialog(WM_HANDLE_CB hcb, Point Pos, PWObj pParent) const {
	auto wc0 = *this;
	wc0.pParent = pParent;
	if (hcb)
		wc0.Para = (uint64_t)hcb;
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
int WM_CREATESTRUCT::DialogBox(WM_HANDLE_CB hcb, Point Pos, PWObj pParent) const {
	return CreateDialog(hcb, Pos, pParent)->DialogExec();
}
PWObj WObj::DialogItem(uint16_t Id) {
	for (auto pObj = pFirstChild; pObj; pObj = pObj->pNext)
		if (pObj->ID() == Id)
			return pObj;
		else if (auto pItem = pObj->DialogItem(Id))
			return pItem;
	return nullptr;
}
#pragma endregion

#pragma region ScrollBar
static ScrollBar *_SetScrollBar(PWObj pWin, bool bEnable, uint16_t Id) {
	auto pBar = (ScrollBar *)pWin->DialogItem(Id);
	if (bEnable) {
		if (!pBar)
			pBar = new ScrollBar(pWin, Id);
		pBar->Visible(true);
		return pBar;
	}
	else if (pBar) {
		pBar->Visible(false);
		pBar->Destroy();
	}
	return nullptr;
}
void WObj::ScrollBarV(bool bEnable) { _SetScrollBar(this, bEnable, GUI_ID_VSCROLL); }
void WObj::ScrollBarH(bool bEnable) { _SetScrollBar(this, bEnable, GUI_ID_HSCROLL); }

static void _SetScrollState(PWObj pWin, SCROLL_STATE s, uint16_t Id) {
	auto pScroll = (ScrollBar *)pWin->DialogItem(Id);
	if (pWin->Styles() & WC_AUTOSCROLL_V) {
		if (!pScroll)
			pScroll = new ScrollBar(pWin, Id);
		pScroll->Visible(s.NeedScroll());
	}
	if (pScroll)
		pScroll->ScrollState(s);
}
void WObj::ScrollStateH(SCROLL_STATE s) {
	_SetScrollState(this, s, GUI_ID_HSCROLL);
}
SCROLL_STATE WObj::ScrollStateH() const {
	if (auto pScroll = const_cast<WObj *>(this)->ScrollBarH())
		return pScroll->ScrollState();
	return {};
}
void WObj::ScrollStateV(SCROLL_STATE s) {
	_SetScrollState(this, s, GUI_ID_VSCROLL);
}
SCROLL_STATE WObj::ScrollStateV() const {
	if (auto pScroll = const_cast<PWObj>(this)->ScrollBarV())
		return pScroll->ScrollState();
	return {};
}
#pragma endregion

#pragma endregion

#pragma region Widget
bool Widget::HandleActive(int MsgId, WM_PARAM &Param) {
	switch (MsgId) {
		case WM_WIDGET_SET_EFFECT: {
			auto Diff = EffectSize();
			pEffect = Param;
			Diff -= EffectSize();
			_UpdateChildPositions({ -Diff, Diff });
			Invalidate();
			return false;
		}
		case WM_MOUSE_CHANGED: {
			MOUSE_CHANGED_STATE State = Param;
			if (State.Pressed)
				Focus();
			return true;
		}
		case WM_MOUSE_CHILD:
			if (MOUSE_STATE State = Param)
				if (State.Pressed) {
					BringToTop();
					return false;
				}
			return true;
		case WM_FOCUS: {
			int Notification;
			if (Param) 
				Notification = WN_GOT_FOCUS;
			else
				Notification = WN_LOST_FOCUS;
			Invalidate();
			NotifyOwner(Notification);
			Param = false;
			return false;
		}
		case WM_GET_INSIDE_RECT: {
			auto &&rInside = WObj::ClientRect() / EffectSize();
			if (auto pBarV = DialogItem(GUI_ID_VSCROLL)) {
				auto Flags = pBarV->Styles();
				if ((Flags & WC_ANCHOR_RIGHT) && (Flags & WC_VISIBLE))
					rInside.x1 = pBarV->Position().x;
			}
			if (auto pBarH = DialogItem(GUI_ID_HSCROLL)) {
				auto Flags = pBarH->Styles();
				if ((Flags & WC_ANCHOR_BOTTOM) && (Flags & WC_VISIBLE))
					rInside.y1 = pBarH->Position().y;
			}
			Param = rInside;
			return false;
		}
	}
	return true;
}
WM_PARAM Widget::DefCallback(Widget *pWidget, int MsgId, WM_PARAM Param, PWObj pSrc) {
	if (!pWidget->HandleActive(MsgId, Param))
		return Param;
	return WObj::DefCallback(pWidget, MsgId, Param, pSrc);
}

void Widget::DrawVLine(int x, int y0, int y1) const {
	SRect r = { x, y0, x, y1 };
	if (StatusEx & WC_EX_VERTICAL)
		r = r.rot90().xmove(WObj::SizeX() - 1);
	GUI.Fill(r);
}
void Widget::Fill(SRect r) const {
	if (StatusEx & WC_EX_VERTICAL)
		r = r.rot90().xmove(WObj::SizeX() - 1);
	GUI.Fill(r);
}
void Widget::DrawFocus(SRect r, int Dist) const {
	if (StatusEx & WC_EX_VERTICAL)
		r = r.rot90().xmove(WObj::SizeX() - 1);
	GUI.DrawFocus(r, Dist);
}

bool Widget::_EffectRequiresRedraw(const SRect &r) const {
	return r > Client2Screen(InvalidRect()) / EffectSize();
}

void Widget::DrawDown(SRect r) const {
	if (!pEffect)
		return;
	if (StatusEx & WC_EX_VERTICAL)
		r = r.rot90().xmove(WObj::SizeX() - 1);
	if (_EffectRequiresRedraw(r))
		pEffect->DrawDown(r);
}
void Widget::DrawUp(SRect r) const {
	if (!pEffect)
		return;
	if (StatusEx & WC_EX_VERTICAL)
		r = r.rot90().xmove(WObj::SizeX() - 1);
	if (_EffectRequiresRedraw(r))
		pEffect->DrawUp(r);
}

#pragma region Effects
class Effect_Simple : public Widget::EffectItf {
public:
	Effect_Simple() : Widget::EffectItf(1) {}
	void DrawUp(const SRect &r) const override {
		GUI.PenColor(RGB_BLACK);
		GUI.Outline(r);
	}
	void DrawDown(const SRect &r) const override {
		GUI.PenColor(RGB_BLACK);
		GUI.Outline(r);
	}
};
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
const Widget::EffectItf
	  Widget::EffectItf::None,
	&&Widget::EffectItf::Simple = Effect_Simple(),
	&&Widget::EffectItf::D3		= Effect_D3(),
	&&Widget::EffectItf::D3L1	= Effect_D3L1(),
	&&Widget::EffectItf::D3L2	= Effect_D3L2(),
	 *Widget::pEffectDefault = &Widget::EffectItf::D3L2;
#pragma endregion

#pragma endregion
