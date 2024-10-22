#pragma once
#include "GUI_ARRAY.h"
#include "Widget.h"
typedef struct {
	int Width;
	int16_t Align;
	GUI_DRAW *pDrawObj;
	char acText[1];
} HEADER_COLUMN;
struct Header_Obj : public Widget {
	RGBC BkColor;
	RGBC TextColor;
	GUI_ARRAY Columns;
	int CapturePosX;
	int CaptureItem;
	int ScrollPos;
	CFont *pFont;
};
Header_Obj *HEADER_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id);
#define HEADER_Create(x0, y0, xsize, ysize, pParent, Id, Flags, ExFlags) \
	HEADER_CreateEx(x0, y0, xsize, ysize, pParent, Flags, ExFlags, Id)
#define HEADER_CreateAttached(pParent, Id, SpecialFlags) \
	HEADER_CreateEx(0, 0, 0, 0, pParent, WC_VISIBLE, SpecialFlags, Id)
#define HEADER_CreateIndirect(pCreateInfo, pParent, x0, y0, cb) \
	HEADER_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,    \
		pCreateInfo->xSize, pCreateInfo->ySize,                    \
		pParent, pCreateInfo->Flags, 0, pCreateInfo->Id)
RGBC              HEADER_SetDefaultBkColor  (RGBC Color);
const Cursor *    HEADER_SetDefaultCursor   (const Cursor *pCursor);
CFont*            HEADER_SetDefaultFont     (CFont* pFont);
int               HEADER_SetDefaultBorderH  (int Spacing);
int               HEADER_SetDefaultBorderV  (int Spacing);
RGBC              HEADER_SetDefaultTextColor(RGBC Color);
RGBC              HEADER_GetDefaultBkColor  (void);
const Cursor     *HEADER_GetDefaultCursor   (void);
CFont*            HEADER_GetDefaultFont     (void);
int               HEADER_GetDefaultBorderH  (void);
int               HEADER_GetDefaultBorderV  (void);
RGBC              HEADER_GetDefaultTextColor(void);
void HEADER_AddItem     (Header_Obj *pObj, int Width, const char * s, int Align);
void HEADER_DeleteItem  (Header_Obj *pObj, unsigned Index);
int  HEADER_GetHeight   (Header_Obj *pObj);
int  HEADER_GetItemWidth(Header_Obj *pObj, unsigned int Index);
int  HEADER_GetNumItems (Header_Obj *pObj);
void HEADER_SetBitmapEx (Header_Obj *pObj, unsigned Index, const Bitmap *pBitmap, int x, int y);
void HEADER_SetBkColor  (Header_Obj *pObj, RGBC Color);
void HEADER_SetFont     (Header_Obj *pObj, CFont* pFont);
void HEADER_SetHeight   (Header_Obj *pObj, int Height);
void HEADER_SetTextAlign(Header_Obj *pObj, unsigned int Index, int Align);
void HEADER_SetItemText (Header_Obj *pObj, unsigned int Index, const char *s);
void HEADER_SetItemWidth(Header_Obj *pObj, unsigned int Index, int Width);
void HEADER_SetScrollPos(Header_Obj *pObj, int ScrollPos);
void HEADER_SetTextColor(Header_Obj *pObj, RGBC Color);
#define HEADER_SetBitmap(pObj, Index, pBitmap)  HEADER_SetBitmapEx(pObj, Index, pBitmap, 0, 0)
#define HEADER_SetDefaultSpacingH(Value) HEADER_SetDefaultBorderH(Value)
#define HEADER_SetDefaultSpacingV(Value) HEADER_SetDefaultBorderV(Value)
#define HEADER_GetDefaultSpacingH()      HEADER_GetDefaultBorderH()
#define HEADER_GetDefaultSpacingV()      HEADER_GetDefaultBorderV()
#define HEADER_Delete(pObj)        WM_DeleteWindow    (pObj)
#define HEADER_Paint(pObj)         WM_Paint           (pObj)
#define HEADER_Invalidate(pObj)    pObj->Invalidate()
void HEADER__SetDrawObj(Header_Obj *pObj, unsigned Index, GUI_DRAW *pDrawObj); // private
