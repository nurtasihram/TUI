#pragma once
#include "GUI_Array.h"
#include "WM.h"

struct Header : public Widget {
	struct Column {
		int Width;
		int16_t Align;
		GUI_DRAW_BASE *pDrawObj = nullptr;
		char *pText = nullptr;
		~Column() {
			pDrawObj = nullptr;
			pText = nullptr;
			GUI_MEM_Free(pDrawObj);
			GUI_MEM_Free(pText);
		}
	};
	RGBC BkColor;
	RGBC TextColor;
	GUI_Array<Column> Columns;
	int CapturePosX;
	int CaptureItem;
	int ScrollPos;
	CFont *pFont;
};
Header *HEADER_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, uint16_t Flags, uint16_t ExFlags, uint16_t Id);
#define HEADER_Create(x0, y0, xsize, ysize, pParent, Id, Flags, ExFlags) \
	HEADER_CreateEx(x0, y0, xsize, ysize, pParent, Flags, ExFlags, Id)
#define HEADER_CreateAttached(pParent, Id, SpecialFlags) \
	HEADER_CreateEx(0, 0, 0, 0, pParent, WC_VISIBLE, SpecialFlags, Id)
#define HEADER(pCreateInfo, pParent, x0, y0, cb) \
	HEADER_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,    \
		pCreateInfo->xSize, pCreateInfo->ySize,                    \
		pParent, pCreateInfo->Flags, 0, pCreateInfo->Id)
RGBC              HEADER_SetDefaultBkColor  (RGBC Color);
CCursor *    HEADER_SetDefaultCursor   (CCursor *pCursor);
CFont*            HEADER_SetDefaultFont     (CFont* pFont);
int               HEADER_SetDefaultBorderH  (int Spacing);
int               HEADER_SetDefaultBorderV  (int Spacing);
RGBC              HEADER_SetDefaultTextColor(RGBC Color);
RGBC              HEADER_GetDefaultBkColor  ();
CCursor     *HEADER_GetDefaultCursor   ();
CFont*            HEADER_GetDefaultFont     ();
int               HEADER_GetDefaultBorderH  ();
int               HEADER_GetDefaultBorderV  ();
RGBC              HEADER_GetDefaultTextColor();
void HEADER_AddItem     (Header *pObj, int Width, const char * s, int Align);
void HEADER_DeleteItem  (Header *pObj, unsigned Index);
int  HEADER_GetHeight   (Header *pObj);
int  HEADER_GetItemWidth(Header *pObj, unsigned int Index);
int  HEADER_GetNumItems (Header *pObj);
void HEADER_SetBitmapEx (Header *pObj, unsigned Index, CBitmap *pBitmap, int x, int y);
void HEADER_SetBkColor  (Header *pObj, RGBC Color);
void HEADER_SetFont     (Header *pObj, CFont* pFont);
void HEADER_SetHeight   (Header *pObj, int Height);
void HEADER_SetTextAlign(Header *pObj, unsigned int Index, int Align);
void HEADER_SetItemText (Header *pObj, unsigned int Index, const char *s);
void HEADER_SetItemWidth(Header *pObj, unsigned int Index, int Width);
void HEADER_SetScrollPos(Header *pObj, int ScrollPos);
void HEADER_SetTextColor(Header *pObj, RGBC Color);
#define HEADER_SetBitmap(pObj, Index, pBitmap)  HEADER_SetBitmapEx(pObj, Index, pBitmap, 0, 0)
#define HEADER_SetDefaultSpacingH(Value) HEADER_SetDefaultBorderH(Value)
#define HEADER_SetDefaultSpacingV(Value) HEADER_SetDefaultBorderV(Value)
#define HEADER_GetDefaultSpacingH()      HEADER_GetDefaultBorderH()
#define HEADER_GetDefaultSpacingV()      HEADER_GetDefaultBorderV()
#define HEADER_Delete(pObj)        WM_DeleteWindow    (pObj)
#define HEADER_Paint(pObj)         WM_Paint           (pObj)
#define HEADER_Invalidate(pObj)    pObj->Invalidate()
void HEADER__SetDrawObj(Header *pObj, unsigned Index, GUI_DRAW_BASE *pDrawObj); // private
