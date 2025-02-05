#pragma once
#include "GUI.h"

#include "WM.inl"

struct KEY_STATE {
	uint16_t Key, PressedCnt;
};
struct SCROLL_STATE {
	int16_t NumItems = 0, v = 0, PageSize = 0;
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
struct PID_CHANGED_STATE : PID_STATE {
	uint8_t StatePrev;
};

class WObj;
struct WM_PARAM {
	uint64_t Data = 0;
	WM_PARAM() {}
	WM_PARAM(int Data) : Data(Data) {}
	template<class AnyType>
	WM_PARAM(const AnyType &t) : Data(*(const uint64_t *)&t)
	{ static_assert(sizeof(t) <= 8); }
	template<class AnyType>
	inline operator AnyType() const {
		static_assert(sizeof(AnyType) <= 8);
		return *(const AnyType *)&Data;
	}
};
using WM_RESULT = WM_PARAM;
using WM_CB = WM_RESULT(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);

struct WM_CREATESTRUCT {
	uint16_t Class = 0;
	int16_t x = 0, y = 0;
	int16_t xsize = 0, ysize = 0;
	const char *pCaption = nullptr;
	WObj *pParent = nullptr;
	uint16_t Id = 0;
	WM_CF Flags = WC_HIDE;
	uint16_t ExFlags = 0;
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
		int16_t Id, WM_CF Flags = WC_HIDE, uint16_t ExFlags = 0,
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
		int16_t Id, WM_CF Flags, uint16_t ExFlags,
		Param Para) :
		Class(Class),
		x(x), y(y), xsize(xsize), ysize(ysize),
		pCaption(pCaption),
		Id(Id), Flags(Flags), ExFlags(ExFlags),
		Para(Para.u64) {}
public:
	int DialogBox(WM_CB *cb = nullptr, Point Pos = 0, WObj *pParent = nullptr) const;
	class WObj *Create() const;
	class WObj *CreateDialog(WM_CB *cb = nullptr, Point Pos = 0, WObj *pParent = nullptr) const;
};

class WObj {
	SRect rect, rInvalid;
	WM_CB *cb = nullptr;
	WObj *pParent = nullptr, *pFirstChild = nullptr;
	WObj *pNext = nullptr, *pNextLin = nullptr;
protected:
	WM_CF Status = WC_HIDE;
	WC_EX StatusEx = 0;
	uint16_t Id = 0;

private:
	static uint16_t nWindows;
	void _InsertWindowIntoList(WObj *pParent);
	void _RemoveWindowFromList();

	static WObj *pWinFirst;
	void _RemoveFromLinList();
	void _AddToLinList();

public:
	operator bool() const;

public:
	WObj(int x0, int y0, int xSize, int ySize,
		 WM_CB *cb,
		 WObj *pParent = nullptr, uint16_t Id = 0,
		 WM_CF Style = WC_HIDE, uint16_t ExStyle = 0);
	WObj(Point pos, Point size,
		 WM_CB *cb,
		 WObj *pParent = nullptr, uint16_t Id = 0,
		 WM_CF Style = WC_HIDE, uint16_t ExStyle = 0) :
		WObj(pos.x, pos.y, size.x, size.y,
			 cb,
			 pParent, Id,
			 Style, ExStyle) {}
	WObj(SRect r,
		 WM_CB *cb,
		 WObj *pParent = nullptr, uint16_t Id = 0,
		 WM_CF Style = WC_HIDE, uint16_t ExStyle = 0) :
		WObj(r.left_top(), r.size(),
			 cb,
			 pParent, Id,
			 Style, ExStyle) {}
protected:
	~WObj() {}

public:
	void Destroy();
	void Select();

	inline void *operator new(size_t size) { return GUI_MEM_Alloc(size); }
	inline void operator delete(void *pObj) { GUI_MEM_Free(pObj); }

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
	inline WM_RESULT SendMessage(int MsgId, WM_PARAM Param = 0, WObj *pSrc = nullptr) {
		if (cb)
			return cb(this, MsgId, Param, pSrc);
		return 0;
	}
	inline WM_RESULT SendToParent(int MsgId, WM_PARAM Param) {
		if (pParent)
			return pParent->SendMessage(MsgId, Param, this);
		return 0;
	}
	inline void NotifyParent(int Notification) { SendToParent(WM_NOTIFY_PARENT, Notification); }

	WM_RESULT _SendMessage(int MsgId, WM_PARAM Param = 0, WObj *pSrc = nullptr);

private:
	void _SendMessageIfEnabled(int MsgId, WM_PARAM Param, WObj *pSrc = nullptr);
	void _SendTouchMessage(int MsgId, PID_STATE *pState);
	bool _IsInModalArea();
	static PID_STATE _StateLast;
	struct CriticalHandles {
		static CriticalHandles *pFirst, Last, Modal;
		WObj *pWin = nullptr;
		CriticalHandles *pNext = nullptr;
	public:
		CriticalHandles() {}
		CriticalHandles(WObj *pWin) : pWin(pWin), pNext(pFirst) { pFirst = pNext; }
		~CriticalHandles() { Remove(); }
	public:
		inline void Add() { pNext = pFirst, pFirst = pNext; }
		void Remove();
		void Check(WObj *pWin);
	};

public:
	static bool HandlePID();
	static inline PID_STATE PrevPidState() { return _StateLast; }

