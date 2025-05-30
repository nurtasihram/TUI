#pragma once
#include "WM.h"

enum PROGBAR_CI {
	PROGBAR_CI_ACTIV = 0,
	PROGBAR_CI_INACTIV
};

struct ProgBar : public Widget {

public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aBkColor[2]{
			/* Active */ RGB_DARKBLUE,
			/* Inactive  */ RGBC_GRAY(0x55)
		};
		RGBC aTextColor[2]{
			/* Active */ RGB_WHITE,
			/* Inactive */ RGB_BLACK
		};
		TEXTALIGN Align{ TEXTALIGN_HCENTER };
	} static DefaultProps;

private:
	Property Props = DefaultProps;
	GUI_STRING text;
	Point Off;
	int16_t Min, Max, v;

private:
	int _Value2X(int v) const;
	void _DrawPart(int Index, SRect rText, GUI_PCSTR pText) const;
	GUI_PCSTR _GetText(GUI_PSTR pBuffer) const;
	SRect _GetTextRect(GUI_PCSTR pText) const;

	void _OnPaint() const;

	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	ProgBar(const SRect &rc = {},
			PWObj pParent = nullptr, uint16_t Id = 0,
			WM_CF Flags = WC_HIDE, WC_EX FlagsEx = 0,
			int16_t Min = 0, int16_t Max = 100, int16_t v = 0, GUI_PCSTR s = nullptr);
	ProgBar(const WM_CREATESTRUCT &wc) :
		ProgBar(wc.rect(),
				wc.pParent, wc.Id,
				wc.Flags, wc.FlagsEx,
				wc.Para.i16_4[0], wc.Para.i16_4[1], wc.Para.i16_4[2], wc.pCaption) {}
protected:
	~ProgBar() = default;

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		Invalidate();
	}
public: // Property - BkColor
	/* R */ inline auto BkColor(PROGBAR_CI Index) const { return Props.aBkColor[Index]; }
	/* W */ inline void BkColor(PROGBAR_CI Index, RGBC Color) {
		Props.aBkColor[Index] = Color;
		Invalidate();
	}
public: // Property - TextColor
	/* R */ inline auto TextColor(PROGBAR_CI Index) const { return Props.aTextColor[Index]; }
	/* W */ inline void TextColor(PROGBAR_CI Index, RGBC Color) {
		Props.aTextColor[Index] = Color;
		Invalidate();
	}
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(int Align) {
		Props.Align = Align;
		Invalidate();
	}
public: // Property - Value
	/* R */ inline auto Value() const { return v; }
	/* W */ void Value(int16_t v);
public: // Property - Text
	/* R */ inline GUI_PCSTR Text() const { return text; }
	/* W */ void Text(GUI_PCSTR s);
public: // Property - TextPos
	/* W */ void TextPos(Point Off) {
		this->Off = Off;
		Invalidate();
	}
public: // Property - Range
	/* R */ inline auto Range() const { struct { int16_t Min, Max; } r{ Min, Max }; return r; }
	/* W */ void Range(int Min, int Max);
#pragma endregion

};
