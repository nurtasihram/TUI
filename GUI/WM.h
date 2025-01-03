#pragma once
#include "GUI.h"

#include "WM.inl"

struct KEY_STATE {
	int Key, PressedCnt;
};
struct SCROLL_STATE {
	int NumItems = 0, v = 0, PageSize = 0;
	inline void Bound() {
		int Max = NumItems - PageSize;
		if (Max < 0) Max = 0;
		if (v < 0) v = 0;
		if (v > Max) v = Max;
	}
	inline auto Pos(int Pos, int LowerDist = 0, int UpperDist = 0) {
		int vOld = v;
		auto pgSize = PageSize - 1;
		if (v < Pos - pgSize)
			v = Pos - pgSize + UpperDist;
		if (v > Pos)
			v = Pos - LowerDist;
		Bound();
		return v - vOld;
	}
	inline auto Value(int v) {
		auto vOld = v;
		this->v = v;
		Bound();
		return v - vOld;
	}
	inline bool operator==(const SCROLL_STATE &s) const {
		if (NumItems != s.NumItems) return false;
		if (v != s.v) return false;
		if (PageSize != s.PageSize) return false;
		return true;
	}
	inline bool operator!=(const SCROLL_STATE &s) const { return !(*this == s); }
};
struct DIALOG_STATE {
	int Done = 0, ReturnValue = 0;
};
struct PID_CHANGED_STATE {
	int x, y;
	uint8_t State, StatePrev;
};

class WObj;
using WM_PARAM = uint64_t;
typedef void WM_CB(WObj *pWin, int msgid, WM_PARAM *pData, WObj *pWinSrc);

struct WM_CREATESTRUCT {
	uint16_t Class = 0;
	int16_t x = 0, y = 0;
	int16_t xsize = 0, ysize = 0;
	const char *pCaption = nullptr;
	WObj *pParent = nullptr;
	uint16_t Id = 0, Flags = 0, ExFlags = 0;
	union Param {
		int64_t i64;
		uint64_t u64;
		int8_t i8_8[8];
		uint8_t u8_8[8];
		int16_t i16_4[4];
		uint16_t u16_t[4];
		int32_t i32_2[2];
		uint32_t u32_2[2];
		void *ptr;
		const char *pString;
		Param(void *ptr = nullptr) : ptr(ptr) {}
		Param(int64_t i64) : i64(i64) {}
		Param(uint64_t u64) : u64(u64) {}
	} Para;
	WM_CREATESTRUCT() {}
	WM_CREATESTRUCT(
		uint16_t Class,
		int16_t x, int16_t y, int16_t xsize, int16_t ysize,
		const char *pCaption,
		int16_t Id, uint16_t Flags = 0, uint16_t ExFlags = 0,
		void *pPara = nullptr) :
		Class(Class),
		x(x), y(y), xsize(xsize), ysize(ysize),
		pCaption(pCaption),
		Id(Id), Flags(Flags), ExFlags(ExFlags),
		Para(pPara) {}
	WM_CREATESTRUCT(
		uint16_t Class,
		int16_t x, int16_t y, int16_t xsize, int16_t ysize,
		const char *pCaption,
		int16_t Id, uint16_t Flags, uint16_t ExFlags,
		Param Para) :
		Class(Class),
		x(x), y(y), xsize(xsize), ysize(ysize),
		pCaption(pCaption),
		Id(Id), Flags(Flags), ExFlags(ExFlags),
		Para(Para.u64) {}
public:
	int DialogBox(WM_CB *cb, WObj *pParent, int x0, int y0) const;
	class WObj *Create() const;
	class WObj *CreateDialog(WM_CB *cb, WObj *pParent, int x0, int y0) const;
};

class WObj {
protected:
	SRect rect, rInvalid;
	WM_CB* cb = nullptr;
	WObj *pParent = nullptr, *pFirstChild = nullptr;
	WM_STYLE Status = 0;

private:
	static uint16_t nWindows;

	WObj *pNext = nullptr;
	void _InsertWindowIntoList(WObj *pParent);
	void _RemoveWindowFromList();

