#define WM_C
#include "WM.h"
#include "SCROLLBAR.h"

uint8_t WM_IsActive;
uint8_t       WM__PaintCallbackCnt;      /* Public for assertions only */
PidState WM_PID__StateLast;
static WObj *NextDrawWin;

void WObj::_InsertWindowIntoList(WObj *pParent) {
	if (!pParent) return;
	pNext = nullptr;
	this->pParent = pParent;
	bool bOnTop = Status & WC_STAYONTOP;
	WObj *i = pParent->pFirstChild;
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

static void _CheckCriticalHandles(WObj *pWin) {
	WM_CRITICAL_HANDLE *pCH;
	for (pCH = WM__pFirstCriticalHandle; pCH; pCH = pCH->pNext)
		if (pCH->pWin == pWin)
			pCH->pWin = nullptr;
}

static void _SetClipRectUserIntersect(const SRect &rSrc) {
	if (GUI.pUserClipRect) {
		SRect r = *GUI.pUserClipRect;
		WM__Client2Screen(GUI.pWndActive, &r);
		r &= rSrc;
		GUI_LCD_SetClipRectEx(r);
	}
	else
		GUI_LCD_SetClipRectEx(rSrc);
}
bool WM__ClipAtParentBorders(SRect *pRect, WObj *pWin) {
	for (;;) {
		if ((pWin->Status & WC_VISIBLE) == 0)
			return false;
		*pRect &= pWin->rect;
		if (!pWin->pParent)
			break;
		pWin = pWin->pParent;
	}
	return WM__ahDesktopWin == pWin;
}

void WObj::Detach() {
	if (!pParent) return;
	_RemoveWindowFromList();
	InvalidateArea(rect);
	Move(-pParent->rect.left_top());
	pParent = nullptr;
}

void WM__Client2Screen(const WObj *pWin, SRect *pRect) {
	*pRect += pWin->rect.left_top();
}
void WM__Screen2Client(const WObj *pWin, SRect *pRect) {
	*pRect -= pWin->rect.left_top();
}

void WM__SendMsgNoData(WObj *pWin, uint8_t MsgId) {
	WM_MESSAGE msg;
	msg.pWin = pWin;
	msg.MsgId = MsgId;
	pWin->SendMessage(&msg);
}
void WM__GetClientRectWin(const WObj *pWin, SRect *pRect) {
	pRect->x0 = pRect->y0 = 0;
	pRect->x1 = pWin->rect.x1 - pWin->rect.x0;
	pRect->y1 = pWin->rect.y1 - pWin->rect.y0;
}

WObj *WObj::Create(
	int x0, int y0, int width, int height,
	WObj *pParent, uint16_t Style, WM_CALLBACK *cb, int NumExtraBytes) {
	if (!pParent)
		if (WM__NumWindows)
			pParent = WM__ahDesktopWin;
	if (pParent == WM_UNATTACHED)
		pParent = nullptr;
	if (pParent) {
		x0 += pParent->rect.x0;
		y0 += pParent->rect.y0;
		if (width == 0)
			width = pParent->rect.x1 - pParent->rect.x0 + 1;
		if (height == 0)
			height = pParent->rect.y1 - pParent->rect.y0 + 1;
	}
	WObj *pWin = (WObj *)GUI_ALLOC_AllocZero(NumExtraBytes + sizeof(WObj));
	if (!pWin)
		return nullptr;
	WM__NumWindows++;
	pWin->rect.x0 = x0;
	pWin->rect.y0 = y0;
	pWin->rect.x1 = x0 + width - 1;
	pWin->rect.y1 = y0 + height - 1;
	pWin->cb = cb;
	pWin->Status = Style & (WC_VISIBLE |
							WC_STAYONTOP |
							WC_CONST_OUTLINE |
							WC_ANCHOR_RIGHT |
							WC_ANCHOR_BOTTOM |
							WC_ANCHOR_LEFT |
							WC_ANCHOR_TOP |
							WC_NOACTIVATE |
							WC_LATE_CLIP);
	pWin->_AddToLinList();
	pWin->_InsertWindowIntoList(pParent);
	if (Style & WC_ACTIVATE)
		pWin->Select();
	if (Style & WC_BGND)
		pWin->BringToBottom();
	if (Style & WC_VISIBLE) {
		pWin->Status |= WC_VISIBLE;
		pWin->Invalidate();
	}
	WM__SendMsgNoData(pWin, WM_CREATE);
	return pWin;
}
void WObj::Delete() {
	if (!*this) return;
	NextDrawWin = nullptr;
	if (pWinFocus == this)
		pWinFocus = nullptr;
	if (WM__pCapture == this)
		WM__pCapture = nullptr;
	_CheckCriticalHandles(this);
	WM_NotifyParent(this, WM_NOTIFICATION_CHILD_DELETED);
	for (auto pChild = pFirstChild; pChild; ) {
		auto pNext = pChild->pNext;
		pChild->Delete();
		pChild = pNext;
	}
	WM__SendMsgNoData(this, WM_DELETE);
	Detach();
	_RemoveFromLinList();
	if (Status & WC_ACTIVATE)
		NumInvalidWindows--;
	WM__NumWindows--;
	GUI_ALLOC_Free(this);
	pWinFirst->Select();
}
void WObj::Select() {
	GUI.pWndActive = this;
	GUI_LCD_SetClipRectMax();
	GUI.xOff = rect.x0;
	GUI.yOff = rect.y0;
}

WObj *WM_GetActiveWindow(void) {
	return GUI.pWndActive;
}

#pragma region IVR
WObj::ContextIVR WObj::_ClipContext;
void WObj::_ActivateClipRect() {
	if (WM_IsActive)
		_SetClipRectUserIntersect(_ClipContext.CurRect);
	else
		_SetClipRectUserIntersect(GUI.pWndActive->rect);
}
bool WObj::_FindNextIVR() {
	static auto _Findy1 = [](WObj *pWin, SRect &r) {
		for (; pWin; pWin = pWin->pNext)
			if (pWin->Status & WC_VISIBLE) {
				SRect rWinClipped = pWin->rect;
				if (r && rWinClipped)
					if (pWin->rect.y0 > r.y0) {
						if (rWinClipped.y0 - 1<r.y1)
							r.y1 = rWinClipped.y0 - 1; /* Check upper border of window */
					}
					else if (rWinClipped.y1<r.y1)
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
		r.left_top(_ClipContext.ClientRect.left_top());
	else {
		r.x0 = _ClipContext.CurRect.x1 + 1;
		r.y0 = _ClipContext.CurRect.y0;
		if (r.x0 > _ClipContext.ClientRect.x1) {
		NextStripe:
			r.x0 = _ClipContext.ClientRect.x0;
			r.y0 = _ClipContext.CurRect.y1 + 1;
		}
	}
	if (r.y0 > _ClipContext.ClientRect.y1)
		return false;
	if (r.x0 == _ClipContext.ClientRect.x0) {
		r.right_bottom(_ClipContext.ClientRect.right_bottom());
		for (auto pParent = GUI.pWndActive; pParent; pParent = pParent->pParent)
			_Findy1(pParent->pNext, r);
		_Findy1(GUI.pWndActive->pFirstChild, r);
	}
Find_x0:
	r.x1 = r.x0;
	for (auto pParent = GUI.pWndActive; pParent; pParent = pParent->pParent)
		if (_Findx0(pParent->pNext, r))
			goto Find_x0;
	if (_Findx0(GUI.pWndActive->pFirstChild, r))
		goto Find_x0;
	r.x1 = _ClipContext.ClientRect.x1;
	if (r.x1 < r.x0) {
		_ClipContext.CurRect = r;
		goto NextStripe;
	}
	for (auto pParent = GUI.pWndActive; pParent; pParent = pParent->pParent)
		_Findx1(pParent->pNext, r);
	_Findx1(GUI.pWndActive->pFirstChild, r);
	if (_ClipContext.Cnt > 200)
		return false;
	_ClipContext.CurRect = r;
	return true;
}
bool WObj::GetNextIVR() {
	static bool bCursorHidden = true;
	if (WM_IsActive == 0)
		return false;
	if (_ClipContext.EntranceCnt > 1) {
		_ClipContext.EntranceCnt--;
		return false;
	}
	if (bCursorHidden) {
		bCursorHidden = false;
		GUI_CURSOR__TempUnhide();
	}
	++_ClipContext.Cnt;
	if (!_FindNextIVR()) {
		_ClipContext.EntranceCnt--;
		return false;
	}
	_ActivateClipRect();
	bCursorHidden = GUI_CURSOR__TempHide(&_ClipContext.CurRect);
	return true;
}
bool WObj::InitSearchIVR(const SRect *pMaxRect) {
	if (WM_IsActive == 0) {
		_ActivateClipRect();
		return true;
	}
	if (++_ClipContext.EntranceCnt > 1)
		return true;
	_ClipContext.Cnt = -1;
	SRect r;
	if (WM__PaintCallbackCnt)
		r = GUI.pWndActive->rInvalid;
	else if (GUI.pWndActive->Status & WC_VISIBLE)
		r = GUI.pWndActive->rect;
	else {
		--_ClipContext.EntranceCnt;
		return false;
	}
	if (pMaxRect)
		r &= *pMaxRect;
	if (GUI.pUserClipRect) {
		WObj *pWin = GUI.pWndActive;
		SRect rUser = *(GUI.pUserClipRect);
		WM__Client2Screen(pWin, &rUser);
		r &= rUser;
	}
	if (WM__ClipAtParentBorders(&r, GUI.pWndActive) == 0) {
		--_ClipContext.EntranceCnt;
		return false;
	}
	_ClipContext.ClientRect = r;
	return GetNextIVR();
}
#pragma endregion

const SRect *WObj::SetUserClipRect(const SRect *pRect) {
	const SRect *pRectReturn = GUI.pUserClipRect;
	GUI.pUserClipRect = pRect;
	_ActivateClipRect();
	return pRectReturn;
}

void WM_SetDefault(void) {
	GUI_SetDefault();
	GUI.pUserClipRect = nullptr;
}

#pragma region Invalidate & Validate
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
		NumInvalidWindows++;
	}
}
void WObj::InvalidateArea(const SRect &r) {
	for (auto i = pWinFirst; i; i = i->pNextLin)
		i->_Invalidate1Abs(r);
}
void WObj::Invalidate(const SRect *pRect) {
	int Status = this->Status;
	if (!(Status & WC_VISIBLE))
		return;
	auto r = this->rect;
	if (pRect) {
		auto rPara = *pRect;
		WM__Client2Screen(this, &rPara);
		r &= rPara;
	}
	if (WM__ClipAtParentBorders(&r, this))
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
		NumInvalidWindows--;
	}
}
#pragma endregion

