#pragma once
#include "GUI_Array.h"
#include "WM.h"

#define MULTIPAGE_ALIGN_LEFT    (0<<0)
#define MULTIPAGE_ALIGN_RIGHT   (1<<0)
#define MULTIPAGE_ALIGN_TOP     (0<<2)
#define MULTIPAGE_ALIGN_BOTTOM  (1<<2)

#define MULTIPAGE_ALIGN_DEFAULT      (MULTIPAGE_ALIGN_LEFT | MULTIPAGE_ALIGN_TOP)

using		MULTIPAGE_STATE = WC_EX;
constexpr	MULTIPAGE_STATE
			MULTIPAGE_STATE_ENABLED    = WC_EX_USER(0),
			MULTIPAGE_STATE_SCROLLMODE = WC_EX_USER(1);
enum MULTIPAGE_CI {
	MULTIPAGE_CI_DISABLED = 0,
	MULTIPAGE_CI_ENABLED
};

class MultiPage : public Widget {

public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aBkColor[2]{
			/* disabled page */ RGBC_GRAY(0xD0),
			/* enabled page */ RGBC_GRAY(0xC0)
		};
		RGBC aTextColor[2]{
			/* disabled page */ RGBC_GRAY(0x80),
			/* enabled page */ RGB_BLACK
		};
	} static DefaultProps;

private:
	struct Page {
		PWObj pWin = nullptr;
		uint8_t Status = 0;
		GUI_STRING Text;
	};
	Property Props = DefaultProps;
	PWObj pClient;
	GUI_Array<Page> Handles;
	int16_t ScrollValue = 0;
	uint16_t Sel = 0xffff;
	TEXTALIGN align = 0;

private:
	void _ShowPage(unsigned Index);
	SRect _CalcClientRect();
	SRect _CalcBorderRect();
	int _GetPageSizeX(unsigned Index);
	int _GetPagePosX(unsigned Index);
	int _GetTextWidth();
	SRect _GetTextRect();
	void _UpdatePositions();
	void _DrawTextItem(GUI_PCSTR pText, unsigned Index,
					   SRect r, int x0, int w, MULTIPAGE_CI ColorIndex);
	bool _ClickedOnMultipage(Point);

	void _OnPaint();
	void _OnMouse(MOUSE_STATE);

	static WM_RESULT _ClientCb(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);
	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	MultiPage(const SRect &rc,
			  PWObj pParent, uint16_t Id,
			  WM_CF Flags, WC_EX FlagsEx = 0);
protected:
	~MultiPage() = default;

public:
	void Add(GUI_PCSTR pText = nullptr, PWObj pWin = nullptr);
	void Delete(unsigned Index, int Delete);

#pragma region Properties
public: // Property - PageRect
	/* R */ inline SRect PageRect() const { return pClient->ClientRect(); }
public: // Property - Text
	/* R */ inline GUI_PCSTR Text(unsigned Index) const { return Handles[Index].Text; }
	/* W */ inline void Text(unsigned Index, GUI_PCSTR pText) {
		Handles[Index].Text = pText;
		_UpdatePositions();
	}
public: // Property - BkColor
	/* R */ inline RGBC BkColor(MULTIPAGE_CI Index) const { return Props.aBkColor[Index]; }
	/* W */ inline void BkColor(MULTIPAGE_CI Index, RGBC Color) {
		Props.aBkColor[Index] = Color;
		Invalidate();
	}
public: // Property - TextColor
	/* R */ inline RGBC TextColor(MULTIPAGE_CI Index) const { return Props.aTextColor[Index]; }
	/* W */ inline void TextColor(MULTIPAGE_CI Index, RGBC Color) {
		Props.aTextColor[Index] = Color;
		Invalidate();
	}
public: // Property - Font
	/* R */ inline CFont *Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		_UpdatePositions();
	}
public: // Property - Align
	/* R */ inline auto Align() const { return align; }
	/* W */ void Align(TEXTALIGN align) {
		this->align = align;
		pClient->Position(_CalcClientRect().left_top());
		_UpdatePositions();
	}
public: // Property - Select
	/* R */ inline auto Select() const { return Sel; }
	/* W */ void Select(unsigned Index);
public: // Property - Window
	/* W */ inline PWObj Window(unsigned Index) { return Handles[Index].pWin; }
public: // Property - Enable
	/* R */ inline bool Enable(unsigned Index) const { return Handles[Index].Status & MULTIPAGE_STATE_ENABLED; }
	/* W */ inline void Enable(unsigned Index, bool bEnable) {
		if (Index >= Handles.NumItems())
			return;
		auto &page = Handles[Index];
		if (bEnable)
			page.Status |= MULTIPAGE_STATE_ENABLED;
		else
			page.Status &= ~MULTIPAGE_STATE_ENABLED;
		Invalidate();
	}
#pragma endregion

};
