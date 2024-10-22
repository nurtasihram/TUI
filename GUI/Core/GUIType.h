#pragma once
#include <stdint.h>
#include <stddef.h>
#include "LCD.h"
typedef struct {
	uint16_t XSize, YSize;
	uint16_t BytesPerLine;
	uint16_t BitsPerPixel;
	const void *pData;
	const LogPalette *pPal;
} Bitmap;
typedef struct {
	int x, y;
	unsigned char Pressed;
} PidState;
/*
* Font
*/
typedef struct {
	int16_t c0, c1;
} GUI_FONT_TRANSLIST;
typedef struct {
	uint16_t FirstChar, LastChar;
	const GUI_FONT_TRANSLIST *pList;
} GUI_FONT_TRANSINFO;
typedef struct {
	uint8_t XSize;
	uint8_t XDist;
	uint8_t BytesPerLine;
	const unsigned char *pData;
} GUI_CHARINFO;
typedef struct GUI_FONT_PROP {
	uint16_t First, Last;
	const GUI_CHARINFO *paCharInfo;
	const struct GUI_FONT_PROP *pNext;
} GUI_FONT_PROP;
typedef struct {
	const unsigned char *pData;
	const uint8_t *pTransData;
	const GUI_FONT_TRANSINFO *pTrans;
	uint16_t FirstChar, LastChar;
	uint8_t XSize;
	uint8_t XDist;
	uint8_t BytesPerLine;
} GUI_FONT_MONO;
typedef struct {
	uint16_t Flags;
	uint8_t Baseline;
	uint8_t LHeight;
	uint8_t CHeight;
} FontInfo;
#define GUI_FONTINFO_FLAG_PROP (1 << 0)
#define GUI_FONTINFO_FLAG_MONO (1 << 1)
/**********************************************************************
*
*         UNICODE Encoding
*
***********************************************************************
*/
typedef uint16_t tGUI_GetCharCode(const char *s);
typedef int      tGUI_GetCharSize(const char *s);
typedef int      tGUI_CalcSizeOfChar(uint16_t Char);
typedef int      tGUI_Encode(char *s, uint16_t Char);
typedef struct {
	tGUI_GetCharCode *pfGetCharCode;
	tGUI_GetCharSize *pfGetCharSize;
	tGUI_CalcSizeOfChar *pfCalcSizeOfChar;
	tGUI_Encode *pfEncode;
} GUI_UC_ENC_APILIST;
/**********************************************************************
*
*                 Font Encoding
*
***********************************************************************
*/
typedef int  tGUI_GetLineDistX(const char *s, int Len);
typedef int  tGUI_GetLineLen(const char *s, int MaxLen);
typedef void tGL_DispLine(const char *s, int Len);
typedef struct {
	tGUI_GetLineDistX *pfGetLineDistX;
	tGUI_GetLineLen *pfGetLineLen;
	tGL_DispLine *pfDispLine;
} tGUI_ENC_APIList;
/*
	  ****************************************
	  *                                      *
	  *      Font methods                    *
	  *                                      *
	  ****************************************
*/
typedef struct Font Font;
typedef const Font CFont;
typedef void GUI_DISPCHAR(uint16_t c);
typedef int  GUI_GETCHARDISTX(uint16_t c);
typedef void GUI_GETFONTINFO(CFont *pFont, FontInfo *pfi);
typedef bool GUI_ISINFONT(CFont *pFont, uint16_t c);
struct Font {
	GUI_DISPCHAR *pfDispChar;
	GUI_GETCHARDISTX *pfGetCharDistX;
	GUI_GETFONTINFO *pfGetFontInfo;
	GUI_ISINFONT *pfIsInFont;
	const tGUI_ENC_APIList *pafEncode;
	uint8_t YSize, YDist;
	union {
		const void *pFontData;
		const GUI_FONT_MONO *pMono;
		const GUI_FONT_PROP *pProp;
	} p;
	uint8_t Baseline;
	uint8_t LHeight, CHeight;
};
#define DECLARE_FONT(Type)                                   \
void GUI##Type##_DispChar    (uint16_t c);                   \
int  GUI##Type##_GetCharDistX(uint16_t c);                   \
void GUI##Type##_GetFontInfo (CFont* pFont, FontInfo * pfi); \
bool GUI##Type##_IsInFont    (CFont* pFont, uint16_t c)
/* MONO: Monospaced fonts */
DECLARE_FONT(MONO);
#define GUI_FONTTYPE_MONO       \
  GUIMONO_DispChar,             \
	GUIMONO_GetCharDistX,       \
	GUIMONO_GetFontInfo,        \
	GUIMONO_IsInFont,           \
  (tGUI_ENC_APIList*)0
/* PROP: Proportional fonts */
DECLARE_FONT(PROP);
#define GUI_FONTTYPE_PROP       \
  GUIPROP_DispChar,             \
	GUIPROP_GetCharDistX,       \
	GUIPROP_GetFontInfo,        \
	GUIPROP_IsInFont,           \
  (tGUI_ENC_APIList*)0
