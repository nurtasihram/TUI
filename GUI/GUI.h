#pragma once

#include <stdint.h>
#include <stddef.h>

#include "GUI__ID.inl"
#include "GUI__CharPix.inl"

#pragma region Colors
using RGBC = uint32_t;
constexpr RGBC RGBC_B(uint8_t b) { return b; }
constexpr RGBC RGBC_G(uint8_t g) { return g << 0x08; }
constexpr RGBC RGBC_R(uint8_t r) { return r << 0x10; }
constexpr RGBC RGBC_C(uint8_t r, uint8_t g, uint8_t b) { return RGBC_R(r) | RGBC_G(g) | RGBC_B(b); }
constexpr RGBC RGBC_GRAY(uint8_t l) { return RGBC_C(l, l, l); }
enum Color : RGBC {
	RGB_BLACK              = RGBC_GRAY(0x00),
	RGB_DARKGRAY           = RGBC_GRAY(0x40),
	RGB_GRAY               = RGBC_GRAY(0x80),
	RGB_LIGHTGRAY          = RGBC_GRAY(0xD3),
	RGB_WHITE              = RGBC_GRAY(0xFF),

	RGB_RED                = RGBC_R(0xFF),
	RGB_GREEN              = RGBC_G(0xFF),
	RGB_BLUE               = RGBC_B(0xFF),

	RGB_DARKRED            = RGBC_R(0x80),
	RGB_DARKGREEN          = RGBC_G(0x80),
	RGB_DARKBLUE           = RGBC_B(0x80),

	RGB_LIGHTBLUE          = RGBC_C(0x80, 0x80, 0xFF),
	RGB_LIGHTGREEN         = RGBC_C(0x80, 0xFF, 0x80),
	RGB_LIGHTRED           = RGBC_C(0xFF, 0x80, 0x80),

	RGB_BROWN              = RGBC_C(0xA5, 0x2A, 0x2A),

	RGB_YELLOW             = RGBC_C(0xFF, 0xFF, 0x00),
	RGB_LIGHTYELLOW        = RGBC_C(0xFF, 0xFF, 0x80),
	RGB_DARKYELLOW         = RGBC_C(0x80, 0x80, 0x00),

	RGB_MAGENTA            = RGBC_C(0xFF, 0x00, 0xFF),
	RGB_LIGHTMAGENTA       = RGBC_C(0xFF, 0x80, 0xFF),
	RGB_DARKMAGENTA        = RGBC_C(0x80, 0x00, 0x80),

	RGB_CYAN               = RGBC_C(0x00, 0xFF, 0xFF),
	RGB_LIGHTCYAN          = RGBC_C(0x80, 0xFF, 0xFF),
	RGB_DARKCYAN           = RGBC_C(0x00, 0x80, 0x80),

	RGB_INVALID_COLOR      = 0xFF000000,
};
#pragma endregion

#define DRAWMODE_NORMAL (0)
#define DRAWMODE_TRANS  (1<<1)
#define DRAWMODE_REV    (1<<2)
typedef int DRAWMODE;

template<class AnyType>
inline AnyType Max(AnyType a, AnyType b) { return a > b ? a : b; }
template<class AnyType>
inline AnyType Min(AnyType a, AnyType b) { return a < b ? a : b; }

