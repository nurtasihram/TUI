#pragma once
#include "GUI.h"

#include "WM.inl"

struct SCROLL_STATE {
	int16_t NumItems = 0, v = 0, PageSize = 0;
	SCROLL_STATE(int16_t NumItems = 0,
				 int16_t v = 0,
				 int16_t PageSize = 0) :
		NumItems(NumItems),
		v(v),
		PageSize(PageSize) {}
	void Bound();
	int Pos(int Pos, int LowerDist = 0, int UpperDist = 0);
	int Value(int v);
	inline bool NeedScroll() const { return PageSize < NumItems; }
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
struct MOUSE_CHANGED_STATE : MOUSE_STATE {
	int8_t StatePrev = 0;
	MOUSE_CHANGED_STATE(Point NowPos, int8_t NewState, int8_t OldState) :
		MOUSE_STATE(NowPos, NewState), StatePrev(OldState) {}
};
struct WM_PARAM {
	uint64_t Data = 0;
	WM_PARAM() {}
	WM_PARAM(int Data) : Data(Data) {}
	WM_PARAM(bool Data) : Data(Data) {}
	template<class AnyType>
	WM_PARAM(const AnyType &t) : Data(*(const uint64_t *)&t)
	{ static_assert(sizeof(t) <= sizeof(WM_PARAM)); }
	template<class AnyType>
	inline operator AnyType() const {
		static_assert(sizeof(AnyType) <= sizeof(WM_PARAM));
		return *(const AnyType *)&Data;
	}
	inline operator bool() const { return Data; }
};
using WM_RESULT = WM_PARAM;

class WObj;
using PWObj = WObj *;
typedef WM_RESULT(*WM_CB)(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

struct WM_CREATESTRUCT {
	uint16_t Class = 0;
	int16_t x = 0, y = 0;
	int16_t xsize = 0, ysize = 0;
	inline SRect rect() const { return SRect::left_top({ x, y }, { xsize, ysize }); }
	GUI_PCSTR pCaption = nullptr;
	PWObj pParent = nullptr;
	uint16_t Id = 0;
	WM_CF Flags = WC_HIDE;
	uint16_t FlagsEx = 0;
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
		GUI_PCSTR pString;
		Param(void *ptr = nullptr) : ptr(ptr) {}
		Param(int64_t i64) : i64(i64) {}
		Param(uint64_t u64) : u64(u64) {}
	} Para;
	WM_CREATESTRUCT() {}
	WM_CREATESTRUCT(
		uint16_t Class,
		int16_t x, int16_t y, int16_t xsize, int16_t ysize,
		GUI_PCSTR pCaption,
		int16_t Id, WM_CF Flags = WC_HIDE, uint16_t FlagsEx = 0,
		void *pPara = nullptr) :
		Class(Class),
		x(x), y(y), xsize(xsize), ysize(ysize),
		pCaption(pCaption),
		Id(Id), Flags(Flags), FlagsEx(FlagsEx),
		Para(pPara) {}
	WM_CREATESTRUCT(
		uint16_t Class,
		int16_t x, int16_t y, int16_t xsize, int16_t ysize,
		GUI_PCSTR pCaption,
		int16_t Id, WM_CF Flags, uint16_t FlagsEx,
		Param Para) :
		Class(Class),
		x(x), y(y), xsize(xsize), ysize(ysize),
		pCaption(pCaption),
		Id(Id), Flags(Flags), FlagsEx(FlagsEx),
		Para(Para.u64) {}
public:
	int DialogBox(WM_CB cb = nullptr, Point Pos = 0, PWObj pParent = nullptr) const;
	PWObj Create() const;
	PWObj CreateDialog(WM_CB cb = nullptr, Point Pos = 0, PWObj pParent = nullptr) const;
};

class WObj {
	SRect rsWin, rsInvalid;
	WM_CB cb = nullptr;
	PWObj pParent = nullptr, pFirstChild = nullptr;
	PWObj pNext = nullptr, pNextLin = nullptr;
protected:
	WM_CF Status = WC_HIDE;
	WC_EX StatusEx = 0;
	uint16_t Id = 0;

private:
	static uint16_t nWindows;
	static PWObj pWinFirst;