	static WObj *pWinFirst;
	WObj *pNextLin = nullptr;
	void _RemoveFromLinList();
	void _AddToLinList();

public:
	struct CriticalHandles {
		static CriticalHandles *pFirst;
		static CriticalHandles Last;
		static CriticalHandles Modal;
		CriticalHandles *pNext = nullptr;
		WObj *pWin = nullptr;
	public:
		inline void Add() { pNext = pFirst, pFirst = pNext; }
		void Remove();
		void Check(WObj *pWin);
	};

public:
	operator bool() const;

public:
	WObj(int x0, int y0, int xSize, int ySize,
		 WM_CB *cb,
		 WObj *pParent = nullptr, WM_STYLE Style = 0);
	inline WObj(Point pos, Point size,
				WM_CB *cb,
				WObj *pParent = nullptr, WM_STYLE Style = 0) :
		WObj(pos.x, pos.y, size.x, size.y, cb, pParent, Style) {}
	inline WObj(SRect r,
				WM_CB *cb,
				WObj *pParent = nullptr, WM_STYLE Style = 0) :
		WObj(r.left_top(), r.size(), cb, pParent, Style) {}

protected:
	~WObj() {}

public:
	void Delete();
	void Select();

#pragma region Invalidate & Validate
private:
	static uint16_t nInvalidWindows;
	bool _ClipAtParentBorders(SRect &) const;
	void _Invalidate1Abs(SRect);
	void _Invalidate(SRect);
public:
	static void InvalidateArea(const SRect &r);
	inline void Invalidate(const SRect &r) { _Invalidate(rect & ClientToScreen(r)); }
	inline void Invalidate() { _Invalidate(rect); }
	void InvalidateDescs();
	void Validate();
#pragma endregion

#pragma region Paint
private:
	static uint8_t nPaintCallback;
	static WObj *pWinNextDraw;
	void _Paint1();
	bool _OnPaint();
public:
	void Paint();
	static bool PaintNext();
#pragma endregion

#pragma region IVR
private:
	struct ContextIVR {
		SRect rClient;
		SRect CurRect;
		int Cnt = 0;
		int EntranceCnt = 0;
	};
	static ContextIVR _ClipContext;
	static void  _ActivateClipRect();
	static bool _FindNextIVR();
private:
	static bool IVR_Next();
	static bool IVR_Init(const SRect *pMaxRect = nullptr);
public:
	template<class AnyDo>
	static inline void IVR(const AnyDo &Do) {
		if (WObj::IVR_Init())
			do {
				Do();
			} while (WObj::IVR_Next());
	}
	template<class AnyDo>
	static inline void IVR(const SRect &r, const AnyDo &Do) {
		if (WObj::IVR_Init(&r))
			do {
				Do();
			} while (WObj::IVR_Next());
	}
#pragma endregion

public:
	inline void SendMessage(int msgid) {
		if (!cb) return;
		WM_PARAM data;
		cb(this, msgid, &data, nullptr);
	}
	inline void SendMessage(int msgid, WM_PARAM *pData, WObj *pWinSrc = nullptr) {
		if (cb)
			cb(this, msgid, pData, pWinSrc);
	}
	inline void SendToParent(int msgid, WM_PARAM *pData) {
		if (pParent)
			pParent->SendMessage(msgid, pData, this);
	}
	inline void NotifyParent(int Notification) {
		WM_PARAM data;
		data = Notification;
		SendToParent(WM_NOTIFY_PARENT, &data);
	}
private:
	void _SendTouchMessage(int msgid, WM_PARAM *pData);
	void _SendMessage(int msgid, WM_PARAM *pData, WObj *pWinSrc = nullptr);
	void _SendMessageIfEnabled(int msgid, WM_PARAM *pData, WObj *pWinSrc = nullptr);
	bool _IsInModalArea();
public:
	static bool HandlePID();

	static bool OnKey(int Key, int Pressed);
	static void DefCallback(WObj *pWin, int msgid, WM_PARAM *pData, WObj *pWinSrc);

	static void Init();