struct Point {
	int16_t x, y;
	Point(int i = 0) : x(i), y(i) {}
	Point(int x, int y) : x(x), y(y) {}
	inline operator bool() const { return x | y; }
	inline Point operator~() const { return{ y, x }; }
	inline Point operator-() const { return{ -x, -y }; }
	inline Point operator+(int p) const { return{ x + p, y + p }; }
	inline Point operator-(int p) const { return{ x - p, y - p }; }
	inline Point operator+(Point p) const { return{ x + p.x, y + p.y }; }
	inline Point operator-(Point p) const { return{ x - p.x, y - p.y }; }
	inline Point&operator+=(Point p) { x += p.x, y += p.y; return*this; }
	inline Point&operator-=(Point p) { x -= p.x, y -= p.y; return*this; }
	inline bool operator==(Point p) const { return x == p.x && y == p.y; }
	inline bool operator!=(Point p) const { return x != p.x || y != p.y; }
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
	inline Point d() const { return { x1 - x0, y1 - y0 }; }
	inline auto dx() const { return x1 - x0; }
	inline auto dy() const { return y1 - y0; }
	inline SRect&move_to(Point p) { *this += p - left_top(); return*this; }
	inline SRect&xmove(int step) { x0 += step, x1 += step; return*this; }
	inline SRect&ymove(int step) { y0 += step, y1 += step; return*this; }
	inline SRect rot90() const { return { -y1, x0, -y0, x1 }; }
public:
	inline SRect operator*(int p) const { return{ x0 - p, y0 - p, x1 + p, y1 + p }; }
	inline SRect operator/(int p) const { return{ x0 + p, y0 + p, x1 - p, y1 - p }; }
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
		if (x0 > p.x) return false;
		if (y0 > p.y) return false;
		if (x1 < p.x) return false;
		if (y1 < p.y) return false;
		return true;
	}
	inline bool operator<=(const SRect &r) const {
		if (x0 > r.x0) return false;
		if (y0 > r.y0) return false;
		if (x1 < r.x1) return false;
		if (y1 < r.y1) return false;
		return true;
	}
	inline bool operator>(const Point &p) const { return !(*this <= p); }
	inline bool operator>(const SRect &r) const { return !(*this <= r); }
	inline bool operator&&(const SRect &r) const {
		if (x0 > r.x1) return false;
		if (y0 > r.y1) return false;
		if (x1 < r.x0) return false;
		if (y1 < r.y0) return false;
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
	const RGBC *pPalEntries = nullptr;
	size_t NumEntries = 0;
	LogPalette() {}
	LogPalette(const RGBC *pPalEntries, size_t NumEntries) :
		pPalEntries(pPalEntries), NumEntries(NumEntries) {}
	template<size_t Len>
	LogPalette(const RGBC (&Entries)[Len]) :
		pPalEntries(Entries), NumEntries(Len) {}
};
using CPalette = const LogPalette;

struct Bitmap {
	Point Size;
	uint16_t BytesPerLine;
	uint8_t BitsPerPixel;
	bool HasTrans;
	const void *pData;
	CPalette *pPal;
	Bitmap(
		Point Size,
		uint16_t BytesPerLine,
		const BPP1_DAT *pData,
		CPalette *pPal = nullptr,
		bool HasTrans = false) :
		Size(Size),
		BytesPerLine(BytesPerLine),
		BitsPerPixel(1),
		HasTrans(HasTrans),
		pData(pData),
		pPal(pPal) {}
	Bitmap(
		Point Size,
		uint16_t BytesPerLine,
		const BPP2_DAT *pData,
		CPalette *pPal = nullptr,
		bool HasTrans = false) :
		Size(Size),
		BytesPerLine(BytesPerLine),
		BitsPerPixel(2),
		HasTrans(HasTrans),
		pData(pData),
		pPal(pPal) {}
	Bitmap(
		Point Size,
		uint16_t BytesPerLine,
		uint8_t BitsPerPixel,
		const void *pData,
		CPalette *pPal = nullptr,
		bool HasTrans = false) :
		Size(Size),
		BytesPerLine(BytesPerLine),
		BitsPerPixel(BitsPerPixel),
		HasTrans(HasTrans),
		pData(pData),
		pPal(pPal) {}
	inline SRect Rect(Point p) const { return SRect::left_top(p, Size); }
	inline auto PalEntries() const { return pPal ? pPal->pPalEntries : nullptr; }
};
using CBitmap = const Bitmap;
struct BitmapRect : SRect {
	const void *pData;
	const RGBC *pPalEntries;
	uint16_t BytesPerLine;
	uint8_t BitsPerPixel;
	uint8_t BitsXOff = 0;
	bool HasTrans;
	BitmapRect(CBitmap &bmp, Point Pos);
	BitmapRect &operator&=(const SRect &);
};

