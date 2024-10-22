#pragma once
#include "GUI.h"
/*********************************************************************
*
*               Messages Ids
* 
*/
#define WM_CREATE                   0x0001
#define WM_MOVE                     0x0003
#define WM_SIZE                     0x0005
#define WM_DELETE                   11
#define WM_TOUCH                    12
#define WM_TOUCH_CHILD              13
#define WM_KEY                      14
#define WM_PAINT                    15
#define WM_MOUSEOVER                16
#define WM_PID_STATE_CHANGED        17
#define WM_GET_INSIDE_RECT          20
#define WM_GET_ID                   21
#define WM_SET_ID                   22
#define WM_GET_CLIENT_WINDOW        23
#define WM_CAPTURE_RELEASED         24
#define WM_INIT_DIALOG              30
#define WM_SET_FOCUS                31
#define WM_GET_ACCEPT_FOCUS         32
#define WM_NOTIFY_CHILD_HAS_FOCUS   33
#define WM_GET_BKCOLOR              34
#define WM_GET_SCROLL_STATE         35
#define WM_SET_SCROLL_STATE         36
#define WM_NOTIFY_CLIENTCHANGE      37
#define WM_NOTIFY_PARENT            38
#define WM_NOTIFY_PARENT_REFLECTION 39
#define WM_NOTIFY_ENABLE            40
#define WM_NOTIFY_VIS_CHANGED       41
#define WM_HANDLE_DIALOG_STATUS     42
#define WM_GET_RADIOGROUP           43
#define WM_MENU                     44
#define WM_TIMER                    0x0113
#define WM_WIDGET                   0x0300
#define WM_USER                     0x0400
#define WM_NOTIFICATION_CLICKED             1
#define WM_NOTIFICATION_RELEASED            2
#define WM_NOTIFICATION_MOVED_OUT           3
#define WM_NOTIFICATION_SEL_CHANGED         4
#define WM_NOTIFICATION_VALUE_CHANGED       5
#define WM_NOTIFICATION_SCROLLBAR_ADDED     6
#define WM_NOTIFICATION_CHILD_DELETED       7
#define WM_NOTIFICATION_GOT_FOCUS           8
#define WM_NOTIFICATION_LOST_FOCUS          9
#define WM_NOTIFICATION_SCROLL_CHANGED     10
#define WM_NOTIFICATION_WIDGET             11
#define WM_NOTIFICATION_USER               16
#define WM_UNATTACHED  ((WObj*)-1) /* Do not attach to a window */
#define WC_HIDE             0
#define WC_VISIBLE             (1 <<  0)
#define WC_STAYONTOP        (1 <<  1)
#define WC_DISABLED         (1 <<  2)
#define WC_ACTIVATE         (1 <<  3)
#define WC_NOACTIVATE       (1 <<  4)
#define WC_BGND             (1 <<  5)
#define WC_ANCHOR_RIGHT     (1 <<  6)
#define WC_ANCHOR_BOTTOM    (1 <<  7)
#define WC_ANCHOR_LEFT      (1 <<  8)
#define WC_ANCHOR_TOP       (1 <<  9)
#define WC_CONST_OUTLINE    (1 << 10)
#define WC_LATE_CLIP        (1 << 11)
typedef struct {
	int Key, PressedCnt;
} WM_KEY_INFO;
typedef struct {
	int NumItems, v, PageSize;
} WM_SCROLL_STATE;
typedef struct {
	int Done, ReturnValue;
} WM_DIALOG_STATUS;
typedef struct {
	int x, y;
	uint8_t State, StatePrev;
} WM_PID_STATE_CHANGED_INFO;
typedef struct {
	int MsgId;
	struct WObj *pWin, *pWinSrc;
	size_t Data;
} WM_MESSAGE;
typedef void WM_CALLBACK(WM_MESSAGE*);
struct WObj {
	SRect rect, rInvalid;
	WM_CALLBACK* cb;
	struct WObj
		*pNextLin,
		*pParent,
		*pFirstChild,
		*pNext;
	uint16_t Status;

private:
	void _InsertWindowIntoList(WObj *pParent);
	void _RemoveWindowFromList();

