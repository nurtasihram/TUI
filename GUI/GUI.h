#pragma once

#include "GUI_Types.h"

#include "GUI_X.h"

#include "Resources/Resources.inl"

/// @brief 光標控制類
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
	static CCursor *ResizeCursor(uint8_t Mode);
public:
	CCursor *operator()(CCursor *pCursor);
	inline operator CCursor *() const { return _pCursor; }
public: // Property - Activate
	/* W */ void Activate(bool);
public: // Property - Visible
	/* R */ inline bool Visible() const { return _CursorOn && _pCursor; }
	/* W */ void Visible(bool);
public: // Property - Position
	/* R */ inline Point Position() const { return Pos; }
	/* W */ void Position(Point);
};

#pragma region GUI_DRAW
struct GUI_DRAW_BASE {
	virtual void Draw(SRect) {}
	virtual Point Size() { return 0; }
};
struct GUI_DRAW_BMP : GUI_DRAW_BASE {
	CBitmap &bmp;
	GUI_DRAW_BMP(CBitmap &bmp) : bmp(bmp) {}
	void Draw(SRect) override;
	Point Size() override { return bmp.Size; }
	static GUI_DRAW_BMP *Create(CBitmap &);
};
using GUI_DRAW_CB = void(SRect);
struct GUI_DRAW_SELF : GUI_DRAW_BASE {
	GUI_DRAW_CB *pfDraw;
	GUI_DRAW_SELF(GUI_DRAW_CB *pfDraw) : pfDraw(pfDraw) {}
	void Draw(SRect) override;
	static GUI_DRAW_SELF *Create(GUI_DRAW_CB *pcb);
};
class GUI_DRAW {
	mutable GUI_DRAW_BASE *pDrawObj;
public:
	GUI_DRAW(const GUI_DRAW &obj) : pDrawObj(obj.pDrawObj) { obj.pDrawObj = nullptr; }
	GUI_DRAW(GUI_DRAW_CB *pcb) : pDrawObj(GUI_DRAW_SELF::Create(pcb)) {}
	GUI_DRAW(CBitmap &bmp) : pDrawObj(GUI_DRAW_BMP::Create(bmp)) {}
	GUI_DRAW(GUI_DRAW_BASE *pDrawObj = nullptr) : pDrawObj(pDrawObj) {}
	~GUI_DRAW();
public:
	inline operator bool() const { return pDrawObj; }
	inline void operator=(const GUI_DRAW &DrawObj) {
		pDrawObj = DrawObj.pDrawObj;
		DrawObj.pDrawObj = nullptr;
	}
	inline void Draw(SRect r) const {
		if (pDrawObj)
			pDrawObj->Draw(r);
	}
	inline Point Size() const {
		if (pDrawObj)
			return pDrawObj->Size();
		return 0;
	}
};
#pragma endregion

/// @brief 繪圖面板
class GUI_Panel {

private:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		TEXTALIGN TextAlign{ TEXTALIGN_LEFT | TEXTALIGN_TOP };
		CCursor *pCursor{ &GUI_CursorArrow };
		RGBC aColor[2]{
			RGB_BLACK,
			RGB_WHITE
		};
	} Props;
	/// @brief 繪圖矩形裁剪區域
	SRect rClip;
	SRect rScreen, rnScreen;
	/// @brief 繪圖偏移量
	Point off;
	/// @brief 字串顯示開始位置
	Point dispPos;
	/// @brief 編碼器
	GUI_Encoder *pEncoder = nullptr;
	/// @brief 光標數據結構
	MOUSE_STATE _MouseLast, _MouseNow;
	/// @brief 
	KEY_STATE _KeyLast, _KeyNow;

public:
	/// @brief 光標控制器
	CursorCtl Cursor;

	void Init();

#pragma region Graphics
public: // 基礎繪圖方法

	/// @brief 清空銀幕
	///		使用背景筆刷 清空整個銀幕
	void Clear();

	/// @brief 清空銀幕
	///		使用背景筆刷 清空指定矩形區域
	/// @param rClear 指定清空的矩形區域
	void Clear(SRect rClear);

	/// @brief 填充區域
	///		使用前景筆刷 填滿指定矩形區域
	/// @param rFill 指定填滿的矩形區域
	void Fill(SRect rFill);

	/// @brief 繪製焦點框綫
	///		使用前景筆刷 繪製焦點框綫
	/// @param rFocus 指定焦點框綫的矩形區域
	/// @param Dist 框綫收縮量
	void DrawFocus(SRect rFocus, int Dist = 0);
	
	/// @brief 繪製框綫
	///		使用前景筆刷 繪製框綫
	/// @param rOutline 指定焦點框綫的
	void Outline(SRect rOutline);

	/// @brief 繪製水平綫
	///		使用前景筆刷 繪製水平綫
	/// @param x0 起始點x
	/// @param y0 起始點y
	/// @param x1 終止點x
	inline void DrawLineH(int x0, int y0, int x1) { Fill({ x0, y0, x1, y0 }); }

	/// @brief 繪製垂直綫
	///		使用前景筆刷 繪製垂直綫
	/// @param x0 起始點x
	/// @param y0 起始點y
	/// @param y1 終止點y
	inline void DrawLineV(int x0, int y0, int y1) { Fill({ x0, y0, x0, y1 }); }

	/// @brief 繪製位圖
	/// @param bm 位圖
	/// @param st 位圖起始點
	void DrawBitmap(CBitmap &bm, Point st);

	/// @brief 繪製位圖
	/// @param bc 位圖區域
	void DrawBitmap(BitmapRect bc);