#pragma region Cursor
struct Cursor : Bitmap {
	Point Hot;
	Cursor(CBitmap &bmp, Point Hot = 0) : Bitmap(bmp), Hot(Hot) {}
};
using CCursor = const Cursor;
class CursorCtl {
	RGBC *_pBuffer = nullptr;
	int _AllocSize = 0;
	SRect _Rect, _ClipRect;
	bool _CursorIsVis = false, _CursorOn = false;
	CCursor *_pCursor = nullptr;
	uint8_t _CursorDeActCnt = 0;
	Point Pos;
	RGBC _ColorIndex[4];
private:
	void _SetPixel(int x, int y, RGBC Color);
	RGBC _GetPixel(int x, int y);
	int _Log2Phys(int Index) {
		if (Index < 4)
			return _ColorIndex[Index];
		return _pCursor->pPal->pPalEntries[Index];
	}
private:
	void _CalcRect() {
		if (!_pCursor)
			return;
		_Rect.x0 = Pos.x - _pCursor->Hot.x;
		_Rect.y0 = Pos.y - _pCursor->Hot.y;
		_Rect.x1 = _Rect.x0 + _pCursor->Size.x - 1;
		_Rect.y1 = _Rect.y0 + _pCursor->Size.y - 1;
	}
	void _Hide() {
		if (!_CursorIsVis)
			return;
		_Undraw();
		_CursorIsVis = false;
	}
	void _Show() {
		if (!_CursorOn || _CursorDeActCnt)
			return;
		_CursorIsVis = true;
		_Draw();
	}
private:
	void _Draw();
	void _Undraw();
public:
	bool _TempHide(const SRect *pRect);
	void _TempShow(void);
	CCursor *Select(CCursor *pCursor);
public: // Property - Activate
	/* W */ void Activate(bool);
public: // Property - Visible
	/* W */ void Visible(bool);
public: // Property - Position
	/* R */ inline Point Position() const { return Pos; }
	/* W */ void Position(Point);
};

extern CPalette GUI_CursorPal;
extern CPalette GUI_CursorPalI;
extern CCursor
	GUI_CursorArrowS, GUI_CursorArrowSI,
	GUI_CursorArrowM, GUI_CursorArrowMI,
	GUI_CursorArrowL, GUI_CursorArrowLI,
	GUI_CursorCrossS, GUI_CursorCrossSI,
	GUI_CursorCrossM, GUI_CursorCrossMI,
	GUI_CursorCrossL, GUI_CursorCrossLI,
	GUI_CursorHeaderM, GUI_CursorHeaderMI;
extern CBitmap GUI_BitmapArrowS, GUI_BitmapArrowSI;
extern CBitmap GUI_BitmapArrowM, GUI_BitmapArrowMI;
extern CBitmap GUI_BitmapArrowL, GUI_BitmapArrowLI;
extern CBitmap GUI_BitmapCrossS, GUI_BitmapCrossSI;
extern CBitmap GUI_BitmapCrossM, GUI_BitmapCrossMI;
extern CBitmap GUI_BitmapCrossL, GUI_BitmapCrossLI;
#pragma endregion

struct GUI_DRAW_BASE {
	virtual Point Size() const { return 0; }
	void Paint(SRect);
protected:
	virtual void Draw(SRect) {}
};
struct GUI_DRAW_BMP : GUI_DRAW_BASE {
	CBitmap *pBitmap;
	GUI_DRAW_BMP(CBitmap *pBitmap) : pBitmap(pBitmap) {}
	void Draw(SRect)  override;
	Point Size() const override;
	static GUI_DRAW_BMP *Create(CBitmap *pBitmap);
};
using GUI_DRAW_CB = void(SRect);
struct GUI_DRAW_SELF : GUI_DRAW_BASE {
	GUI_DRAW_CB *pfDraw;
	GUI_DRAW_SELF(GUI_DRAW_CB *pfDraw) : pfDraw(pfDraw) {}
	void Draw(SRect)  override;
	static GUI_DRAW_SELF *Create(GUI_DRAW_CB *pfDraw);
};

struct PidState : Point {
	int8_t Pressed;
	PidState(Point p = 0, int8_t Pressed = 0) : Point(p), Pressed(Pressed) {}
	inline bool operator==(const PidState &pid) const { return (const Point &)*this == pid && Pressed == pid.Pressed; }
	inline bool operator!=(const PidState &pid) const { return (const Point &)*this != pid || Pressed != pid.Pressed; }
};

#pragma region Font
typedef uint16_t tGUI_GetCharCode(const char *s);
typedef int tGUI_GetCharSize(const char *s);
typedef int tGUI_CalcSizeOfChar(uint16_t Char);
typedef int tGUI_Encode(char *s, uint16_t Char);
typedef struct {
	tGUI_GetCharCode *pfGetCharCode;
	tGUI_GetCharSize *pfGetCharSize;
	tGUI_CalcSizeOfChar *pfCalcSizeOfChar;
	tGUI_Encode *pfEncode;
} GUI_UC_ENC_APILIST;