#pragma region Paint
void WObj::_Paint1() {
	if (!cb) return;
	if (!(Status & WC_VISIBLE))
		return;
	WM__PaintCallbackCnt++;
	if (Status & WC_LATE_CLIP) {
		WM_MESSAGE msg;
		msg.pWin = this;
		msg.MsgId = WM_PAINT;
		msg.Data = (size_t)&rInvalid;
		WM_SetDefault();
		SendMessage(&msg);
	}
	else {
		WM_ITERATE_START(&rInvalid) {
			WM_MESSAGE msg;
			msg.pWin = this;
			msg.MsgId = WM_PAINT;
			msg.Data = (size_t)&rInvalid;
			WM_SetDefault();
			SendMessage(&msg);
		} WM_ITERATE_END();
	}
	WM__PaintCallbackCnt--;
}
bool WObj::_Paint() {
	if (!(Status & WC_ACTIVATE))
		return false;
	bool res = false;
	if (cb)
		if (WM__ClipAtParentBorders(&rInvalid, this)) {
			Select();
			_Paint1();
			res = true;
		}
	Status &= ~WC_ACTIVATE;
	NumInvalidWindows--;
	return res;
}
bool WObj::PaintNext() {
	if (!WM_IsActive || !NumInvalidWindows)
		return false;
	int UpdateRem = 1;
	auto pWin = NextDrawWin ? NextDrawWin : pWinFirst;
	for (; pWin && UpdateRem; ) {
		if (pWin->_Paint())
			UpdateRem--;
		pWin = pWin->pNextLin;
	}
	NextDrawWin = pWin;
	return true;
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

void WM_Activate(void) {
	WM_IsActive = 1;
}
void WM_Deactivate(void) {
	WM_IsActive = 0;
	GUI_LCD_SetClipRectMax();
}
void WM_DefaultProc(WM_MESSAGE *pMsg) {
	WObj *pWin = pMsg->pWin;
	switch (pMsg->MsgId) {
	case WM_GET_INSIDE_RECT:
		WM__GetClientRectWin(pWin, (SRect *)pMsg->Data);
		break;
	case WM_GET_CLIENT_WINDOW:
		pMsg->Data = (size_t)pWin;
		return;
	case WM_KEY:
		WM_SendToParent(pWin, pMsg);
		return;
	case WM_GET_BKCOLOR:
		pMsg->Data = GUI_INVALID_COLOR;
		return;
	case WM_NOTIFY_ENABLE:
		pWin->Invalidate();
		return;
	}
	pMsg->Data = 0;
}
void WObj::Init() {
	if (WM__ahDesktopWin) return;
	NextDrawWin = nullptr;
	GUI.pUserClipRect = nullptr;
	WM__NumWindows = NumInvalidWindows = 0;
	WM__ahDesktopWin = WObj::Create(0, 0, GUI_XMAX, GUI_YMAX, WC_VISIBLE, [](WM_MESSAGE *pMsg) {
		switch (pMsg->MsgId) {
			case WM_KEY:
			{
				const WM_KEY_INFO *pKeyInfo = (const WM_KEY_INFO *)pMsg->Data;
				if (pKeyInfo->PressedCnt == 1)
					GUI_StoreKey(pKeyInfo->Key);
				break;
			}
			case WM_PAINT:
				if (WM__aBkColor != GUI_INVALID_COLOR) {
					GUI.BkColor(WM__aBkColor);
					GUI_Clear();
				}
			default:
				WM_DefaultProc(pMsg);
		}
	}, 0);
	WM__aBkColor = GUI_INVALID_COLOR;
	WM__ahDesktopWin->Invalidate();
	WM__AddCriticalHandle(&WM__CHWinModal);
	WM__AddCriticalHandle(&WM__CHWinLast);
	WM__ahDesktopWin->Select();
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

void WM__AddCriticalHandle(WM_CRITICAL_HANDLE *pCriticalHandle) {
	pCriticalHandle->pNext = WM__pFirstCriticalHandle;
	WM__pFirstCriticalHandle = pCriticalHandle;
}
void WM__RemoveCriticalHandle(WM_CRITICAL_HANDLE *pCriticalHandle) {
	if (!WM__pFirstCriticalHandle)
		return;
	WM_CRITICAL_HANDLE *pCH, *pLast = 0;
	for (pCH = WM__pFirstCriticalHandle; pCH; pCH = pCH->pNext) {
		if (pCH == pCriticalHandle) {
			if (pLast)
				pLast->pNext = pCH->pNext;
			else if (pCH->pNext)
				WM__pFirstCriticalHandle = pCH->pNext;
			else
				WM__pFirstCriticalHandle = 0;
			break;
		}
		pLast = pCH;
	}
}

RGBC WM_GetBkColor(WObj *pWin) {
	if (!pWin)
		return GUI_INVALID_COLOR;
	WM_MESSAGE msg;
	msg.MsgId = WM_GET_BKCOLOR;
	pWin->SendMessage(&msg);
	return (RGBC)msg.Data;
}
void WM_GetClientRect(SRect *pRect) {
	*pRect = GUI.pWndActive->ClientRect();
}
WObj *WM_GetClientWindow(WObj *pWin) {
	WM_MESSAGE msg;
	msg.Data = 0;
	msg.MsgId = WM_GET_CLIENT_WINDOW;
	pWin->SendMessage(&msg);
	return (WObj *)msg.Data;
}
WObj *WM_GetDesktopWindow(void) {
	return WM__ahDesktopWin;
}
int WM_GetNumWindows(void) {
	return WM__NumWindows;
}
int WM_GetNumInvalidWindows(void) {
	return NumInvalidWindows;
}
uint16_t WM_GetFlags(WObj *pWin) {
	return pWin ? pWin->Status : 0;
}
SRect WObj::InsideRect() const {
	SRect rInsideRect;
	WM_MESSAGE msg;
	msg.Data = (size_t)&rInsideRect;
	msg.MsgId = WM_GET_INSIDE_RECT;
	const_cast<WObj *>(this)->SendMessage(&msg);
	return rInsideRect;
}
void WM_GetInsideRect(SRect *pRect) {
	*pRect = GUI.pWndActive->InsideRect();
}
bool WM_GetInvalidRect(WObj *pWin, SRect *pRect) {
	if (!pWin)
		return false;
	if (pWin->Status & WC_ACTIVATE) {
		*pRect = pWin->rInvalid;
		return true;
	}
	return false;
}

void WM_GetWindowRect(SRect *pRect) {
	if (!pRect)
		return;
	*pRect = GUI.pWndActive->rect;
}
bool WM_HasCaptured(WObj *pWin) {
	return pWin == WM__pCapture;
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
	WM_MESSAGE msg;
	msg.MsgId = WM_NOTIFY_ENABLE;
	msg.Data = bEnable;
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
	if (WM_GetFlags(this) & WC_NOACTIVATE)
		return nullptr;
	WM_NOTIFY_CHILD_HAS_FOCUS_INFO Info;
	Info.pOld = pWinFocus;
	Info.pNew = this;
	WM_MESSAGE msg = { 0 };
	msg.MsgId = WM_SET_FOCUS;
	msg.Data = 0;
	if (pWinFocus)
		pWinFocus->SendMessage(&msg);
	msg.Data = 1;
	pWinFocus = this;
	SendMessage(&msg);
	if (msg.Data)
		return this;
	for (auto pWin = this; (pWin = pWin->Parent());) {
		msg.MsgId = WM_NOTIFY_CHILD_HAS_FOCUS;
		msg.Data = (size_t)&Info;
		pWin->SendMessage(&msg);
	}
	if (auto pWin = Info.pOld)
		if (*pWin)
			while ((pWin = pWin->Parent()) != nullptr) {
				msg.MsgId = WM_NOTIFY_CHILD_HAS_FOCUS;
				msg.Data = (size_t)&Info;
				pWin->SendMessage(&msg);
			}
	return nullptr;
}
#pragma endregion

void WM_MakeModal(WObj *pWin) {
	WM__CHWinModal.pWin = pWin;
	if (!WM__CHWinLast.pWin)
		return;
	if (WM__IsInModalArea(WM__CHWinLast.pWin))
		return;
	WM_MESSAGE msg = { 0 };
	msg.MsgId = WM_TOUCH;
	WM__SendPIDMessage(WM__CHWinLast.pWin, &msg);
	WM__CHWinLast.pWin = 0;
}
void WM_MoveChildTo(WObj *pWin, int x, int y) {
	if (auto pParent = pWin->Parent())
		pWin->Position(pWin->rect.left_top() - pParent->rect.left_top());
}
void WM_NotifyParent(WObj *pWin, int Notification) {
	WM_MESSAGE msg;
	msg.MsgId = WM_NOTIFY_PARENT;
	msg.Data = Notification;
	WM_SendToParent(pWin, &msg);
}
bool WObj::OnKey(int Key, int Pressed) {
	if (!pWinFocus)
		return false;
	WM_KEY_INFO Info;
	Info.Key = Key;
	Info.PressedCnt = Pressed;
	WM_MESSAGE msg;
	msg.MsgId = WM_KEY;
	msg.Data = (size_t)&Info;
	WM__SendMessage(pWinFocus, &msg);
	return true;
}
void WObj::Paint() {
	Select();
	WM_SetDefault();
	Invalidate();  /* Important ... Window procedure is informed about invalid rect and may optimize */
	_Paint1();
	Validate();
}
void WM_PID__GetPrevState(PidState *pPrevState) {
	*pPrevState = WM_PID__StateLast;
}

void WM_SendToParent(WObj *pChild, WM_MESSAGE *pMsg) {
	if (!pMsg)
		return;
	WObj *pParent = pChild->Parent();
	if (!pParent)
		return;
	pMsg->pWinSrc = pChild;
	pParent->SendMessage(pMsg);
}
void WM_SetAnchor(WObj *pWin, uint16_t AnchorFlags) {
	if (!pWin)
		return;
	uint16_t Mask = (WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT | WC_ANCHOR_TOP | WC_ANCHOR_BOTTOM);
	pWin->Status &= ~Mask;
	pWin->Status |= AnchorFlags;
}
WM_CALLBACK *WM_SetCallback(WObj *pWin, WM_CALLBACK *cb) {
	if (!pWin)
		return nullptr;
	WM_CALLBACK *r = pWin->cb;
	pWin->cb = cb;
	pWin->Invalidate();
	return r;
}
RGBC WM_SetDesktopColor(RGBC Color) {
	RGBC r = WM__aBkColor;
	WM__aBkColor = Color;
	WM__ahDesktopWin->Invalidate();
	return r;
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
bool WM__IsAncestor(WObj *pChild, WObj *pParent) {
	if (!pChild)
		return false;
	if (!pParent)
		return false;
	for (; pChild; pChild = pChild->pParent)
		if (pChild->pParent == pParent)
			return true;
	return false;
}
bool WM__IsAncestorOrSelf(WObj *pChild, WObj *pParent) {
	return pChild == pParent ? true :
		WM__IsAncestor(pChild, pParent);
}
void WM__SendMessage(WObj *pWin, WM_MESSAGE *pMsg) {
	pMsg->pWin = pWin;
	if (pWin->cb)
		pWin->cb(pMsg);
	else
		WM_DefaultProc(pMsg);
}
void WM__SendMessageIfEnabled(WObj *pWin, WM_MESSAGE *pMsg) {
	if (pWin->Enable())
		WM__SendMessage(pWin, pMsg);
}
void WM_SendMessageNoPara(WObj *pWin, int MsgId) {
	if (!pWin->cb)
		return;
	WM_MESSAGE msg = { 0 };
	msg.pWin = pWin;
	msg.MsgId = MsgId;
	pWin->cb(&msg);
}
static void _NotifyVisChanged(WObj *pWin, SRect *pRect) {
	for (pWin = pWin->FirstChild(); pWin; pWin = pWin->NextSibling()) {
		if (!(pWin->Status & WC_VISIBLE))
			continue;
		if (pWin->rect && pRect) {
			WM__SendMsgNoData(pWin, WM_NOTIFY_VIS_CHANGED);
			_NotifyVisChanged(pWin, pRect);
		}
	}
}
void WM__NotifyVisChanged(WObj *pWin, SRect *pRect) {
	WObj *pParent = pWin->pParent;
	if (pParent)
		_NotifyVisChanged(pParent, pRect);
}

#pragma region Capture
WObj *WM__pCapture;
char WM__CaptureReleaseAuto;
void WM_SetCaptureMove(WObj *pWin, const PidState *pState, int MinVisibility) {
	static Point WM__CapturePoint;
	if (!WM_HasCaptured(pWin)) {
		WM_SetCapture(pWin, 1);
		WM__CapturePoint.x = pState->x;
		WM__CapturePoint.y = pState->y;
		return;
	}
	int dx = pState->x - WM__CapturePoint.x,
		dy = pState->y - WM__CapturePoint.y;
	if (MinVisibility == 0) {
		pWin->Move({ dx, dy });
		return;
	}
	auto &&rect = pWin->Rect() + Point(dx, dy),
		 &&RectParent = pWin->Parent()->Rect();
	RectParent /= MinVisibility;
	if (rect &&  RectParent)
		pWin->Move({ dx, dy });
}
void WM_ReleaseCapture(void) {
	if (!WM__pCapture)
		return;
	WM_MESSAGE msg;
	msg.MsgId = WM_CAPTURE_RELEASED;
	WM__pCapture->SendMessage(&msg);
	WM__pCapture = 0;
}
void WM_SetCapture(WObj *pWin, int AutoRelease) {
	if (WM__pCapture != pWin)
		WM_ReleaseCapture();
	WM__pCapture = pWin;
	WM__CaptureReleaseAuto = AutoRelease;
}
#pragma endregion

WObj *WObj::ScreenToWin(Point Pos, WObj *pStop, WObj *pWin) {
	if (!pWin->Visible()) return nullptr;
	if (pWin->Rect() > Pos) return nullptr;
	for (auto pChild = pWin->pFirstChild;
		 pChild && pChild != pStop;
		 pChild = pChild->pNext)
		if (auto pHit = ScreenToWin(Pos, pStop, pChild))
			pWin = pHit;
	return pWin;
}

static void _MoveDescendents(WObj* pWin, Point d) {
	for (; pWin; pWin = pWin->pNext) {
		pWin->rect += d;
		pWin->rInvalid += d;
		_MoveDescendents(pWin->pFirstChild, d);  /* Children need to be moved along ...*/
		WM__SendMsgNoData(pWin, WM_MOVE);
	}
}
void WObj::Move(Point dPos) {
	if (!dPos) return;
	auto rOld = rect;
	rect += dPos;
	rInvalid += dPos;
	_MoveDescendents(pFirstChild, dPos);
	if (Status & WC_VISIBLE) {
		InvalidateArea(rect);
		InvalidateArea(rOld);
	}
	WM__SendMsgNoData(this, WM_MOVE);
}

void WM__UpdateChildPositions(WObj *pObj, int dx0, int dy0, int dx1, int dy1) {
	for (auto pChild = pObj->pFirstChild; pChild; pChild = pChild->pNext) {
		SRect rOld = pChild->rect, rNew = rOld;
		int Status = pChild->Status & (WC_ANCHOR_RIGHT | WC_ANCHOR_LEFT);
		switch (Status) {
			case WC_ANCHOR_RIGHT:
				rNew.x0 += dx1;
				rNew.x1 += dx1;
				break;
			case WC_ANCHOR_RIGHT | WC_ANCHOR_LEFT:
				rNew.x0 += dx0;
				rNew.x1 += dx1;
				break;
			default:
				rNew.x0 += dx0;
				rNew.x1 += dx0;
		}
		Status = pChild->Status & (WC_ANCHOR_TOP | WC_ANCHOR_BOTTOM);
		switch (Status) {
			case WC_ANCHOR_BOTTOM: /* Bottom ANCHOR */
				rNew.y0 += dy1;
				rNew.y1 += dy1;
				break;
			case WC_ANCHOR_BOTTOM | WC_ANCHOR_TOP:    /* resize window */
				rNew.y0 += dy0;
				rNew.y1 += dy1;
				break;
			default: /* Top ANCHOR */
				rNew.y0 += dy0;
				rNew.y1 += dy0;
		}
		pChild->Rect(rNew);
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
	WM__UpdateChildPositions(this, rNew.x0, rNew.y0, rNew.x1, rNew.y1);
	rInvalid &= rect;
	WM__SendMsgNoData(this, WM_SIZE);
}

static WObj *_GetDialogItem(WObj *pWin, int Id) {
	for (auto pObj = pWin->pFirstChild; pObj; pObj = pObj->pNext)
		if (pObj->ID() == Id)
			return pObj;
		else if (auto pItem = _GetDialogItem(pObj, Id))
			return pItem;
	return nullptr;
}
WObj *WM_GetDialogItem(WObj *pWin, int Id) {
	return _GetDialogItem(pWin, Id);
}

WObj *WM_SetFocusOnNextChild(WObj *pParent) {
	if (!pParent)
		return nullptr;
	static auto _GetNextChild = [](WObj *pParent, WObj *pChild) {
		WObj *pObj = nullptr;
		if (pChild)
			pObj = pChild->pNext;
		if (!pObj)
			pObj = pParent->pFirstChild;
		return pObj != pChild ? pObj : nullptr;
	};
	auto pChild = _GetNextChild(pParent, pParent->FocussedChild());
	auto pWin = pChild;
	while (pWin) {
		if (pWin->Focussable())
			break;
		pWin = _GetNextChild(pParent, pWin);
		if (pWin == pChild)
			break;
	}
	return pWin->Focus();
}

void WM_CheckScrollBounds(WM_SCROLL_STATE *pScrollState) {
	int Max = pScrollState->NumItems - pScrollState->PageSize;
	if (Max < 0)
		Max = 0;
	if (pScrollState->v < 0)
		pScrollState->v = 0;
	if (pScrollState->v > Max)
		pScrollState->v = Max;
}
int  WM_CheckScrollPos(WM_SCROLL_STATE *pScrollState, int Pos, int LowerDist, int UpperDist) {
	int vOld = pScrollState->v;
	if (Pos > pScrollState->v + pScrollState->PageSize - 1)
		pScrollState->v = Pos - (pScrollState->PageSize - 1) + UpperDist;
	if (Pos < pScrollState->v)
		pScrollState->v = Pos - LowerDist;
	WM_CheckScrollBounds(pScrollState);
	return pScrollState->v - vOld;
}
int  WM_SetScrollValue(WM_SCROLL_STATE *pScrollState, int v) {
	int vOld = pScrollState->v;
	pScrollState->v = v;
	WM_CheckScrollBounds(pScrollState);
	return pScrollState->v - vOld;
}
void WM_GetInsideRectExScrollbar(WObj *pWin, SRect *pRect) {
	if (!pWin)
		return;
	if (!pRect)
		return;
	uint16_t WinFlags;
	SRect rScrollbar;
	WObj *pBarH = WM_GetDialogItem(pWin, GUI_ID_HSCROLL),
		*pBarV = WM_GetDialogItem(pWin, GUI_ID_VSCROLL);
	auto &&rWin = pWin->Rect(),
		&&rInside = pWin->InsideRect();
	if (pBarV) {
		rScrollbar = pBarV->Rect() - rWin.left_top();
		WinFlags = WM_GetFlags(pBarV);
		if ((WinFlags & WC_ANCHOR_RIGHT) && (WinFlags & WC_VISIBLE))
			rInside.x1 = rScrollbar.x0 - 1;
	}
	if (pBarH) {
		rScrollbar = pBarH->Rect() - rWin.left_top();
		WinFlags = WM_GetFlags(pBarH);
		if ((WinFlags & WC_ANCHOR_BOTTOM) && (WinFlags & WC_VISIBLE))
			rInside.y1 = rScrollbar.y0 - 1;
	}
	*pRect = rInside;
}
struct ScrollBar_Obj *WM_GetScrollbarH(WObj *pWin) {
	return (struct ScrollBar_Obj *)WM_GetDialogItem(pWin, GUI_ID_HSCROLL);
}
struct ScrollBar_Obj *WM_GetScrollbarV(WObj *pWin) {
	return (struct ScrollBar_Obj *)WM_GetDialogItem(pWin, GUI_ID_VSCROLL);
}
WObj *WM_GetScrollPartner(WObj *pScroll) {
	int Id = pScroll->ID();
	if (Id == GUI_ID_HSCROLL)
		Id = GUI_ID_VSCROLL;
	else if (Id == GUI_ID_VSCROLL)
		Id = GUI_ID_HSCROLL;
	return WM_GetDialogItem(pScroll->Parent(), Id);
}
void WM_GetScrollState(WObj *pWin, WM_SCROLL_STATE *pScrollState) {
	WM_MESSAGE msg;
	msg.MsgId = WM_GET_SCROLL_STATE;
	msg.Data = (size_t)pScrollState;
	pWin->SendMessage(&msg);
}
void WM_SetScrollState(WObj *pWin, const WM_SCROLL_STATE *pState) {
	if (!pWin)
		return;
	if (!pState)
		return;
	WM_MESSAGE msg;
	msg.MsgId = WM_SET_SCROLL_STATE;
	msg.Data = (size_t)pState;
	pWin->SendMessage(&msg);
}
static bool _SetScrollbar(WObj *pWin, int OnOff, int Id, int Flags) {
	WObj *pBar = WM_GetDialogItem(pWin, Id);
	if (!pBar)
		return false;
	if (OnOff)
		SCROLLBAR_CreateAttached(pWin, Flags);
	else {
		pBar->Visible(false);
		pBar->Delete();
	}
	return true;
}
int WM_SetScrollbarV(WObj *pWin, int OnOff) {
	return _SetScrollbar(pWin, OnOff, GUI_ID_VSCROLL, SCROLLBAR_CF_VERTICAL);
}
int WM_SetScrollbarH(WObj *pWin, int OnOff) {
	return _SetScrollbar(pWin, OnOff, GUI_ID_HSCROLL, 0);
}
