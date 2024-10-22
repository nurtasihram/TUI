#pragma once
#include "GUI_Array.h"
#include "WM.h"

#define MULTIPAGE_ALIGN_LEFT    (0<<0)
#define MULTIPAGE_ALIGN_RIGHT   (1<<0)
#define MULTIPAGE_ALIGN_TOP     (0<<2)
#define MULTIPAGE_ALIGN_BOTTOM  (1<<2)
struct Page {
	WObj *pWin;
	uint8_t Status;
	char *pText;
	~Page() {
		GUI_ALLOC_Free(pText);
		pText = nullptr;
	}
};
struct MultiPage : public Widget {
	WObj *pClient;
	GUI_Array<Page> Handles;
	unsigned Selection;
	int ScrollState;
	unsigned Align;
	CFont *Font;
	RGBC aBkColor[2];
	RGBC aTextColor[2];
};
MultiPage *MULTIPAGE_CreateEx      (int x0, int y0, int xsize, int ysize, WObj *pParent, uint16_t Flags, uint16_t ExFlags, uint16_t Id);
#define MULTIPAGE_Create(x0, y0, xsize, ysize, pParent, Id, Flags, ExFlags) \
	MULTIPAGE_CreateEx(x0, y0, xsize, ysize, pParent, Flags, ExFlags, Id)
void    MULTIPAGE_AddPage       (MultiPage *pObj, WObj *pWin ,const char* pText);
void    MULTIPAGE_DeletePage    (MultiPage *pObj, unsigned Index, int Delete);
void    MULTIPAGE_SelectPage    (MultiPage *pObj, unsigned Index);
void    MULTIPAGE_EnablePage    (MultiPage *pObj, unsigned Index);
void    MULTIPAGE_DisablePage   (MultiPage *pObj, unsigned Index);
void    MULTIPAGE_SetText       (MultiPage *pObj, const char* pText, unsigned Index);
void    MULTIPAGE_SetBkColor    (MultiPage *pObj, RGBC Color, unsigned Index);
void    MULTIPAGE_SetTextColor  (MultiPage *pObj, RGBC Color, unsigned Index);
void    MULTIPAGE_SetFont       (MultiPage *pObj, CFont* pFont);
void    MULTIPAGE_SetAlign      (MultiPage *pObj, unsigned Align);
int     MULTIPAGE_GetSelection  (MultiPage *pObj);
WObj *MULTIPAGE_GetWindow     (MultiPage *pObj, unsigned Index);
int     MULTIPAGE_IsPageEnabled (MultiPage *pObj, unsigned Index);
unsigned MULTIPAGE_GetDefaultAlign    (void);
RGBC     MULTIPAGE_GetDefaultBkColor  (unsigned Index);
CFont*   MULTIPAGE_GetDefaultFont     (void);
RGBC     MULTIPAGE_GetDefaultTextColor(unsigned Index);
void     MULTIPAGE_SetDefaultAlign    (unsigned Align);
void     MULTIPAGE_SetDefaultBkColor  (RGBC Color, unsigned Index);
void     MULTIPAGE_SetDefaultFont     (CFont* pFont);
void     MULTIPAGE_SetDefaultTextColor(RGBC Color, unsigned Index);