typedef int  tGUI_GetLineDistX(const char *s, int Len);
typedef int  tGUI_GetLineLen(const char *s, int MaxLen);
typedef void tGL_DispLine(const char *s, int Len);
typedef struct {
	tGUI_GetLineDistX *pfGetLineDistX;
	tGUI_GetLineLen *pfGetLineLen;
	tGL_DispLine *pfDispLine;
} tGUI_ENC_APIList;

typedef class Font Font;
typedef const Font CFont;
class Font {
public:
	const tGUI_ENC_APIList *pafEncode = nullptr;
	uint8_t YSize, YDist;
	uint8_t Baseline;
	uint8_t LHeight, CHeight;
public:
	Font(
		uint8_t YSize, uint8_t YDist,
		uint8_t Baseline,
		uint8_t LHeight, uint8_t CHeight) : 
		YSize(YSize), YDist(YDist),
		Baseline(Baseline),
		LHeight(LHeight), CHeight(CHeight) {}
public:
	virtual void DispChar(uint16_t) const = 0;
	virtual int  XDist(uint16_t) const = 0;
	virtual bool IsInFont(uint16_t) const = 0;
};
class FontProp : public Font {
public:
	struct CharInfo {
		uint8_t XSize;
		uint8_t XDist;
		uint8_t BytesPerLine;
		const BPP1_DAT *pData;
	};
	struct Prop {
		uint16_t First, Last;
		const CharInfo *paCharInfo;
		const Prop *pNext;
	public:
		Prop(uint16_t First, uint16_t Last,
			 const CharInfo *paCharInfo,
			 const Prop *pNext) :
			First(First), Last(Last),
			paCharInfo(paCharInfo),
			pNext(pNext) {}
	};
private:
	Prop prop;
public:
	FontProp(
		uint8_t YSize, uint8_t YDist,
		uint8_t Baseline,
		uint8_t LHeight, uint8_t CHeight,
		uint16_t First, uint16_t Last,
		const CharInfo *paCharInfo,
		const FontProp *pNext = nullptr) :
		Font(YSize, YDist, Baseline, LHeight, CHeight),
		prop(First, Last, paCharInfo, pNext ? &pNext->prop : nullptr) {}
private:
	const Prop *_FindChar(uint16_t) const;
public:
	void DispChar(uint16_t) const override;
	int  XDist(uint16_t) const override;
	bool IsInFont(uint16_t) const override;
};
class FontMono : public Font {
public:
	struct TransList {
		int16_t c0, c1;
	};
	struct TransInfo {
		uint16_t FirstChar, LastChar;
		const TransList *pList;
	};
private:
	const BPP1_DAT *pData;
	const BPP1_DAT *pTransData;
	const TransInfo *pTrans;
	uint16_t FirstChar, LastChar;
	uint8_t xSize, xDist;
	uint8_t BytesPerLine;
public:
	FontMono(
		uint8_t YSize, uint8_t YDist,
		uint8_t Baseline,
		uint8_t LHeight, uint8_t CHeight,
		const BPP1_DAT *pData,
		const BPP1_DAT *pTransData,
		const TransInfo *pTrans,
		uint16_t FirstChar, uint16_t LastChar,
		uint8_t XSize, uint8_t XDist,
		uint8_t BytesPerLine) : 
		Font(YSize, YDist, Baseline, LHeight, CHeight),
		pData(pData), pTransData(pTransData), pTrans(pTrans),
		FirstChar(FirstChar), LastChar(LastChar),
		xSize(xSize), xDist(xDist),
		BytesPerLine(BytesPerLine) {}
public:
	void DispChar(uint16_t) const override;
	int  XDist(uint16_t) const override;
	bool IsInFont(uint16_t) const override;
};

/* Proportional fonts */
extern const FontProp GUI_Font8_ASCII, GUI_Font8_1;
extern const FontProp GUI_Font13_ASCII, GUI_Font13_1;
/* Monospaced fonts*/
extern const FontMono GUI_Font6x8, GUI_Font6x9;

#pragma endregion

