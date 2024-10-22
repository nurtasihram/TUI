#pragma once
#include "GUI_ARRAY.h"
#include "LISTBOX.h"
#include "Widget.h"
#define DROPDOWN_CF_AUTOSCROLLBAR   (1 << 0)
#define DROPDOWN_CF_UP              (1 << 1)
#define DROPDOWN_CI_UNSEL    0
#define DROPDOWN_CI_SEL      1
#define DROPDOWN_CI_SELFOCUS 2
typedef struct {
	CFont  *pFont;
	RGBC    aBackColor[3];
	RGBC    aTextColor[3];
	int16_t TextBorderSize;
	int16_t Align;
} DROPDOWN_PROPS;
struct Dropdown_Obj : public Widget {
	int16_t   Sel;                        /* current selection */
	int16_t   ySizeEx;                    /* Drop down size */
	int16_t   TextHeight;
	GUI_ARRAY Handles;
	WM_SCROLL_STATE ScrollState;
	DROPDOWN_PROPS Props;
	ListBox_Obj *pListWin;
	uint8_t      Flags;
	uint16_t     ItemSpacing;
	uint8_t      ScrollbarWidth;
	char  IsPressed;
};
Dropdown_Obj *DROPDOWN_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id);
#define DROPDOWN_Create(pParent, x0, y0, xsize, ysize, Flags) DROPDOWN_CreateEx(x0, y0, xsize, ysize, pParent, Flags, 0, 0)
#define DROPDOWN_CreateIndirect(pCreateInfo, pParent, x0, y0, cb)      \
	DROPDOWN_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,      \
					  pCreateInfo->xSize, pCreateInfo->ySize,          \
					  pParent, 0, pCreateInfo->Flags, pCreateInfo->Id)
void     DROPDOWN_AddKey           (Dropdown_Obj *pObj, int Key);
void     DROPDOWN_AddString        (Dropdown_Obj *pObj, const char* s);
void     DROPDOWN_Collapse         (Dropdown_Obj *pObj);
void     DROPDOWN_DecSel           (Dropdown_Obj *pObj);
void     DROPDOWN_DeleteItem       (Dropdown_Obj *pObj, unsigned int Index);
void     DROPDOWN_Expand           (Dropdown_Obj *pObj);
unsigned DROPDOWN_GetItemSpacing   (Dropdown_Obj *pObj);
int      DROPDOWN_GetNumItems      (Dropdown_Obj *pObj);
int      DROPDOWN_GetSel           (Dropdown_Obj *pObj);
void     DROPDOWN_IncSel           (Dropdown_Obj *pObj);
void     DROPDOWN_InsertString     (Dropdown_Obj *pObj, const char* s, unsigned int Index);
void     DROPDOWN_SetAutoScroll    (Dropdown_Obj *pObj, int OnOff);
void     DROPDOWN_SetBkColor       (Dropdown_Obj *pObj, unsigned int index, RGBC color);
void     DROPDOWN_SetFont          (Dropdown_Obj *pObj, CFont* pfont);
void     DROPDOWN_SetItemSpacing   (Dropdown_Obj *pObj, unsigned Value);
void     DROPDOWN_SetSel           (Dropdown_Obj *pObj, int Sel);
void     DROPDOWN_SetScrollbarWidth(Dropdown_Obj *pObj, unsigned Width);
void     DROPDOWN_SetTextAlign     (Dropdown_Obj *pObj, int Align);
void     DROPDOWN_SetTextColor     (Dropdown_Obj *pObj, unsigned int index, RGBC color);
void     DROPDOWN_SetTextHeight    (Dropdown_Obj *pObj, unsigned TextHeight);
CFont* DROPDOWN_GetDefaultFont(void);
void   DROPDOWN_SetDefaultFont(CFont* pFont);
#define DROPDOWN_Delete(pObj)        WM_DeleteWindow(pObj)
#define DROPDOWN_Paint(pObj)         WM_Paint(pObj)
#define DROPDOWN_Invalidate(pObj)    pObj->Invalidate()
void DROPDOWN__AdjustHeight(Dropdown_Obj *pObj); // private
