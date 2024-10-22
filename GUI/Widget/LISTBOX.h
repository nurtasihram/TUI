#pragma once
#include "GUI_ARRAY.h"
#include "Widget.h"
#define LISTBOX_ALL_ITEMS  -1
#define LISTBOX_CI_UNSEL    0
#define LISTBOX_CI_SEL      1
#define LISTBOX_CI_SELFOCUS 2
#define LISTBOX_NOTIFICATION_LOST_FOCUS    (WM_NOTIFICATION_WIDGET + 0)
#define LISTBOX_CF_AUTOSCROLLBAR_H   (1<<0)
#define LISTBOX_CF_AUTOSCROLLBAR_V   (1<<1)
#define LISTBOX_CF_MULTISEL          (1<<2)
typedef struct {
	uint16_t xSize, ySize;
	uint8_t  Status;
	char     acText[1];
} LISTBOX_ITEM;
typedef struct {
	CFont   *pFont;
	uint16_t ScrollStepH;
	RGBC     aBackColor[4];
	RGBC     aTextColor[4];
} LISTBOX_PROPS;
struct ListBox_Obj : public Widget {
	GUI_ARRAY ItemArray;
	WIDGET_DRAW_ITEM_FUNC *pfDrawItem;
	WM_SCROLL_STATE ScrollStateV;
	WM_SCROLL_STATE ScrollStateH;
	LISTBOX_PROPS Props;
	WObj *pOwner;
	int16_t Sel;   /* current selection */
	uint8_t Flags;
	uint8_t  ScrollbarWidth;
	uint16_t ItemSpacing;
};
ListBox_Obj *LISTBOX_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id, const char **ppText);
#define LISTBOX_Create(ppText, x0, y0, xsize, ysize, Flags) LISTBOX_CreateEx(x0, y0, xsize, ysize, nullptr, Flags, 0, 0, ppText)
#define LISTBOX_CreateAsChild(ppText, pParent, x0, y0, xsize, ysize, Flags) LISTBOX_CreateEx(x0, y0, xsize, ysize, pParent, Flags, 0, 0, ppText)
#define LISTBOX_CreateIndirect(pCreateInfo, pParent, x0, y0, cb) \
	LISTBOX_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,    \
					 pCreateInfo->xSize, pCreateInfo->ySize,        \
					 pParent, 0, pCreateInfo->Flags, pCreateInfo->Id, 0)
int      LISTBOX_AddKey           (ListBox_Obj *pObj, int Key);
void     LISTBOX_AddString        (ListBox_Obj *pObj, const char* s);
void     LISTBOX_DeleteItem       (ListBox_Obj *pObj, unsigned int Index);
unsigned LISTBOX_GetItemSpacing   (ListBox_Obj *pObj);
unsigned LISTBOX_GetNumItems      (ListBox_Obj *pObj);
int      LISTBOX_GetSel           (ListBox_Obj *pObj);
CFont*   LISTBOX_GetFont          (ListBox_Obj *pObj);
int      LISTBOX_GetItemDisabled  (ListBox_Obj *pObj, unsigned Index);
int      LISTBOX_GetItemSel       (ListBox_Obj *pObj, unsigned Index);
void     LISTBOX_GetItemText      (ListBox_Obj *pObj, unsigned Index, char * pBuffer, int MaxSize);
int      LISTBOX_GetMulti         (ListBox_Obj *pObj);
int      LISTBOX_GetScrollStepH   (ListBox_Obj *pObj);
void     LISTBOX_MoveSel          (ListBox_Obj *pObj, int Dir);
void     LISTBOX_InsertString     (ListBox_Obj *pObj, const char* s, unsigned int Index);
void     LISTBOX_SetAutoScroll    (ListBox_Obj *pObj, int OnOff, char H0V1);
void     LISTBOX_SetBkColor       (ListBox_Obj *pObj, unsigned Index, RGBC color);
void     LISTBOX_SetFont          (ListBox_Obj *pObj, CFont* pFont);
void     LISTBOX_SetItemDisabled  (ListBox_Obj *pObj, unsigned Index, int OnOff);
void     LISTBOX_SetItemSel       (ListBox_Obj *pObj, unsigned Index, int OnOff);
void     LISTBOX_SetItemSpacing   (ListBox_Obj *pObj, unsigned Value);
void     LISTBOX_SetMulti         (ListBox_Obj *pObj, int Mode);
void     LISTBOX_SetOwner         (ListBox_Obj *pObj, WObj *pOwner);
void     LISTBOX_SetOwnerDraw     (ListBox_Obj *pObj, WIDGET_DRAW_ITEM_FUNC * pfDrawItem);
void     LISTBOX_SetScrollStepH   (ListBox_Obj *pObj, int Value);
void     LISTBOX_SetSel           (ListBox_Obj *pObj, int Sel);
void     LISTBOX_SetScrollbarWidth(ListBox_Obj *pObj, unsigned Width);
void     LISTBOX_SetString        (ListBox_Obj *pObj, const char *s, unsigned int Index);
void     LISTBOX_SetText          (ListBox_Obj *pObj, const char **ppText);
RGBC     LISTBOX_SetTextColor     (ListBox_Obj *pObj, unsigned int Index, RGBC Color);
int      LISTBOX_UpdateScrollers  (ListBox_Obj *pObj);
void     LISTBOX_InvalidateItem   (ListBox_Obj *pObj, int Index);
int      LISTBOX_OwnerDraw        (const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);
CFont* LISTBOX_GetDefaultFont       (void);
int    LISTBOX_GetDefaultScrollStepH(void);
RGBC   LISTBOX_GetDefaultBkColor    (unsigned Index);
RGBC   LISTBOX_GetDefaultTextColor  (unsigned Index);
void   LISTBOX_SetDefaultFont       (CFont* pFont);
void   LISTBOX_SetDefaultScrollStepH(int Value);
void   LISTBOX_SetDefaultBkColor    (unsigned Index, RGBC Color);
void   LISTBOX_SetDefaultTextColor  (unsigned Index, RGBC Color);
#define  LISTBOX_IncSel(pObj)     LISTBOX_MoveSel(pObj, 1)
#define  LISTBOX_DecSel(pObj)     LISTBOX_MoveSel(pObj, -1)
#define  LISTBOX_SetAutoScrollH(pObj, OnOff) LISTBOX_SetAutoScroll(pObj, OnOff, 0)
#define  LISTBOX_SetAutoScrollV(pObj, OnOff) LISTBOX_SetAutoScroll(pObj, OnOff, 1)
#define LISTBOX_SetBackColor(pObj, Index, Color) LISTBOX_SetBkColor(pObj, Index, Color)
#define LISTBOX_DeleteString  LISTBOX_DeleteItem
#define LISTBOX_Delete(pObj)        WM_DeleteWindow(pObj)
#define LISTBOX_Paint(pObj)         WM_Paint(pObj)
#define LISTBOX_Invalidate(pObj)    pObj->Invalidate()
// private
unsigned    LISTBOX__GetNumItems           (const ListBox_Obj *pObj);
const char *LISTBOX__GetpString            (const ListBox_Obj *pObj, int Index);
void        LISTBOX__InvalidateInsideArea  (ListBox_Obj *pObj);
void        LISTBOX__InvalidateItem        (ListBox_Obj *pObj, int Sel);
void        LISTBOX__InvalidateItemAndBelow(ListBox_Obj *pObj, int Sel);
void        LISTBOX__InvalidateItemSize    (const ListBox_Obj *pObj, unsigned Index);
void        LISTBOX__SetScrollbarWidth     (ListBox_Obj *pObj);
