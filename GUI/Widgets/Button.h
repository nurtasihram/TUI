#pragma once
#include "WM.h"

#define BUTTON_CI_UNPRESSED 0
#define BUTTON_CI_PRESSED   1
#define BUTTON_CI_DISABLED  2

#define BUTTON_BI_UNPRESSED 0
#define BUTTON_BI_PRESSED   1
#define BUTTON_BI_DISABLED  2

#define BUTTON_STATE_HASFOCUS 0
#define BUTTON_STATE_FOCUS    WIDGET_STATE_FOCUS
#define BUTTON_STATE_PRESSED  WIDGET_STATE_USER0

class Button : public Widget {
public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aTextColor[3] = {
			RGB_BLACK,
			RGB_BLACK,
			RGB_DARKGRAY
		};
		RGBC aBkColor[3] = {
			RGBC_GRAY(0xD0),
			RGB_WHITE,
			RGB_LIGHTGRAY
		};
		int16_t Align = TEXTALIGN_HCENTER | TEXTALIGN_VCENTER;
	};
	static Property DefaultProps;
private:
	Property Props;
	TString text;
	GUI_DRAW_BASE *apDrawObj[3] = { 0 };

#pragma region Callbacks
private:
	void _Pressed();
	void _Released(int Notification);
private:
	void _OnPaint() const;
	void _OnTouch(const PID_STATE *);
	void _OnPidStateChange(const PID_CHANGED_STATE *pState);
	bool _OnKey(const KEY_STATE *);

	static void _Callback(WObj *pWin, int msgid, WM_PARAM *pData, WObj *pWinSrc);
#pragma endregion

public:
	Button(int x0, int y0, int xsize, int ysize,
		   WObj *pParent, uint16_t Id = 0, uint16_t Flags = 0,
		   const char *pText = nullptr);
	Button(const WM_CREATESTRUCT &wc) : Button(wc.x, wc.y, wc.xsize, wc.ysize, wc.pParent, wc.Id, wc.Flags, wc.pCaption) {}
protected:
	~Button();

#pragma region Properties
public: // Property - BkColor
	/* R */ inline RGBC BkColor(uint8_t Index) { return Props.aBkColor[Index]; }
public: // Property - TextColor
	/* R */ inline RGBC TextColor(uint8_t Index) { return Props.aTextColor[Index]; }
public: // Property - TextAlign
	/* W */ inline void TextAlign(uint16_t Align) {
		if (Props.Align != Align) {
			Props.Align = Align;
			Invalidate();
		}
	}
	/* R */ inline auto TextAlign() const { return Props.Align; }
public: // Property - Font
	/* W */ inline void Font(CFont *pFont) {
		if (Props.pFont != pFont) {
			Props.pFont = pFont;
			Invalidate();
		}
	}
	/* R */ inline auto Font() const { return Props.pFont; }
public: // Property - Pressed
	/* W */ void Pressed(bool bPressed);
	/* R */ inline bool Pressed() const { return State & BUTTON_STATE_PRESSED; }
public: // Property - Focussable
	/* W */ void Focussable(bool bFocussable);
	/* R */ inline bool Focussable() const { return State & WIDGET_STATE_FOCUSSABLE; }
public: // Property - Text
	/* W */ inline void Text(const char *pText) {
		if (GUI__SetText(&this->text, pText))
			Invalidate();
	}
	/* R */ inline auto Text() const { return text; }
public: // Property - SelfDraw
	/* W */ inline void SelfDraw(uint8_t Index, GUI_DRAW_BASE *pDrawObj) {
		GUI_MEM_Free(apDrawObj[Index]);
		apDrawObj[Index] = pDrawObj;
		Invalidate();
	}
	/* W */ inline void SelfDraw(uint8_t Index, GUI_DRAW_CB *pDrawCb) { SelfDraw(Index, GUI_DRAW_SELF::Create(pDrawCb)); }
	/* W */ inline void SelfDraw(uint8_t Index, CBitmap *pBitmap) { SelfDraw(Index, GUI_DRAW_BMP::Create(pBitmap)); }
	/* R */ inline auto SelfDraw(uint8_t Index) const { return apDrawObj[Index]; }
#pragma endregion

};
