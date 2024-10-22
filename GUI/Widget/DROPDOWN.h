#pragma once
#include "ListBox.h"

#define DROPDOWN_CI_UNSEL    0
#define DROPDOWN_CI_SEL      1
#define DROPDOWN_CI_SELFOCUS 2

#define DROPDOWN_CF_AUTOSCROLLBAR   LISTBOX_CF_AUTOSCROLLBAR_V
#define DROPDOWN_CF_UP              (1 << 2)

struct DropDown : public Widget {
public:
	struct Property {
		CFont *pFont = &GUI_Font13_1;
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
		int16_t TextBorderSize = 2;
		int16_t Align = GUI_TA_LEFT;
	} static DefaultProps;
private:
	int16_t sel;
	int16_t ySizeEx; /* Drop down size */
	int16_t textHeight;
	GUI_Array<TString> Handles;
	WM_SCROLL_STATE ScrollState;
	Property Props;
	ListBox *pListbox = nullptr;
	uint8_t Flags = 0;
	uint8_t ScrollbarWidth = 0;
	uint16_t ItemSpacing = 0;
	bool bPressed = false;
private:
	void _SelectByKey(int Key);
	void _AdjustHeight();

	void _OnPaint() const;
	bool _OnTouch(const PidState *pState);
	bool _OnKey(const WM_KEY_INFO *pKi);

	static void _Callback(WM_MSG *pMsg);
public:
	DropDown(int x0, int y0, int xsize, int ysize,
			 WObj *pParent, uint16_t Id, uint16_t Flags, uint8_t ExFlags);
protected:
	~DropDown();

public:
	inline void Inc() { Sel(sel + 1); }
	inline void Dec() { if (sel > 0) Sel(sel - 1); }

	bool AddKey(int Key);

	void Collapse();
	void Expand();

	inline void Add(const char *s) {
		Handles.Add(s);
		Invalidate();
	}
	void InsertString(uint16_t Index, const char *s);
	void Delete(uint16_t Index);

#pragma region Properties
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(int Align) {
		if (Props.Align != Align) {
			Props.Align = Align;
			Invalidate();
		}
	}
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return Handles.NumItems(); }
public: // Property - Sel
	/* R */ inline auto Sel() const { return sel; }
	/* W */ void Sel(int16_t);
public: // Property - Spacing
	/* R */ inline auto Spacing() { return ItemSpacing; }
	/* W */ inline void Spacing(uint16_t ItemSpacing) {
		if (this->ItemSpacing != ItemSpacing) {
			this->ItemSpacing = ItemSpacing;
			if (pListbox)
				pListbox->Spacing(ItemSpacing);
		}
	}
public: // Property - ScrollBarWidth
	/* R */ inline auto ScrollBarWidth() const { return ScrollbarWidth; }
	/* W */ inline void ScrollBarWidth(uint8_t Width) {
		if (this->ScrollbarWidth != Width) {
			this->ScrollbarWidth = Width;
			if (pListbox)
				pListbox->ScrollBarWidth(Width);
		}
	}
public: // Property - AutoScroll
	/* R */ inline bool AutoScroll() const { return Flags & DROPDOWN_CF_AUTOSCROLLBAR; }
	/* W */ void AutoScroll(bool bEnable);
public:
	/* R */ inline auto TextHeight() const {}
	void TextHeight(int16_t TextHeight) {
		if (textHeight != TextHeight) {
			this->textHeight = TextHeight;
			_AdjustHeight(); textHeight = TextHeight;
			Invalidate();
		}
	}
#pragma endregion
};
