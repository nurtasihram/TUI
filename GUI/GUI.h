#pragma once

#include <stdint.h>
#include <stddef.h>

#include "GUI.inl"

template<class AnyType>
inline AnyType Max(AnyType a, AnyType b) { return a > b ? a : b; }
template<class AnyType>
inline AnyType Min(AnyType a, AnyType b) { return a < b ? a : b; }

#pragma region Flat
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
	inline Point &operator+=(Point p) { x += p.x, y += p.y; return*this; }
	inline Point &operator-=(Point p) { x -= p.x, y -= p.y; return*this; }
	inline bool operator==(Point p) const { return x == p.x && y == p.y; }
	inline bool operator!=(Point p) const { return x != p.x || y != p.y; }
};
struct SRect {
	int16_t x0 = 0, y0 = 0;
	int16_t x1 = 0, y1 = 0;
	SRect() {}
	SRect(Point p0) :
		x0(p0.x), y0(p0.y), x1(p0.x), y1(p0.y) {}
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
	inline SRect &move_to(Point p) { *this += p - left_top(); return*this; }
	inline SRect &xmove(int step) { x0 += step, x1 += step; return*this; }
	inline SRect &ymove(int step) { y0 += step, y1 += step; return*this; }
	inline SRect rot90()  const { return { -y1,  x0, -y0,  x1 }; }
	inline SRect rot270() const { return {  y0, -x1,  y1, -x0 }; }
	SRect &AlignTo(ALIGN a, const SRect &r2);
	inline SRect operator~() const { return{ ~left_top(), ~right_bottom() }; }
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
constexpr int16_t
	GUI_XMIN = -4095,
	GUI_XMAX = 4095,
	GUI_YMIN = -4095,
	GUI_YMAX = 4095;
#pragma endregion

#pragma region Image
struct LogPalette {
	const RGBC *pPalEntries = nullptr;
	size_t NumEntries = 0;
	LogPalette() {}
	LogPalette(const RGBC *pPalEntries, size_t NumEntries) :
		pPalEntries(pPalEntries), NumEntries(NumEntries) {}
	template<size_t Len>
	LogPalette(const RGBC(&Entries)[Len]) :
		pPalEntries(Entries), NumEntries(Len) {}
};
using CPalette = const LogPalette;
struct Bitmap {
	Point Size;
	uint16_t BytesPerLine = 0;
	uint8_t BitsPerPixel = 0;
	bool HasTrans = false;
	void *pData = nullptr;
	CPalette *pPal = nullptr;
public:
	Bitmap() {}
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
		pData(const_cast<BPP1_DAT *>(pData)),
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
		pData(const_cast<BPP2_DAT *>(pData)),
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
		pData(const_cast<void *>(pData)),
		pPal(pPal) {}
public:
	inline SRect Rect(Point p) const { return SRect::left_top(p, Size); }
	inline auto PalEntries() const { return pPal ? pPal->pPalEntries : nullptr; }
public:
	struct BitmapRect operator+(Point Pos);
	const struct BitmapRect operator+(Point Pos) const;
};
using CBitmap = const Bitmap;
struct BitmapRect : SRect {
	void *pData = nullptr;
	const RGBC *pPalEntries = nullptr;
	uint16_t BytesPerLine = 0;
	uint8_t BitsPerPixel = 0;
	uint8_t BitsXOff = 0;
	bool HasTrans = false;
public:
	BitmapRect() {}
	BitmapRect(const SRect &r) : SRect(r) {}
	BitmapRect(CBitmap &bmp, Point Pos = 0);
public:
	inline auto CountPixel() const { return xsize() * ysize(); }
	inline auto Bytes() const { return ysize() * BytesPerLine; }
	BitmapRect &operator&=(const SRect &rClip);
	inline const BitmapRect &operator&=(const SRect &rClip) const { return const_cast<BitmapRect &>(*this) &= rClip; }
	inline BitmapRect operator&(const SRect &rClip) { auto br = *this; br &= rClip; return br; }
	inline const BitmapRect operator&(const SRect &rClip) const { return const_cast<BitmapRect &>(*this) & rClip; }
	inline BitmapRect &operator=(const SRect &r) { SRect::operator=(r); return*this; }
};
using CBitmapRect = const BitmapRect;
inline BitmapRect Bitmap::operator+(Point Pos) {
	BitmapRect br = *this;
	br += Pos;
	return br;
}
inline const BitmapRect Bitmap::operator+(Point Pos) const {
	BitmapRect br = *this;
	br += Pos;
	return br;
}
#pragma endregion

struct PID_STATE : Point {
	int8_t Pressed;
	PID_STATE(Point p = 0, int8_t Pressed = 0) : Point(p), Pressed(Pressed) {}
	inline PID_STATE operator+(const PID_STATE &pid) const { return{ Point::operator+(pid), pid.Pressed }; }
	inline bool operator==(const PID_STATE &pid) const { return (const Point &)*this == pid && Pressed == pid.Pressed; }
	inline bool operator!=(const PID_STATE &pid) const { return (const Point &)*this != pid || Pressed != pid.Pressed; }
};

static constexpr uint8_t
	SIZE_RESIZE_X		= 1 << 0,
	SIZE_RESIZE_Y		= 1 << 1,
	SIZE_REPOS_X		= 1 << 2,
	SIZE_REPOS_Y		= 1 << 3,
	SIZE_MOUSEMOVE		= 1 << 4,
	SIZE_MOUSEOVER		= 1 << 5;
static constexpr uint8_t
	SIZE_RESIZE =
	SIZE_RESIZE_X | SIZE_RESIZE_Y |
	SIZE_REPOS_X | SIZE_REPOS_Y |
	SIZE_MOUSEMOVE;

#pragma region Cursor
struct Cursor : Bitmap {
	Point Hot;
	Cursor(CBitmap &bmp, Point Hot = 0) : Bitmap(bmp), Hot(Hot) {}
};
using CCursor = const Cursor;
class CursorCtl {
	BitmapRect brClip, brCursor;
	CCursor *_pCursor = nullptr;
	bool _CursorIsVis = false, _CursorOn = false;
	uint8_t _CursorDeActCnt = 0;
	Point Pos;
private:
	void _Hide();
private:
	void _Draw();
	void _Undraw();
public:
	bool _TempHide(const SRect *pRect);
	void _TempShow();
	static CCursor *GetResizeCursor(uint8_t Mode);
public:
	CCursor *operator()(CCursor *pCursor);
	inline operator CCursor *() const { return _pCursor; }
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
#pragma endregion

#pragma region Draw
struct GUI_DRAW_BASE {
	virtual void Draw(SRect) {}
	virtual Point Size() { return 0; }
};
struct GUI_DRAW_BMP : GUI_DRAW_BASE {
	CBitmap *pBitmap;
	GUI_DRAW_BMP(CBitmap *pBitmap) : pBitmap(pBitmap) {}
	void Draw(SRect) override;
	Point Size() override { return pBitmap->Size; }
	static GUI_DRAW_BMP *Create(CBitmap *pBitmap);
};
using GUI_DRAW_CB = void(SRect);
struct GUI_DRAW_SELF : GUI_DRAW_BASE {
	GUI_DRAW_CB *pfDraw;
	GUI_DRAW_SELF(GUI_DRAW_CB *pfDraw) : pfDraw(pfDraw) {}
	void Draw(SRect) override;
	static GUI_DRAW_SELF *Create(GUI_DRAW_CB *pfDraw);
};
#pragma endregion

#pragma region Font
typedef class Font Font;
typedef const Font CFont;
class Font {
public:
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
	virtual int  XDist(const char *pString, int NumChars) const;
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
		xSize(XSize), xDist(XDist),
		BytesPerLine(BytesPerLine) {}
public:
	void DispChar(uint16_t) const override;
	int  XDist(uint16_t) const override;
	bool IsInFont(uint16_t) const override;
};
#pragma endregion

#include "Resources.inl"

#pragma region LCD
void GUI_LCD_SetBitmap(BitmapRect);

void LCD_SetBitmap(const BitmapRect &);
void LCD_GetBitmap(BitmapRect &);
void LCD_FillRect(const SRect &);

BitmapRect LCD_AllocBitmap(const SRect &);
void LCD_FreeBitmap(BitmapRect &);
#pragma endregion

SRect LCD_Rect();

void GUI_DispChar(uint16_t c);
void GUI_DispString(const char *s);
void GUI_DispStringAt(const char *s, Point Pos);
int  GUI_GetYAdjust();
void GUI_DispNextLine();

void GUI__strcpy(char *s, const char *c);
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len);

uint16_t GUI__NumTexts(const char *pTexts);
const char *GUI__NextText(const char *pTexts);
uint16_t GUI__NumLines(const char *pTexts);
const char *GUI__NextLines(const char *pText);
uint16_t GUI__NumCharsLine(const char *pText);
uint16_t GUI__NumChars(const char *pText);
void GUI__SetText(char **ppText, const char *pText);

void *GUI_MEM_Alloc(size_t size);
void *GUI_MEM_AllocZero(size_t size);
void  GUI_MEM_Free(void *);
void *GUI_MEM_Realloc(void *hOld, int NewSize);

struct TString {
	char *pText = nullptr;
	TString() {}
	TString(const TString &s) { GUI__SetText(&pText, s); }
	TString(const char *s) { GUI__SetText(&pText, s); }
	~TString() {
		GUI_MEM_Free(pText);
		pText = nullptr;
	}
	inline char **operator&() { return &pText; }
	inline void operator=(const char *s) { GUI__SetText(&pText, s); }
	inline operator const char *() const { return pText; }
};

/* Message layer */
void GUI_StoreKeyMsg(int Key, int Pressed);
void GUI_SendKeyMsg(int Key, int Pressed);
bool GUI_PollKeyMsg();

/* Application layer */
int  GUI_GetKey();
void GUI_StoreKey(int c);
void GUI_ClearKeyBuffer();

void GUI__DispLine(const char *s, int MaxNumChars, Point Pos);

struct GUI_PANEL {
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		TEXTSTYLES TextStyle{ TS_NORMAL };
		TEXTALIGN TextAlign{ TEXTALIGN_LEFT | TEXTALIGN_TOP };
		CCursor *pCursor{ &GUI_CursorArrow };
		RGBC aColor[2]{
			RGB_BLACK,
			RGB_WHITE
		};
	} Props;
	static Property DefaultProps;
	/* Variables in LCD module */
	SRect rClip;
	/* Variables in GUICHAR module */
	Point dispPos, off;
	/* Variables in WM module */
	const SRect *prUserClip = nullptr;
	/* Cursor Control */
	CursorCtl Cursor;
	PID_STATE PID_STATE;

