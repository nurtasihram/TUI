#pragma once
#include "GUI_ARRAY.h"
#include "Widget.h"
#define RADIO_BI_INACTIV 0
#define RADIO_BI_ACTIV   1
#define RADIO_BI_CHECK   2
#define RADIO_TEXTPOS_RIGHT       0
#define RADIO_TEXTPOS_LEFT        WIDGET_STATE_USER0  /* Not implemented, TBD */
struct Radio_Obj : public Widget {
	const Bitmap *apBmRadio[2];
	const Bitmap *pBmCheck;
	GUI_ARRAY TextArray;
	int16_t Sel;                   /* current selection */
	uint16_t Spacing;
	uint16_t Height;
	uint16_t NumItems;
	uint8_t  GroupId;
	RGBC BkColor;
	RGBC TextColor;
	CFont *pFont;
};
Radio_Obj *RADIO_CreateEx      (int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id, int NumItems, int Spacing);
Radio_Obj *RADIO_CreateIndirect(const GUI_WIDGET_CREATE_INFO* pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK* cb);
#define RADIO_Create(x0, y0, xsize, ysize, pParent, Id, Flags, Para) \
	RADIO_CreateEx(x0, y0, xsize, ysize, pParent, Flags, 0, Id, Para & 0xFF, (Para >> 8) & 0xFF)
void RADIO_SetDefaultFont     (CFont* pFont);
void RADIO_SetDefaultImage    (const Bitmap * pBitmap, unsigned int Index);
void RADIO_SetDefaultTextColor(RGBC TextColor);
CFont* RADIO_GetDefaultFont     (void);
RGBC   RADIO_GetDefaultTextColor(void);
void RADIO_AddValue    (Radio_Obj *pObj, int Add);
void RADIO_Dec         (Radio_Obj *pObj);
void RADIO_Inc         (Radio_Obj *pObj);
void RADIO_SetBkColor  (Radio_Obj *pObj, RGBC Color);
void RADIO_SetFont     (Radio_Obj *pObj, CFont* pFont);
void RADIO_SetGroupId  (Radio_Obj *pObj, uint8_t GroupId);
void RADIO_SetImage    (Radio_Obj *pObj, const Bitmap * pBitmap, unsigned int Index);
void RADIO_SetText     (Radio_Obj *pObj, const char* pText, unsigned Index);
void RADIO_SetTextColor(Radio_Obj *pObj, RGBC Color);
void RADIO_SetValue    (Radio_Obj *pObj, int v);
int  RADIO_GetValue    (Radio_Obj *pObj);
// private
void RADIO__SetValue(Radio_Obj *pObj, int v);
