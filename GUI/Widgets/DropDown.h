#pragma once
#include "ListBox.h"

using		DROPDOWN_CF = WC_EX;
constexpr	DROPDOWN_CF
			DROPDOWN_CF_UP				= WC_EX_USER(0),
			DROPDOWN_CF_AUTOSCROLLBAR	= LISTBOX_CF_AUTOSCROLLBAR_V;
enum DROPDOWN_CI {
	DROPDOWN_CI_UNSEL = 0,
	DROPDOWN_CI_SEL,
	DROPDOWN_CI_SELFOCUS
};

struct DropDown : public Widget {

public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aBkColor[3]{
			RGB_WHITE,
			RGB_GRAY,
			RGB_BLUE 
		};
		RGBC aTextColor[3]{
			RGB_BLACK,
			RGB_WHITE,
			RGB_WHITE
		};
		int16_t TextBorderSize{ 2 };
		TEXTALIGN Align{ TEXTALIGN_LEFT };
	} static DefaultProps;

private:
	GUI_Array<GUI_STRING> Handles;
	Property Props = DefaultProps;
	ListBox *pListbox = nullptr;
	SCROLL_STATE ScrollState;
	uint16_t ItemSpacing = 0;
	uint8_t Flags = 0;
	uint8_t ScrollbarWidth = 0;
	int16_t sel = 0;
	int16_t ySizeEx = 0;
	int16_t textHeight = 0;

private:
	void _SelectByKey(int Key);
	void _AdjustHeight();

	void _OnPaint() const;
	bool _OnMouse(const MOUSE_STATE *pState);
	bool _OnKey(KEY_STATE);

	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	DropDown(const SRect &rc = {},
			 PWObj pParent = nullptr, uint16_t Id = 0,
			 WM_CF Flags = WC_HIDE, DROPDOWN_CF FlagsEx = 0);
	DropDown(const WM_CREATESTRUCT &wc) :
		DropDown(wc.rect(),
				 wc.pParent, wc.Id,
				 wc.Flags, wc.FlagsEx) {}
protected:
	~DropDown();

public:
	inline void Inc() { Sel(sel + 1); }
	inline void Dec() { if (sel > 0) Sel(sel - 1); }

	bool AddKey(int Key);

	void Collapse();
	void Expand();

	inline void Add(GUI_PCSTR s) {
		Handles.Add() = s;
		Invalidate();
	}
	void Insert(uint16_t Index, GUI_PCSTR s);
	void Delete(uint16_t Index);

#pragma region Properties
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(TEXTALIGN Align) {
		Props.Align = Align;
		Invalidate();
	}
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return Handles.NumItems(); }
public: // Property - Sel
	/* R */ inline auto Sel() const { return sel; }
	/* W */ void Sel(int16_t);
public: // Property - Spacing
	/* R */ inline auto Spacing() { return ItemSpacing; }
	/* W */ inline void Spacing(uint16_t ItemSpacing) {
		this->ItemSpacing = ItemSpacing;
		if (pListbox)
			pListbox->Spacing(ItemSpacing);
	}
public: // Property - ScrollBarWidth
	/* R */ inline auto ScrollBarWidth() const { return ScrollbarWidth; }
	/* W */ inline void ScrollBarWidth(uint8_t Width) {
		ScrollbarWidth = Width;
		if (pListbox)
			pListbox->ScrollBarWidth(Width);
	}
public: // Property - AutoScroll
	/* R */ inline bool AutoScroll() const { return Flags & DROPDOWN_CF_AUTOSCROLLBAR; }
	/* W */ void AutoScroll(bool bEnable);
public: // Property - TextHeight
	/* R */ inline auto TextHeight() const { return textHeight; }
	/* W */ inline void TextHeight(int16_t TextHeight) {
		textHeight = TextHeight;
		_AdjustHeight();
		textHeight = TextHeight;
		Invalidate();
	}
#pragma endregion

};