	void Init();

#pragma region Basic Graphics
public:
	void Clear();
	void Clear(SRect);
	void Fill(SRect);
	void OutlineFocus(SRect, int Dist = 0);
	void Outline(SRect);
	void DrawBitmap(CBitmap &, Point);
	inline void DrawLineH(int x0, int y0, int x1) { Fill({ x0, y0, x1, y0 }); }
	inline void DrawLineV(int x0, int y0, int y1) { Fill({ x0, y0, x0, y1 }); }
public:
	inline void ClipRect() { rClip = LCD_Rect(); }
	inline void ClipRect(const SRect &r) { rClip = LCD_Rect() & r; }
	inline void RevColor() {
		auto tmp = Props.aColor[0];
		Props.aColor[0] = Props.aColor[1];
		Props.aColor[1] = tmp;
	}
public:
	void DispString(const char *s, SRect r, int MaxNumChars = 0x7fff);
#pragma endregion

	int XDist(const char *pString, int NumChars) { return Props.pFont->XDist(pString, NumChars); }
	inline int XDist(const char *pString) { return XDist(pString, GUI__NumChars(pString)); }

#pragma region Properties
public: // Property - DispPos
	/* W */ inline void DispPos(Point Pos) { dispPos = Pos; }
	/* R */ inline auto DispPos() const { return dispPos; }
public: // Property - BkColor
	/* W */ inline void BkColor(RGBC Color) { Props.aColor[0] = Color; }
	/* R */ inline RGBC BkColor() const { return Props.aColor[0]; }
public: // Property - PenColor
	/* W */ void PenColor(RGBC Color) { Props.aColor[1] = Color; }
	/* R */ inline RGBC PenColor() const { return Props.aColor[1]; }
public: // Property - Palette
	/* R */ CPalette Palette() const { return Props.aColor; }
public: // Property - Font
	/* W */ inline void Font(CFont *pFont) { Props.pFont = pFont; }
	/* R */ inline auto Font() const { return Props.pFont; }
public: // Property - TextStyle
	/* W */ inline void TextStyle(TEXTSTYLES Style) { Props.TextStyle = Style; }
	/* R */ inline auto TextStyle() const { return Props.TextStyle; }
public: // Property - TextAlign
	/* W */ inline void TextAlign(TEXTALIGN Align) { Props.TextAlign = Align; }
	/* R */ inline auto TextAlign() const { return Props.TextAlign; }
#pragma endregion

};
extern GUI_PANEL GUI, GUI_Default;