	static bool Exec1();
	static inline void Exec() { while (Exec1()) {} }

	void BringToTop();
	void BringToBottom();

	void Detach();

#pragma region Resize & Move
private:
	void _MoveDescendents(Point d);
public:
	void Move(Point dPos);
	inline void MoveChildTo(Point Pos) {
		if (pParent)
			Position(Pos + pParent->rect.left_top());
	}
protected:
	void _UpdateChildPositions(SRect d);
public:
	void Resize(Point dSize);
#pragma endregion

#pragma region Focus
private:
	static WObj *pWinFocus;
public:
	/* A */ WObj *Focus();
	/* S */ inline bool Focussed() const { return this == pWinFocus; }
	/* S */ inline WObj *FocussedChild() { return pWinFocus ? pWinFocus->IsChildOf(this) ? pWinFocus : nullptr : nullptr; }
	/* A */ WObj *FocusNextChild();
	/* S */ inline bool Focussable() const {
		WM_PARAM data;
		const_cast<WObj *>(this)->SendMessage(WM_GET_ACCEPT_FOCUS, &data);
		return (bool)data;
	}
#pragma endregion

#pragma region Capture
private:
	static WObj *pWinCapture;
	static bool bCaptureAutoRelease;
	static Point CapturePoint;
public:
	/* A */ void Capture(bool bAutoRelease);
	/* A */ static void CaptureRelease();
	/* A */ void CaptureMove(const PID_STATE &pid, int MinVisibility);
	/* S */ inline bool Captured() const { return pWinCapture == this; }
#pragma endregion

#pragma region Desktop
private:
	static WObj *pWinDesktop;
	static RGBC aColorDesktop;
public: // Property - Desktop
	/* R */ static inline WObj *Desktop() { return pWinDesktop; }
public: // Property - DesktopColor
	/* W */ static inline void DesktopColor(RGBC Color) {
		if (aColorDesktop != Color) {
			aColorDesktop = Color;
			pWinDesktop->Invalidate();
		}
	}
	/* R */ static inline RGBC DesktopColor() { return aColorDesktop; }
#pragma endregion

#pragma region Active
private:
	static WObj *pWinActive;
public:
	/* R */ static inline WObj *ActiveWindow() { return pWinActive; }
#pragma endregion

#pragma region Rect
	static const SRect *UserClip(const SRect *pRect);

	static WObj *ScreenToWin(Point Pos, WObj *pStop = nullptr, WObj *pWin = pWinFirst);

	inline SRect ClientToScreen(const SRect &r) const { return r + rect.left_top(); }
	inline SRect ScreenToClient(const SRect &r) const { return r - rect.left_top(); }
#pragma endregion

