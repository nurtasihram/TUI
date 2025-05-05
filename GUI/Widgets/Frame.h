#pragma once
#include "Button.h"
#include "Menu.h"

using	  FRAME_CF = WC_EX;
constexpr FRAME_CF
		  FRAME_CF_ACTIVE     = WC_EX_USER(3),
		  FRAME_CF_UNMOVEABLE   = WC_EX_USER(4),
		  FRAME_CF_RESIZEABLE = WC_EX_USER(5),
		  FRAME_CF_TITLEVIS   = WC_EX_USER(6),
		  FRAME_CF_MINIMIZED  = WC_EX_USER(7),
		  FRAME_CF_MAXIMIZED  = WC_EX_USER(8);
using	  FRAME_BUTTON = WM_CF;
constexpr FRAME_BUTTON
		  FRAME_BUTTON_LEFT = WC_VISIBLE | WC_ANCHOR_LEFT,
		  FRAME_BUTTON_RIGHT = WC_VISIBLE | WC_ANCHOR_RIGHT;
enum FRAME_CI {
	FRAME_CI_LOSEFOCUS = 0,
	FRAME_CI_FOCUSED
};

class Frame : public Widget {

public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aTextColor[2] {
			/* Lose focused */ RGB_BLACK,
			/* Focused */ RGB_WHITE
		};
		RGBC aBarColor[2] {
			/* Lose focused */  RGBC_GRAY(0x80),
			/* Focused */ RGBC_B(0x80)
		};
		RGBC ClientColor{ RGBC_GRAY(0xE4) };
		RGBC FrameColor{ RGBC_GRAY(0xAA) };
		uint16_t TitleHeight{ 20 };
		uint16_t BorderSize{ 2 };
		uint16_t IBorderSize{ 1 };
		TEXTALIGN Align{ TEXTALIGN_VCENTER };
		uint8_t Border{ 0 };
	} static DefaultProps;

private:
	Property Props = DefaultProps;
	WM_CB cb = nullptr;
	Menu *pMenu = nullptr;
	GUI_STRING Title;
	SRect rRestore;
	PWObj pClient = nullptr;
	PWObj pFocussedChild = nullptr;
	DIALOG_STATE *pDialogStatus = nullptr;

private:
	struct Positions {
		uint16_t TitleHeight;
		uint16_t MenuHeight;
		SRect rClient;
		SRect rTitleText;
	};
	Positions _CalcPositions() const;
	uint16_t _CalcTitleHeight() const;

	void _UpdatePositions();

	void _ChangeWindowPosSize(Point);
	bool _ForwardMouseOverMsg(int MsgId, MOUSE_STATE State);
	void _SetCapture(Point Pos, uint8_t Mode);
	uint8_t _CheckReactBorder(Point Pos);

	void _InvalidateButton(uint16_t Id);
	void _RestoreMinimized();
	void _RestoreMaximized();

	void _OnMouse(const MOUSE_STATE *pState);
	bool _OnTouchResize(const MOUSE_STATE *pState);
	void _OnPaint() const;
	void _OnChildHasFocus(const FOCUS_CHANGED_STATE *pInfo);
	bool _HandleResize(int MsgId, const MOUSE_STATE *pState);

	static WM_RESULT _cbClient(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);
	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	Frame(const SRect &rc = {},
		  PWObj pParent = nullptr, uint16_t Id = 0,
		  WM_CF Flags = WC_HIDE, FRAME_CF FlagsEx = 0,
		  GUI_PCSTR pTitle = nullptr, WM_CB cb = nullptr);
	Frame(const WM_CREATESTRUCT &wc) : Frame(
		wc.rect(),
		wc.pParent, wc.Id,
		wc.Flags, wc.FlagsEx,
		wc.pCaption, (WM_CB)wc.Para.ptr) {}

public:
	Button *AddButton(FRAME_BUTTON Flags, int Off, uint16_t Id);
	Button *AddMinButton(FRAME_BUTTON Flags = FRAME_BUTTON_RIGHT, int Off = 1);
	Button *AddMaxButton(FRAME_BUTTON Flags = FRAME_BUTTON_RIGHT, int Off = 1);
	Button *AddCloseButton(FRAME_BUTTON Flags = FRAME_BUTTON_RIGHT, int Off = 1);

public: // Action - Restore
	void Restore();
public: // Action - Minimize
	void Minimize();
	inline bool Minimized() const { return StatusEx & FRAME_CF_MINIMIZED; }
public: // Action - Maximize
	void Maximize();
	inline bool Maximized() const { return StatusEx & FRAME_CF_MAXIMIZED; }

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		_UpdatePositions();
		Invalidate();
	}
public: // Property - BarColor
	/* R */ inline RGBC BarColor(FRAME_CI Index) const { return Props.aBarColor[Index]; }
	/* W */ inline void BarColor(FRAME_CI Index, RGBC Color) {\
		Props.aBarColor[Index] = Color;
		Invalidate();
	}
public: // Property - TextColor
	/* R */ inline RGBC TextColor(FRAME_CI Index) const { return Props.aTextColor[Index]; }
	/* W */ inline void TextColor(FRAME_CI Index, RGBC Color) {
		Props.aTextColor[Index] = Color;
		Invalidate();
	}
public: // Property - ClientColor
	/* R */ inline RGBC ClientColor() const { return Props.ClientColor; }
	/* W */ inline void ClientColor(RGBC Color) {
		Props.ClientColor = Color;
		pClient->Invalidate();
	}
public: // Property - Unmoveable
	/* R */ inline bool Unmoveable() const { return StatusEx & FRAME_CF_UNMOVEABLE; }
	/* W */ inline void Unmoveable(bool bUnmoveable) {
		if (bUnmoveable)
			StatusEx |= FRAME_CF_UNMOVEABLE;
		else
			StatusEx &= ~FRAME_CF_UNMOVEABLE;
	}
public: // Property - Resizeable
	/* R */ inline bool Resizeable() const { return StatusEx & FRAME_CF_RESIZEABLE; }
	/* W */ inline void Resizeable(bool bResizeable) {
		if (bResizeable)
			StatusEx |= FRAME_CF_RESIZEABLE;
		else
			StatusEx &= ~FRAME_CF_RESIZEABLE;
	}
public: // Property - Active
	/* R */ inline bool Active() const { return StatusEx & FRAME_CF_ACTIVE; }
	/* W */ void Active(bool bActive);
public: // Property - TitleVis
	/* R */ inline bool TitleVis() const { return StatusEx & FRAME_CF_TITLEVIS; }
	/* W */ void TitleVis(bool bTitleVis);
public: // Property - TitleHeight
	/* R */ inline auto TitleHeight() const {
		return Props.TitleHeight ?
			Props.TitleHeight :
			_CalcPositions().TitleHeight;
	}
	/* W */ void TitleHeight(int Height);
public: // Property - BorderSize
	/* R */ inline auto BorderSize() const { return Props.BorderSize; }
	/* W */ void BorderSize(int Size);
public: // Property - Text
	/* R */ inline GUI_PCSTR Text() const { return Title; }
	/* W */ inline void Text(GUI_PCSTR s) {
		GUI.SetText(&Title, s);
		Invalidate();
	}
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(TEXTALIGN Align) {
		Props.Align = Align;
		Invalidate();
	}
public: // Property - Menu
	/* R */ inline auto Menu() { return pMenu; }
	/* R */ inline auto Menu() const { return pMenu; }
	/* W */ void Menu(::Menu *pMenu);
#pragma endregion

};
