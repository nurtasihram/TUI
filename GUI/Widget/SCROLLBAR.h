#pragma once
#include "Widget.h"
#define SCROLLBAR_STATE_PRESSED   WIDGET_STATE_USER0
#define SCROLLBAR_CF_VERTICAL     WIDGET_CF_VERTICAL
#define SCROLLBAR_CF_FOCUSSABLE   WIDGET_STATE_FOCUSSABLE
struct ScrollBar_Obj : public Widget {
	RGBC aBkColor[2];
	RGBC aColor[2];
	int NumItems, v, PageSize;
};
typedef struct {
	int x0_LeftArrow;
	int x1_LeftArrow;
	int x0_Thumb;
	int x1_Thumb;
	int x0_RightArrow;
	int x1_RightArrow;
	int x1;
	int xSizeMoveable;
	int ThumbSize;
} SCROLLBAR_POSITIONS;
ScrollBar_Obj *SCROLLBAR_CreateEx      (int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id);
ScrollBar_Obj *SCROLLBAR_CreateIndirect(const GUI_WIDGET_CREATE_INFO* pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK* cb);
ScrollBar_Obj *SCROLLBAR_CreateAttached(WObj *pParent, int SpecialFlags);
#define SCROLLBAR_Create(x0, y0, xsize, ysize, pParent, Id, WinFlags, SpecialFlags) \
	SCROLLBAR_CreateEx(x0, y0, xsize, ysize, pParent, WinFlags, SpecialFlags, Id)
void SCROLLBAR_AddValue   (ScrollBar_Obj *pObj, int Add);
void SCROLLBAR_Dec        (ScrollBar_Obj *pObj);
void SCROLLBAR_Inc        (ScrollBar_Obj *pObj);
void SCROLLBAR_SetNumItems(ScrollBar_Obj *pObj, int NumItems);
void SCROLLBAR_SetPageSize(ScrollBar_Obj *pObj, int PageSize);
void SCROLLBAR_SetValue   (ScrollBar_Obj *pObj, int v);
void SCROLLBAR_SetWidth   (ScrollBar_Obj *pObj, int Width);
void SCROLLBAR_SetState   (ScrollBar_Obj *pObj, const WM_SCROLL_STATE* pState);
RGBC SCROLLBAR_GetDefaultBkColor(unsigned int Index); /* Not yet documented */
RGBC SCROLLBAR_GetDefaultColor  (unsigned int Index); /* Not yet documented */
int  SCROLLBAR_GetDefaultWidth  (void);
RGBC SCROLLBAR_SetDefaultBkColor(RGBC Color, unsigned int Index); /* Not yet documented */
RGBC SCROLLBAR_SetDefaultColor  (RGBC Color, unsigned int Index); /* Not yet documented */
int  SCROLLBAR_SetDefaultWidth  (int DefaultWidth);
int  SCROLLBAR_GetValue(ScrollBar_Obj *pObj);
// private
void SCROLLBAR__InvalidatePartner(ScrollBar_Obj *pObj);
