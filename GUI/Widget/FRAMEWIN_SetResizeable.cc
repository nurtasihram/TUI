#include "FRAMEWIN.h"
#define FRAMEWIN_REACT_BORDER 3
#define FRAMEWIN_MINSIZE_X    20
#define FRAMEWIN_MINSIZE_Y    20
#define FRAMEWIN_RESIZE_X     (1<<0)
#define FRAMEWIN_RESIZE_Y     (1<<1)
#define FRAMEWIN_REPOS_X      (1<<2)
#define FRAMEWIN_REPOS_Y      (1<<3)
#define FRAMEWIN_MOUSEOVER    (1<<4)
#define FRAMEWIN_RESIZE       (FRAMEWIN_RESIZE_X | FRAMEWIN_RESIZE_Y | FRAMEWIN_REPOS_X | FRAMEWIN_REPOS_Y)
static int      _CaptureX;
static int      _CaptureY;
static int      _CaptureFlags;
#include "chars_pic.inl"
static const Cursor * _pOldCursor;
static const RGBC _ColorsCursor[]  = { 0x0000FF,0x000000,0xFFFFFF };
static const LogPalette _PalCursor = { 3, 1, &_ColorsCursor[0] };
static void _SetResizeCursor(int Mode) {
	static const unsigned char _acResizeCursorH[] = {
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
	static const Bitmap _bmResizeCursorH = {
		17, 17, 5, 2, 
		_acResizeCursorH,
		&_PalCursor
	};
	static const Cursor _ResizeCursorH = {
		&_bmResizeCursorH, 8, 8
	};
	static const unsigned char _acResizeCursorV[] = {
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
	static const Bitmap _bmResizeCursorV = {
		17, 17, 5, 2,
		_acResizeCursorV,
		&_PalCursor
	};
	static const Cursor _ResizeCursorV = {
		&_bmResizeCursorV, 8, 8
	};
	static const unsigned char _acResizeCursorDD[] = {
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
	static const Bitmap _bmResizeCursorDD = {
		17, 17, 5, 2, 
		_acResizeCursorDD,
		&_PalCursor
	};
	static const Cursor _ResizeCursorDD = {
		&_bmResizeCursorDD, 8, 8
	};
	static const unsigned char _acResizeCursorDU[] = {
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
	static const Bitmap _bmResizeCursorDU = {
		17, 17, 5, 2, 
		_acResizeCursorDU,
		&_PalCursor
	};
	static const Cursor _ResizeCursorDU = {
		&_bmResizeCursorDU, 8, 8
	};
	const Cursor *pNewCursor = nullptr;
	if (Mode) {
		int Direction = Mode & (FRAMEWIN_RESIZE_X | FRAMEWIN_RESIZE_Y);
		if (Direction == FRAMEWIN_RESIZE_X)
			pNewCursor = &_ResizeCursorH;
		else if (Direction == FRAMEWIN_RESIZE_Y)
			pNewCursor = &_ResizeCursorV;
		else {
			
			Direction = Mode & (FRAMEWIN_REPOS_X | FRAMEWIN_REPOS_Y);
			
			pNewCursor = (Direction == (FRAMEWIN_REPOS_X | FRAMEWIN_REPOS_Y)) || !Direction ?
				&_ResizeCursorDD : &_ResizeCursorDU;
		}
	}
	
	if (pNewCursor) {
		const Cursor * pOldCursor = GUI_CURSOR_Select(pNewCursor);
		if (_pOldCursor == nullptr)
			_pOldCursor = pOldCursor;
	}
	else if (_pOldCursor) {
		GUI_CURSOR_Select(_pOldCursor);
		_pOldCursor = nullptr;
	}
}

static void _SetCapture(Frame *pObj, int x, int y, int Mode) {
	if (!(_CaptureFlags & FRAMEWIN_REPOS_X))
		_CaptureX = x;
	if (!(_CaptureFlags & FRAMEWIN_REPOS_Y))
		_CaptureY = y;
	if (!Mode)
		return;
	if (WM_HasCaptured(pObj) == 0)
		WM_SetCapture(pObj, 0);
	_SetResizeCursor(Mode);
	if (Mode & FRAMEWIN_MOUSEOVER)
		Mode = 0;
	_CaptureFlags = (Mode | FRAMEWIN_MOUSEOVER);
}

static void _ChangeWindowPosSize(Frame *pObj, int* px, int* py) {
	int dx = 0, dy = 0;
	SRect rect = pObj->ClientRect();
	if (_CaptureFlags & FRAMEWIN_RESIZE_X)
		dx = (_CaptureFlags & FRAMEWIN_REPOS_X) ? (_CaptureX - *px) : (*px - _CaptureX);
	if (_CaptureFlags & FRAMEWIN_RESIZE_Y)
		dy = (_CaptureFlags & FRAMEWIN_REPOS_Y) ? (_CaptureY - *py) : (*py - _CaptureY);
	if ((rect.x1 + dx + 1) < FRAMEWIN_MINSIZE_X) {
		dx = FRAMEWIN_MINSIZE_X - rect.x1 - 1;
		*px = _CaptureX + dx;
	}
	if ((rect.y1 + dy + 1) < FRAMEWIN_MINSIZE_Y) {
		dy = FRAMEWIN_MINSIZE_Y - rect.y1 - 1;
		*py = _CaptureY + dy;
	}
	pObj->Move({_CaptureFlags & FRAMEWIN_REPOS_X ? -dx : 0,
				_CaptureFlags & FRAMEWIN_REPOS_Y ? -dy : 0 });
	pObj->Resize({ dx, dy });
}
static int _CheckReactBorder(Frame *pObj, int x, int y) {
	static auto _CheckBorderX = [](int x, int x1, int Border) {
		return x > x1 - Border ?
			FRAMEWIN_RESIZE_X :
			x < Border ?
			FRAMEWIN_RESIZE_X | FRAMEWIN_REPOS_X : 0;
	};
	static auto _CheckBorderY = [](int y, int y1, int Border) {
		return y > y1 - Border ?
			FRAMEWIN_RESIZE_Y :
			y < Border ?
			FRAMEWIN_RESIZE_Y | FRAMEWIN_REPOS_Y : 0;
	};
	auto &&r = pObj->ClientRect();
	if (x < 0 || y < 0)
		return 0;
	if (x > r.x1 || y > r.y1)
		return 0;
	int Mode = _CheckBorderX(x, r.x1, FRAMEWIN_REACT_BORDER);
	if (Mode)
		Mode |= _CheckBorderY(y, r.y1, 4 * FRAMEWIN_REACT_BORDER);
	else {
		Mode |= _CheckBorderY(y, r.y1, FRAMEWIN_REACT_BORDER);
		if (Mode)
			Mode |= _CheckBorderX(x, r.x1, 4 * FRAMEWIN_REACT_BORDER);
	}
	return Mode;
}
int Frame::_OnTouchResize(WM_MESSAGE* pMsg) {
	auto pState = (const PidState*)pMsg->Data;
	if (!pState) return false;
	int x = pState->x, y = pState->y;
	int Mode = _CheckReactBorder(this, x, y);
	if (pState->Pressed != 1) {
		if (!WM_HasCaptured(this))
			return false;
		_CaptureFlags &= ~(FRAMEWIN_RESIZE);
		if (!Mode)
			WM_ReleaseCapture();
		return true;
	}
	if (_CaptureFlags & FRAMEWIN_RESIZE) {
		_ChangeWindowPosSize(this, &x, &y);
		_SetCapture(this, x, y, 0);
		return true;
	}
	if (Mode) {
		Focus();
		BringToTop();
		_SetCapture(this, x, y, Mode);
		return true;
	}
	if (_CaptureFlags) {
		WM_ReleaseCapture();
		return true;
	}
	return false;
}
static bool _ForwardMouseOverMsg(Frame *pObj, WM_MESSAGE *pMsg) {
	auto pState = (PidState *)pMsg->Data;
	*pState += pObj->Position();
	if (auto pBelow = WObj::ScreenToWin(*pState)) {
		if (pBelow == pObj) return false;
		*pState -= pBelow->Position();
		pBelow->SendMessage(pMsg);
		return true;
	}
	return false;
}
bool Frame::_OnResizeable(WM_MESSAGE* pMsg) {
	auto pWin = (Frame *)pMsg->pWin;
	if (WM_HasCaptured(pWin) && _CaptureFlags == 0)
		return false;
	if (pWin->Minimized() || pWin->Maximized())
		return false;
	switch (pMsg->MsgId) {
	case WM_TOUCH:
		return pWin->_OnTouchResize(pMsg);
	case WM_MOUSEOVER: 
		if (auto pState = (const PidState *)pMsg->Data) {
			int x = pState->x, y = pState->y;
			int Mode = _CheckReactBorder(pWin, x, y);
			if (Mode) {
				if (!_ForwardMouseOverMsg(pWin, pMsg))
					_SetCapture(pWin, x, y, Mode | FRAMEWIN_MOUSEOVER);
				return true;
			}
			if (!WM_HasCaptured(pWin))
				return false;
			if (_CaptureFlags & FRAMEWIN_RESIZE)
				return true;
			WM_ReleaseCapture();
			_ForwardMouseOverMsg(pWin, pMsg);
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
