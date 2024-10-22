#pragma once
#include "GUI_ID.inl"
#include <stdint.h>
#include <stddef.h>

#define DRAWMODE_NORMAL (0)
#define DRAWMODE_TRANS  (1<<1)
#define DRAWMODE_REV    (1<<2)
template<class AnyType>
inline AnyType Max(AnyType a, AnyType b) { return a > b ? a : b; }
template<class AnyType>
inline AnyType Min(AnyType a, AnyType b) { return a < b ? a : b; }
typedef int DRAWMODE;
typedef uint32_t RGBC;
struct Point {
	int16_t x, y;
	Point(int i = 0) : x(i), y(i) {}
	Point(int x, int y) : x(x), y(y) {}
	inline operator bool() const { return x | y; }
	inline Point operator-() const { return{ -x, -y }; }
	inline Point operator+(int p) const { return{ x + p, y + p }; }
	inline Point operator-(int p) const { return{ x - p, y - p }; }
	inline Point operator+(Point p) const { return{ x + p.x, y + p.y }; }
	inline Point operator-(Point p) const { return{ x - p.x, y - p.y }; }
	inline Point&operator+=(Point p) { x += p.x, y += p.y; return*this; }
	inline Point&operator-=(Point p) { x -= p.x, y -= p.y; return*this; }
};
struct SRect {
	int16_t x0 = 0, y0 = 0;
	int16_t x1 = 0, y1 = 0;
	SRect() {}
	SRect(Point p0, Point p1) :
		x0(p0.x), y0(p0.y), x1(p1.x), y1(p1.y) {}
	SRect(int x0, int y0, int x1, int y1) :
		x0((int16_t)x0), y0((int16_t)y0),
		x1((int16_t)x1), y1((int16_t)y1) {}
public:
	inline Point left_top() const { return { x0, y0 }; }
	inline auto &left_top(Point p) { x0 = p.x, y0 = p.y; return*this; }
	inline static SRect left_top(Point p, Point s) { return { p.x, p.y, p.x + s.x - 1, p.y + s.y - 1 }; }
	inline Point right_bottom() const { return { x1, y1 }; }
	inline auto &right_bottom(Point p) { x1 = p.x, y1 = p.y; return*this; }
	inline Point size() const { return{ x1 - x0 + 1, y1 - y0 + 1 }; }
	inline auto xsize() const { return x1 - x0 + 1; }
	inline auto ysize() const { return y1 - y0 + 1; }
	inline void move_to(Point p) { *this += p - left_top(); }
public:
	inline SRect operator*(int p) const { return{ x0 + p, y0 + p, x1 - p, y1 - p }; }
	inline SRect operator/(int p) const { return{ x0 - p, y0 - p, x1 + p, y1 + p }; }
	inline SRect operator+(const SRect &r) const { return{ x0 + r.x0, y0 + r.y0, x1 + r.x1, y1 + r.y1 }; }
	inline SRect operator-(const SRect &r) const { return{ x0 - r.x0, y0 - r.y0, x1 - r.x1, y1 - r.y1 }; }
	inline SRect operator|(const SRect &r) const { return{ Min(x0, r.x0), Min(y0, r.y0), Max(x1, r.x1), Max(y1, r.y1) }; }
	inline SRect operator&(const SRect &r) const { return{ Max(x0, r.x0), Max(y0, r.y0), Min(x1, r.x1), Min(y1, r.y1) }; }
	inline SRect operator+(const Point &p) const { return{ x0 + p.x, y0 + p.y, x1 + p.x, y1 + p.y }; }
	inline SRect operator-(const Point &p) const { return{ x0 - p.x, y0 - p.y, x1 - p.x, y1 - p.y }; }
	inline SRect &operator*=(int p) { x0 -= p, y0 -= p, x1 += p, y1 += p; return*this; }
	inline SRect &operator/=(int p) { x0 += p, y0 += p, x1 -= p, y1 -= p; return*this; }
	inline SRect &operator-=(const Point &p) {
		x0 -= p.x, y0 -= p.y;
		x1 -= p.x, y1 -= p.y;
		return*this;
	}
	inline SRect &operator+=(const Point &p) {
		x0 += p.x, y0 += p.y;
		x1 += p.x, y1 += p.y;
		return*this;
	}
	inline SRect &operator+=(const SRect &r) {
		x0 += r.x0, y0 += r.y0;
		x1 += r.x1, y1 += r.y1;
		return*this;
	}
	inline SRect &operator-=(const SRect &r) {
		x0 -= r.x0, y0 -= r.y0;
		x1 -= r.x1, y1 -= r.y1;
		return*this;
	}
	inline SRect &operator&=(const SRect &r) {
		if (x0 < r.x0) x0 = r.x0;
		if (y0 < r.y0) y0 = r.y0;
		if (x1 > r.x1) x1 = r.x1;
		if (y1 > r.y1) y1 = r.y1;
		return*this;
	}
	inline SRect &operator|=(const SRect &r) {
		if (x0 > r.x0) x0 = r.x0;
		if (y0 > r.y0) y0 = r.y0;
		if (x1 < r.x1) x1 = r.x1;
		if (y1 < r.y1) y1 = r.y1;
		return*this;
	}
	inline operator bool() const {
		if (x0 > x1) return false;
		if (y0 > y1) return false;
		return true;
	}
	inline bool operator<=(const Point &p) const {
		if (p.x < x0) return false;
		if (p.y < y0) return false;
		if (p.x > x1) return false;
		if (p.y > y1) return false;
		return true;
	}
	inline bool operator>(const Point &p) const { return !(*this <= p); }
	inline bool operator&&(const SRect &r) const {
		if (x0 > r.x1) return false;
		if (y0 > r.y1) return false;
		if (r.x0 > x1) return false;
		if (r.y0 > y1) return false;
		return true;
	}
	inline bool operator==(const SRect &r) const {
		if (x0 != r.x0) return false;
		if (y0 != r.y0) return false;
		if (x1 != r.x1) return false;
		if (y1 != r.y1) return false;
		return true;
	}
	inline bool operator!=(const SRect &r) const { return !(*this == r); }
};