	inline bool IsChildOf(WObj *pParent) { return this->pParent == pParent; }

#pragma region Dialog
public: // Property - DialogStatusPtr
	/* R */ inline DIALOG_STATE *DialogStatusPtr() {
		WM_PARAM data = 0;
		SendMessage(WM_HANDLE_DIALOG_STATUS, &data);
		return (DIALOG_STATE *)data;
	}
	/* W */ inline void DialogStatusPtr(DIALOG_STATE *pDialogStatus) {
		WM_PARAM data;
		data = (WM_PARAM)pDialogStatus;
		SendMessage(WM_HANDLE_DIALOG_STATUS, &data);
	}
public:
	int DialogExec();
	void DialogEnd(int);
#pragma endregion

#pragma region Properties
public: // Property - Styles
	/* R */ inline auto Styles() const { return Status; }
public: // Property - Anchor
	/* W */ inline void Anchor(uint16_t AnchorFlags) {
		Status &= ~WC_ANCHOR_MASK;
		Status |= AnchorFlags;
	}
public: // Property - Enable
	/* W */ void Enable(bool bEnable);
	/* R */ inline bool Enable() const { return !(Status & WC_DISABLED); }
public: // Property - StayOnTop
	/* W */ void StayOnTop(bool bEnable);
	/* R */ inline bool StayOnTop() const { return Status & WC_STAYONTOP; }
public: // Property - Visible
	/* W */ void Visible(bool bVisible);
	/* R */ inline bool Visible() const { return Status & WC_VISIBLE; }
public: // Property - RectAbs
	/* W */ inline void RectAbs(const SRect &r) {
		Position(r.left_top());
		Size(r.size());
	}
	/* R */ inline SRect RectAbs() const { return rect; }
public: // Property - Rect
	/* W */ inline void Rect(const SRect &r) {
		auto rAbs = r;
		if (auto pParent = Parent())
			rAbs += pParent->Position();
		RectAbs(r);
	}
	/* R */ inline SRect Rect() const {
		auto r = RectAbs();
		if (auto pParent = Parent())
			r -= pParent->Position();
		return r;
	}
public: // Property - Size
	/* W */ inline void Size(Point s) { Resize(s - rect.size()); }
	/* R */ inline Point Size() const { return rect.size(); }
public: // Property - SizeX
	/* W */ inline void SizeX(uint16_t xSize) { Resize({ xSize - rect.xsize(), 0 }); }
	/* R */ inline uint16_t SizeX() const { return rect.xsize(); }
public: // Property - SizeY
	/* W */ inline void SizeY(uint16_t ySize) { Resize({ 0, ySize - rect.ysize() }); }
	/* R */ inline uint16_t SizeY() const { return rect.ysize(); }
public: // Property - Position
	/* W */ inline void Position(Point p) { Move(p - rect.left_top()); }
	/* R */ inline Point Position() const { return rect.left_top(); }
public: // Property - PositionScreen
	/* W */ inline void PositionScreen(Point p) { Move(p - rect.left_top()); }
	/* R */ inline Point PositionScreen() const { return rect.left_top(); }
public: // Property - BkColor
	/* R */ inline RGBC BkColor() const {
		WM_PARAM data;
		const_cast<WObj *>(this)->SendMessage(WM_GET_BKCOLOR, &data);
		return (RGBC)data;
	}
public: // Property - InvalidRect
	/* R */ inline SRect InvalidRect() const { return rInvalid; }
public: // Property - InsideRect
	/* R */ SRect InsideRect() const;
public: // Property - InsideRectAbs
		/* R */ SRect InsideRectAbs() const;
public: // Property - ClientRect
	/* R */ inline SRect ClientRect() const { return{ 0, rect.d() }; }
public: // Property - Client
	/* R */ inline WObj *Client() {
		WM_PARAM data;
		SendMessage(WM_GET_CLIENT_WINDOW, &data);
		return (WObj *)data;
	}
	/* R */ inline const WObj *Client() const { return const_cast<WObj *>(this)->Client(); }
public: // Property - Parent
	/* W */ void Parent(WObj *pParent);
	/* W */ void Parent(WObj *pParent, Point p);
	/* R */ inline WObj *Parent() const { return pParent; }
public: // Property - FirstChild
	/* R */ inline WObj *FirstChild() { return pFirstChild; }
	/* R */ inline const WObj *FirstChild() const { return pFirstChild; }
public: // Property - FirstSibling
	/* R */ inline WObj *FirstSibling() { return pParent ? pParent->pFirstChild : nullptr; }
	/* R */ inline const WObj *FirstSibling() const { return pParent ? pParent->pFirstChild : nullptr; }
public: // Property - NextSibling
	/* R */ inline WObj *NextSibling() { return pNext; }
	/* R */ inline const WObj *NextSibling() const { return pNext; }
public: // Property - LastSibling
	/* R */ inline WObj *LastSibling();
public: // Property - PrevSibling
	/* R */ WObj *PrevSibling();
public: // Property - ID
	/* W */ inline void ID(int id) {
		WM_PARAM data;
		data = id;
		this->SendMessage(WM_SET_ID , &data);
	}
	/* R */ inline int ID() const {
		WM_PARAM data;
		const_cast<WObj *>(this)->SendMessage(WM_GET_ID, &data);
		return (int)data;
	}
public: // Property - DialogItem
	/* R */ WObj *DialogItem(uint16_t Id);
	/* R */ inline const WObj *DialogItem(uint16_t Id) const { return const_cast<WObj *>(this)->DialogItem(Id); }
public: // Property - ScrollBarH
	/* W */ void ScrollBarH(bool bEnable);
	/* R */ struct ScrollBar *ScrollBarH() { return (struct ScrollBar *)DialogItem(GUI_ID_HSCROLL); }
public: // Property - ScrollBarV
	/* W */ void ScrollBarV(bool bEnable);
	/* R */ struct ScrollBar *ScrollBarV() { return (struct ScrollBar *)DialogItem(GUI_ID_VSCROLL); }\
public: // Property - Callback
	/* W */ inline void Callback(WM_CB cb) {
		if (this->cb != cb) {
			this->cb = cb;
			Invalidate();
		}
	}
	/* R */ inline WM_CB *Callback() const { return cb; }
#pragma endregion

};
static inline bool IsWindow(const WObj *pObj) { return pObj ? *pObj : false; }

