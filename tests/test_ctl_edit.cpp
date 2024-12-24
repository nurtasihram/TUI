#include "Static.h"
#include "Button.h"
#include "CheckBox.h"
#include "ProgBar.h"
#include "ScrollBar.h"
#include "Slider.h"
#include "Radio.h"
#include "ListBox.h"
#include "Frame.h"
#include "Header.h"

class CtlEdit : public WObj {
	Widget *pWidget = nullptr;
public:
	struct Property {
		RGBC Color = RGB_BLACK;
		RGBC BkColor = RGB_WHITE;
		int16_t Border = 4;
	} static DefaultProps;
private:
	Property Props;
public:
	void _Expend(bool);
	uint8_t _CheckReactBorder(Point Pos) const;
	void _ChangeWindowPosSize(Point);
	void _SetCapture(Point Pos, uint8_t Mode);

	void _OnPaint() const;
	bool _HandleResize(int MsgId, const PID_STATE *pState);

	static WM_RESULT _Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);
public:
	CtlEdit(Widget *pWidget);
};

CtlEdit::Property CtlEdit::DefaultProps;

void CtlEdit::_OnPaint() const {
	if (!Focussed() && !pWidget->Focussed())
		return;
	auto Border = Props.Border;
	auto &&s = pWidget->Size() + 2;
	SRect r1{ s.x + Border - 1, s.y + Border - 1, s.x + Border * 2 - 1, s.y + Border * 2 - 1};
	SRect r2{ r1.x0 / 2, r1.y0 / 2, (r1.x1 + Border) / 2, (r1.y1 + Border) / 2 };
	GUI.BkColor(Parent()->BkColor());
	GUI.Clear();
	GUI.PenColor(Props.BkColor);
	/* L T  */ GUI.Fill({ 0, 0, Border, Border });
	/*  RT  */ GUI.Fill({ r1.x0, 0, r1.x1, Border });
	/* L  B */ GUI.Fill({ 0, r1.y0, Border, r1.y1 });
	/*  R B */ GUI.Fill(r1);
	/* L    */ GUI.Fill({ 0, r2.y0, Border, r2.y1 });
	/*  R   */ GUI.Fill({ r1.x0, r2.y0, r1.x1, r2.y1 });
	/*   T  */ GUI.Fill({ r2.x0, 0, r2.x1, Border });
	/*    B */ GUI.Fill({ r2.x0, r1.y0, r2.x1, r1.y1 });
	GUI.PenColor(Props.Color);
	/* L T  */ GUI.Outline({ 0, 0, Border, Border });
	/*  RT  */ GUI.Outline({ r1.x0, 0, r1.x1, Border });
	/* L  B */ GUI.Outline({ 0, r1.y0, Border, r1.y1 });
	/*  R B */ GUI.Outline(r1);
	/* L    */ GUI.Outline({ 0, r2.y0, Border, r2.y1 });
	/*  R   */ GUI.Outline({ r1.x0, r2.y0, r1.x1, r2.y1 });
	/*   T  */ GUI.Outline({ r2.x0, 0, r2.x1, Border });
	/*    B */ GUI.Outline({ r2.x0, r1.y0, r2.x1, r1.y1 });
}
#pragma region PosSize
uint8_t CtlEdit::_CheckReactBorder(Point Pos) const {
	auto Border = Props.Border;
	auto &&sWidget = pWidget->Size() + 2;
	SRect r1{
		sWidget.x + Border - 1, sWidget.y + Border - 1,
		sWidget.x + Border * 2, sWidget.y + Border * 2 };
	SRect r2{
		r1.x0 / 2, r1.y0 / 2,
		(r1.x1 + Border) / 2, (r1.y1 + Border) / 2 };
	/* L */ if (SRect{ 0, r2.y0, Border, r2.y1 } <= Pos)
		return SIZE_RESIZE_X | SIZE_REPOS_X;
	/* LT */ if (SRect{ 0, 0, Border, Border } <= Pos)
		return SIZE_RESIZE_X | SIZE_REPOS_X | SIZE_RESIZE_Y | SIZE_REPOS_Y;
	/* T */ if (SRect{ r2.x0, 0, r2.x1, Border } <= Pos)
		return SIZE_RESIZE_Y | SIZE_REPOS_Y;
	/* RT */ if (SRect{ r1.x0, 0, r1.x1, Border } <= Pos)
		return SIZE_RESIZE_X | SIZE_RESIZE_Y | SIZE_REPOS_Y;
	/* R */ if (SRect{ r1.x0, r2.y0, r1.x1, r2.y1 } <= Pos)
		return SIZE_RESIZE_X;
	/* RB */ if (r1 <= Pos)
		return SIZE_RESIZE_X | SIZE_RESIZE_Y;
	/* B */ if (SRect{ r2.x0, r1.y0, r2.x1, r1.y1 } <= Pos)
		return SIZE_RESIZE_Y;
	/* LB */ if (SRect{ 0, r1.y0, Border, r1.y1 } <= Pos)
		return SIZE_RESIZE_X | SIZE_REPOS_X | SIZE_RESIZE_Y;
	if (SRect{ Border, sWidget } <= Pos)
		return 0;
	if (SRect{ 0, Size() } <= Pos)
		return SIZE_RESIZE;
	return 0;
}
static Point _Capture;
static uint8_t _CaptureFlags = 0;
void CtlEdit::_SetCapture(Point Pos, uint8_t Mode) {
	if (!(_CaptureFlags & SIZE_REPOS_X))
		_Capture.x = Pos.x;
	if (!(_CaptureFlags & SIZE_REPOS_Y))
		_Capture.y = Pos.y;
	if (!Mode) return;
	if (!Captured())
		Capture(false);
	if (auto pCursor = CursorCtl::GetResizeCursor(Mode))
		GUI.Cursor(pCursor);
	if (Mode & SIZE_MOUSEOVER)
		Mode = 0;
	_CaptureFlags = Mode | SIZE_MOUSEOVER;
}
void CtlEdit::_ChangeWindowPosSize(Point p) {
	Point d;
	if (_CaptureFlags & SIZE_RESIZE_X)
		d.x = (_CaptureFlags & SIZE_REPOS_X) ? _Capture.x - p.x : p.x - _Capture.x;
	if (_CaptureFlags & SIZE_RESIZE_Y)
		d.y = (_CaptureFlags & SIZE_REPOS_Y) ? _Capture.y - p.y : p.y - _Capture.y;
	auto &&ptEnd = ClientRect().right_bottom();
	auto xMin = Props.Border * 2 + 1 - ptEnd.x;
	if (d.x < xMin)
		d.x = xMin;
	auto yMin = Props.Border * 2 + 1 - ptEnd.y;
	if (d.y < yMin)
		d.y = yMin;
	Move({ _CaptureFlags & SIZE_REPOS_X ? -d.x : 0,
		   _CaptureFlags & SIZE_REPOS_Y ? -d.y : 0 });
	if (!(_CaptureFlags & SIZE_MOUSEMOVE))
		Resize(d);
}
bool CtlEdit::_HandleResize(int MsgId, const PID_STATE *pState) {
	if (Captured() && _CaptureFlags == 0)
		return false;
	switch (MsgId) {
		case WM_TOUCH:
			if (pState) {
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
					BringToTop();
					_SetCapture(Pos, Mode);
					return true;
				}
				if (_CaptureFlags) {
					CaptureRelease();
					return true;
				}
			}
			return false;
		case WM_MOUSEOVER:
			if (pState) {
				if (auto Mode = _CheckReactBorder(*pState)) {
					_SetCapture(*pState, Mode | SIZE_MOUSEOVER);
					return true;
				}
				if (!Captured())
					return false;
				if (!(_CaptureFlags & SIZE_RESIZE))
					CaptureRelease();
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
void CtlEdit::_Expend(bool bOpen) {
	auto Border = Props.Border + 1;
	auto &&rExpend = RectAbs();
	if (bOpen) {
		rExpend *= (int)Border;
		BringToTop();
	}
	else 
		rExpend /= (int)Border;
	RectAbs(rExpend);
}

WM_RESULT CtlEdit::_Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc) {
	auto pObj = (CtlEdit *)pWin;
	auto pWidget = pObj->pWidget;
	if (pObj->_HandleResize(MsgId, Param))
		return Param;
	switch (MsgId) {
		case WM_CREATE:
			return 0;
		case WM_GET_ACCEPT_FOCUS:
			return !pWidget->Focussable();
		case WM_NOTIFY_PARENT: {
			switch ((int)Param) {
				case WN_GOT_FOCUS:
					pObj->_Expend(true);
					break;
				case WN_LOST_FOCUS:
					pObj->_Expend(false);
					break;
			}
			break;
		}
		case WM_SET_FOCUS:
			pObj->_Expend(Param);
			return true;
		case WM_TOUCH_CHILD:
			if (const PID_STATE *pState = Param)
				if (pState->Pressed == 1) {
					if (pWidget->Focussable())
						pWidget->Focus();
					else
						pObj->Focus();
				}
			return 0;
		case WM_GET_BKCOLOR:
			return pObj->Parent()->BkColor();
		case WM_PAINT:
			pObj->_OnPaint();
			return 0;
	}
	return WObj::DefCallback(pWin, MsgId, Param, pSrc);
}

CtlEdit::CtlEdit(Widget *pWidget) :
	WObj(pWidget->Rect(),
		   _Callback,
		   pWidget->Parent(), pWidget->ID(),
		   WC_VISIBLE, WC_FOCUSSABLE),
	pWidget(pWidget) {
	pWidget->Anchor(WC_ANCHOR_MASK);
	pWidget->Parent(this, 0);
}

void MainTask() {
	auto pStatic = new Static(
		10, 10, 45, 20,
		WObj::Desktop(), 0,
		WC_VISIBLE, 0, "Static");
	new CtlEdit(pStatic);
	auto pButton = new Button(
		10, 40, 45, 20,
		WObj::Desktop(), 0,
		WC_VISIBLE, "Button");
	new CtlEdit(pButton);
	auto pCheckBox = new CheckBox(
		10, 70, 85, 20,
		WObj::Desktop(), 0,
		WC_VISIBLE, "CheckBox");
	new CtlEdit(pCheckBox);
	auto pProgBar = new ProgBar(
		10, 100, 85, 20,
		WObj::Desktop(), 0,
		WC_VISIBLE);
	new CtlEdit(pProgBar);
	auto pScrollBarH = new ScrollBar(
		10, 130, 85, 15,
		WObj::Desktop(), 0,
		WC_VISIBLE, 0);
	new CtlEdit(pScrollBarH);
	auto pScrollBarV = new ScrollBar(
		10, 155, 15, 80,
		WObj::Desktop(), 0,
		WC_VISIBLE, SCROLLBAR_CF_VERTICAL);
	new CtlEdit(pScrollBarV);
	auto pSlider = new Slider(
		30, 155, 85, 20,
		WObj::Desktop(), 0,
		WC_VISIBLE, 0);
	new CtlEdit(pSlider);
	auto pRadio = new Radio(
		30, 185, 85, 60,
		WObj::Desktop(), 0,
		WC_VISIBLE, 0,
		"Radio 1\0"
		"Radio 2\0"
		"Radio 3\0");
	new CtlEdit(pRadio);
	auto pListBox = new ListBox(
		10, 250, 105, 60,
		WObj::Desktop(), 0,
		WC_VISIBLE, 0,
		"ListBox 1\0"
		"ListBox 2\0"
		"ListBox 3\0");
	new CtlEdit(pListBox);
	auto pFrame = new Frame(
		130, 10, 105, 300,
		WObj::Desktop(), 0,
		WC_VISIBLE, 0,
		"Frame");
	new CtlEdit(pFrame);
	auto pHeader = new Header(
		240, 10, 105, 0,
		WObj::Desktop(), 0,
		WC_VISIBLE);
	pHeader->Add("Col 1");
	pHeader->Add("Col 2");
	pHeader->Add("Col 3");
	new CtlEdit(pHeader);
	for (;;)
		WObj::Exec();
}
