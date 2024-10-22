#pragma once
#include "WM.h"

#define EDIT_CF_LEFT    GUI_TA_LEFT
#define EDIT_CF_RIGHT   GUI_TA_RIGHT
#define EDIT_CF_HCENTER GUI_TA_HCENTER
#define EDIT_CF_VCENTER GUI_TA_VCENTER
#define EDIT_CF_TOP     GUI_TA_TOP
#define EDIT_CF_BOTTOM  GUI_TA_BOTTOM
#define EDIT_CI_DISABLED 0
#define EDIT_CI_ENABLED  1
#define GUI_EDIT_NORMAL 0
#define GUI_EDIT_SIGNED 1
#define GUI_EDIT_MODE_INSERT    0
#define GUI_EDIT_MODE_OVERWRITE 1
#define EDIT_CI_DISABELD EDIT_CI_DISABLED
#define EDIT_CI_ENABELD  EDIT_CI_ENABLED

typedef struct {
	int Align;
	int Border;
	CFont *pFont;
	RGBC aTextColor[2];
	RGBC aBkColor[2];
} EDIT_PROPS;
struct Edit;
typedef void tEDIT_AddKeyEx(Edit *pObj, int Key);
typedef void tEDIT_UpdateBuffer(Edit *pObj);
struct Edit : public Widget {
	char *pText;
	int16_t MaxLen;
	uint16_t BufferSize;
	int CursorPos;           /* Cursor position. 0 means left most */
	unsigned SelSize;        /* Number of selected characters */
	uint8_t EditMode;             /* Insert or overwrite mode */
	uint8_t XSizeCursor;          /* Size of cursor when working in insert mode */
	uint8_t Flags;
	tEDIT_AddKeyEx *pfAddKeyEx;          /* Handle key input */
	tEDIT_UpdateBuffer *pfUpdateBuffer;  /* Update textbuffer */
	EDIT_PROPS Props;
};
#define EDIT_Create(x0, y0, xsize, ysize, Id, MaxLen, Flags) \
	EDIT_CreateEx(x0, y0, xsize, ysize, nullptr, Flags, 0, Id, MaxLen)
#define EDIT_CreateAsChild(x0, y0, xsize, ysize, pParent, Id, Flags, MaxLen) \
	EDIT_CreateEx(x0, y0, xsize, ysize, pParent, Flags, 0, Id, MaxLen)
Edit *EDIT_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, uint16_t Flags, uint16_t ExFlags, uint16_t Id, int MaxLen);
void   EDIT_SetDefaultBkColor  (unsigned int Index, RGBC Color);
void   EDIT_SetDefaultFont     (CFont* pFont);
void   EDIT_SetDefaultTextAlign(int Align);
void   EDIT_SetDefaultTextColor(unsigned int Index, RGBC Color);
RGBC   EDIT_GetDefaultBkColor  (unsigned int Index);
CFont* EDIT_GetDefaultFont     (void);
int    EDIT_GetDefaultTextAlign(void);
RGBC   EDIT_GetDefaultTextColor(unsigned int Index);
void EDIT_AddKey           (Edit *pObj, int Key);
void EDIT_SetBkColor       (Edit *pObj, unsigned int Index, RGBC color);
void EDIT_SetCursorAtChar  (Edit *pObj, int Pos);
void EDIT_SetCursorAtPixel (Edit *pObj, int xPos);
void EDIT_SetFont          (Edit *pObj, CFont* pfont);
int  EDIT_SetInsertMode    (Edit *pObj, int OnOff);
void EDIT_SetMaxLen        (Edit *pObj, int MaxLen);
void EDIT_SetpfAddKeyEx    (Edit *pObj, tEDIT_AddKeyEx * pfAddKeyEx);
void EDIT_SetpfUpdateBuffer(Edit *pObj, tEDIT_UpdateBuffer * pfUpdateBuffer);
void EDIT_SetText          (Edit *pObj, const char* s);
void EDIT_SetTextAlign     (Edit *pObj, int Align);
void EDIT_SetTextColor     (Edit *pObj, unsigned int Index, RGBC color);
void EDIT_SetSel           (Edit *pObj, int FirstChar, int LastChar);
void EDIT_GetText          (Edit *pObj, char* sDest, int MaxLen);
int  EDIT_GetNumChars      (Edit *pObj);
#define EDIT_Delete(pObj)        WM_DeleteWindow(pObj)
#define EDIT_Paint(pObj)         WM_Paint(pObj)
#define EDIT_Invalidate(pObj)    pObj->Invalidate()
// private
void EDIT__SetCursorPos(Edit *pObj, int CursorPos);
uint16_t EDIT__GetCurrentChar(Edit *pObj);
