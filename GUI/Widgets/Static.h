#pragma once
#include "WM.h"

#define TEXT_CF_LEFT    TEXTALIGN_LEFT
#define TEXT_CF_RIGHT   TEXTALIGN_RIGHT
#define TEXT_CF_HCENTER TEXTALIGN_HCENTER
#define TEXT_CF_VCENTER TEXTALIGN_VCENTER
#define TEXT_CF_TOP     TEXTALIGN_TOP
#define TEXT_CF_BOTTOM  TEXTALIGN_BOTTOM

class Static : public Widget {
public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC Color{ RGB_BLACK };
		RGBC BkColor{ RGB_INVALID_COLOR };
		TEXTALIGN Align{ TEXTALIGN_LEFT };
	} static DefaultProps;
private:
	Property Props;
	TString text;
private:
	void _OnPaint() const;

	static WM_RESULT _Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);
public:
	Static(int x0, int y0, int xsize, int ysize,
		   WObj *pParent, uint16_t Id,
		   WM_CF Flags, uint16_t ExFlags,
		   const char *pText);
	Static(const WM_CREATESTRUCT &wc) :
		Static(wc.x, wc.y, wc.xsize, wc.ysize,
			   wc.pParent, wc.Id,
			   wc.Flags, wc.ExFlags,
			   wc.pCaption) {}

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		if (Props.pFont != pFont && pFont) {
			Props.pFont = pFont;
			Invalidate();
		}
	}
public: // Property - BkColor
	/* R */ inline auto BkColor() const { return Props.BkColor; }
	/* W */ inline void BkColor(RGBC Color) {
		if (Props.BkColor != Color) {
			Props.BkColor = Color;
			Invalidate();
		}
	}
public: // Property - TextColor
	/* R */ inline auto TextColor() const { return Props.Color; }
	/* W */ inline void TextColor(RGBC Color) {
		if (Props.Color != Color) {
			Props.Color = Color;
			Invalidate();
		}
	}
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(TEXTALIGN Align) {
		if (Props.Align != Align) {
			Props.Align = Align;
			Invalidate();
		}
	}
public: // Property - Text
	/* R */ inline const char *Text() const { return text; }
	/* W */ inline void Text(const char *pText) {
		text = pText;
		Invalidate();
	}
#pragma endregion

};
