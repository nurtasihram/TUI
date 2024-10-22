#pragma once
#include "GUI_Array.h"
#include "Header.h"

#define LISTVIEW_CI_UNSEL    0
#define LISTVIEW_CI_SEL      1
#define LISTVIEW_CI_SELFOCUS 2
typedef struct {
	RGBC aBkColor[3];
	RGBC aTextColor[3];
} LISTVIEW_ITEM_INFO;
struct ListView : public Widget {
public:
	struct Property {
		CFont *pFont = &GUI_Font13_1;
		RGBC aBkColor[3]{
			/* Not selected */ RGB_WHITE,
			/* Selected, no focus */ RGB_GRAY,
			/* Selected, focus */ RGB_BLUE
		};
		RGBC aTextColor[3]{
			/* Not selected */ RGB_BLACK,
			/* Selected, no focus */ RGB_WHITE,
			/* Selected, focus */ RGB_WHITE
		};
		RGBC GridColor = RGB_LIGHTGRAY;
	} static DefaultProps;
	struct Item {
		LISTVIEW_ITEM_INFO *pItemInfo;
		char *pText = nullptr;
		~Item() {
			GUI_ALLOC_Free(pText);
			GUI_ALLOC_Free(pItemInfo);
			pText = nullptr;
			pItemInfo = nullptr;
		}
	};
public:
	Header *pHeader;
	GUI_Array<GUI_Array<Item>> RowArray;
	GUI_Array<TEXTALIGN> AlignArray;
	Property Props;
	int Sel;
	int ShowGrid;
	uint16_t RowDistY;
	uint16_t LBorder, RBorder;
	WM_SCROLL_STATE scrollStateV, scrollStateH;
	WObj *pOwner;
};

ListView *LISTVIEW_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, uint16_t Flags, uint16_t ExFlags, uint16_t Id);
void          LISTVIEW_AddColumn       (ListView *pObj, int Width, const char * s, int Align);
void          LISTVIEW_AddRow          (ListView *pObj, const char **ppText);
void          LISTVIEW_DecSel          (ListView *pObj);
void          LISTVIEW_DeleteColumn    (ListView *pObj, unsigned Index);
void          LISTVIEW_DeleteRow       (ListView *pObj, unsigned Index);
RGBC          LISTVIEW_GetBkColor      (ListView *pObj, unsigned Index);
CFont*        LISTVIEW_GetFont         (ListView *pObj);
Header   *LISTVIEW_GetHeader       (ListView *pObj);
unsigned      LISTVIEW_GetNumColumns   (ListView *pObj);
unsigned      LISTVIEW_GetNumRows      (ListView *pObj);
int           LISTVIEW_GetSel          (ListView *pObj);
RGBC          LISTVIEW_GetTextColor    (ListView *pObj, unsigned Index);
void          LISTVIEW_IncSel          (ListView *pObj);
void          LISTVIEW_SetBkColor      (ListView *pObj, unsigned int Index, RGBC Color);
void          LISTVIEW_SetColumnWidth  (ListView *pObj, unsigned int Index, int Width);
void          LISTVIEW_SetFont         (ListView *pObj, CFont* pFont);
int           LISTVIEW_SetGridVis      (ListView *pObj, int Show);
void          LISTVIEW_SetItemBkColor  (ListView *pObj, unsigned Column, unsigned Row, unsigned int Index, RGBC Color);
void          LISTVIEW_SetItemText     (ListView *pObj, unsigned Column, unsigned Row, const char * s);
void          LISTVIEW_SetItemTextColor(ListView *pObj, unsigned Column, unsigned Row, unsigned int Index, RGBC Color);
void          LISTVIEW_SetLBorder      (ListView *pObj, unsigned BorderSize);
void          LISTVIEW_SetRBorder      (ListView *pObj, unsigned BorderSize);
unsigned      LISTVIEW_SetRowHeight    (ListView *pObj, unsigned RowHeight);
void          LISTVIEW_SetSel          (ListView *pObj, int Sel);
void          LISTVIEW_SetTextAlign    (ListView *pObj, unsigned int Index, int Align);
void          LISTVIEW_SetTextColor    (ListView *pObj, unsigned int Index, RGBC Color);
// private
int      LISTVIEW__UpdateScrollParas(ListView *pObj);
void     LISTVIEW__InvalidateInsideArea(ListView *pObj);
unsigned LISTVIEW__GetRowDistY(const ListView* pObj);
void     LISTVIEW__InvalidateRow(ListView *pObj, int Sel);
int      LISTVIEW__UpdateScrollPos(ListView *pObj);
