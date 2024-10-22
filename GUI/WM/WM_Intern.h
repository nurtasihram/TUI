#pragma once
#include "WM.h"
#include "GUI_Protected.h"       /* For GUI */
/*********************************************************************
*
*       defines
*
*********************************************************************/
#define WM_SF_DISABLED          WM_CF_DISABLED  /* Disabled: Does not receive PID (mouse & touch) input */
#define WM_SF_ISVIS             WM_CF_SHOW      /* Is visible flag */
#define WM_SF_STAYONTOP         WM_CF_STAYONTOP
#define WM_SF_LATE_CLIP         WM_CF_LATE_CLIP
#define WM_SF_ANCHOR_RIGHT      WM_CF_ANCHOR_RIGHT
#define WM_SF_ANCHOR_BOTTOM     WM_CF_ANCHOR_BOTTOM
#define WM_SF_ANCHOR_LEFT       WM_CF_ANCHOR_LEFT
#define WM_SF_ANCHOR_TOP        WM_CF_ANCHOR_TOP
#define WM_SF_INVALID           WM_CF_ACTIVATE      /* We reuse this flag, as it is create only and Invalid is status only */
#define WM_SF_CONST_OUTLINE     WM_CF_CONST_OUTLINE /* Constant outline.*/
#define WM_H2P(hWin)               asasdasdasdasd
#if GUI_DEBUG_LEVEL  >= GUI_DEBUG_LEVEL_LOG_WARNINGS
#	define WM_ASSERT_NOT_IN_PAINT() { \
		if (WM__PaintCallbackCnt)     \
			GUI_DEBUG_ERROROUT("Function may not be called from within a paint event"); \
	}
#else
#	define WM_ASSERT_NOT_IN_PAINT()
#endif
typedef struct {
	WM_Obj *pOld, *pNew;
} WM_NOTIFY_CHILD_HAS_FOCUS_INFO;
typedef struct WM_CRITICAL_HANDLE {
	struct  WM_CRITICAL_HANDLE* pNext;
	WM_Obj *pWin;
} WM_CRITICAL_HANDLE;
extern WM_Obj *WM__pCapture;
extern WM_Obj *WM__pWinFocus;
extern char          WM__CaptureReleaseAuto;
extern WM_tfPollPID* WM_pfPollPID;
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
GUI_EXTERN uint16_t WM__NumInvalidWindows;
GUI_EXTERN WM_Obj *WM__FirstWin;
GUI_EXTERN WM_CRITICAL_HANDLE *WM__pFirstCriticalHandle;
GUI_EXTERN WM_Obj *WM__ahDesktopWin;
GUI_EXTERN RGBC    WM__aBkColor;
#undef GUI_EXTERN
void    WM__ActivateClipRect(void);
bool    WM__ClipAtParentBorders(Rect* pRect, WM_Obj *pWin);
void    WM__Client2Screen(const WM_Obj* pWin, Rect* pRect);
void    WM__DetachWindow(WM_Obj *pChild);
void    WM_ForEachDesc(WM_Obj *pWin, WM_tfForEach* pcb, void* pData);
void    WM__GetClientRectWin(const WM_Obj* pWin, Rect* pRect);
WM_Obj *WM__GetFirstSibling(WM_Obj *pWin);
WM_Obj *WM__GetFocussedChild(WM_Obj *pWin);
WM_Obj *WM__GetLastSibling(WM_Obj *pWin);
WM_Obj *WM_GetPrevSibling(WM_Obj *pWin);
int     WM__GetWindowSizeX(const WM_Obj* pWin);
int     WM__GetWindowSizeY(const WM_Obj* pWin);
void    WM__InsertWindowIntoList(WM_Obj *pWin, WM_Obj *pParent);
bool    WM__IsAncestor(WM_Obj *pChild, WM_Obj *pParent);
bool    WM__IsAncestorOrSelf(WM_Obj *pChild, WM_Obj *pParent);
bool    WM__IsChild(WM_Obj *pWin, WM_Obj *pParent);
bool    WM_IsEnabled(WM_Obj *pWin);
bool    WM__IsInModalArea(WM_Obj *pWin);
bool    WM__IsInWindow(WM_Obj* pWin, int x, int y);
bool    WM_IsWindow(WM_Obj *pWin);
void    WM_MoveTo(WM_Obj *pWin, int x, int y);
void    WM_MoveWindow(WM_Obj *pWin, int dx, int dy);
void    WM__NotifyVisChanged(WM_Obj *pWin, Rect* pRect);
bool    WM__RectIsNZ(const Rect* pr);
void    WM__RemoveWindowFromList(WM_Obj *pWin);
void    WM__RemoveFromLinList(WM_Obj *pWin);
void    WM__Screen2Client(const WM_Obj* pWin, Rect* pRect);
void    WM__SendMsgNoData(WM_Obj *pWin, uint8_t MsgId);
void    WM__SendMessage(WM_Obj *pWin, WM_MESSAGE* pm);
void    WM__SendMessageIfEnabled(WM_Obj *pWin, WM_MESSAGE* pm);
void    WM_SendMessageNoPara(WM_Obj *pWin, int MsgId);
void    WM__SendPIDMessage(WM_Obj *pWin, WM_MESSAGE* pMsg);
int     WM_SetScrollbarH(WM_Obj *pWin, int OnOff);
int     WM_SetScrollbarV(WM_Obj *pWin, int OnOff);
void    WM__UpdateChildPositions(WM_Obj* pObj, int dx0, int dy0, int dx1, int dy1);
void    WM_PID__GetPrevState(PidState* pPrevState);
void    WM__SendTouchMessage(WM_Obj *pWin, WM_MESSAGE* pMsg);
uint16_t WM_GetFlags(WM_Obj *pWin);
void    WM__AddCriticalHandle(WM_CRITICAL_HANDLE* pCH);
void    WM__RemoveCriticalHandle(WM_CRITICAL_HANDLE* pCH);