struct LogPalette {
	int         NumEntries;
	char        HasTrans;
	const RGBC *pPalEntries;
};
struct Bitmap {
	uint16_t XSize, YSize;
	uint16_t BytesPerLine;
	uint16_t BitsPerPixel;
	const void *pData;
	const LogPalette *pPal;
};
struct PidState : Point {
	int8_t Pressed;
	PidState(Point p = 0, int8_t Pressed = 0) : Point(p), Pressed(Pressed) {}
};

/*
* Font
*/
struct GUI_FONT_TRANSLIST {
	int16_t c0, c1;
};
struct GUI_FONT_TRANSINFO {
	uint16_t FirstChar, LastChar;
	const GUI_FONT_TRANSLIST *pList;
};
struct GUI_CHARINFO {
	uint8_t XSize;
	uint8_t XDist;
	uint8_t BytesPerLine;
	const unsigned char *pData;
};
struct GUI_FONT_PROP {
	uint16_t First, Last;
	const GUI_CHARINFO *paCharInfo;
	const struct GUI_FONT_PROP *pNext;
};
struct GUI_FONT_MONO {
	const unsigned char *pData;
	const uint8_t *pTransData;
	const GUI_FONT_TRANSINFO *pTrans;
	uint16_t FirstChar, LastChar;
	uint8_t XSize;
	uint8_t XDist;
	uint8_t BytesPerLine;
};
struct FontInfo {
	uint16_t Flags;
	uint8_t Baseline;
	uint8_t LHeight;
	uint8_t CHeight;
} ;
#define GUI_FONTINFO_FLAG_PROP (1 << 0)
#define GUI_FONTINFO_FLAG_MONO (1 << 1)
/*
*         UNICODE Encoding
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
/*
*                 Font Encoding
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

RGBC GUI_LCD_GetPixel(int x, int y);
void GUI_LCD_SetPixel(int x0, int y0);
void GUI_LCD_DrawHLine(int x0, int y0, int x1);
void GUI_LCD_DrawVLine(int x, int y0, int y1);
void GUI_LCD_FillRect(int x0, int y0, int x1, int y1);
void GUI_LCD_SetClipRectMax(void);
void GUI_LCD_SetClipRectEx(const SRect &rClip);
DRAWMODE GUI_LCD_SetDrawMode(DRAWMODE dm);

void LCD_DrawHLine(int x0, int y0, int x1);
void LCD_DrawVLine(int x, int y0, int y1);
void LCD_FillRect(int x0, int y0, int x1, int y1);
RGBC LCD_GetPixel(int x, int y);
void LCD_SetPixel(int x, int y, RGBC Color);
SRect LCD_Rect();
void LCD_Init(void);

#define GUI_LCD_PENCOLOR GUI.aColor[1]
#define GUI_LCD_BKCOLOR  GUI.aColor[0]
#define GUI_LCD_ACOLOR  GUI.aColor
void GUI_LCD_DrawBitmap(int x0, int y0, int xsize, int ysize,
						int BitsPerPixel, int BytesPerLine,
						const void *pPixel, const RGBC *pTrans);

#define GUI_COUNTOF(a) (sizeof(a) / sizeof(a[0]))
/* Proportional fonts */
extern const Font GUI_Font8_ASCII, GUI_Font8_1;
extern const Font GUI_Font13_ASCII, GUI_Font13_1;
/* Monospaced */
extern const Font GUI_Font6x8, GUI_Font6x9;
/* Text alignment flags, horizontal */
#define GUI_TA_LEFT        (0<<0)
#define GUI_TA_RIGHT	   (1<<0)
#define GUI_TA_CENTER	   (2<<0)
#define GUI_TA_HCENTER	   GUI_TA_CENTER
#define GUI_TA_HORIZONTAL  (3<<0)
/* Text alignment flags, vertical */
#define GUI_TA_TOP	      (0<<2)
#define GUI_TA_BOTTOM	  (1<<2)
#define GUI_TA_BASELINE   (2<<2)
#define GUI_TA_VCENTER    (3<<2)
#define GUI_TA_VERTICAL   (3<<2)
/*    *********************************
	  *
	  *     Min/Max coordinates
	  *
	  *********************************
*/
#define GUI_XMIN -4095
#define GUI_XMAX  4095
#define GUI_YMIN -4095
#define GUI_YMAX  4095
/*********************************************************************
*
*             GUI_CONTEXT
*
*********************************************************************/
struct GUI_CONTEXT {
	/* Variables in LCD module */
	RGBC    aColor[2];
	SRect    ClipRect;
	uint8_t DrawMode;
	uint8_t TextStyle;
	/* Variables in GL module */
	SRect*   pClipRect_HL;                /* High level clip rectangle ... Speed optimization so drawing routines can optimize */
	/* Variables in GUICHAR module */
	CFont* pAFont;
	const GUI_UC_ENC_APILIST* pUC_API;    /* Unicode encoding API */
	int LBorder;
	uint8_t TextMode, TextAlign;
	int DispPosX, DispPosY;
	/* Variables in WM module */
	const SRect* pUserClipRect;
	struct WObj *pWndActive;
	int xOff, yOff;

public: // Property - 
public: // Property - BkColor
	/* W */ inline void BkColor(RGBC Color) {
		if (DrawMode & DRAWMODE_REV)
			aColor[1] = Color;
		else
			aColor[0] = Color;
	}
	/* R */ inline RGBC BkColor() const { return aColor[0]; }
public: // Property - PenColor
	/* W */ void PenColor(RGBC Color) {
		if (DrawMode & DRAWMODE_REV)
			aColor[0] = Color;
		else
			aColor[1] = Color;
	}
	/* R */ inline RGBC PenColor() const { return aColor[1]; }
public: // Property - Font
	/* W */ inline void Font(CFont *pFont) { pAFont = pFont; }
	/* R */ inline CFont *Font() const { return pAFont; }

};
void GUI_Init(void);
void GUI_SetDefault(void);
#define GUI_SetDrawMode GUI_LCD_SetDrawMode
int  GUI__DivideRound(int a, int b);
bool GUI__SetText(char** phText, const char* s);
void GUI_Clear(void);
void GUI_ClearRect(int x0, int y0, int x1, int y1);
void GUI_ClearRectEx(const SRect* pRect);
void GUI_DrawBitmap(const Bitmap * pBM, int x0, int y0);
void GUI_DrawPixel(int x, int y);
void GUI_DrawFocusRect(const SRect* pRect, int Dist);
void GUI_DrawRect(int x0, int y0, int x1, int y1);
void GUI_DrawHLine(int y0, int x0, int x1);
void GUI_DrawVLine(int x0, int y0, int y1);
void GUI_FillRect(int x0, int y0, int x1, int y1);
void GUI_FillRectEx(const SRect* pRect);
void WM_GetClientRect(SRect* pRect);
void LCD_DrawBitmap(int x0, int y0, int xsize, int ysize,
						   int BitsPerPixel, int BytesPerLine,
						   const uint8_t *pData, int Diff, const RGBC *pTrans);

