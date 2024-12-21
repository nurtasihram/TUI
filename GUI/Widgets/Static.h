#pragma once
#include "WM.h"

#define TEXT_CF_LEFT    TEXTALIGN_LEFT
#define TEXT_CF_RIGHT   TEXTALIGN_RIGHT
#define TEXT_CF_HCENTER TEXTALIGN_HCENTER
#define TEXT_CF_VCENTER TEXTALIGN_VCENTER
#define TEXT_CF_TOP     TEXTALIGN_TOP
#define TEXT_CF_BOTTOM  TEXTALIGN_BOTTOM

struct Static : public Widget {
public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		TEXTALIGN Align = TEXTALIGN_LEFT;
		RGBC textColor = RGB_BLACK;
		RGBC bkColor = RGB_INVALID_COLOR;
	} static DefaultProps;
private:
	Property Props;
	TString text;
private:
	void _OnPaint() const;

	static void _Callback(WObj *pWin, int msgid, WM_PARAM *pData, WObj *pWinSrc);
public:
	Static(int x0, int y0, int xsize, int ysize,
		   WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
		   const char *pText);
	Static(const WM_CREATESTRUCT &wc) : Static(
		wc.x, wc.y, wc.xsize, wc.ysize,
		wc.pParent, wc.Id, wc.Flags, wc.ExFlags,
		wc.pCaption) {}

#pragma region Properties
public: // Property - Font
	/* W */ inline void Font(CFont *pFont) {
		if (Props.pFont != pFont && pFont) {
			Props.pFont = pFont;
			Invalidate();
		}
	}
	/* R */ inline auto Font() const { return Props.pFont; }
public: // Property - BkColor
	/* W */ inline void BkColor(RGBC Color) {
		if (Props.bkColor != Color) {
			Props.bkColor = Color;
			Invalidate();
		}
	}
	/* R */ inline auto BkColor() const { return Props.bkColor; }
public: // Property - TextColor
	/* W */ inline void TextColor(RGBC Color) {
		if (Props.textColor != Color) {
			Props.textColor = Color;
			Invalidate();
		}
	}
	/* R */ inline auto TextColor() const { return Props.textColor; }
public: // Property - TextAlign
	/* W */ inline void TextAling(TEXTALIGN Align) {
		if (Props.Align != Align) {
			Props.Align = Align;
			Invalidate();
		}
	}
	/* R */ inline auto TextAlign() const { return Props.Align; }
public: // Property - Text
	/* W */ inline void Text(const char *pText) {
		text = pText;
		Invalidate();
	}
	/* R */ inline const char *Text() const { return text; }
#pragma endregion

};
