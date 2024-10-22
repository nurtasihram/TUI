#pragma once
#include "Widget.h"
/* For compatibility only ! */
#define BUTTON_CF_HIDE   WC_HIDE
#define BUTTON_CF_SHOW   WC_VISIBLE
#define BUTTON_CI_UNPRESSED 0
#define BUTTON_CI_PRESSED   1
#define BUTTON_CI_DISABLED  2
#define BUTTON_BI_UNPRESSED 0
#define BUTTON_BI_PRESSED   1
#define BUTTON_BI_DISABLED  2
#define BUTTON_STATE_FOCUS      WIDGET_STATE_FOCUS
#define BUTTON_STATE_PRESSED    WIDGET_STATE_USER0
#define BUTTON_STATE_HASFOCUS 0
typedef struct {
	RGBC aBkColor[3];
	RGBC aTextColor[3];
	CFont *pFont;
	int16_t Align;
} BUTTON_PROPS;
struct Button : public Widget {
	BUTTON_PROPS Props;
	char *pText;
	GUI_DRAW *apDrawObj[3];
};
Button *BUTTON_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id);
#define BUTTON_Create(x0, y0, xsize, ysize, Id, Flags) BUTTON_CreateEx(x0, y0, xsize, ysize, nullptr, Flags, 0, Id)
#define BUTTON_CreateAsChild(x0, y0, xsize, ysize, pParent, Id, Flags) BUTTON_CreateEx(x0, y0, xsize, ysize, pParent, Flags, 0, Id)
Button *BUTTON_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb);
RGBC   BUTTON_GetDefaultBkColor  (unsigned Index);
CFont* BUTTON_GetDefaultFont     (void);
int    BUTTON_GetDefaultTextAlign(void);
RGBC   BUTTON_GetDefaultTextColor(unsigned Index);
void   BUTTON_SetDefaultBkColor  (RGBC Color, unsigned Index);
void   BUTTON_SetDefaultFont     (CFont* pFont);
void   BUTTON_SetDefaultTextAlign(int Align);
void   BUTTON_SetDefaultTextColor(RGBC Color, unsigned Index);
RGBC   BUTTON_GetBkColor   (Button *pObj, unsigned int Index);
CFont *BUTTON_GetFont      (Button *pObj);
void   BUTTON_GetText      (Button *pObj, char * pBuffer, int MaxLen);
RGBC   BUTTON_GetTextColor (Button *pObj, unsigned int Index);
bool   BUTTON_IsPressed    (Button *pObj);
void   BUTTON_SetBkColor   (Button *pObj, unsigned int Index, RGBC Color);
void   BUTTON_SetFont      (Button *pObj, CFont* pfont);
void   BUTTON_SetState     (Button *pObj, int State);
void   BUTTON_SetPressed   (Button *pObj, int State);
void   BUTTON_SetFocussable(Button *pObj, int State);
void   BUTTON_SetText      (Button *pObj, const char* s);
void   BUTTON_SetTextAlign (Button *pObj, int Align);
void   BUTTON_SetTextColor (Button *pObj, unsigned int Index, RGBC Color);
void   BUTTON_SetDrawObj   (Button *pObj, int Index, GUI_DRAW *pDrawObj);
void   BUTTON_Callback(WM_MESSAGE *pMsg);
#define BUTTON_SetSelfDrawEx(pObj, Index, pDraw, x, y)  BUTTON_SetDrawObj(pObj, Index, GUI_DRAW_SELF_Create(pDraw, x, y))
#define BUTTON_SetBitmapEx(pObj, Index, pBitmap, x, y)  BUTTON_SetDrawObj(pObj, Index, GUI_DRAW_BITMAP_Create(pBitmap, x, y))
#define BUTTON_SetBitmap(pObj, Index, pBitmap)  BUTTON_SetBitmapEx(pObj, Index, pBitmap, 0, 0)
#define BUTTON_SetSelfDraw(pObj, Index, pDraw)  BUTTON_SetSelfDrawEx(pObj, Index, pDraw, 0, 0)
#define BUTTON_Delete(pObj)        WM_DeleteWindow    (pObj)
#define BUTTON_Paint(pObj)         WM_Paint           (pObj)
#define BUTTON_Invalidate(pObj)    pObj->Invalidate()
