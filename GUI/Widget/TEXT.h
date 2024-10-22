#pragma once
#include "Widget.h"
#define TEXT_CF_LEFT    GUI_TA_LEFT
#define TEXT_CF_RIGHT   GUI_TA_RIGHT
#define TEXT_CF_HCENTER GUI_TA_HCENTER
#define TEXT_CF_VCENTER GUI_TA_VCENTER
#define TEXT_CF_TOP     GUI_TA_TOP
#define TEXT_CF_BOTTOM  GUI_TA_BOTTOM
struct Text_Obj : public Widget {
	char *pText;
	CFont *pFont;
	int16_t Align;
	RGBC TextColor;
	RGBC BkColor;
};
Text_Obj *TEXT_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent,
						  int WinFlags, int ExFlags, int Id, const char *pText);
#define TEXT_Create(x0, y0, xsize, ysize, Id, Flags, s, Align) \
	TEXT_CreateEx(x0, y0, xsize, ysize, nullptr, Flags, Align, Id, s)
#define TEXT_CreateAsChild(x0, y0, xsize, ysize, pParent, Id, Flags, s, Align) \
	TEXT_CreateEx(x0, y0, xsize, ysize, pParent, Flags, Align, Id, s)
Text_Obj *TEXT_CreateIndirect(const GUI_WIDGET_CREATE_INFO* pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK* cb);
void TEXT_SetBkColor  (Text_Obj *pObj, RGBC Color); /* Obsolete. Left in GUI for compatibility to older versions */
void TEXT_SetFont     (Text_Obj *pObj, CFont* pFont);
void TEXT_SetText     (Text_Obj *pObj, const char* s);
void TEXT_SetTextAlign(Text_Obj *pObj, int Align);
void TEXT_SetTextColor(Text_Obj *pObj, RGBC Color);
void   TEXT_SetDefaultFont(CFont* pFont);
CFont* TEXT_GetDefaultFont(void);
void   TEXT_SetDefaultTextColor(RGBC Color);
#define TEXT_Delete(pObj)        WM_DeleteWindow(pObj)
#define TEXT_Paint(pObj)         WM_Paint(pObj)
#define TEXT_Invalidate(pObj)    pObj->Invalidate()
