#pragma once

#include "GUI_Types.h"

#include "GUI_X_LCD.h"
#include "GUI_X_MEM.h"

#include "Resources.inl"

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

void GUI__strcpy(char *s, const char *c);
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len);

/// @brief 繪圖面板
class GUI_PANEL {

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
	/// @brief 繪圖偏移量
	Point off;
	/// @brief 字串顯示開始位置
	Point dispPos;

public:
	/// @brief 光標控制器
	CursorCtl Cursor;
	/// @brief 光標數據結構
	PID_STATE PID_STATE;

private:
	KEY_STATE _KeyLast, _KeyNow;

public:
	void Init();

public:
	void StoreKeyMsg(uint16_t Key, int Pressed);
	void SendKeyMsg(uint16_t Key, int Pressed);
	bool PollKeyMsg();

public:
	inline uint16_t Key() {
		uint16_t r = _KeyNow.Key;
		_KeyNow.Key = 0;
		return r;
	}
	inline void Key(uint16_t Key) {
		if (!_KeyNow.Key)
			_KeyNow.Key = Key;
	}

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
	void DrawString(const char *s);

	/// @brief 繪製字串
	/// @param s 字串緩衝區
	/// @param r 繪製區域
	/// @return 字串所佔區域
	SRect DrawStringIn(const char *s, SRect r);

	/// @brief 繪製字串
	/// @param s 字串緩衝區
	/// @param Pos 繪製開始位置
	inline void DrawStringAt(const char *s, Point Pos) {
		DispPos(Pos);
		DrawString(s);
	}
#pragma endregion

	/// @brief 反轉筆刷前景色背景色
	inline void RevColor() {
		auto tmp = Props.aColor[0];
		Props.aColor[0] = Props.aColor[1];
		Props.aColor[1] = tmp;
	}

	int XDist(const char *pString, int NumChars) { return Props.pFont->XDist(pString, NumChars); }
	inline int XDist(const char *pString) { return XDist(pString, NumChars(pString)); }

public:
	uint16_t NumTexts(const char *pText);
	const char *NextText(const char *pTexts);

	uint16_t NumLines(const char *pText);
	const char *NextLines(const char *pText);
	
	uint16_t NumCharsLine(const char *pText);
	uint16_t NumChars(const char *pText);
	uint16_t BytesChars(const char *pText);

	uint16_t NextChar(const char *&pText);
	const char *NextChars(const char *pText, uint16_t NumChars);
	uint16_t Char(const char *pText) { return NextChar(pText); }

	void SetText(char **ppText, const char *pText);

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) { Props.pFont = pFont; }
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
	/* W */ inline void ClipRect(const SRect &r) { rClip = LCD_Rect() & r; }
	/* A */ inline void ClipRect() { rClip = LCD_Rect(); }
#pragma endregion

};
extern GUI_PANEL GUI;

struct GUI_ISTRING {
	mutable char *pText = nullptr;
public:
	GUI_ISTRING() {}
	GUI_ISTRING(char *pText) : pText(pText) {}
public:
	inline auto Chars() const { return GUI.NumChars(pText); }
	inline auto Bytes() const { return GUI.BytesChars(pText); }
public:
	inline GUI_ISTRING operator()(uint16_t num) {}
	inline GUI_ISTRING operator[](uint16_t bytes) { return pText + bytes; }
	inline char **operator&() { return &pText; }
	inline uint16_t operator*() const { return GUI.Char(pText); }
	inline uint16_t operator++() const { return GUI.NextChar((const char *&)pText); }
	inline operator char *() { return pText; }
	inline operator const char *() const { return pText; }
};
struct GUI_STRING : GUI_ISTRING {
	GUI_STRING() {}
	GUI_STRING(const GUI_STRING &s) { GUI.SetText(&pText, s); }
	GUI_STRING(const char *s) { GUI.SetText(&pText, s); }
	~GUI_STRING() {
		if (pText) {
			GUI_MEM_Free(pText);
			pText = nullptr;
		}
	}
	uint16_t operator++() = delete;
	inline void operator=(const char *s) { GUI.SetText(&pText, s); }
};
