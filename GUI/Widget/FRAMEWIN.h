#pragma once
#include "WM.h"
#include "BUTTON.h"
#include "MENU.h"

#define FRAME_CF_ACTIVE     (1<<3)
#define FRAME_CF_MOVEABLE   (1<<4)
#define FRAME_CF_RESIZEABLE (1<<5)
#define FRAME_CF_TITLEVIS   (1<<6)
#define FRAME_CF_MINIMIZED  (1<<7)
#define FRAME_CF_MAXIMIZED  (1<<8)
#define FRAMEWIN_BUTTON_RIGHT   (1<<0)
#define FRAMEWIN_BUTTON_LEFT    (1<<1)

struct FRAMEWIN_PROPS {
	CFont *pFont;
	RGBC    aBarColor[2];
	RGBC    aTextColor[2];
	RGBC    ClientColor;
	int16_t TitleHeight;
	int16_t BorderSize;
	int16_t IBorderSize;
} ;
class Frame : public Widget {
	FRAMEWIN_PROPS Props;
	WM_CALLBACK *cb;
	WObj *pClient;
	::Menu *pMenu;
	char *pText;
	SRect rRestore;
	int16_t textAlign;
	uint16_t Flags;
	WObj *pFocussedChild; /* Handle to focussed child .. default none (0) */
	WM_DIALOG_STATUS *pDialogStatus;

	struct Positions {
		int16_t TitleHeight;
		int16_t MenuHeight;
		SRect rClient;
		SRect rTitleText;
	};

public:
	static Frame *Create(int x0, int y0, int xsize, int ysize, WObj *pParent, int WinFlags, int ExFlags, int Id, const char *pTitle, WM_CALLBACK *cb);
	static inline auto Create(const char *pText, WM_CALLBACK *cb, int Flags, int x0, int y0, int xsize, int ysize)
	{ return Create(x0, y0, xsize, ysize, nullptr, Flags, 0, 0, pText, cb); }
	
private:
	int _CalcTitleHeight() const;
	Positions _CalcPositions() const;

	void _UpdatePositions();
	void _UpdateButtons(int Height);

	void _OnTouch(WM_MESSAGE *pMsg);
	int _OnTouchResize(WM_MESSAGE *pMsg);
	void _OnPaint() const;
	void _OnChildHasFocus(WM_MESSAGE *pMsg);
	static bool _OnResizeable(WM_MESSAGE *pMsg);
	static void _Callback(WM_MESSAGE *pMsg);
	static void _cbClient(WM_MESSAGE *pMsg);

public:
	Button *AddButton(int Flags, int Off, int Id);
	Button *AddMaxButton(int Flags, int Off);
	Button *AddCloseButton(int Flags, int Off);
	Button *AddMinButton(int Flags, int Off);

private:
	void _InvalidateButton(int Id);
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
		return GUI_INVALID_COLOR;
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
		return GUI_INVALID_COLOR;
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
		if (GUI__SetText(&pText, s))
			Invalidate();
	}
	/* R */ inline auto Text() const { return pText; }
public: // Property - TextAlign
	/* W */ inline void TextAlign(int Align) {
		if (textAlign != Align) {
			textAlign = Align;
			Invalidate();
		}
	}
	/* R */ inline auto TextAlign() const { return textAlign; }
public: // Property - Menu
	/* W */ void Menu(::Menu *pMenu);
	/* R */ inline auto Menu() { return pMenu; }
	/* R */ inline auto Menu() const { return pMenu; }
#pragma endregion

};