struct Cursor {
	const  Bitmap* pBitmap;
	int xHot, yHot;
};
bool GUI_CURSOR__TempHide(const SRect *pRect);
void GUI_CURSOR__TempUnhide(void);
void GUI_CURSOR_Activate(void);
void GUI_CURSOR_Deactivate(void);
void GUI_CURSOR_Show(void);
void GUI_CURSOR_Hide(void);
void GUI_CURSOR_SetPosition(int x, int y);
const Cursor *GUI_CURSOR_Select(const Cursor *pCursor);
extern const Cursor GUI_CursorArrowS, GUI_CursorArrowSI;
extern const Cursor GUI_CursorArrowM, GUI_CursorArrowMI;
extern const Cursor GUI_CursorArrowL, GUI_CursorArrowLI;
extern const Cursor GUI_CursorCrossS, GUI_CursorCrossSI;
extern const Cursor GUI_CursorCrossM, GUI_CursorCrossMI;
extern const Cursor GUI_CursorCrossL, GUI_CursorCrossLI;
extern const Cursor GUI_CursorHeaderM, GUI_CursorHeaderMI;
extern const Bitmap GUI_BitmapArrowS, GUI_BitmapArrowSI;
extern const Bitmap GUI_BitmapArrowM, GUI_BitmapArrowMI;
extern const Bitmap GUI_BitmapArrowL, GUI_BitmapArrowLI;
extern const Bitmap GUI_BitmapCrossS, GUI_BitmapCrossSI;
extern const Bitmap GUI_BitmapCrossM, GUI_BitmapCrossMI;
extern const Bitmap GUI_BitmapCrossL, GUI_BitmapCrossLI;
void  GUI_DispChar(uint16_t c);
void  GUI_DispChars(uint16_t c, int NumChars);
void  GUI_DispCharAt(uint16_t c, int x, int y);
void  GUI_DispString(const char * s);
void  GUI_DispStringAt(const char * s, int x, int y);
void  GUI_DispStringHCenterAt(const char * s, int x, int y);
void  GUI__DispStringInRect(const char * s, SRect* pRect, int TextAlign, int MaxNumChars);
void  GUI_DispStringInRect(const char * s, SRect* pRect, int Flags);
void  GUI_DispStringInRectMax(const char * s, SRect* pRect, int TextAlign, int MaxLen); /* Not to be doc. */
void  GUI_DispStringLen(const char * s, int Len);
void  GUI_GetTextExtend(SRect* pRect, const char * s, int Len);
int   GUI_GetYAdjust(void);
int   GUI_GetCharDistX(uint16_t c);
int   GUI_GetStringDistX(const char * s);
int   GUI_GetFontDistY(void);
int   GUI_GetFontSizeY(void);
void  GUI_GetFontInfo(CFont* pFont, FontInfo* pfi);
int   GUI_GetYSizeOfFont(CFont* pFont);
int   GUI_GetYDistOfFont(CFont* pFont);
int   GUI_GetTextAlign(void);
int   GUI_GetTextMode(void);
char  GUI_IsInFont(CFont* pFont, uint16_t c);
int   GUI_SetTextAlign(int Align);
int   GUI_SetTextMode(int Mode);
char  GUI_SetTextStyle(char Style);
int   GUI_SetLBorder(int x);
void  GUI_GotoXY(int x, int y);
void  GUI_DispNextLine(void);
int      GUI_UC_Encode(char* s, uint16_t Char);
int      GUI_UC_GetCharSize(const char * s);
uint16_t GUI_UC_GetCharCode(const char * s);
void     GUI_UC_SetEncodeNone(void);
void     GUI_UC2DB(uint16_t Code, uint8_t* pOut);
uint16_t GUI_DB2UC(uint8_t Byte0, uint8_t Byte1);
void * GUI_ALLOC_AllocInit(const void* pInitData, size_t Size);
void * GUI_ALLOC_AllocNoInit(size_t size);
void * GUI_ALLOC_AllocZero(size_t size);
void     GUI_ALLOC_Free(void *  hMem);
void     GUI_ALLOC_Init(void);
void * GUI_ALLOC_Realloc(void * hOld, int NewSize);
/******************************************************************
*
*                 Keyboard
*
*******************************************************************
*/
/* Message layer */
void GUI_StoreKeyMsg(int Key, int Pressed);
void GUI_SendKeyMsg(int Key, int Pressed);
bool GUI_PollKeyMsg(void);
/* Message hook */
typedef int GUI_KEY_MSG_HOOK(int Key, int Pressed);
extern  GUI_KEY_MSG_HOOK* GUI_pfKeyMsgHook;
GUI_KEY_MSG_HOOK* GUI_SetKeyMsgHook(GUI_KEY_MSG_HOOK* pHook);
/* Application layer */
int  GUI_GetKey(void);
void GUI_StoreKey(int c);
void GUI_ClearKeyBuffer(void);
void GUI_PID_StoreState(const PidState* pState);
int  GUI_PID_GetState(PidState* pState);

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
void GUI__DispLine(const char *s, int Len, const SRect *pr);
void GUI__CalcTextRect(const char *pText, const SRect *pTextRectIn, SRect *pTextRectOut, int TextAlign);
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
*        Cursors
*
*************************************************************
*/
extern const LogPalette GUI_CursorPal;
extern const LogPalette GUI_CursorPalI;
RGBC GUI_GetBitmapPixel(const Bitmap *pBMP, unsigned x, unsigned y);
extern GUI_CONTEXT     GUI;
extern void (*GUI_pfDispCharStyle)(uint16_t Char);
extern const GUI_UC_ENC_APILIST GUI__API_TableNone;