public: // 字串繪製方法

	/// @brief 繪製字串
	/// @param s 字串緩衝區
	/// @param r 繪製區域
	/// @return 字串所佔區域
	SRect DrawStringIn(GUI_PCSTR s, SRect r);

#pragma endregion

	int XDist(GUI_PCSTR pString, int NumChars) { return Props.pFont->XDist(pString, NumChars); }
	inline int XDist(GUI_PCSTR pString) { return XDist(pString, NumChars(pString)); }

public:
	inline uint16_t NumChars(GUI_PCSTR pText) { return pEncoder->Size(pText).NumChars; }
	inline uint16_t NumBytes(GUI_PCSTR pText) { return pEncoder->Size(pText).NumBytes; }
	uint16_t NumCharsLine(GUI_PCSTR pText);

	inline GUI_CHAR CharNext(GUI_PCSTR &pText) { return pEncoder->CharNext(pText); }
	inline GUI_CHAR Char(GUI_PCSTR pText) { return pEncoder->CharNext(pText); }

	uint16_t NumTexts(GUI_PCSTR pText);
	GUI_PCSTR NextText(GUI_PCSTR pTexts);

	uint16_t NumLines(GUI_PCSTR pText);
	GUI_PCSTR NextLines(GUI_PCSTR pText);
	
	void SetText(GUI_PSTR *ppText, GUI_PCSTR pText);

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) { Props.pFont = pFont, pEncoder = &pFont->encoder; }
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.TextAlign; }
	/* W */ inline void TextAlign(TEXTALIGN Align) { Props.TextAlign = Align; }
public: // Property - BkColor
	/* R */ inline RGBC BkColor() const { return Props.aColor[0]; }
	/* W */ inline void BkColor(RGBC Color) { Props.aColor[0] = Color; }
public: // Property - PenColor
	/* R */ inline RGBC PenColor() const { return Props.aColor[1]; }
	/* W */ void PenColor(RGBC Color) { Props.aColor[1] = Color; }
public: // Property - Palette
	/* R */ CPalette Palette() const { return Props.aColor; }
public: // Property - DispPos
	/* R */ inline auto DispPos() const { return dispPos; }
	/* W */ inline void DispPos(Point Pos) { dispPos = Pos; }
public: // Property - Off
	/* R */ inline auto Off() const { return off; }
	/* W */ inline void Off(Point off) { this->off = off; }
public: // Property - ClipRect
	/* W */ inline void ClipRect(const SRect &r) { rClip = rScreen & r; }
	/* A */ inline void ClipRect() { rClip = rScreen; }
public: // Property - Mouse
	/* W */ inline auto Mouse(MOUSE_STATE State) {
		auto old = _MouseLast;
		_MouseLast = _MouseNow;
		_MouseNow = State;
		return old;
	}
	/* R */ inline auto MouseNA() { return _MouseLast == _MouseNow; }
	/* R */ inline auto Mouse() { return _MouseLast = _MouseNow; }
	/* R */ inline auto MousePrev() { return _MouseLast; }
public: // Property - Key
	/* W */ inline auto Key(KEY_STATE State) {
		auto old = _KeyLast;
		_KeyLast = _KeyNow;
		_KeyNow = State;
		return old;
	}
	/* R */ inline auto KeyNA() { return _KeyLast == _KeyNow; }
	/* R */ inline auto Key() { return _KeyLast = _KeyNow; }
	/* R */ inline auto KeyPrev() { return _KeyLast; }
public: // Property - Rect
	/* W */ inline void Rect(SRect rs) { rnScreen = rs; }
	/* W */ inline auto RectNA() { return rScreen == rnScreen; }
	/* A */ inline auto RectN() { return rScreen = rnScreen; }
	/* R */ inline auto Rect() { return rScreen; }
#pragma endregion

};
extern GUI_Panel GUI;

struct GUI_ISTRING {
	mutable GUI_PSTR pText = nullptr;
public:
	GUI_ISTRING() {}
	GUI_ISTRING(GUI_PSTR pText) : pText(pText) {}
public:
	inline auto Chars() const { return GUI.NumChars(pText); }
	inline auto Bytes() const { return GUI.NumBytes(pText); }
public:
//	inline GUI_ISTRING operator()(uint16_t num) {}
	inline GUI_ISTRING operator[](uint16_t bytes) { return pText + bytes; }
	inline GUI_PSTR *operator&() { return &pText; }
	inline GUI_CHAR operator*() const { return GUI.Char(pText); }
	inline GUI_CHAR operator++() const { return GUI.CharNext((GUI_PCSTR &)pText); }
	inline operator GUI_PSTR() { return pText; }
	inline operator GUI_PCSTR() const { return pText; }
};
struct GUI_STRING : GUI_ISTRING {
	GUI_STRING() {}
	GUI_STRING(const GUI_STRING &s) { GUI.SetText(&pText, s); }
	GUI_STRING(GUI_PCSTR s) { GUI.SetText(&pText, s); }
	~GUI_STRING() {
		if (pText) {
			GUI_MEM_Free(pText);
			pText = nullptr;
		}
	}
	uint16_t operator++() = delete;
	inline void operator=(GUI_PCSTR s) { GUI.SetText(&pText, s); }
};
