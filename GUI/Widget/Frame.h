#pragma once
#include "Button.h"
#include "Menu.h"

#define FRAME_CF_ACTIVE     (1<<3)
#define FRAME_CF_MOVEABLE   (1<<4)
#define FRAME_CF_RESIZEABLE (1<<5)
#define FRAME_CF_TITLEVIS   (1<<6)
#define FRAME_CF_MINIMIZED  (1<<7)
#define FRAME_CF_MAXIMIZED  (1<<8)
#define FRAME_BUTTON_RIGHT   (1<<0)
#define FRAME_BUTTON_LEFT    (1<<1)

class Frame : public Widget {
public:
	struct Property {
		CFont *pFont{ &GUI_Font8_1 };
		RGBC aTextColor[2] {
			/* Lose focused */ RGB_BLACK,
			/* Focused */ RGB_WHITE
		};
		RGBC aBarColor[2] {
			/* Lose focused */  RGBC_GRAY(0x80),
			/* Focused */ RGBC_B(0x80)
		};
		RGBC ClientColor{ RGBC_GRAY(0xD4) };
		RGBC FrameColor{ RGBC_GRAY(0xAA) };
		uint16_t TitleHeight{ 18 };
		uint16_t BorderSize{ 2 };
		uint16_t IBorderSize{ 1 };
		int16_t Align = TEXTALIGN_VCENTER;
	} static DefaultProps;
private:
	Property Props;
	WM_CB *cb = nullptr;
	WObj *pClient = nullptr;
	Menu *pMenu = nullptr;
	TString Title;
	SRect rRestore;
	uint16_t Flags = 0;
	WObj *pFocussedChild = nullptr;
	WM_DIALOG_STATUS *pDialogStatus = nullptr;

	struct Positions {
		int16_t TitleHeight;
		int16_t MenuHeight;
		SRect rClient;
		SRect rTitleText;
	};

public:
	Frame(int x0, int y0, int xsize, int ysize,
		  WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
		  const char *pTitle, WM_CB *cb = nullptr);
	Frame(Point pos, Point size,
		  WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
		  const char *pTitle, WM_CB *cb = nullptr) :
		Frame(pos.x, pos.y, size.x, size.y,
			  pParent, Flags, ExFlags, Id, pTitle, cb) {}
	Frame(SRect r,
		  WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
		  const char *pTitle, WM_CB *cb = nullptr) :
		Frame(r.left_top(), r.size(),
			  pParent, Flags, ExFlags, Id, pTitle, cb) {}
	Frame(int x0, int y0,
		  int xsize, int ysize,
		  uint16_t Id, uint16_t Flags, uint16_t ExFlags,
		  const char *pTitle, WM_CB *cb) :
		Frame(x0, y0, xsize, ysize, nullptr, Flags, ExFlags, Id, pTitle, cb) {}
	Frame(const WM_CREATESTRUCT &wc) : Frame(
		wc.x, wc.y, wc.xsize, wc.ysize, wc.pParent, wc.Id, wc.Flags, wc.ExFlags,
		wc.pCaption, (WM_CB *)wc.Para.ptr) {}
private:
	int16_t _CalcTitleHeight() const;
	Positions _CalcPositions() const;

	void _UpdatePositions();
	void _UpdateButtons(int Height);

	void _ChangeWindowPosSize(Point &);
	bool _ForwardMouseOverMsg(WM_MSG *pMsg);
	void _SetCapture(Point Pos, int Mode);
	int _CheckReactBorder(Point Pos);

