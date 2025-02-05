#pragma once
#include "Button.h"
#include "Menu.h"

enum FRAME_CF_ : uint16_t {
	FRAME_CF_ACTIVE     = 1 << 3,
	FRAME_CF_MOVEABLE   = 1 << 4,
	FRAME_CF_RESIZEABLE = 1 << 5,
	FRAME_CF_TITLEVIS   = 1 << 6,
	FRAME_CF_MINIMIZED  = 1 << 7,
	FRAME_CF_MAXIMIZED  = 1 << 8,
};
using FRAME_CF = uint16_t;

using	  FRAME_BUTTON_CF = WM_CF;
constexpr FRAME_BUTTON_CF 
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
	} static DefaultProps;
private:
	Property Props;
	WM_CB *cb = nullptr;
	Menu *pMenu = nullptr;
	TString Title;
	SRect rRestore;
	WObj *pClient = nullptr;
	WObj *pFocussedChild = nullptr;
	DIALOG_STATE *pDialogStatus = nullptr;
	uint16_t Flags = 0;
	struct Positions {
		uint16_t TitleHeight;
		uint16_t MenuHeight;
		SRect rClient;
		SRect rTitleText;
	};
public:
	Frame(int x0, int y0, int xsize, int ysize,
		  WObj *pParent, uint16_t Id,
		  WM_CF Flags, FRAME_CF ExFlags,
		  const char *pTitle, WM_CB *cb = nullptr);
	Frame(int x0, int y0, int xsize, int ysize,
		  WM_CF Flags, FRAME_CF ExFlags,
		  const char *pTitle, WM_CB *cb = nullptr) :
		Frame(x0, y0, xsize, ysize,
			  nullptr, 0,
			  Flags, ExFlags,
			  pTitle, cb) {}
	Frame(const WM_CREATESTRUCT &wc) : Frame(
		wc.x, wc.y, wc.xsize, wc.ysize,
		wc.pParent, wc.Id,
		wc.Flags, wc.ExFlags,
		wc.pCaption, (WM_CB *)wc.Para.ptr) {}
private:
	uint16_t _CalcTitleHeight() const;
	Positions _CalcPositions() const;

	void _UpdatePositions();

	void _ChangeWindowPosSize(Point);
	bool _ForwardMouseOverMsg(int MsgId, const PID_STATE *pState);
	void _SetCapture(Point Pos, uint8_t Mode);
	uint8_t _CheckReactBorder(Point Pos);

	void _OnTouch(const PID_STATE *pState);
	bool _OnTouchResize(const PID_STATE *pState);
	void _OnPaint() const;
	void _OnChildHasFocus(const FOCUSED_STATE *pInfo);
	bool _HandleResize(int MsgId, const PID_STATE *pState);

	static WM_RESULT _Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);
	static WM_RESULT _cbClient(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);

public:
	Button *AddButton(FRAME_BUTTON_CF Flags, int Off, uint16_t Id);
	Button *AddMinButton(FRAME_BUTTON_CF Flags = FRAME_BUTTON_RIGHT, int Off = 1);
	Button *AddMaxButton(FRAME_BUTTON_CF Flags = FRAME_BUTTON_RIGHT, int Off = 1);
	Button *AddCloseButton(FRAME_BUTTON_CF Flags = FRAME_BUTTON_RIGHT, int Off = 1);

private:
	void _InvalidateButton(uint16_t Id);
	void _RestoreMinimized();
	void _RestoreMaximized();

public: // Action - Restore
	void Restore();
public: // Action - Minimize
	void Minimize();
	inline bool Minimized() const { return Flags & FRAME_CF_MINIMIZED; }
public: // Action - Maximize
	void Maximize();
	inline bool Maximized() const { return Flags & FRAME_CF_MAXIMIZED; }

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
public: // Property - Moveable
	/* R */ inline bool Moveable() const { return Flags & FRAME_CF_MOVEABLE; }
	/* W */ inline void Moveable(bool bMoveable) {
		if (bMoveable)
			Flags |= FRAME_CF_MOVEABLE;
		else
			Flags &= ~FRAME_CF_MOVEABLE;
	}
public: // Property - Resizeable
	/* R */ inline bool Resizeable() const { return Flags & FRAME_CF_RESIZEABLE; }
	/* W */ inline void Resizeable(bool bResizeable) {
		if (bResizeable)
			Flags |= FRAME_CF_RESIZEABLE;
		else
			Flags &= ~FRAME_CF_RESIZEABLE;
	}
public: // Property - Active
	/* R */ inline bool Active() const { return Flags & FRAME_CF_ACTIVE; }
	/* W */ void Active(bool bActive);
public: // Property - TitleVis
	/* R */ inline bool TitleVis() const { return Flags & FRAME_CF_TITLEVIS; }
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
	/* R */ inline const char *Text() const { return Title; }
	/* W */ inline void Text(const char *s) {
		GUI__SetText(&Title, s);
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