struct FOCUSED_STATE {
	WObj *pOld, *pNew;
};

extern PID_STATE WM_PID__StateLast;

bool    WM__IsAncestor(WObj *pChild, WObj *pParent);
bool    WM__IsAncestorOrSelf(WObj *pChild, WObj *pParent);
void    WM_PID__GetPrevState(PID_STATE *pPrevState);

#pragma region Widget
#define WIDGET_STATE_FOCUS              (1<<0)
#define WIDGET_STATE_VERTICAL           (1<<1)
#define WIDGET_STATE_FOCUSSABLE         (1<<2)
#define WIDGET_STATE_USER0              (1<<3)
#define WIDGET_STATE_USER1              (1<<4)
#define WIDGET_STATE_USER2              (1<<5)
#define WIDGET_CF_VERTICAL      WIDGET_STATE_VERTICAL

#define WIDGET_ITEM_DRAW                0
#define WIDGET_ITEM_GET_XSIZE           1
#define WIDGET_ITEM_GET_YSIZE           2
#define WM_WIDGET_SET_EFFECT    WM_WIDGET + 0

struct WIDGET_ITEM_DRAW_INFO {
	WObj *pWin;
	int Cmd;
	/* WIDGET_ITEM_GET_XSIZE, WIDGET_ITEM_GET_YSIZE, WIDGET_ITEM_DRAW, */
	int ItemIndex;
	int x0, y0;
};
typedef int WIDGET_DRAW_ITEM_FUNC(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo);

class Widget : public WObj {
protected:
	uint16_t Id, State;
protected:
	Widget(int x0, int y0, int width, int height,
		   WM_CB *cb,
		   WObj *pParent, uint16_t Id = 0, WM_STYLE Style = 0, uint16_t State = 0) :
		WObj(x0, y0, width, height, cb, pParent, Style),
		Id(Id), State(State),
		pEffect(Widget::pEffectDefault) {}
	Widget(Point pos, Point size,
		   WM_CB *cb,
		   WObj *pParent, uint16_t Id = 0, WM_STYLE Style = 0, uint16_t State = 0) :
		Widget(pos.x, pos.y, size.x, size.y,
			   cb, pParent, Id, Style, State) {}
	Widget(SRect r,
		   WM_CB *cb,
		   WObj *pParent, uint16_t Id = 0, WM_STYLE Style = 0, uint16_t State = 0) :
		Widget(r.left_top(), r.size(),
			   cb, pParent, Id, Style, State) {}

protected:
	bool HandleActive(int msgid, WM_PARAM *);

protected: // Graphics
	void DrawVLine(int x, int y0, int y1) const;
	void Fill(SRect) const;
	void OutlineFocus(SRect, int Dist = 0) const;

