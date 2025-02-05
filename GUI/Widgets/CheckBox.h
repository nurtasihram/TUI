#pragma once
#include "WM.h"

enum CHECKBOX_BI {
	 CHECKBOX_BI_INACTIV = 0,
	 CHECKBOX_BI_ACTIV,
	 CHECKBOX_BI_INACTIV_3STATE,
	 CHECKBOX_BI_ACTIV_3STATE
};
enum CHECKBOX_CI {
	 CHECKBOX_CI_INACTIV = 0,
	 CHECKBOX_CI_ACTIV
};

class CheckBox : public Widget {
public:
	static CBitmap abmCheck[4];
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		CBitmap *apBm[4]{
			/* Inactive */ &abmCheck[0],
			/* Active */ &abmCheck[1],
			/* Inactive 3-State */ &abmCheck[2],
			/* Active 3-State */ &abmCheck[3]
		};
		RGBC aBkColorBox[2]{
			/* Inactive */ RGBC_GRAY(0x80),
			/* Active */ RGB_WHITE
		};
		RGBC BkColor{ RGB_INVALID_COLOR };
		RGBC TextColor{ RGB_BLACK };
		TEXTALIGN Align{ TEXTALIGN_LEFT | TEXTALIGN_VCENTER };
		uint8_t Spacing{ 4 };
	} static DefaultProps;
private:
	Property Props = DefaultProps;
	TString text;
	uint8_t nStates = 2, CurrentState = 0;

private:
	void _OnPaint();
	void _OnTouch(const PID_STATE *pState);
	bool _OnKey(const KEY_STATE *pKeyInfo);

	static WM_RESULT _Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);

public:
	CheckBox(int x0, int y0, int xsize, int ysize,
			 WObj *pParent, uint16_t Id,
			 WM_CF Flags,
			 const char *pText);
	CheckBox(const WM_CREATESTRUCT &wc) :
		CheckBox(wc.x, wc.y, wc.xsize, wc.ysize,
				 wc.pParent, wc.Id, wc.Flags,
				 wc.pCaption) {}
protected:
	~CheckBox() = default;

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		Invalidate();
	}
public: // Property - TextColor
	/* R */ inline auto TextColor() const { return Props.TextColor; }
	/* W */ inline void TextColor(RGBC Color) {
		Props.TextColor = Color;
		Invalidate();
	}
public: // Property - BkColor
	/* R */ inline auto BkColor() const { return Props.BkColor; }
	/* W */ inline void BkColor(RGBC Color) {
		Props.BkColor = Color;
		Invalidate();
	}
public: // Property - BkColorBox
	/* R */ inline auto BkColorBox(CHECKBOX_CI Index) const { return Props.aBkColorBox[Index]; }
	/* W */ inline void BkColorBox(CHECKBOX_CI Index, RGBC Color) {
		Props.aBkColorBox[Index] = Color;
		Invalidate();
	}
public: // Property - Spacing
	/* R */ inline auto Spacing() const { return Props.Spacing; }
	/* W */ inline void Spacing(unsigned Spacing) {
		Props.Spacing = Spacing;
		Invalidate();
	}
public: // Property - Text
	/* R */ inline const char *Text() const { return text; }
	/* W */ inline void Text(const char *s) {
		text = s;
		Invalidate();
	}
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(TEXTALIGN Align) {
		Props.Align = Align;
		Invalidate();
	}
public: // Property - Image
	/* R */ inline auto Image(CHECKBOX_BI Index) const { return Props.apBm[Index]; }
	/* W */ inline void Image(CHECKBOX_BI Index, CBitmap *pBitmap) {
		if (!pBitmap) pBitmap = DefaultProps.apBm[Index];
		Props.apBm[Index] = pBitmap;
		Invalidate();
	}
public: // Property - CheckState
	/* R */ inline uint8_t CheckState() const { return CurrentState; }
	/* W */ inline void CheckState(uint8_t State) {
		if (nStates >= State) {
			CurrentState = State;
			Invalidate();
		}
	}
public: // Property - NumStates
	/* R */ inline auto NumStates() const { return nStates; }
	/* W */ inline void NumStates(uint8_t nStates) {
		if (nStates == 2 || nStates == 3) {
			Props.apBm[2] = DefaultProps.apBm[2];
			Props.apBm[3] = DefaultProps.apBm[3];
			this->nStates = nStates;
		}
	}
#pragma endregion

};
