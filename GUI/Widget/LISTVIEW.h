#pragma once
#include "HEADER.h"
#include "Widget.h"
#define LISTVIEW_CI_UNSEL    0
#define LISTVIEW_CI_SEL      1
#define LISTVIEW_CI_SELFOCUS 2
typedef struct {
	RGBC aBkColor[3];
	RGBC aTextColor[3];
} LISTVIEW_ITEM_INFO;
typedef struct {
	LISTVIEW_ITEM_INFO *pItemInfo;
	char acText[1];
} LISTVIEW_ITEM;
typedef struct {
	RGBC   aBkColor[3];
	RGBC   aTextColor[3];
	RGBC   GridColor;
	CFont *pFont;
} LISTVIEW_PROPS;
struct ListView_Obj : public Widget {
	Header_Obj  *pHeader;
	GUI_ARRAY       RowArray;         /* One entry per line. Every entry is a handle of GUI_ARRAY of strings */
	GUI_ARRAY       AlignArray;       /* One entry per column */
	LISTVIEW_PROPS  Props;
	int             Sel;
	int             ShowGrid;
	unsigned        RowDistY;
	unsigned        LBorder, RBorder;
	WM_SCROLL_STATE ScrollStateV, ScrollStateH;
	WObj         *pOwner;
};
ListView_Obj *LISTVIEW_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id);
#define LISTVIEW_Create(x0, y0, xsize, ysize, pParent, Id, Flags, ExFlags) LISTVIEW_CreateEx(x0, y0, xsize, ysize, pParent, Flags, ExFlags, Id)
#define LISTVIEW_CreateAttached(pParent, Id, SpecialFlags) LISTVIEW_CreateEx(0, 0, 0, 0, pParent, WC_VISIBLE, SpecialFlags, Id)
#define LISTVIEW_CreateIndirect(pCreateInfo, pParent, x0, y0, cb)       \
	LISTVIEW_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,          \
					  pCreateInfo->xSize, pCreateInfo->ySize,              \
					  pParent, 0, pCreateInfo->Flags, pCreateInfo->Id)
void          LISTVIEW_AddColumn       (ListView_Obj *pObj, int Width, const char * s, int Align);
void          LISTVIEW_AddRow          (ListView_Obj *pObj, const char **ppText);
void          LISTVIEW_DecSel          (ListView_Obj *pObj);
void          LISTVIEW_DeleteColumn    (ListView_Obj *pObj, unsigned Index);
void          LISTVIEW_DeleteRow       (ListView_Obj *pObj, unsigned Index);
RGBC          LISTVIEW_GetBkColor      (ListView_Obj *pObj, unsigned Index);
CFont*        LISTVIEW_GetFont         (ListView_Obj *pObj);
Header_Obj   *LISTVIEW_GetHeader       (ListView_Obj *pObj);
unsigned      LISTVIEW_GetNumColumns   (ListView_Obj *pObj);
unsigned      LISTVIEW_GetNumRows      (ListView_Obj *pObj);
int           LISTVIEW_GetSel          (ListView_Obj *pObj);
RGBC          LISTVIEW_GetTextColor    (ListView_Obj *pObj, unsigned Index);
void          LISTVIEW_IncSel          (ListView_Obj *pObj);
void          LISTVIEW_SetBkColor      (ListView_Obj *pObj, unsigned int Index, RGBC Color);
void          LISTVIEW_SetColumnWidth  (ListView_Obj *pObj, unsigned int Index, int Width);
void          LISTVIEW_SetFont         (ListView_Obj *pObj, CFont* pFont);
int           LISTVIEW_SetGridVis      (ListView_Obj *pObj, int Show);
void          LISTVIEW_SetItemBkColor  (ListView_Obj *pObj, unsigned Column, unsigned Row, unsigned int Index, RGBC Color);
void          LISTVIEW_SetItemText     (ListView_Obj *pObj, unsigned Column, unsigned Row, const char * s);
void          LISTVIEW_SetItemTextColor(ListView_Obj *pObj, unsigned Column, unsigned Row, unsigned int Index, RGBC Color);
void          LISTVIEW_SetLBorder      (ListView_Obj *pObj, unsigned BorderSize);
void          LISTVIEW_SetRBorder      (ListView_Obj *pObj, unsigned BorderSize);
unsigned      LISTVIEW_SetRowHeight    (ListView_Obj *pObj, unsigned RowHeight);
void          LISTVIEW_SetSel          (ListView_Obj *pObj, int Sel);
void          LISTVIEW_SetTextAlign    (ListView_Obj *pObj, unsigned int Index, int Align);
void          LISTVIEW_SetTextColor    (ListView_Obj *pObj, unsigned int Index, RGBC Color);
RGBC   LISTVIEW_SetDefaultBkColor  (unsigned Index, RGBC Color);
CFont* LISTVIEW_SetDefaultFont     (CFont* pFont);
RGBC   LISTVIEW_SetDefaultGridColor(RGBC Color);
RGBC   LISTVIEW_SetDefaultTextColor(unsigned Index, RGBC Color);
#define LISTVIEW_Delete(pObj)        WM_DeleteWindow    (pObj)
#define LISTVIEW_Invalidate(pObj)    pObj->Invalidate()
#define LISTVIEW_Paint(pObj)         WM_Paint           (pObj)
// private
int      LISTVIEW__UpdateScrollParas(ListView_Obj *pObj);
void     LISTVIEW__InvalidateInsideArea(ListView_Obj *pObj);
unsigned LISTVIEW__GetRowDistY(const ListView_Obj* pObj);
void     LISTVIEW__InvalidateRow(ListView_Obj *pObj, int Sel);
int      LISTVIEW__UpdateScrollPos(ListView_Obj *pObj);