RGBC GUI_LCD_GetPixel(int x, int y);
void GUI_LCD_SetPixel(Point);
void GUI_LCD_FillRect(SRect);
void GUI_LCD_SetClipRectMax();
void GUI_LCD_SetClipRectEx(const SRect &rClip);
DRAWMODE GUI_LCD_SetDrawMode(DRAWMODE dm);
void GUI_LCD_DrawBitmap(BitmapRect);

SRect LCD_Rect();
void LCD_FillRect(const SRect&);
RGBC LCD_GetPixel(Point);
void LCD_SetPixel(Point, RGBC);
void LCD_DrawBitmap(const BitmapRect&);

#define GUI_LCD_ACOLOR  GUI.aColor

#define GUI_COUNTOF(a) (sizeof(a) / sizeof(a[0]))

/* Text alignment flags, horizontal */
#define GUI_TA_LEFT        (0<<0)
#define GUI_TA_RIGHT	   (1<<0)
#define GUI_TA_HCENTER	   (2<<0)
#define GUI_TA_HORIZONTAL  (3<<0)
/* Text alignment flags, vertical */
#define GUI_TA_TOP	      (0<<2)
#define GUI_TA_BOTTOM	  (1<<2)
#define GUI_TA_BASELINE   (2<<2)
#define GUI_TA_VCENTER    (3<<2)
using TEXTALIGN = uint8_t;

#define GUI_XMIN -4095
#define GUI_XMAX  4095
#define GUI_YMIN -4095
#define GUI_YMAX  4095

struct GUI_CONTEXT {
	/* Variables in LCD module */
	RGBC    aColor[2];
	SRect    rClip;
	DRAWMODE DrawMode;
	uint8_t TextStyle;
	/* Variables in GL module */
	SRect* pClipRect_HL; /* High level clip rectangle ... Speed optimization so drawing routines can optimize */
	/* Variables in GUICHAR module */
	CFont* pAFont;
	const GUI_UC_ENC_APILIST* pUC_API;
	uint8_t TextMode, TextAlign;
	Point dispPos;
	/* Variables in WM module */
	const SRect* pUserClipRect = nullptr;
	Point off;
	CursorCtl Cursor;

#pragma region Basic graphics
public:
	void Clear();
	void Clear(SRect);
	void FillRect(SRect);
	void DrawFocus(SRect, int Dist = 0);
	void DrawRect(SRect);
	inline void DrawLineH(int y0, int x0, int x1) { FillRect({ x0, y0, x1, y0 }); }
	inline void DrawLineV(int x0, int y0, int y1) { FillRect({ x0, y0, x0, y1 }); }
	void DrawBitmap(CBitmap &, Point);
#pragma endregion

#pragma region Properties
public: // Property - DispPos
	/* W */ inline void DispPos(Point Pos) { dispPos = Pos; }
	/* R */ inline auto DispPos() const { return dispPos; }
public: // Property - BkColor
	/* W */ inline void BkColor(RGBC Color) { aColor[0] = Color; }
	/* R */ inline RGBC BkColor() const { return aColor[0]; }
public: // Property - PenColor
	/* W */ void PenColor(RGBC Color) { aColor[1] = Color; }
	/* R */ inline RGBC PenColor() const { return aColor[1]; }
public: // Property - Font
	/* W */ inline void Font(CFont *pFont) { pAFont = pFont; }
	/* R */ inline CFont *Font() const { return pAFont; }
#pragma endregion

};

void GUI_Init(void);
void GUI_SetDefault(void);

#define GUI_SetDrawMode GUI_LCD_SetDrawMode
int  GUI__DivideRound(int a, int b);
bool GUI__SetText(char** ppText, const char* s);

RGBC GUI_GetBitmapPixel(CBitmap *pBMP, unsigned x, unsigned y);