	void _InsertWindowIntoList(PWObj pParent);
	void _RemoveWindowFromList();

	void _AddToLinList();
	void _RemoveFromLinList();

public:
	WObj(Point pos, Point size,
		 WM_CB cb,
		 PWObj pParent = nullptr, uint16_t Id = 0,
		 WM_CF Style = WC_HIDE, uint16_t ExStyle = 0) :
		WObj(SRect::left_top(pos, size),
			 cb,
			 pParent, Id,
			 Style, ExStyle) {}
	WObj(const SRect &rc,
		 WM_CB cb,
		 PWObj pParent = nullptr, uint16_t Id = 0,
		 WM_CF Style = WC_HIDE, uint16_t ExStyle = 0);
protected:
	~WObj() {}

public:

	/// @brief 銷毀窗體
	///		析構窗體類並釋放内存
	void Destroy();

	/// @brief 選中窗體
	void Select();

	operator bool() const;
	inline void *operator new(size_t size) { return GUI_MEM_Alloc(size); }
	inline void operator delete(void *pObj) { GUI_MEM_Free(pObj); }

#pragma region Invalidate
private:
	static uint16_t nInvalidWindows;
	bool _ClipAtParentBorders(SRect &) const;
	void _Invalidate1Abs(SRect);
	void _Invalidate(SRect);
public:
	static void InvalidateArea(const SRect &r);
	inline void Invalidate(const SRect &r) { _Invalidate(rsWin & Client2Screen(r)); }
	inline void Invalidate() { _Invalidate(rsWin); }
	void InvalidateDescs();
#pragma endregion

#pragma region Paint
private:
	static uint8_t nPaintCallback;
	static PWObj pWinNextDraw;
	void _Paint1();
	bool _OnPaint();
public:
	/// @brief 繼續繪製窗體
	/// @return 是否有效執行繪製任務
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
	static bool IVR_Next();
	static bool IVR_Init(const SRect *pMaxRect = nullptr);
public:

	/// @brief 依圖層順序重繪窗體
	/// @tparam AnyDo 繪圖方法類
	/// @param Do 繪圖方法
	template<class AnyDo>
	static inline void IVR(const AnyDo &Do) {
		if (WObj::IVR_Init())
			do {
				Do();
			} while (WObj::IVR_Next());
	}

	/// @brief 依圖層順序重繪窗體
	/// @tparam AnyDo 繪圖方法類
	/// @param r 繪圖區域
	/// @param Do 繪圖方法
	template<class AnyDo>
	static inline void IVR(const SRect &r, const AnyDo &Do) {
		if (WObj::IVR_Init(&r))
			do {
				Do();
			} while (WObj::IVR_Next());
	}
#pragma endregion

#pragma region Messager
public:

	/// @brief 發送消息
	/// @param MsgId 消息ID
	/// @param Param 參數
	/// @param pSrc 數據源窗體
	/// @return 返迴數據
	inline WM_RESULT SendMessage(int MsgId, WM_PARAM Param = 0, PWObj pSrc = nullptr) {
		if (cb)
			return cb(this, MsgId, Param, pSrc);
		return 0;
	}

	/// @brief 發送至父窗體
	/// @param MsgId 消息ID
	/// @param Param 參數
	/// @return 返迴數據
	inline WM_RESULT SendToParent(int MsgId, WM_PARAM Param) {
		if (pParent)
			return pParent->SendMessage(MsgId, Param, this);
		return 0;
	}

	/// @brief 通知消息
	/// @param Notification 通知ID
	inline void NotifyParent(int Notification) { SendToParent(WM_NOTIFY_CHILD, Notification); }

	static bool HandleMouse();
	static inline MOUSE_STATE PrevPidState() { return _StateLast; }

	/// @brief 鍵盤事件
	/// @param Key 鍵位
	/// @param Pressed 按下或釋放
	/// @return 鍵位是否被處理
	static bool OnKey(uint16_t Key, int Pressed);

