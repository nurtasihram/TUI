#pragma once

#include <stdint.h>

#include "GUI.inl"

template<class AnyType>
inline AnyType Max(AnyType a, AnyType b) { return a > b ? a : b; }
template<class AnyType>
inline AnyType Min(AnyType a, AnyType b) { return a < b ? a : b; }

#pragma region Coordinates
/// @brief 點
///		用扵存儲坐標或尺寸
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

/// @brief 矩形區域
struct SRect {
	int16_t x0 = 0, y0 = 0;
	int16_t x1 = -1, y1 = -1;
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
	inline SRect rot270() const { return { y0, -x1,  y1, -x0 }; }
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
#pragma endregion

#pragma region Image
/// @brief 調色板類
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

/// @brief 位圖類
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

/// @brief 光標類
struct Cursor : Bitmap {
	Point Hot;
	Cursor(CBitmap &bmp, Point Hot = 0) : Bitmap(bmp), Hot(Hot) {}
};
using CCursor = const Cursor;

/// @brief 位圖矩形類
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

public:
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

/// @brief 按鍵數據結構
struct KEY_STATE {
	uint16_t Key = 0, PressedCnt = 0;
};

/// @brief 鼠標數據結構
struct MOUSE_STATE : Point {
	int8_t Pressed;
	MOUSE_STATE(Point p = 0, int8_t Pressed = 0) : Point(p), Pressed(Pressed) {}
	inline MOUSE_STATE operator+(const MOUSE_STATE &pid) const { return{ Point::operator+(pid), pid.Pressed }; }
	inline bool operator==(const MOUSE_STATE &pid) const { return (const Point &)*this == pid && Pressed == pid.Pressed; }
	inline bool operator!=(const MOUSE_STATE &pid) const { return (const Point &)*this != pid || Pressed != pid.Pressed; }
};

using GUI_CHAR = int;
using GUI_PSTR = char *;
using GUI_PCSTR = const char *;

struct STRING_SIZE {
	uint16_t NumChars = 0;
	uint16_t NumBytes = 0;
};
struct GUI_Encoder {
	virtual GUI_CHAR CharNext(GUI_PCSTR &s) { return *s++; }
	virtual uint8_t CharSize(GUI_PCSTR s) { return 1; }
	virtual STRING_SIZE Size(GUI_PCSTR pText) {
		if (!pText)
			return {};
		uint16_t NumChars = 0;
		for (; *pText; ++pText)
			++NumChars;
		return{ NumChars, (uint16_t)(NumChars + 1) };
	}
};
extern GUI_Encoder DefaultEncoder;

#pragma region Font
/// @brief 字體類
class Font {

public:
	GUI_Encoder &encoder;
	uint8_t YDist, Baseline;
	uint8_t LHeight, CHeight;

public:
	Font(uint8_t YDist, uint8_t Baseline,
		 uint8_t LHeight, uint8_t CHeight,
		 GUI_Encoder &encoder = DefaultEncoder) :
		encoder(encoder),
		YDist(YDist), Baseline(Baseline),
		LHeight(LHeight), CHeight(CHeight) {}

public:

	/// @brief 顯示字元
	/// @param ch 字元
	/// @return 顯示字元的寬度，單位為圖元
	virtual int  DispChar(GUI_CHAR ch) const = 0;

	/// @brief 字元寬度
	/// @param ch 字元
	/// @return 字元寬度，單位為圖元
	virtual int XDist(GUI_CHAR ch) const = 0;

	/// @brief 計算字串的寬度
	/// @param pString 字串
	/// @param NumChars 字串長度
	/// @return 字串的寬度，單位為圖元
	virtual int XDist(GUI_PCSTR pString, uint16_t NumChars) const;

	/// @brief 是否在字體範圍內
	/// @param ch 字元
	/// @return 在字體範圍內返回true，否則返回false
	virtual bool IsInFont(GUI_CHAR ch) const = 0;

	/// @brief 字串的大小
	/// @param s 字串
	/// @return 字串的大小，單位為圖元
	Point Size(GUI_PCSTR s) const;

};
using CFont = const Font;

/// @brief 等綫字體
class FontProp : public Font {
public:

	/// @brief 字形數據結構
	struct Glyph {
		uint8_t XDist, BytesPerLine;
		const BPP1_DAT *pData;
	};

	/// @brief 字形片段存儲結構
	struct Prop {
		uint16_t First, Last;
		const Glyph *paGlyph;
		const Prop *pNext;
	};

private:
	Prop prop;

public:
	FontProp(uint8_t YDist, uint8_t Baseline,
			 uint8_t LHeight, uint8_t CHeight,
			 uint16_t First, uint16_t Last,
			 const Glyph *paGlyph,
			 GUI_Encoder &encoder = DefaultEncoder) :
		Font(YDist, Baseline, LHeight, CHeight, encoder),
		prop({ First, Last, paGlyph, nullptr }) {}
	FontProp(uint8_t YDist, uint8_t Baseline,
			 uint8_t LHeight, uint8_t CHeight,
			 uint16_t First, uint16_t Last,
			 const Glyph *paGlyph,
			 const FontProp *pNext,
			 GUI_Encoder &encoder = DefaultEncoder) :
		Font(YDist, Baseline, LHeight, CHeight, encoder),
		prop({ First, Last, paGlyph, &pNext->prop }) {}

private:
	const Prop *_FindChar(GUI_CHAR) const;

public:
	int DispChar(GUI_CHAR) const override;
	int XDist(GUI_CHAR) const override;
	bool IsInFont(GUI_CHAR) const override;

};

/// @brief 等寬字體
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
	const BPP1_DAT *pData, *pTransData;
	const TransInfo *pTrans;
	uint16_t FirstChar, LastChar;
	uint8_t xSize, xDist;
	uint8_t BytesPerLine;

public:
	FontMono(uint8_t YDist, uint8_t Baseline,
			 uint8_t LHeight, uint8_t CHeight,
			 const BPP1_DAT *pData,
			 const BPP1_DAT *pTransData,
			 const TransInfo *pTrans,
			 uint16_t FirstChar, uint16_t LastChar,
			 uint8_t XSize, uint8_t XDist,
			 uint8_t BytesPerLine,
			 GUI_Encoder &encoder = DefaultEncoder) :
		Font(YDist, Baseline, LHeight, CHeight, encoder),
		pData(pData), pTransData(pTransData),
		pTrans(pTrans),
		FirstChar(FirstChar), LastChar(LastChar),
		xSize(XSize), xDist(XDist),
		BytesPerLine(BytesPerLine) {}

public:
	int DispChar(GUI_CHAR) const override;
	int XDist(GUI_CHAR) const override;
	bool IsInFont(GUI_CHAR) const override;

};
#pragma endregion