void  GUI_DispChar(uint16_t c);
void  GUI_DispChars(uint16_t c, int NumChars);
void  GUI_DispCharAt(uint16_t c, Point Pos);
void  GUI_DispString(const char * s);
void  GUI_DispStringAt(const char * s, Point Pos);
void  GUI__DispStringInRect(const char * s, const SRect &r, int TextAlign, int MaxNumChars);
void  GUI_DispStringInRect(const char * s, const SRect &r, int Flags);
void  GUI_DispStringInRect(const char * s, const SRect &r, int TextAlign, int MaxLen); /* Not to be doc. */
void  GUI_DispStringLen(const char * s, int Len);
void  GUI_GetTextExtend(SRect* pRect, const char * s, int Len);
int   GUI_GetYAdjust(void);
int   GUI_GetCharDistX(uint16_t c);
int   GUI_GetStringDistX(const char * s);
int   GUI_GetFontSizeY(void);
int   GUI_GetYSizeOfFont(CFont* pFont);
int   GUI_GetYDistOfFont(CFont* pFont);
int   GUI_GetTextAlign(void);
int   GUI_GetTextMode(void);
int   GUI_SetTextAlign(int Align);
int   GUI_SetTextMode(int Mode);
char  GUI_SetTextStyle(char Style);
void  GUI_DispNextLine(void);
int      GUI_UC_Encode(char* s, uint16_t Char);
int      GUI_UC_GetCharSize(const char * s);
uint16_t GUI_UC_GetCharCode(const char * s);
void     GUI_UC_SetEncodeNone(void);
void     GUI_UC2DB(uint16_t Code, uint8_t* pOut);
uint16_t GUI_DB2UC(uint8_t Byte0, uint8_t Byte1);
void * GUI_ALLOC_Alloc(size_t size);
void * GUI_ALLOC_AllocZero(size_t size);
void     GUI_ALLOC_Free(void *);
void * GUI_ALLOC_Realloc(void *hOld, int NewSize);

struct TString {
	char *pText = nullptr;
	TString() {}
	TString(const TString &s) { GUI__SetText(&pText, s); }
	TString(const char *s) { GUI__SetText(&pText, s); }
	~TString() {
		GUI_ALLOC_Free(pText);
		pText = nullptr;
	}
	inline char **operator&() { return &pText; }
	inline void operator=(const char *s) { GUI__SetText(&pText, s); }
	inline operator const char *() const { return pText; }
};

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
#define GUI_DEFAULT_BKCOLOR RGB_BLACK
#define GUI_DEFAULT_COLOR   RGB_WHITE

enum GUI_WRAPMODE {
	GUI_WRAPMODE_NONE,
	GUI_WRAPMODE_WORD,
	GUI_WRAPMODE_CHAR
};

#define GUI_ZEROINIT(Obj) GUI__memset(Obj, 0, sizeof(Obj))
#define GUI_UC__GetCharSize(sText)  GUI.pUC_API->pfGetCharSize(sText)
#define GUI_UC__GetCharCode(sText)  GUI.pUC_API->pfGetCharCode(sText)
int      GUI_UC__CalcSizeOfChar(uint16_t Char);
uint16_t GUI_UC__GetCharCodeInc(const char **ps);
int      GUI_UC__NumChars2NumBytes(const char *s, int NumChars);
int      GUI_UC__NumBytes2NumChars(const char *s, intptr_t NumBytes);
int  GUI__GetLineNumChars(const char *s, int MaxNumChars);
int  GUI__GetNumChars(const char *s);
int  GUI__GetLineDistX(const char *s, int Len);
int  GUI__HandleEOLine(const char **ps);
void GUI__DispLine(const char *s, int MaxNumChars, SRect r);
void GL_DispChar(uint16_t c);

void GUI__CalcTextRect(const char *pText, const SRect *pTextRectIn, SRect *pTextRectOut, int TextAlign);

int  GUI__WrapGetNumCharsDisp(const char *pText, int xSize, GUI_WRAPMODE WrapMode);
int  GUI__WrapGetNumCharsToNextLine(const char *pText, int xSize, GUI_WRAPMODE WrapMode);
int  GUI__WrapGetNumBytesToNextLine(const char *pText, int xSize, GUI_WRAPMODE WrapMode);

int  GUI__strlen(const char *s);
void GUI__strcpy(char *s, const char *c);
int  GUI__strcmp(const char *s0, const char *s1);
void GUI__memset(void *d, uint8_t Fill, size_t NumBytes);
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len);
int  GUI__memcmp(void *sDest, const void *pSrc, size_t Len);
uint16_t GUI__countStrings(const char *pStrings);
const char *GUI__nextString(const char *pStrings);

extern GUI_CONTEXT GUI;
extern void (*GUI_pfDispCharStyle)(uint16_t Char);
extern const GUI_UC_ENC_APILIST GUI__API_TableNone;
