#pragma once
#include "Widget.h"
#define MULTIPAGE_ALIGN_LEFT    (0<<0)
#define MULTIPAGE_ALIGN_RIGHT   (1<<0)
#define MULTIPAGE_ALIGN_TOP     (0<<2)
#define MULTIPAGE_ALIGN_BOTTOM  (1<<2)
typedef struct {
	WObj *pWin;
	uint8_t Status;
	char    acText;
} MULTIPAGE_PAGE;
struct MultiPage_Obj : public Widget {
	WObj         *pClient;
	GUI_ARRAY       Handles;
	unsigned        Selection;
	int             ScrollState;
	unsigned        Align;
	CFont *Font;
	RGBC       aBkColor[2];
	RGBC       aTextColor[2];
};
MultiPage_Obj *MULTIPAGE_CreateEx      (int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id);
MultiPage_Obj *MULTIPAGE_CreateIndirect(const GUI_WIDGET_CREATE_INFO* pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK* cb);
#define MULTIPAGE_Create(x0, y0, xsize, ysize, pParent, Id, Flags, ExFlags) \
	MULTIPAGE_CreateEx(x0, y0, xsize, ysize, pParent, Flags, ExFlags, Id)
void    MULTIPAGE_AddPage       (MultiPage_Obj *pObj, WObj *pWin ,const char* pText);
void    MULTIPAGE_DeletePage    (MultiPage_Obj *pObj, unsigned Index, int Delete);
void    MULTIPAGE_SelectPage    (MultiPage_Obj *pObj, unsigned Index);
void    MULTIPAGE_EnablePage    (MultiPage_Obj *pObj, unsigned Index);
void    MULTIPAGE_DisablePage   (MultiPage_Obj *pObj, unsigned Index);
void    MULTIPAGE_SetText       (MultiPage_Obj *pObj, const char* pText, unsigned Index);
void    MULTIPAGE_SetBkColor    (MultiPage_Obj *pObj, RGBC Color, unsigned Index);
void    MULTIPAGE_SetTextColor  (MultiPage_Obj *pObj, RGBC Color, unsigned Index);
void    MULTIPAGE_SetFont       (MultiPage_Obj *pObj, CFont* pFont);
void    MULTIPAGE_SetAlign      (MultiPage_Obj *pObj, unsigned Align);
int     MULTIPAGE_GetSelection  (MultiPage_Obj *pObj);
WObj *MULTIPAGE_GetWindow     (MultiPage_Obj *pObj, unsigned Index);
int     MULTIPAGE_IsPageEnabled (MultiPage_Obj *pObj, unsigned Index);
unsigned MULTIPAGE_GetDefaultAlign    (void);
RGBC     MULTIPAGE_GetDefaultBkColor  (unsigned Index);
CFont*   MULTIPAGE_GetDefaultFont     (void);
RGBC     MULTIPAGE_GetDefaultTextColor(unsigned Index);
void     MULTIPAGE_SetDefaultAlign    (unsigned Align);
void     MULTIPAGE_SetDefaultBkColor  (RGBC Color, unsigned Index);
void     MULTIPAGE_SetDefaultFont     (CFont* pFont);
void     MULTIPAGE_SetDefaultTextColor(RGBC Color, unsigned Index);