	/// @brief 默認窗體消息事件迴調
	/// @param pWin 窗體對象
	/// @param MsgId 消息ID
	/// @param Param 消息參數
	/// @param pSrc 消息原窗體
	/// @return 消息返迴值
	static WM_RESULT DefCallback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

	/// @brief 初始化窗體管理器
	static void Init();

	/// @brief 執行窗體管理器
	/// @return 是否活動
	static bool Exec();

private:
	WM_RESULT _SendMessage(int MsgId, WM_PARAM Param = 0, PWObj pSrc = nullptr);
	void _SendMessageIfEnabled(int MsgId, WM_PARAM Param, PWObj pSrc = nullptr);
	void _SendMouseMessage(int MsgId, MOUSE_STATE *pState);
	bool _IsInModalArea();
	static MOUSE_STATE _StateLast;
	struct CriticalHandles {
		static CriticalHandles *pFirst, Last, Modal;
		PWObj pWin = nullptr;
		CriticalHandles *pNext = nullptr;
	public:
		CriticalHandles() {}
		CriticalHandles(PWObj pWin) : pWin(pWin), pNext(pFirst) { pFirst = pNext; }
		~CriticalHandles() { Remove(); }
	public:
		inline void Add() { pNext = pFirst, pFirst = pNext; }
		void Remove();
		void Check(PWObj pWin);
	};
#pragma endregion

public:

	/// @brief 置頂窗體
	void BringToTop();
	/// @brief 置底窗體
	void BringToBottom();

	/// @brief 卸載父窗體
	inline void Detach() { Parent(nullptr); }

#pragma region Resize & Move
private:
	void _MoveDescendents(Point d);
protected:
	void _UpdateChildPositions(SRect d);
public:
	/// @brief 重置窗體大小
	/// @param dSize 大小增量
	void Resize(Point dSize);
	/// @brief 移動窗體
	/// @param dPos 位置增量
	void Move(Point dPos);
#pragma endregion

#pragma region Focus
private:
	static PWObj pWinFocus;
public:

	/// @brief 對焦窗體
	/// @return 對焦成功後則返迴焦點窗體
	PWObj Focus();

	/// @brief 是否為焦點窗體
	inline bool Focussed() const { return this == pWinFocus; }

	/// @brief 是否為焦點窗體的子窗體
	/// @return 焦點窗體
	PWObj FocussedChild();

	/// @brief 聚焦到下一個子窗體
	/// @return 焦點窗體
	PWObj FocusNextChild();
#pragma endregion

#pragma region Capture
private:
	static PWObj pWinCapture;
	static bool bCaptureAutoRelease;
	static Point capturePoint;
	static CCursor *pCursorCapture;
public:

	/// @brief 捕獲窗體
	/// @param bAutoRelease 自動釋放
	void Capture(bool bAutoRelease);

	/// @brief 釋放捕獲
	static void CaptureRelease();

	/// @brief 移動捕獲窗體
	/// @param Pos 捕獲點
	/// @param MinVisibility 最小分辨率
	void CaptureMove(Point Pos, int MinVisibility = 0);

	/// @brief 是否被捕獲
	inline bool Captured() const { return pWinCapture == this; }

public: // Property - CapturePoint
	/* W */ static inline void CapturePoint(Point Pos) { capturePoint = Pos; }
	/* R */ static inline Point CapturePoint() { return capturePoint; }
#pragma endregion

#pragma region Desktop
private:
	static PWObj pDesktop;
	static RGBC clDesktop;
public:

	/// @brief 桌面窗體消息事件迴調
	/// @param pWin 窗體對象
	/// @param MsgId 消息ID
	/// @param Param 消息參數
	/// @param pSrc 消息原窗體
	/// @return 消息返迴值
	static WM_RESULT DesktopCallback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public: // Property - Desktop
	/* R */ static inline PWObj Desktop() { return pDesktop; }
public: // Property - DesktopColor
	/* W */ static inline void DesktopColor(RGBC Color) {
		clDesktop = Color;
		pDesktop->Invalidate();
	}
	/* R */ static inline RGBC DesktopColor() { return clDesktop; }
#pragma endregion

#pragma region Active
private:
	static PWObj pWinActive;
public: // Property - ActiveWindow
	/* R */ static inline PWObj ActiveWindow() { return pWinActive; }
#pragma endregion

#pragma region Rect
private:
	static const SRect *pUserClip;
public:

	/// @brief 用戶動態裁剪區域
	/// @param pRect 矩形區域句柄
	/// @return 矩形區域句柄
	static const SRect *UserClip(const SRect *pRect);

	/// @brief 獲取坐標點所在得頂層窗體
	/// @param Pos 坐標點
	/// @param pStop 停止窗體
	/// @param pWin 起始窗體
	/// @return 查找到得頂層窗體
	static PWObj FindOnScreen(Point Pos, PWObj pStop = nullptr, PWObj pWin = pWinFirst);

	/// @brief 客戶坐標點轉銀幕坐標點
	/// @param ptc 客戶内坐標點
	/// @return 銀幕内坐標點
	inline Point Client2Screen(Point ptc) const { return ptc + rsWin.left_top(); }

	/// @brief 客戶區域轉銀幕區域
	/// @param rc 客戶内區域
	/// @return 銀幕内區域
	inline SRect Client2Screen(const SRect &rc) const { return rc + rsWin.left_top(); }

	/// @brief 銀幕坐標點轉客戶坐標點
	/// @param pts 客戶内坐標點
	/// @return 銀幕内坐標點
	inline Point Screen2Client(Point pts) const { return pts - rsWin.left_top(); }

	/// @brief 銀幕區域轉客戶區域
	/// @param rs 客戶内區域
	/// @return 銀幕内區域
	inline SRect Screen2Client(const SRect &rs) const { return rs - rsWin.left_top(); }

	/// @brief 父窗體坐標點轉銀幕坐標點
	/// @param ptc 父窗體内坐標點
	/// @return 銀幕内坐標點
	inline Point Parent2Screen(Point ptc) const { return pParent ? pParent->Client2Screen(ptc) : ptc; }

	/// @brief 父窗體區域轉銀幕區域
	/// @param rc 父窗體内區域
	/// @return 銀幕内區域
	inline SRect Parent2Screen(const SRect &rc) const { return pParent ? pParent->Client2Screen(rc) : rc; }

	/// @brief 銀幕坐標點轉父窗體坐標點
	/// @param ptc 父窗體内坐標點
	/// @return 銀幕内坐標點
	inline Point Screen2Parent(Point pts) const { return pParent ? pParent->Screen2Client(pts) : pts; }

	/// @brief 銀幕區域轉父窗體區域
	/// @param rs 父窗體内區域
	/// @return 銀幕内區域
	inline SRect Screen2Parent(const SRect &rs) const { return pParent ? pParent->Screen2Client(rs) : rs; }
#pragma endregion

	/// @brief 是否為子窗體
	/// @param pParent 擬定的父窗體
	inline bool IsChildOf(PWObj pParent) { return this->pParent == pParent; }

	/// @brief 是否為孫窗體
	/// @param pParent 擬定的父窗體
	bool IsAncestor(PWObj pParent) const;

	/// @brief 是否為孫窗體或自身
	/// @param pParent 擬定的父窗體
	bool IsAncestorOrSelf(PWObj pParent) const { return this == pParent ? true : IsAncestor(pParent); }

#pragma region Dialog
public: // Property - DialogStatusPtr
	/* R */ inline DIALOG_STATE *DialogStatusPtr() { return SendMessage(WM_HANDLE_DIALOG_STATUS); }
	/* W */ inline void DialogStatusPtr(DIALOG_STATE *pDialogStatus) { SendMessage(WM_HANDLE_DIALOG_STATUS, pDialogStatus); }
public:

	/// @brief 執行對話框
	/// @return 傳迴代碼
	int DialogExec();