	static WObj *pWinFirst;
	void _RemoveFromLinList();
	void _AddToLinList();

public:
	operator bool() const;

public:
	static WObj *Create(int x0, int y0, int xSize, int ySize, WObj *pParent, uint16_t Style, WM_CALLBACK *cb, int NumExtraBytes);
	static inline WObj*Create(int x0, int y0, int width, int height, uint16_t Style, WM_CALLBACK *cb, int NumExtraBytes) 
	{ return Create(x0, y0, width, height, 0, Style, cb, NumExtraBytes); }

	void Delete();
	void Select();

	static bool OnKey(int Key, int Pressed);

#pragma region IVR
private:
	struct ContextIVR {
		SRect ClientRect;
		SRect CurRect;
		int Cnt = 0;
		int EntranceCnt = 0;
	};
	static ContextIVR _ClipContext;
	static void  _ActivateClipRect();
	static bool _FindNextIVR();
public:
	static bool GetNextIVR();
	static bool InitSearchIVR(const SRect *pMaxRect = nullptr);
#pragma endregion

	static const SRect *SetUserClipRect(const SRect *pRect);

#pragma region Invalidate & Validate
private:
	void _Invalidate1Abs(SRect r);
public:
	static void InvalidateArea(const SRect &r);
	void Invalidate(const SRect *pRect = nullptr);
	void InvalidateDescs();
	void Validate();
#pragma endregion

#pragma region Paint
private:
	void _Paint1();
	bool _Paint();
public:
	void Paint();
	static bool PaintNext();
#pragma endregion

public:
	static bool HandlePID();

	static void Init();

	static bool Exec1();
	static inline void Exec() { while (Exec1()) {} }

	inline void SendMessage(WM_MESSAGE *pMsg) {
		if (cb) {
			pMsg->pWin = this;
			cb(pMsg);
		}
	}

	void BringToTop();
	void BringToBottom();

	void Detach();
	void Move(Point dPos);
	void Resize(Point dSize);

	static WObj *ScreenToWin(Point Pos, WObj *pStop = nullptr, WObj *pWin = pWinFirst);

private: // Action - Focus
	static WObj *pWinFocus;
public:
	/* A */ WObj *Focus();
	/* S */ inline bool Focussed() const { return this == pWinFocus; }
	/* S */ inline WObj *FocussedChild() { return pWinFocus->IsChildOf(this) ? pWinFocus : nullptr; }
	/* S */ inline bool Focussable() const {
		WM_MESSAGE msg;
		msg.Data = 0;
		msg.MsgId = WM_GET_ACCEPT_FOCUS;
		const_cast<WObj *>(this)->SendMessage(&msg);
		return (bool)msg.Data;
	}

public:
	inline bool IsChildOf(WObj *pParent) { return this->pParent == pParent; }

#pragma region Properties
public: // Property - Styles
	/* R */ inline auto Styles() const { return Status; }
public: // Property - Enable
	/* W */ void Enable(bool bEnable);
	/* R */ inline auto Enable() const { return !(Status & WC_DISABLED); }
public: // Property - StayOnTop
	/* W */ void StayOnTop(bool bEnable);
	/* R */ inline bool StayOnTop() const { return Status & WC_STAYONTOP; }
public: // Property - Visible
	/* W */ void Visible(bool bVisible);
	/* R */ inline bool Visible() const { return Status & WC_VISIBLE; }
public: // Property - SRect
	/* W */ inline void Rect(const SRect &r) {
		Position(r.left_top());
		Size(r.size());
	}
	/* R */ inline auto Rect() const { return rect; }
public: // Property - Size
	/* W */ inline void Size(Point s) { Resize(s - rect.size()); }
	/* R */ inline auto Size() const { return rect.size(); }
public: // Property - SizeX
	/* W */ inline void SizeX(int16_t xSize) { Resize({ xSize - rect.xsize(), 0 }); }
	/* R */ inline auto SizeX() const { return rect.xsize(); }
public: // Property - SizeY
	/* W */ inline void SizeY(int16_t ySize) { Resize({ 0, ySize - rect.ysize() }); }
	/* R */ inline auto SizeY() const { return rect.ysize(); }
public: // Property - Position
	/* W */ inline void Position(Point p) { Move(p - rect.left_top()); }
	/* R */ inline auto Position() const { return rect.left_top(); }
public: // Property - ID
	/* W */ inline void ID(int id) {
		WM_MESSAGE msg;
		msg.MsgId = WM_SET_ID;
		msg.Data = id;
		this->SendMessage(&msg);
	}
	/* R */ inline int ID() const {
		WM_MESSAGE msg;
		msg.MsgId = WM_GET_ID;
		const_cast<WObj *>(this)->SendMessage(&msg);
		return (int)msg.Data;
	}
public: // Property - InsideRect
	/* R */ SRect InsideRect() const;
	public: // Property - ClientRect
	/* R */ inline SRect ClientRect() const { return{ 0, rect.right_bottom() - rect.left_top() }; }
public: // Property - Parent
	/* W */ void Parent(WObj *pParent);
	/* W */ void Parent(WObj *pParent, Point p);
	/* R */ inline auto Parent() { return pParent; }
	/* R */ inline auto Parent() const { return pParent; }
public: // Property - FirstChild
	/* R */ inline auto FirstChild() { return pFirstChild; }
	/* R */ inline auto FirstChild() const { return pFirstChild; }
public: // Property - FirstSibling
	/* R */ inline auto FirstSibling() { return pParent ? pParent->pFirstChild : nullptr; }
	/* R */ inline auto FirstSibling() const { return pParent ? pParent->pFirstChild : nullptr; }
public: // Property - NextSibling
	/* R */ inline auto NextSibling() { return pNext; }
	/* R */ inline auto NextSibling() const { return pNext; }
public: // Property - LastSibling
	/* R */ inline WObj *LastSibling();
public: // Property - PrevSibling
	/* R */ WObj *PrevSibling();
#pragma endregion

};
static inline bool IsWindow(const WObj *pObj) { return pObj ? *pObj : false; }

