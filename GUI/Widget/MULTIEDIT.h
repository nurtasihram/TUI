#pragma once

#include "Widget.h"
#define MULTIEDIT_CF_READONLY        (1 << 0)
#define MULTIEDIT_CF_INSERT          (1 << 2)
#define MULTIEDIT_CF_AUTOSCROLLBAR_V (1 << 3)
#define MULTIEDIT_CF_AUTOSCROLLBAR_H (1 << 4)
#define MULTIEDIT_CF_PASSWORD        (1 << 5)
#define MULTIEDIT_CI_EDIT     0
#define MULTIEDIT_CI_READONLY 1
#define NUM_DISP_MODES 2
struct MultiEdit_Obj : public Widget {
	RGBC aBkColor[NUM_DISP_MODES];
	RGBC aColor[NUM_DISP_MODES];
	char *pText;
	uint16_t MaxNumChars;         /* Maximum number of characters including the prompt */
	uint16_t NumChars;            /* Number of characters (text and prompt) in object */
	uint16_t NumCharsPrompt;      /* Number of prompt characters */
	uint16_t NumLines;            /* Number of text lines needed to show all data */
	uint16_t TextSizeX;           /* Size in X of text depending of wrapping mode */
	uint16_t BufferSize;
	uint16_t CursorLine;          /* Number of current cursor line */
	uint16_t CursorPosChar;       /* Character offset number of cursor */
	uint16_t CursorPosByte;       /* Byte offset number of cursor */
	uint16_t CursorPosX;          /* Cursor position in X */
	uint16_t CursorPosY;          /* Cursor position in Y */
	uint16_t CacheLinePosByte;    /*  */
	uint16_t CacheLineNumber;     /*  */
	uint16_t CacheFirstVisibleLine;
	uint16_t CacheFirstVisibleByte;
	WM_SCROLL_STATE ScrollStateV;
	WM_SCROLL_STATE ScrollStateH;
	CFont *pFont;
	uint8_t Flags;
	uint8_t InvalidFlags;         /* Flags to save validation status */
	uint8_t EditMode;
	uint8_t HBorder;
	GUI_WRAPMODE WrapMode;
};
MultiEdit_Obj *MULTIEDIT_CreateEx      (int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id, int BufferSize, const char* pText);
MultiEdit_Obj *MULTIEDIT_CreateIndirect(const GUI_WIDGET_CREATE_INFO* pCreateInfo,  WObj *pParent, int x0, int y0, WM_CALLBACK* cb);
#define MULTIEDIT_Create(x0, y0, xsize, ysize, pParent, Id, Flags, ExFlags, pText, MaxLen) \
	MULTIEDIT_CreateEx(x0, y0, xsize, ysize, pParent, Flags, ExFlags, Id, MaxLen, pText)
int  MULTIEDIT_AddKey           (MultiEdit_Obj *pObj, uint16_t Key);
void MULTIEDIT_GetPrompt        (MultiEdit_Obj *pObj, char* sDest, int MaxNumChars);
int  MULTIEDIT_GetTextSize      (MultiEdit_Obj *pObj);
void MULTIEDIT_GetText          (MultiEdit_Obj *pObj, char* sDest, int MaxNumChars);
void MULTIEDIT_SetAutoScrollH   (MultiEdit_Obj *pObj, int OnOff);
void MULTIEDIT_SetAutoScrollV   (MultiEdit_Obj *pObj, int OnOff);
void MULTIEDIT_SetBkColor       (MultiEdit_Obj *pObj, unsigned Index, RGBC color);
void MULTIEDIT_SetCursorOffset  (MultiEdit_Obj *pObj, int Offset);
void MULTIEDIT_SetHBorder       (MultiEdit_Obj *pObj, unsigned HBorder);
void MULTIEDIT_SetFont          (MultiEdit_Obj *pObj, CFont* pFont);
void MULTIEDIT_SetInsertMode    (MultiEdit_Obj *pObj, int OnOff);
void MULTIEDIT_SetBufferSize    (MultiEdit_Obj *pObj, int BufferSize);
void MULTIEDIT_SetMaxNumChars   (MultiEdit_Obj *pObj, unsigned MaxNumChars);
void MULTIEDIT_SetPrompt        (MultiEdit_Obj *pObj, const char* sPrompt);
void MULTIEDIT_SetReadOnly      (MultiEdit_Obj *pObj, int OnOff);
void MULTIEDIT_SetPasswordMode  (MultiEdit_Obj *pObj, int OnOff);
void MULTIEDIT_SetText          (MultiEdit_Obj *pObj, const char* s);
void MULTIEDIT_SetTextColor     (MultiEdit_Obj *pObj, unsigned Index, RGBC color);
void MULTIEDIT_SetWrapNone      (MultiEdit_Obj *pObj);
void MULTIEDIT_SetWrapChar      (MultiEdit_Obj *pObj);
void MULTIEDIT_SetWrapWord      (MultiEdit_Obj *pObj);
#define MULTIEDIT_SetMaxLen(pObj, MaxLen) MULTIEDIT_SetBufferSize(pObj, MaxLen)
#define MULTIEDIT_GetStringSize           MULTIEDIT_GetTextSize