	/// @brief 結束對話框
	/// @param code 傳迴代碼
	void DialogEnd(int code);
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
public: // Property - Focussable
	/* R */ inline bool Focussable() const { return const_cast<PWObj>(this)->SendMessage(WM_FOCUSSABLE); }
	/* W */ void Focussable(bool bFocussable);
public: // Property - StayOnTop
	/* R */ inline bool StayOnTop() const { return Status & WC_STAYONTOP; }
	/* W */ void StayOnTop(bool bEnable);
public: // Property - Visible
	/* R */ inline bool Visible() const { return Status & WC_VISIBLE; }
	/* W */ void Visible(bool bVisible);
public: // Property - RectScreen
	/* R */ inline SRect RectScreen() const { return rsWin; }
	/* W */ void RectScreen(const SRect &rsNew);
public: // Property - Rect
	/* R */ inline SRect Rect() const { return Screen2Parent(RectScreen()); }
	/* W */ inline void Rect(const SRect &rcNew) { RectScreen(Parent2Screen(rcNew)); }
public: // Property - Position
	/* R */ inline Point PositionScreen() const { return rsWin.left_top(); }
	/* W */ inline void PositionScreen(Point ptsPosition) { Move(Screen2Client(ptsPosition)); }
public: // Property - Position
	/* R */ inline Point Position() const { return Screen2Parent(PositionScreen()); }
	/* W */ inline void Position(Point ptcPosition) { PositionScreen(Parent2Screen(ptcPosition)); }
public: // Property - Size
	/* R */ inline Point Size() const { return rsWin.size(); }
	/* W */ inline void Size(Point s) { Resize(s - rsWin.size()); }
public: // Property - SizeX
	/* R */ inline uint16_t SizeX() const { return rsWin.xsize(); }
	/* W */ inline void SizeX(uint16_t xSize) { Resize({ xSize - rsWin.xsize(), 0 }); }
public: // Property - SizeY
	/* R */ inline uint16_t SizeY() const { return rsWin.ysize(); }
	/* W */ inline void SizeY(uint16_t ySize) { Resize({ 0, ySize - rsWin.ysize() }); }
public: // Property - BkColor
	/* R */ inline RGBC BkColor() const { return const_cast<PWObj>(this)->SendMessage(WM_GET_BKCOLOR); }
	/* R */ inline RGBC BkColorProp(RGBC PropColor) const { return PropColor == RGB_INVALID_COLOR ? Parent()->BkColor() : PropColor; }
public: // Property - InvalidRect
	/* R */ inline SRect InvalidRect() const { return rsInvalid; }
	/* R */ inline void InvalidRect(const SRect& r) { rsInvalid = r; }
public: // Property - InsideRect
	/* R */ inline SRect InsideRect() const { return const_cast<PWObj>(this)->SendMessage(WM_GET_INSIDE_RECT); }
public: // Property - ServeRect
	/* R */ inline SRect ServeRect() const { return const_cast<PWObj>(this)->SendMessage(WM_GET_SERVE_RECT); }
public: // Property - ClientRect
	/* R */ inline SRect ClientRect() const { return{ 0, rsWin.d() }; }
public: // Property - Client
	/* R */ inline PWObj Client() { return SendMessage(WM_GET_CLIENT_WINDOW); }
	/* R */ inline PWObj Client() const { return const_cast<PWObj>(this)->Client(); }
public: // Property - Parent
	/* R */ inline PWObj Parent() const { return pParent; }
	/* W */ inline void Parent(PWObj pParent) { Parent(pParent, Position()); }
	/* W */ void Parent(PWObj pParent, Point ptcPosition);
public: // Property - FirstChild
	/* R */ inline PWObj FirstChild() { return pFirstChild; }
	/* R */ inline PWObj FirstChild() const { return pFirstChild; }
public: // Property - FirstSibling
	/* R */ inline PWObj FirstSibling() { return pParent ? pParent->pFirstChild : nullptr; }
	/* R */ inline PWObj FirstSibling() const { return pParent ? pParent->pFirstChild : nullptr; }
public: // Property - NextSibling
	/* R */ inline PWObj NextSibling() { return pNext; }
	/* R */ inline PWObj NextSibling() const { return pNext; }
public: // Property - LastSibling
	/* R */ inline PWObj LastSibling();
public: // Property - PrevSibling
	/* R */ PWObj PrevSibling();
public: // Property - ID
	/* R */ inline uint16_t ID() const { return const_cast<PWObj>(this)->SendMessage(WM_GET_ID); }
	/* W */ inline void ID(uint16_t id) { this->SendMessage(WM_SET_ID, id); }
public: // Property - ClassName
	/* R */ inline GUI_PCSTR ClassName() const { return const_cast<PWObj>(this)->SendMessage(WM_GET_CLASS); }
public: // Property - DialogItem
	/* R */ PWObj DialogItem(uint16_t Id);
	/* R */ inline PWObj DialogItem(uint16_t Id) const { return const_cast<PWObj>(this)->DialogItem(Id); }
public: // Property - ScrollBarH
	/* R */ class ScrollBar *ScrollBarH() { return (class ScrollBar *)DialogItem(GUI_ID_HSCROLL); }
	/* W */ void ScrollBarH(bool bEnable);
public: // Property - ScrollBarV
	/* R */ class ScrollBar *ScrollBarV() { return (class ScrollBar *)DialogItem(GUI_ID_VSCROLL); }
	/* W */ void ScrollBarV(bool bEnable);
public: // Property - ScrollStateH
	/* R */ void ScrollStateH(SCROLL_STATE);
	/* W */ SCROLL_STATE ScrollStateH() const;
public: // Property - ScrollStateV
	/* R */ void ScrollStateV(SCROLL_STATE);
	/* W */ SCROLL_STATE ScrollStateV() const;
public: // Property - Callback
	/* R */ inline WM_CB Callback() const { return cb; }
	/* W */ inline void Callback(WM_CB cb) {
		this->cb = cb;
		Invalidate();
	} 
#pragma endregion

};
static inline bool IsWindow(PWObj pObj) { return pObj ? *pObj : false; }
inline PWObj WM_UNATTACHED = reinterpret_cast<PWObj>(-1);

struct FOCUS_CHANGED_STATE {
	PWObj pOld, pNew;
};

#pragma region Widget
#define WIDGET_ITEM_DRAW       0
#define WIDGET_ITEM_GET_XSIZE  1
#define WIDGET_ITEM_GET_YSIZE  2

struct WIDGET_ITEM_DRAW_INFO {
	PWObj pWin;
	/* WIDGET_ITEM_GET_XSIZE, WIDGET_ITEM_GET_YSIZE, WIDGET_ITEM_DRAW, */
	int16_t Cmd;
	int16_t ItemIndex;
	Point Pos;
};
typedef int WIDGET_DRAW_ITEM_FUNC(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo);

class Widget : public WObj {
protected:
	Widget(const SRect &rc,
		   WM_CB cb,
		   PWObj pParent, uint16_t Id = 0,
		   WM_CF Style = WC_HIDE, uint16_t StyleEx = 0) :
		WObj(rc,
			 cb,
			 pParent, Id,
			 Style, StyleEx),
		pEffect(Widget::pEffectDefault) {}

protected:
	bool HandleActive(int MsgId, WM_PARAM &Param);

protected: // Graphics
	void DrawVLine(int x, int y0, int y1) const;
	void Fill(SRect) const;
	void DrawFocus(SRect, int Dist = 0) const;

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
	static void DefaultEffect(const EffectItf &effect) { pEffectDefault = &effect; }
	static auto&DefaultEffect() { return pEffectDefault ? *pEffectDefault : EffectItf::None; }
#pragma endregion

protected:
	inline void OrState(uint16_t Flags) {
		auto nState = StatusEx | Flags;
		if (StatusEx != nState) {
			StatusEx = nState;
			Invalidate();
		}
	}
	inline void MaskState(uint16_t Mask) {
		auto nState = StatusEx & ~Mask;
		if (StatusEx != nState) {
			StatusEx = nState;
			Invalidate();
		}
	}
	inline void EnableState(bool bEnable, uint16_t Flag) {

	}

#pragma region Properties
public: // Property - Effect
	/* R */ inline auto&Effect() const { return pEffect ? *pEffect : EffectItf::None; }
	/* W */ inline void Effect(const EffectItf &effect) { SendMessage(WM_WIDGET_SET_EFFECT, &effect); }
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
extern GUI_PCSTR ClassNames[];
#pragma endregion
