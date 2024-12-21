#pragma once
#include "WM.h"

struct ProgBar : public Widget {
public:
	struct Property {
		CFont *pFont { &GUI_Font13_1 };
		RGBC aBarColor[2]{
			RGBC_GRAY(0x55),
			RGB_DARKBLUE
		};
		RGBC aTextColor[2]{
			RGB_WHITE,
			RGB_BLACK
		};
		TEXTALIGN Align = TEXTALIGN_HCENTER;
	} static DefaultProps;
private:
	TString text;
	Point Off;
	int16_t Min = 0, Max = 100, v = 0;
	Property Props;
public:
	ProgBar(int x0, int y0, int xsize, int ysize,
			WObj *pParent, uint16_t Id = 0, uint16_t Flags = 0, uint16_t ExFlags = 0,
			int16_t Min = 0, int16_t Max = 0, int16_t v = 0, const char *s = nullptr);
protected:
	~ProgBar() = default;

private:
	int _Value2X(int v) const;
	void _DrawPart(int Index, Point ptText, const char *pText) const;
	const char *_GetText(char *pBuffer) const;
	SRect _GetTextRect(const char *pText) const;

	void _OnPaint() const;
	static void _Callback(WObj *pWin, int msgid, WM_PARAM *pData, WObj *pWinSrc);

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		if (Props.pFont != pFont && pFont) {
			Props.pFont = pFont;
			Invalidate();
		}
	}
public: // Property - BarColor
	/* R */ inline auto BarColor(uint8_t Index) const { return Props.aBarColor[Index]; }
	/* W */ inline void BarColor(uint8_t Index, RGBC Color) {
		if (Props.aBarColor[Index] != Color) {
			Props.aBarColor[Index] = Color;
			Invalidate();
		}
	}
public: // Property - TextColor
	/* R */ inline auto TextColor(uint8_t Index) const { return Props.aTextColor[Index]; }
	/* W */ inline void TextColor(uint8_t Index, RGBC Color) {
		if (Props.aTextColor[Index] != Color) {
			Props.aTextColor[Index] = Color;
			Invalidate();
		}
	}
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(int Align) {
		if (Props.Align != Align) {
			Props.Align = Align;
			Invalidate();
		}
	}
public: // Property - Value
	/* R */ inline auto Value() const { return v; }
	/* W */ void Value(int16_t v);
public: // Property - Text
	/* R */ inline const char *Text() const { return text; }
	/* W */ void Text(const char *s);
public: // Property - TextPos
	/* W */ void TextPos(Point Off) {
		if (this->Off != Off) {
			this->Off = Off;
			Invalidate();
		}
	}
public: // Property - Range
	/* R */ inline auto Range() const { struct { int16_t Min, Max; } r{ Min, Max }; return r; }
	/* W */ void Range(int Min, int Max);
#pragma endregion

};
