#pragma once
#include "Widget.h"
#define CHECKBOX_BI_INACTIV        0
#define CHECKBOX_BI_ACTIV          1
#define CHECKBOX_BI_INACTIV_3STATE 2
#define CHECKBOX_BI_ACTIV_3STATE   3
typedef struct {
	CFont* pFont;
	RGBC aBkColorBox[2]; /* Colors used to draw the box background */
	RGBC BkColor;        /* Widget background color */
	RGBC TextColor;
	int16_t Align;
	uint8_t  Spacing;
	const Bitmap* apBm[4];
} CHECKBOX_PROPS;
struct CheckBox_Obj : public Widget {
	CHECKBOX_PROPS Props;
	uint8_t NumStates;
	uint8_t CurrentState;
	char *pText;
};
CheckBox_Obj *CHECKBOX_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id);
#define CHECKBOX_Create(x0, y0, xsize, ysize, pParent, Id, Flags) CHECKBOX_CreateEx(x0, y0, xsize, ysize, pParent, Flags, 0, Id)
#define CHECKBOX_CreateIndirect(pCreateInfo, pParent, x0, y0, cb) \
	CHECKBOX_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,    \
		pCreateInfo->xSize, pCreateInfo->ySize,                      \
		pParent, 0, pCreateInfo->Flags, pCreateInfo->Id)
int    CHECKBOX_GetDefaultAlign    (void);
RGBC   CHECKBOX_GetDefaultBkColor  (void);
CFont *CHECKBOX_GetDefaultFont     (void);
int    CHECKBOX_GetDefaultSpacing  (void);
int    CHECKBOX_GetDefaultTextAlign(void);
RGBC   CHECKBOX_GetDefaultTextColor(void);
void   CHECKBOX_SetDefaultAlign    (int Align);
void   CHECKBOX_SetDefaultBkColor  (RGBC Color);
void   CHECKBOX_SetDefaultFont     (CFont* pFont);
void   CHECKBOX_SetDefaultImage    (const Bitmap * pBitmap, unsigned int Index);
void   CHECKBOX_SetDefaultSpacing  (int Spacing);
void   CHECKBOX_SetDefaultTextAlign(int Align);
void   CHECKBOX_SetDefaultTextColor(RGBC Color);
int  CHECKBOX_GetState    (CheckBox_Obj *pObj);
bool CHECKBOX_IsChecked   (CheckBox_Obj *pObj);
void CHECKBOX_SetBkColor  (CheckBox_Obj *pObj, RGBC Color);
void CHECKBOX_SetFont     (CheckBox_Obj *pObj, CFont* pFont);
void CHECKBOX_SetImage    (CheckBox_Obj *pObj, const Bitmap * pBitmap, unsigned int Index);
void CHECKBOX_SetNumStates(CheckBox_Obj *pObj, unsigned NumStates);
void CHECKBOX_SetSpacing  (CheckBox_Obj *pObj, unsigned Spacing);
void CHECKBOX_SetState    (CheckBox_Obj *pObj, unsigned State);
void CHECKBOX_SetText     (CheckBox_Obj *pObj, const char * pText);
void CHECKBOX_SetTextAlign(CheckBox_Obj *pObj, int Align);
void CHECKBOX_SetTextColor(CheckBox_Obj *pObj, RGBC Color);
#define CHECKBOX_Check(pObj)   CHECKBOX_SetState(pObj, 1)
#define CHECKBOX_Uncheck(pObj) CHECKBOX_SetState(pObj, 0)
