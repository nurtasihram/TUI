#pragma once
#include "WM.h"

using		_BUTTON_STATE = WC_EX;
using		 BUTTON_CF = _BUTTON_STATE;
constexpr	_BUTTON_STATE
			_BUTTON_STATE_HASFOCUS  = 0,
			_BUTTON_STATE_PRESSED   = WC_EX_USER(0),
			_BUTTON_STATE_SWITCHED  = WC_EX_USER(1),
			BUTTON_CF_SWITCH		= WC_EX_USER(2);
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
		TEXTALIGN Align{ TEXTALIGN_CENTER };
	} static DefaultProps;

private:
	Property Props = DefaultProps;
	GUI_STRING text;
	GUI_DRAW aDrawObj[3];

private:
	void _Pressed();
	void _Released(int Notification);

	void _OnPaint() const;
	void _OnMouse(MOUSE_STATE);
	void _OnMouseChange(MOUSE_CHANGED_STATE);
	bool _OnKey(KEY_STATE);

	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	Button(const SRect &rc = {},
		   PWObj pParent = nullptr, uint16_t Id = 0,
		   WM_CF Flags = WC_HIDE, BUTTON_CF FlagsEx = 0,
		   GUI_PCSTR pText = nullptr);
	Button(const WM_CREATESTRUCT &wc) : Button(
		wc.rect(),
		wc.pParent, wc.Id,
		wc.Flags, wc.FlagsEx,
		wc.pCaption) {}
protected:
	~Button() = default;

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		Invalidate();
	}
public: // Property - BkColor
	/* R */ inline auto BkColor(BUTTON_CI Index) { return Props.aBkColor[Index]; }
public: // Property - TextColor
	/* R */ inline auto TextColor(BUTTON_CI Index) { return Props.aTextColor[Index]; }
public: // Property - SelfDraw
	/* W */ inline void SelfDraw(BUTTON_BI Index, GUI_DRAW_CB *pDrawCb) { SelfDraw(Index, (GUI_DRAW)pDrawCb); }
	/* W */ inline void SelfDraw(BUTTON_BI Index, const GUI_DRAW &DrawObj) {
		aDrawObj[Index] = DrawObj;
		Invalidate();
	}
//	/* R */ inline auto SelfDraw(BUTTON_BI Index) const { return DrawObj[Index]; }
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(TEXTALIGN Align) {
		Props.Align = Align;
		Invalidate();
	}
public: // Property - Pressed
	/* R */ inline bool Pressed() const { return StatusEx & _BUTTON_STATE_PRESSED; }
	/* W */ void Pressed(bool bPressed);
public: // Property - Text
	/* R */ inline GUI_PCSTR Text() const { return text; }
	/* W */ inline void Text(GUI_PCSTR pText) {
		text = pText;
		Invalidate();
	}
#pragma endregion

};
