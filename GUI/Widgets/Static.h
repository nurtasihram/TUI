#pragma once
#include "WM.h"

using		STATIC_CF = uint16_t;
constexpr	STATIC_CF
			STATIC_CF_LEFT    = TEXTALIGN_LEFT,
			STATIC_CF_HCENTER = TEXTALIGN_HCENTER,
			STATIC_CF_RIGHT   = TEXTALIGN_RIGHT,
			STATIC_CF_TOP     = TEXTALIGN_TOP,
			STATIC_CF_VCENTER = TEXTALIGN_VCENTER,
			STATIC_CF_BOTTOM  = TEXTALIGN_BOTTOM,
			STATIC_CF_CENTER  = TEXTALIGN_CENTER;

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
	GUI_STRING text;

private:
	void _OnPaint() const;

	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	Static(const SRect &rc = {},
		   PWObj pParent = nullptr, uint16_t Id = 0,
		   WM_CF Flags = WC_HIDE, STATIC_CF FlagsEx = 0,
		   const char *pText = nullptr);
	Static(const WM_CREATESTRUCT &wc) :
		Static(wc.rect(),
			   wc.pParent, wc.Id,
			   wc.Flags, wc.FlagsEx,
			   wc.pCaption) {}
protected:
	~Static() = default;

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		Invalidate();
	}
public: // Property - BkColor
	/* R */ inline auto BkColor() const { return Props.BkColor; }
	/* W */ inline void BkColor(RGBC Color) {
		Props.BkColor = Color;
		Invalidate();
	}
public: // Property - TextColor
	/* R */ inline auto TextColor() const { return Props.Color; }
	/* W */ inline void TextColor(RGBC Color) {
		Props.Color = Color;
		Invalidate();
	}
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(TEXTALIGN Align) {
		Props.Align = Align;
		Invalidate();
	}
public: // Property - Text
	/* R */ inline const char *Text() const { return text; }
	/* W */ inline void Text(const char *pText) {
		text = pText;
		Invalidate();
	}
#pragma endregion

};