	void _OnTouch(WM_MSG *pMsg);
	int _OnTouchResize(WM_MSG *pMsg);
	void _OnPaint() const;
	void _OnChildHasFocus(WM_MSG *pMsg);
	static bool _OnResizeable(WM_MSG *pMsg);
	static void _Callback(WM_MSG *pMsg);
	static void _cbClient(WM_MSG *pMsg);

public:
	Button *AddButton(uint16_t Flags, int Off, uint16_t Id);
	Button *AddMaxButton(uint16_t Flags, int Off);
	Button *AddCloseButton(uint16_t Flags, int Off);
	Button *AddMinButton(uint16_t Flags, int Off);

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
	/* W */ inline void Font(CFont *pFont) {
		if (Props.pFont != pFont) {
			Props.pFont = pFont;
			int OldHeight = _CalcTitleHeight();
			_UpdatePositions();
			_UpdateButtons(OldHeight);
			Invalidate();
		}
	}
	/* R */ auto Font() const { return Props.pFont; }
public: // Property - BarColor
	/* W */ inline void BarColor(unsigned Index, RGBC Color) {
		if (Index < GUI_COUNTOF(Props.aBarColor))
			if (Props.aBarColor[Index] != Color) {
				Props.aBarColor[Index] = Color;
				Invalidate();
			}
	}
	/* R */ inline RGBC BarColor(unsigned Index) const {
		if (Index < GUI_COUNTOF(Props.aBarColor))
			return Props.aBarColor[Index];
		return RGB_INVALID_COLOR;
	}
public: // Property - TextColor
	/* W */ inline void TextColor(RGBC Color) {
		for (auto i = 0; i < GUI_COUNTOF(Props.aTextColor); ++i)
			Props.aTextColor[i] = Color;
		Invalidate();
	}
	/* W */ void TextColor(unsigned Index, RGBC Color) {
		if (Index < GUI_COUNTOF(Props.aTextColor))
			if (Props.aTextColor[Index] != Color) {
				Props.aTextColor[Index] = Color;
				Invalidate();
			}
	}
	/* R */ inline RGBC TextColor(unsigned Index) const {
		if (Index < GUI_COUNTOF(Props.aTextColor))
			return Props.aTextColor[Index];
		return RGB_INVALID_COLOR;
	}
public: // Property - ClientColor
	/* W */ inline void ClientColor(RGBC Color) {
		if (Props.ClientColor != Color) {
			Props.ClientColor = Color;
			pClient->Invalidate();
		}
	}
	/* R */ inline RGBC ClientColor() const { return Props.ClientColor; }
public: // Property - Moveable
	/* W */ inline void Moveable(bool bMoveable) {
		if (bMoveable)
			Flags |= FRAME_CF_MOVEABLE;
		else
			Flags &= ~FRAME_CF_MOVEABLE;
	}
	/* R */ inline bool Moveable() const { return Flags & FRAME_CF_MOVEABLE; }
public: // Property - Resizeable
	/* W */ inline void Resizeable(bool bResizeable) {
		if (bResizeable)
			State |= FRAME_CF_RESIZEABLE;
		else
			State &= ~FRAME_CF_RESIZEABLE;
	}
	/* R */ inline bool Resizeable() const { return State & FRAME_CF_RESIZEABLE; }
public: // Property - Active
	/* W */ void Active(bool bActive);
	/* R */ inline bool Active() const { return Flags & FRAME_CF_ACTIVE; }
public: // Property - TitleVis
	/* W */ void TitleVis(bool bTitleVis);
	/* R */ inline bool TitleVis() const { return State & FRAME_CF_TITLEVIS; }
public: // Property - TitleHeight
	/* W */ void TitleHeight(int Height);
	/* R */ inline auto TitleHeight() const {
		return Props.TitleHeight ?
			Props.TitleHeight :
			_CalcPositions().TitleHeight;
	}
public: // Property - BorderSize
	/* W */ void BorderSize(int Size);
	/* R */ inline auto BorderSize() const { return Props.BorderSize; }
public: // Property - Text
	/* W */ inline void Text(const char *s) {
		if (GUI__SetText(&Title, s))
			Invalidate();
	}
	/* R */ inline auto Text() const { return Title; }
public: // Property - TextAlign
	/* W */ inline void TextAlign(int Align) {
		if (Props.Align != Align) {
			Props.Align = Align;
			Invalidate();
		}
	}
	/* R */ inline auto TextAlign() const { return Props.Align; }
public: // Property - Menu
	/* W */ void Menu(::Menu *pMenu);
	/* R */ inline auto Menu() { return pMenu; }
	/* R */ inline auto Menu() const { return pMenu; }
#pragma endregion

};
