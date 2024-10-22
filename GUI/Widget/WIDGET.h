#pragma once
#include "WM.h"  /* Window manager, including some internals, which speed things up */
#define WIDGET_STATE_FOCUS              (1<<0)
#define WIDGET_STATE_VERTICAL           (1<<1)
#define WIDGET_STATE_FOCUSSABLE         (1<<2)
#define WIDGET_STATE_USER0              (1<<3)
#define WIDGET_STATE_USER1              (1<<4)
#define WIDGET_STATE_USER2              (1<<5)
#define WIDGET_ITEM_DRAW                0
#define WIDGET_ITEM_GET_XSIZE           1
#define WIDGET_ITEM_GET_YSIZE           2
#define WM_WIDGET_SET_EFFECT    WM_WIDGET + 0
#define WIDGET_CF_VERTICAL      WIDGET_STATE_VERTICAL
typedef struct {
	WObj *pWin;
	int     Cmd;         /* WIDGET_ITEM_GET_XSIZE, WIDGET_ITEM_GET_YSIZE, WIDGET_ITEM_DRAW, */
	int     ItemIndex;
	int     x0, y0;
} WIDGET_ITEM_DRAW_INFO;
typedef int WIDGET_DRAW_ITEM_FUNC(const WIDGET_ITEM_DRAW_INFO* pDrawItemInfo);
struct Widget : public WObj {
	struct Effect {
		int EffectSize;
		Effect(int EffectSize = 0) : EffectSize(EffectSize) {}
		virtual void DrawUp() const {}
		virtual void DrawDown() const {}
		virtual void DrawUpRect(const SRect &r) const {}
		virtual void DrawDownRect(const SRect &r) const {}
		static const Effect None;
		static const Effect &&Simple;
		static const Effect &&D3;
		static const Effect &&D3L1;
		static const Effect &&D3L2;
	} const *pEffect;
	static const Effect *pEffectDefault;
	int Id;
	int State;
public: // Property - 
public: // Property - DefaultEffect
	static void DefaultEffect(const Effect *pEffect) { pEffectDefault = pEffect; }
	static const Effect *DefaultEffect() { return pEffectDefault; }
};
typedef struct  GUI_WIDGET_CREATE_INFO_struct GUI_WIDGET_CREATE_INFO;
typedef WObj* GUI_WIDGET_CREATE_FUNC(const GUI_WIDGET_CREATE_INFO* pCreate, WObj *pWin, int x0, int y0, WM_CALLBACK* cb);
struct GUI_WIDGET_CREATE_INFO_struct {
	GUI_WIDGET_CREATE_FUNC* pfCreateIndirect;
	const char* pName;                     /* Text ... Not used on all widgets */
	int16_t Id;                            /* ID ... should be unique in a dialog */
	int16_t x0, y0, xSize, ySize;          /* Define position and size */
	uint16_t Flags;                        /* Widget specific create flags (opt.) */
	int32_t Para;                          /* Widget specific parameter (opt.) */
};
/*********************************************************************
*
*         GUI_DRAW
*
* The GUI_DRAW object is used as base class for selfdrawing,
* non-windows objects. They are used as content of different widgets,
* such as the bitmap or header widgets.
*/
/* Declare Object struct */
typedef struct GUI_DRAW GUI_DRAW;
typedef void   GUI_DRAW_SELF_CB(void);
/* Declare Object constants (member functions etc)  */
typedef struct {
	void (*pfDraw)    (const GUI_DRAW* pObj, int x, int y);
	int  (*pfGetXSize)(const GUI_DRAW* pObj);
	int  (*pfGetYSize)(const GUI_DRAW* pObj);
} GUI_DRAW_CONSTS;
/* Declare Object */
struct GUI_DRAW {
	const GUI_DRAW_CONSTS* pConsts;
	union {
		const void* pData;
		GUI_DRAW_SELF_CB* pfDraw;
	} Data;
	int16_t xOff, yOff;
};
/* GUI_DRAW_ API */
void GUI_DRAW__Draw(GUI_DRAW *pDrawObj, int x, int y);
int  GUI_DRAW__GetXSize(GUI_DRAW *pDrawObj);
int  GUI_DRAW__GetYSize(GUI_DRAW *pDrawObj);
/* GUI_DRAW_ Constructurs for different objects */
GUI_DRAW *GUI_DRAW_BITMAP_Create(const Bitmap* pBitmap, int x, int y);
GUI_DRAW *GUI_DRAW_SELF_Create(GUI_DRAW_SELF_CB* pfDraw, int x, int y);
void WIDGET__DrawFocusRect(Widget* pWidget, const SRect* pRect, int Dist);
void WIDGET__DrawVLine(Widget* pWidget, int x, int y0, int y1);
void WIDGET__EFFECT_DrawDownRect(Widget* pWidget, SRect* pRect);
void WIDGET__EFFECT_DrawDown(Widget* pWidget);
void WIDGET__EFFECT_DrawUpRect(Widget* pWidget, SRect* pRect);
void WIDGET__FillRectEx(Widget* pWidget, const SRect* pRect);
int  WIDGET__GetWindowSizeX(Widget *pWidget);
RGBC WIDGET__GetBkColor(Widget *pWidget);
int  WIDGET__GetXSize(const Widget* pWidget);
int  WIDGET__GetYSize(const Widget* pWidget);
void WIDGET__GetClientRect(Widget* pWidget, SRect* pRect);
void WIDGET__GetInsideRect(Widget* pWidget, SRect* pRect);
void WIDGET__Init(Widget* pWidget, int Id, uint16_t State);
void WIDGET__RotateRect90(Widget* pWidget, SRect* pDest, const SRect* pRect);
void WIDGET__SetScrollState(Widget *pWidget, const WM_SCROLL_STATE* pVState, const WM_SCROLL_STATE* pState);
void WIDGET__FillStringInRect(const char * pText, const SRect* pFillRect, const SRect* pTextRectMax, const SRect* pTextRectAct);
void WIDGET_SetState(Widget *pWidget, int State);
void WIDGET_AndState(Widget *pWidget, int State);
void WIDGET_OrState(Widget *pWidget, int State);
int  WIDGET_GetState(Widget *pWidget);
void WIDGET_SetWidth(Widget *pWidget, int Width);
void WIDGET_SetEffect(Widget *pWidget, const Widget::Effect* pEffect);
int  WIDGET_HandleActive(Widget *pWidget, WM_MESSAGE *pMsg);