	static bool OnKey(int Key, int Pressed);
	static WM_RESULT DefCallback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);

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
	/* S */ inline bool Focussable() const { return const_cast<WObj *>(this)->SendMessage(WM_GET_ACCEPT_FOCUS); }
#pragma endregion

#pragma region Capture
private:
	static WObj *pWinCapture;
	static bool bCaptureAutoRelease;
	static Point capturePoint;
	static CCursor *pCursorCapture;
public:
	/* A */ void Capture(bool bAutoRelease);
	/* A */ static void CaptureRelease();
	/* A */ void CaptureMove(Point Pos, int MinVisibility = 0);
	/* W */ static inline void CapturePoint(Point Pos) { capturePoint = Pos; }
	/* R */ static inline Point CapturePoint() { return capturePoint; }
	/* S */ inline bool Captured() const { return pWinCapture == this; }
#pragma endregion

#pragma region Desktop
private:
	static WObj *pWinDesktop;
	static RGBC aColorDesktop;
public: // Property - Desktop
	/* R */ static inline WObj *Desktop() { return pWinDesktop; }
public: // Property - DesktopCallback
	static WM_RESULT DesktopCallback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);
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
	bool IsAncestor(const WObj *pParent) const;
	bool IsAncestorOrSelf(const WObj *pParent) const { return this == pParent ? true : IsAncestor(pParent); }

#pragma region Dialog
public: // Property - DialogStatusPtr
	/* R */ inline DIALOG_STATE *DialogStatusPtr() { return SendMessage(WM_HANDLE_DIALOG_STATUS); }
	/* W */ inline void DialogStatusPtr(DIALOG_STATE *pDialogStatus) { SendMessage(WM_HANDLE_DIALOG_STATUS, pDialogStatus); }
public:
	int DialogExec();
	void DialogEnd(int);
#pragma endregion

#pragma region Properties
public: // Property - Styles
	/* R */ inline auto Styles() const { return Status; }
public: // Property - StyleEx
	/* R */ inline auto StyleEx() const { return StatusEx; }
public: // Property - Anchor
	/* W */ inline void Anchor(WM_CF AnchorFlags) {
		Status &= ~WC_ANCHOR_MASK;
		Status |= AnchorFlags;
	}
public: // Property - Enable
	/* R */ inline bool Enable() const { return !(Status & WC_DISABLED); }
	/* W */ void Enable(bool bEnable);
public: // Property - StayOnTop
	/* R */ inline bool StayOnTop() const { return Status & WC_STAYONTOP; }
	/* W */ void StayOnTop(bool bEnable);
public: // Property - Visible
	/* R */ inline bool Visible() const { return Status & WC_VISIBLE; }
	/* W */ void Visible(bool bVisible);
public: // Property - RectAbs
	/* R */ inline SRect RectAbs() const { return rect; }
	/* W */ void RectAbs(const SRect &r);
public: // Property - Rect
	/* R */ inline SRect Rect() const {
		auto r = RectAbs();
		if (auto pParent = Parent())
			r -= pParent->Position();
		return r;
	}
	/* W */ inline void Rect(const SRect &r) {
		if (auto pParent = Parent())
			RectAbs(r + pParent->Position());
		else
			RectAbs(r);
	}
public: // Property - Size
	/* R */ inline Point Size() const { return rect.size(); }
	/* W */ inline void Size(Point s) { Resize(s - rect.size()); }
public: // Property - SizeX
	/* R */ inline uint16_t SizeX() const { return rect.xsize(); }
	/* W */ inline void SizeX(uint16_t xSize) { Resize({ xSize - rect.xsize(), 0 }); }
public: // Property - SizeY
	/* R */ inline uint16_t SizeY() const { return rect.ysize(); }
	/* W */ inline void SizeY(uint16_t ySize) { Resize({ 0, ySize - rect.ysize() }); }
public: // Property - Position
	/* R */ inline Point Position() const { return rect.left_top(); }
	/* W */ inline void Position(Point p) { Move(p - rect.left_top()); }
public: // Property - BkColor
	/* R */ inline RGBC BkColor() const { return const_cast<WObj *>(this)->SendMessage(WM_GET_BKCOLOR); }
public: // Property - InvalidRect
	/* R */ inline SRect InvalidRect() const { return rInvalid; }
public: // Property - InsideRect
	/* R */ SRect InsideRect() const;
public: // Property - InsideRectAbs
	/* R */ inline SRect InsideRectAbs() const { return const_cast<WObj *>(this)->SendMessage(WM_GET_INSIDE_RECT); }
public: // Property - ClientRect
	/* R */ inline SRect ClientRect() const { return{ 0, rect.d() }; }
