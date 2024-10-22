#pragma once
#include "GUI.h"
#include "GUI_X.h"
#include "WM_GUI.h"
#include "LCD_Protected.h"
#include "GUIDebug.h"
/**********************************************************************
*
*                        Defaults for config switches
*
***********************************************************************
*/
#define GUI_DEFAULT_FONT    &GUI_Font6x8
#define GUI_DEFAULT_CURSOR  &GUI_CursorArrowM
#define GUI_DEFAULT_BKCOLOR GUI_BLACK
#define GUI_DEFAULT_COLOR   GUI_WHITE
#define GUI_X_WAIT_EVENT()  GUI_X_ExecIdle()
typedef enum {
	GUI_WRAPMODE_NONE,
	GUI_WRAPMODE_WORD,
	GUI_WRAPMODE_CHAR
} GUI_WRAPMODE;
/*********************************************************************
*
*                     Helper functions
*
***********************************************************************
*/
#define GUI_ZEROINIT(Obj) GUI__memset(Obj, 0, sizeof(Obj))
#define GUI_UC__GetCharSize(sText)  GUI.pUC_API->pfGetCharSize(sText)
#define GUI_UC__GetCharCode(sText)  GUI.pUC_API->pfGetCharCode(sText)
int      GUI_UC__CalcSizeOfChar(uint16_t Char);
uint16_t GUI_UC__GetCharCodeInc(const char **ps);
int      GUI_UC__NumChars2NumBytes(const char *s, int NumChars);
int      GUI_UC__NumBytes2NumChars(const char *s, int NumBytes);
int  GUI__GetLineNumChars(const char *s, int MaxNumChars);
int  GUI__GetNumChars(const char *s);
int  GUI__GetLineDistX(const char *s, int Len);
int  GUI__HandleEOLine(const char **ps);
void GUI__DispLine(const char *s, int Len, const Rect *pr);
void GUI__CalcTextRect(const char *pText, const Rect *pTextRectIn, Rect *pTextRectOut, int TextAlign);
int  GUI__WrapGetNumCharsDisp(const char *pText, int xSize, GUI_WRAPMODE WrapMode);
int  GUI__WrapGetNumCharsToNextLine(const char *pText, int xSize, GUI_WRAPMODE WrapMode);
int  GUI__WrapGetNumBytesToNextLine(const char *pText, int xSize, GUI_WRAPMODE WrapMode);
int  GUI__strlen(const char *s);
void GUI__strcpy(char *s, const char *c);
int  GUI__strcmp(const char *s0, const char *s1);
void GUI__memset(void *d, uint8_t Fill, size_t NumBytes);
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len);
int GUI__memcmp(void *sDest, const void *pSrc, size_t Len);
void GL_DispChar(uint16_t c);
/************************************************************
*
*                 Callback pointers for dynamic linkage
*
*************************************************************
*/
typedef bool GUI_CURSOR_tfTempHide(const Rect *pRect);
typedef void GUI_CURSOR_tfTempUnhide(void);
typedef bool WM_tfHandlePID(void);
/************************************************************
*
*        Cursors
*
*************************************************************
*/
extern const LogPalette GUI_CursorPal;
extern const LogPalette GUI_CursorPalI;
RGBC GUI_GetBitmapPixel(const Bitmap *pBMP, unsigned x, unsigned y);
/************************************************************
*
*         GUI_EXTERN declartions/definitions
*
*************************************************************
*/
#ifdef  GL_CORE_C
#	define GUI_EXTERN
#else
#	ifdef __cplusplus
#	define GUI_EXTERN extern "C"
#	else
#		define GUI_EXTERN extern
#	endif
#endif
extern GUI_CONTEXT     GUI;
extern void (*GUI_pfDispCharStyle)(uint16_t Char);
extern const GUI_UC_ENC_APILIST GUI__API_TableNone;
#undef GUI_EXTERN
