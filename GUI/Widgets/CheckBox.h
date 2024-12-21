#pragma once
#include "WM.h"

#define CHECKBOX_BI_INACTIV        0
#define CHECKBOX_BI_ACTIV          1
#define CHECKBOX_BI_INACTIV_3STATE 2
#define CHECKBOX_BI_ACTIV_3STATE   3

class CheckBox : public Widget {
public:
	static CBitmap abmCheck[2];
	static CBitmap abmCheck3[2];
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		CBitmap *apBm[4] = {
			&abmCheck[0],
			&abmCheck[1],
			&abmCheck3[0],
			&abmCheck3[1]
		};
		RGBC aBkColorBox[2] = {
			/* Inactive color */ RGBC_GRAY(0x80),
			/* Active color */ RGB_WHITE
		};
		RGBC BkColor = RGB_INVALID_COLOR;
		RGBC TextColor = RGB_BLACK;
		int16_t Align = TEXTALIGN_LEFT | TEXTALIGN_VCENTER;
		uint8_t Spacing = 4;
	} static DefaultProps;
private:
	Property Props = DefaultProps;
	uint8_t nStates = 2, CurrentState = 0;
	TString text;
private:
	void _OnPaint();
	void _OnTouch(WM_PARAM *pData);
	bool _OnKey(WM_PARAM *pData);

	static void _Callback(WObj *pWin, int msgid, WM_PARAM *pData, WObj *pWinSrc);

public:
	CheckBox(int x0, int y0, int xsize, int ysize,
			 WObj *pParent, uint16_t Id = 0, uint16_t Flags = 0,
			 const char *pText = nullptr);
	CheckBox(const WM_CREATESTRUCT &wc) : CheckBox(
		wc.x, wc.y, wc.xsize, wc.ysize,
		wc.pParent, wc.Id, wc.Flags,
		wc.pCaption) {}
protected:
	~CheckBox() = default;

#pragma region Properties
public: // Property - Font
	/* W */ inline void Font(CFont *pFont) {
		if (Props.pFont != pFont) {
			Props.pFont = pFont;
			Invalidate();
		}
	}
public: // Property - TextColor
	/* W */ inline void TextColor(RGBC Color) {
		if (Props.TextColor != Color) {
			Props.TextColor = Color;
			Invalidate();
		}
	}
public: // Property - BkColor
	/* W */ inline void BkColor(RGBC Color) {
		if (Props.BkColor != Color) {
			Props.BkColor = Color;
			Invalidate();
		}
	}
	/* R */ inline auto BkColor() const { return Props.BkColor; }
public: // Property - Spacing
	/* W */ inline void Spacing(unsigned Spacing) {
		if (Props.Spacing == Spacing)
			return;
		Props.Spacing = Spacing;
		Invalidate();
	}
public: // Property - Text
	/* W */ void Text(const char *s) {
		if (GUI__SetText(&text, s))
			Invalidate();
	}
public: // Property - TextAlign
	/* W */ inline void TextAlign(int Align) {
		if (Props.Align != Align) {
			Props.Align = Align;
			Invalidate();
		}
	}
public: // Property - CheckState
	/* W */ inline void CheckState(uint8_t State) {
		if (nStates >= State) {
			CurrentState = State;
			Invalidate();
		}
	}
	/* R */ inline uint8_t CheckState() const { return CurrentState; }
public: // Property - NumStates
	/* W */ inline void NumStates(uint8_t nStates) {
		if (nStates == 2 || nStates == 3) {
			Props.apBm[2] = DefaultProps.apBm[2];
			Props.apBm[3] = DefaultProps.apBm[3];
			this->nStates = nStates;
		}
	}
	/* R */ inline auto NumStates() const { return nStates; }
#pragma endregion

};