	inline RGBC BkColorProp(RGBC PropColor) const { return PropColor == RGB_INVALID_COLOR ? Parent()->BkColor() : PropColor; }

#pragma region Effect
public:
	struct EffectItf {
		int EffectSize;
		EffectItf(int EffectSize = 0) : EffectSize(EffectSize) {}
		virtual void DrawUp(const SRect &r) const {}
		virtual void DrawDown(const SRect &r) const {}
		static const EffectItf
			None,
			&&Simple,
			&&D3,
			&&D3L1,
			&&D3L2;
	};
private:
	const EffectItf *pEffect = nullptr;
	static const EffectItf *pEffectDefault;
	bool _EffectRequiresRedraw(const SRect &) const;
protected: // Effective
	void DrawUp(SRect) const;
	void DrawDown(SRect) const;
	inline void DrawUp() const {
		if (pEffect)
			pEffect->DrawUp(ClientRect());
	}
	inline void DrawDown() const { DrawDown(ClientRect()); }
protected: // Property - EffectSize
	/* R */ inline auto EffectSize() const { return pEffect ? pEffect->EffectSize : 0; }
public: // Property - DefaultEffect
	static void DefaultEffect(const EffectItf *pEffect) { pEffectDefault = pEffect; }
	static auto DefaultEffect() { return pEffectDefault; }
#pragma endregion

	inline void OrState(uint16_t nState) {
		if (State != (State & nState)) {
			State |= nState;
			Invalidate();
		}
	}
	inline void AndState(uint16_t Mask) {
		auto StateNew = State & ~Mask;
		if (State != StateNew) {
			State = StateNew;
			Invalidate();
		}
	}

#pragma region Properties
public: // Property - StyleEx
	/* R */ inline auto StyleEx() const { return State; }
public: // Property - Effect
	/* W */ void Effect(const EffectItf *pEffect);
	/* R */ inline auto Effect() const { return pEffect; }
public: // Property - ScrollStateH
	/* W */ void ScrollStateH(const SCROLL_STATE &s);
public: // Property - ScrollStateV
	/* W */ void ScrollStateV(const SCROLL_STATE &s);
public: // Property - InsideRect
	/* R */ inline auto InsideRect() const { return WObj::InsideRect() / pEffect->EffectSize; }
public: // Property - ClientRect
	/* R */ inline SRect ClientRect() const {
		auto &&r = WObj::ClientRect();
		return State & WIDGET_STATE_VERTICAL ? ~r : r;
	}
public: // Property - Size
	/* R */ inline Point Size() const { return State & WIDGET_STATE_VERTICAL ? ~WObj::Size() : WObj::Size(); }
	/* W */ inline void Size(Point Size) {
		if (State & WIDGET_STATE_VERTICAL)
			WObj::Size(~Size);
		else
			WObj::Size(Size);
	}
public: // Property - SizeX
	/* R */ inline uint16_t SizeX() const { return State & WIDGET_STATE_VERTICAL ? WObj::SizeY() : WObj::SizeX(); }
	/* W */ inline void SizeX(int16_t xSize) {
		if (State & WIDGET_STATE_VERTICAL)
			WObj::SizeY(xSize);
		else
			WObj::SizeX(xSize);
	}
public: // Property - SizeY
	/* R */ inline uint16_t SizeY() const { return State & WIDGET_STATE_VERTICAL ? WObj::SizeX() : WObj::SizeY(); }
	/* W */ inline void SizeY(int16_t ySize) {
		if (State & WIDGET_STATE_VERTICAL)
			WObj::SizeX(ySize);
		else
			WObj::SizeY(ySize);
	}
#pragma endregion

};

void WIDGET__FillStringInRect(const char *pText, const SRect &rFill, const SRect &rTextMax, const SRect &rTextAct);

enum WIDGET_CLASSES : uint16_t {
	WCLS_EOF,
	WCLS_BUTTON,
	WCLS_CHECKBOX,
	WCLS_DROPDOWN,
	WCLS_EDIT,
	WCLS_HEADER,
	WCLS_LISTBOX,
	WCLS_LISTVIEW,
	WCLS_MENU,
	WCLS_MULTIEDIT,
	WCLS_MULTIPAGE,
	WCLS_PROGBAR,
	WCLS_RADIO,
	WCLS_SCROLLBAR,
	WCLS_SLIDER,
	WCLS_STATIC,
	WCLS_FRAME
};
#pragma endregion