void WM_Activate(void);
void WM_Deactivate(void);

void WM_SetCapture(WObj *pWin, int AutoRelease);
void WM_SetCaptureMove(WObj *pWin, const PidState* pState, int MinVisibility);
void WM_ReleaseCapture(void);

int     WM_CheckScrollPos(WM_SCROLL_STATE* pScrollState, int Pos, int LowerDist, int UpperDist);
bool    WM_GetInvalidRect(WObj *pWin, SRect* pRect);
void    WM_SetAnchor(WObj *pWin, uint16_t AnchorFlags);
void WM_MoveChildTo(WObj *pWin, int x, int y);
int  WM_SetScrollbarH(WObj *pWin, int OnOff);
int  WM_SetScrollbarV(WObj *pWin, int OnOff);
int WM_GetNumWindows(void);
int WM_GetNumInvalidWindows(void);
int  WM_SetScrollValue(WM_SCROLL_STATE* pScrollState, int v);
void WM_CheckScrollBounds(WM_SCROLL_STATE* pScrollState);
WM_CALLBACK* WM_SetCallback(WObj *pWin, WM_CALLBACK* cb);
void WM_GetClientRect(SRect* pRect);
void WM_GetInsideRect(SRect* pRect);
void WM_GetInsideRectExScrollbar(WObj *pWin, SRect* pRect);
void WM_GetWindowRect(SRect* pRect);
struct ScrollBar_Obj *WM_GetScrollbarV(WObj *pWin);
struct ScrollBar_Obj *WM_GetScrollbarH(WObj *pWin);
WObj *WM_GetScrollPartner(WObj *pWin);
WObj *WM_GetClientWindow(WObj *pWin);
RGBC    WM_GetBkColor(WObj *pWin);
RGBC WM_SetDesktopColor(RGBC Color);
WObj *WM_GetActiveWindow(void);
WObj *WM_GetDesktopWindow(void);
void        WM_SetDefault(void);
void WM_MakeModal(WObj *pWin);
void      WM_NotifyParent(WObj *pWin, int Notification);
void      WM_SendMessageNoPara(WObj *pWin, int MsgId);
void      WM_DefaultProc(WM_MESSAGE *pMsg);
void      WM_SetScrollState(WObj *pWin, const WM_SCROLL_STATE* pState);
void      WM_SendToParent(WObj *pWin, WM_MESSAGE *pMsg);
bool      WM_HasCaptured(WObj *pWin);
WObj *  WM_SetFocusOnNextChild(WObj *pParent);
WObj *  WM_GetDialogItem(WObj *pWin, int Id);
void      WM_GetScrollState(WObj *pWin, WM_SCROLL_STATE* pScrollState);

