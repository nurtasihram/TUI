#pragma once
#include "WM.h"

#define BUTTON_STATE_HASFOCUS 0
#define BUTTON_STATE_PRESSED  WC_USER(0)

enum BUTTON_BI {
	 BUTTON_BI_UNPRESSED = 0,
	 BUTTON_BI_PRESSED,
	 BUTTON_BI_DISABLED
};
enum BUTTON_CI {
	 BUTTON_CI_UNPRESSED = 0,
	 BUTTON_CI_PRESSED,
	 BUTTON_CI_DISABLED
};

class Button : public Widget {
public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aTextColor[3]{
			/* Unpressed */ RGB_BLACK,
			/* Pressed */ RGB_BLACK,
			/* Disabled */ RGB_DARKGRAY
		};
		RGBC aBkColor[3]{
			/* Unpressed */ RGBC_GRAY(0xD0),
			/* Pressed */ RGB_WHITE,
			/* Disabled */ RGB_LIGHTGRAY
		};
		TEXTALIGN Align{ TEXTALIGN_HCENTER | TEXTALIGN_VCENTER };
	};
	static Property DefaultProps;
private:
	Property Props;
	TString text;
	GUI_DRAW_BASE *apDrawObj[3] = { 0 };

private:
	void _Pressed();
	void _Released(int Notification);
private:
	void _OnPaint() const;
	void _OnTouch(const PID_STATE *);
	void _OnPidStateChange(const PID_CHANGED_STATE *pState);
	bool _OnKey(const KEY_STATE *);

	static WM_RESULT _Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);

public:
	Button(int x0, int y0, int xsize, int ysize,
		   WObj *pParent, uint16_t Id = 0,
		   WM_CF Flags = WC_HIDE,
		   const char *pText = nullptr);
	Button(const WM_CREATESTRUCT &wc) :
		Button(wc.x, wc.y, wc.xsize, wc.ysize,
			   wc.pParent, wc.Id,
			   wc.Flags, wc.pCaption) {}
protected:
	~Button();

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		if (Props.pFont != pFont) {
			Props.pFont = pFont;
			Invalidate();
		}
	}
public: // Property - BkColor
	/* R */ inline auto BkColor(BUTTON_CI Index) { return Props.aBkColor[Index]; }
public: // Property - TextColor
	/* R */ inline auto TextColor(BUTTON_CI Index) { return Props.aTextColor[Index]; }
public: // Property - SelfDraw
	/* W */ inline void SelfDraw(BUTTON_BI Index, GUI_DRAW_BASE *pDrawObj) {
		GUI_MEM_Free(apDrawObj[Index]);
		apDrawObj[Index] = pDrawObj;
		Invalidate();
	}
	/* W */ inline void SelfDraw(BUTTON_BI Index, GUI_DRAW_CB *pDrawCb) { SelfDraw(Index, GUI_DRAW_SELF::Create(pDrawCb)); }
	/* W */ inline void SelfDraw(BUTTON_BI Index, CBitmap *pBitmap) { SelfDraw(Index, GUI_DRAW_BMP::Create(pBitmap)); }
	/* R */ inline auto SelfDraw(BUTTON_BI Index) const { return apDrawObj[Index]; }
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(TEXTALIGN Align) {
		if (Props.Align != Align) {
			Props.Align = Align;
			Invalidate();
		}
	}
public: // Property - Pressed
	/* R */ inline bool Pressed() const { return StatusEx & BUTTON_STATE_PRESSED; }
	/* W */ void Pressed(bool bPressed);
public: // Property - Focussable
	/* R */ inline bool Focussable() const { return StatusEx & WC_FOCUSSABLE; }
	/* W */ void Focussable(bool bFocussable);
public: // Property - Text
	/* R */ inline auto Text() const { return text; }
	/* W */ inline void Text(const char *pText) {
		GUI__SetText(&text, pText);
		Invalidate();
	}
#pragma endregion

};