public: // Property - Client
	/* R */ inline WObj *Client() { return SendMessage(WM_GET_CLIENT_WINDOW); }
	/* R */ inline const WObj *Client() const { return const_cast<WObj *>(this)->Client(); }
public: // Property - Parent
	/* R */ inline WObj *Parent() const { return pParent; }
	/* W */ void Parent(WObj *pParent);
	/* W */ void Parent(WObj *pParent, Point p);
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
	/* R */ inline uint16_t ID() const { return const_cast<WObj *>(this)->SendMessage(WM_GET_ID); }
	/* W */ inline void ID(uint16_t id) { this->SendMessage(WM_SET_ID, id); }
public: // Property - DialogItem
	/* R */ WObj *DialogItem(uint16_t Id);
	/* R */ inline const WObj *DialogItem(uint16_t Id) const { return const_cast<WObj *>(this)->DialogItem(Id); }
public: // Property - ScrollBarH
	/* R */ class ScrollBar *ScrollBarH() { return (class ScrollBar *)DialogItem(GUI_ID_HSCROLL); }
	/* W */ void ScrollBarH(bool bEnable);
public: // Property - ScrollBarV
	/* R */ class ScrollBar *ScrollBarV() { return (class ScrollBar *)DialogItem(GUI_ID_VSCROLL); }\
		/* W */ void ScrollBarV(bool bEnable);
public: // Property - Callback
	/* R */ inline WM_CB *Callback() const { return cb; }
	/* W */ inline void Callback(WM_CB cb) {
		if (this->cb != cb) {
			this->cb = cb;
			Invalidate();
		}
	}
#pragma endregion

};
static inline bool IsWindow(const WObj *pObj) { return pObj ? *pObj : false; }
#define WM_UNATTACHED ((WObj *)-1)

struct FOCUSED_STATE {
	WObj *pOld, *pNew;
};

#pragma region Widget
#define WIDGET_ITEM_DRAW       0
#define WIDGET_ITEM_GET_XSIZE  1
#define WIDGET_ITEM_GET_YSIZE  2
#define WM_WIDGET_SET_EFFECT   WM_WIDGET + 0

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
	Widget(int x0, int y0, int width, int height,
		   WM_CB *cb,
		   WObj *pParent, uint16_t Id = 0,
		   WM_CF Style = WC_HIDE, uint16_t StyleEx = 0) :
		WObj(x0, y0, width, height,
			 cb,
			 pParent, Id,
			 Style, StyleEx),
		pEffect(Widget::pEffectDefault) {}
	Widget(Point pos, Point size,
		   WM_CB *cb,
		   WObj *pParent, uint16_t Id = 0,
		   WM_CF Style = WC_HIDE, uint16_t StyleEx = 0) :
		Widget(pos.x, pos.y, size.x, size.y,
			   cb,
			   pParent, Id,
			   Style, StyleEx) {}
	Widget(SRect r,
		   WM_CB *cb,
		   WObj *pParent, uint16_t Id = 0,
		   WM_CF Style = WC_HIDE, uint16_t StyleEx = 0) :
		Widget(r.left_top(), r.size(),
			   cb,
			   pParent, Id,
			   Style, StyleEx) {}

protected:
	bool HandleActive(int MsgId, WM_PARAM &Param);

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
		if (StatusEx != (StatusEx & nState)) {
			StatusEx |= nState;
			Invalidate();
		}
	}
	inline void AndState(uint16_t Mask) {
		auto StateNew = StatusEx & ~Mask;
		if (StatusEx != StateNew) {
			StatusEx = StateNew;
			Invalidate();
		}
	}

#pragma region Properties
public: // Property - Effect
	/* R */ inline auto Effect() const { return pEffect; }
	/* W */ inline void Effect(const EffectItf *pEffect) { SendMessage(WM_WIDGET_SET_EFFECT, pEffect); }
public: // Property - ScrollStateH
	/* W */ void ScrollStateH(const SCROLL_STATE &s);
public: // Property - ScrollStateV
	/* W */ void ScrollStateV(const SCROLL_STATE &s);
public: // Property - ClientRect
	/* R */ inline SRect ClientRect() const {
		auto &&r = WObj::ClientRect();
		return StatusEx & WC_EX_VERTICAL ? ~r : r;
	}
public: // Property - SizeX
	/* R */ inline uint16_t SizeX() const { return StatusEx & WC_EX_VERTICAL ? WObj::SizeY() : WObj::SizeX(); }
	/* W */ inline void SizeX(int16_t xSize) {
		if (StatusEx & WC_EX_VERTICAL)
			WObj::SizeY(xSize);
		else
			WObj::SizeX(xSize);
	}
public: // Property - SizeY
	/* R */ inline uint16_t SizeY() const { return StatusEx & WC_EX_VERTICAL ? WObj::SizeX() : WObj::SizeY(); }
	/* W */ inline void SizeY(int16_t ySize) {
		if (StatusEx & WC_EX_VERTICAL)
			WObj::SizeX(ySize);
		else
			WObj::SizeY(ySize);
	}
#pragma endregion

};

enum WIDGET_CLASSES : uint16_t {
	WCLS_EOF = 0,
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