typedef struct {
	WObj *pOld, *pNew;
} WM_NOTIFY_CHILD_HAS_FOCUS_INFO;
struct WM_CRITICAL_HANDLE {
	WM_CRITICAL_HANDLE *pNext;
	WObj *pWin;
};
extern WObj *WM__pCapture;
extern char          WM__CaptureReleaseAuto;
extern uint8_t       WM__PaintCallbackCnt;      /* Public for assertions only */
extern PidState      WM_PID__StateLast;
extern WM_CRITICAL_HANDLE     WM__CHWinModal;
extern WM_CRITICAL_HANDLE     WM__CHWinLast;
#ifdef WM_C
#	define GUI_EXTERN
#else
#	define GUI_EXTERN extern
#endif
GUI_EXTERN uint16_t WM__NumWindows;
GUI_EXTERN uint16_t NumInvalidWindows;
GUI_EXTERN WM_CRITICAL_HANDLE *WM__pFirstCriticalHandle;
GUI_EXTERN WObj *WM__ahDesktopWin;
GUI_EXTERN RGBC    WM__aBkColor;
#undef GUI_EXTERN

bool    WM__ClipAtParentBorders(SRect *pRect, WObj *pWin);
void    WM__Client2Screen(const WObj *pWin, SRect *pRect);
void    WM__GetClientRectWin(const WObj *pWin, SRect *pRect);
bool    WM__IsAncestor(WObj *pChild, WObj *pParent);
bool    WM__IsAncestorOrSelf(WObj *pChild, WObj *pParent);
bool    WM__IsInModalArea(WObj *pWin);
void    WM__NotifyVisChanged(WObj *pWin, SRect *pRect);
void    WM__Screen2Client(const WObj *pWin, SRect *pRect);
void    WM__SendMsgNoData(WObj *pWin, uint8_t MsgId);
void    WM__SendMessage(WObj *pWin, WM_MESSAGE *pm);
void    WM__SendMessageIfEnabled(WObj *pWin, WM_MESSAGE *pm);
void    WM_SendMessageNoPara(WObj *pWin, int MsgId);
void    WM__SendPIDMessage(WObj *pWin, WM_MESSAGE *pMsg);
int     WM_SetScrollbarH(WObj *pWin, int OnOff);
int     WM_SetScrollbarV(WObj *pWin, int OnOff);
void    WM__UpdateChildPositions(WObj *pObj, int dx0, int dy0, int dx1, int dy1);
void    WM_PID__GetPrevState(PidState *pPrevState);
void    WM__SendTouchMessage(WObj *pWin, WM_MESSAGE *pMsg);
uint16_t WM_GetFlags(WObj *pWin);
void    WM__AddCriticalHandle(WM_CRITICAL_HANDLE *pCH);
void    WM__RemoveCriticalHandle(WM_CRITICAL_HANDLE *pCH);

#define WM_ITERATE_START(...) \
	if (WObj::InitSearchIVR(__VA_ARGS__)) \
		do {
#define WM_ITERATE_END() \
		} while (WObj::GetNextIVR());
#define WM_ADDORGX(x)    x += GUI.xOff
#define WM_ADDORGY(y)    y += GUI.yOff
#define WM_ADDORG(x0,y0) WM_ADDORGX(x0); WM_ADDORGY(y0)
#define WM_SUBORGX(x)    x -= GUI.xOff
#define WM_SUBORGY(y)    y -= GUI.yOff
#define WM_SUBORG(x0,y0) WM_SUBORGX(x0); WM_SUBORGY(y0)
