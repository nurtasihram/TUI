#pragma once
#include "GUI_Array.h"
#include "WM.h"

using		LISTBOX_CF = WC_EX;
constexpr	LISTBOX_CF
			LISTBOX_CF_AUTOSCROLLBAR_H = WC_EX_USER(0),
			LISTBOX_CF_AUTOSCROLLBAR_V = WC_EX_USER(1),
			LISTBOX_CF_MULTISEL        = WC_EX_USER(2);
enum LISTBOX_CI {
	LISTBOX_CI_UNSEL = 0,
	LISTBOX_CI_SEL,
	LISTBOX_CI_SELFOCUS,
	LISTBOX_CI_DISABLED
};
constexpr int LISTBOX_NOTIFICATION_LOST_FOCUS = WN_WIDGET;
constexpr int16_t LISTBOX_ALL_ITEMS = -1;

class ListBox : public Widget {

public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aBkColor[4]{
			/* Unselect */ RGB_WHITE,
			/* Selected */ RGB_GRAY,
			/* Selected focussed */ RGB_DARKBLUE,
			/* Disabled */ RGBC_GRAY(0xC0)
		};
		RGBC aTextColor[4]{
			/* Unselect */ RGB_BLACK,
			/* Selected */ RGB_WHITE,
			/* Selected focussed */ RGB_WHITE,
			/* Disabled */ RGB_GRAY
		};
		uint16_t ScrollStepH{ 10 };
	} static DefaultProps;

private:
	struct Item {
		GUI_STRING Text;
		uint16_t xSize = 0, ySize = 0;
		uint8_t Status = 0;
	};
	Property Props = DefaultProps;
	GUI_Array<Item> ItemArray;
	WIDGET_DRAW_ITEM_FUNC *pfDrawItem = nullptr;
	PWObj pOwner = nullptr;
	SCROLL_STATE scrollStateV, scrollStateH;
	int16_t sel = 0;
	uint16_t ItemSpacing = 0;
	uint8_t ScrollbarWidth = 0;

private:
	int _CallOwnerDraw(int Cmd, uint16_t ItemIndex);
	int _GetYSize();
	int _GetItemSize(uint16_t Index, uint8_t XY);
	int _GetContentsSizeX();
	int _GetItemPosY(uint16_t Index);
	bool _IsPartiallyVis();
	uint16_t _GetNumVisItems();
	void _NotifyOwner(int Notification);
	int _UpdateScrollPos();
	void _InvalidateItemSize(uint16_t Index);
	void _InvalidateInsideArea();
	void _InvalidateItem(int sel);
	void _InvalidateItemAndBelow(int sel);
	void _SetScrollbarWidth();
	int _CalcScrollParas();
	void _ManageAutoScroll();
	int _UpdateScrollers();
	void _SelectByKey(int Key);
	void _ToggleMultiSel(int sel);
	int _GetItemFromPos(int x, int y);

	void _OnPaint(SRect rClip);
	void _OnMouse(const MOUSE_STATE *pState);
	bool _OnKey(KEY_STATE);
	void _OnMouseOver(const MOUSE_STATE *pState);
	
	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	ListBox(const SRect &rc = {},
			PWObj pParent = nullptr, uint16_t Id = 0,
			WM_CF Flags = WC_HIDE, LISTBOX_CF FlagsEx = 0,
			int NumItems = 0);
	ListBox(const SRect &rc = {},
			PWObj pParent = nullptr, uint16_t Id = 0,
			WM_CF Flags = WC_HIDE, LISTBOX_CF FlagsEx = 0,
			GUI_PCSTR pItems = nullptr);
	ListBox(const WM_CREATESTRUCT &wc) : ListBox(
		wc.rect(),
		wc.pParent, wc.Id,
		wc.Flags, wc.FlagsEx,
		wc.pCaption) {}
protected:
	~ListBox() = default;

public:
	bool AddKey(int Key);

	void Add(GUI_PCSTR s);
	void Insert(uint16_t Index, GUI_PCSTR s);
	void Delete(uint16_t Index);

	void InvalidateItem(int Index);

	void AutoScroll(bool bEnabled, bool H0V1);

	inline void AutoScrollH(bool bEnabled) { return AutoScroll(bEnabled, false); }
	inline void AutoScrollV(bool bEnabled) { return AutoScroll(bEnabled, true); }

	void MoveSel(int Dir);
	inline void IncSel() { MoveSel(1); }
	inline void DecSel() { MoveSel(-1); }

	static int OwnerDrawProc(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo);

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		_InvalidateInsideArea();
	}
public: // Property - BkColor
	/* R */ inline auto BkColor(LISTBOX_CI Index) const { return Props.aBkColor[Index]; }
	/* W */ inline void BkColor(LISTBOX_CI Index, RGBC Color) {
		Props.aBkColor[Index] = Color;
		_InvalidateInsideArea();
	}
public: // Property - TextColor
	/* R */ inline auto TextColor(LISTBOX_CI Index) const { return Props.aTextColor[Index]; }
	/* W */ inline void TextColor(LISTBOX_CI Index, RGBC Color) {
		Props.aTextColor[Index] = Color;
		_InvalidateInsideArea();
	}
public: // Property - OwnerDraw
	/* R */ inline void OwnerDraw(WIDGET_DRAW_ITEM_FUNC *pfDrawItem) {
		this->pfDrawItem = pfDrawItem;
		InvalidateItem(LISTBOX_ALL_ITEMS);
	}
public: // Property - Spacing
	/* R */ inline auto Spacing() const { return ItemSpacing; }
	/* W */ inline void Spacing(uint16_t Value) {
		ItemSpacing = Value;
		InvalidateItem(LISTBOX_ALL_ITEMS);
	}
public: // Property - ScrollStepH
	/* R */ inline int ScrollStepH() const { return Props.ScrollStepH; }
	/* W */ inline void ScrollStepH(int Value) { Props.ScrollStepH = Value; }
public: // Property - ScrollBarWidth
	/* R */ inline auto ScrollBarWidth() const { return ScrollbarWidth; }
	/* W */ inline void ScrollBarWidth(uint8_t Width) {
		ScrollbarWidth = Width;
		_SetScrollbarWidth();
		Invalidate();
	}
public: // Property - ItemSel
	/* R */ bool ItemSel(uint16_t Index) const;
	/* W */ void ItemSel(uint16_t Index, bool bSel);
public: // Property - ItemEnabled
	/* R */ bool ItemEnabled(uint16_t Index) const;
	/* W */ void ItemEnabled(uint16_t Index, bool bEnabled);
public: // Property - ItemText
	/* R */ inline GUI_PCSTR ItemText(uint16_t Index) const { return Index < ItemArray.NumItems() ? (GUI_PCSTR )ItemArray[Index].Text : nullptr; }
	/* W */ void ItemText(uint16_t Index, GUI_PCSTR s);
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return ItemArray.NumItems(); }
public: // Property - Sel
	/* R */ inline auto Sel() const { return sel; }
	/* W */ void Sel(int NewSel);
public: // Property - MultiSel
	/* R */ inline bool MultiSel() const { return StatusEx & LISTBOX_CF_MULTISEL; }
	/* W */ void MultiSel(bool bEnabled);
public: // Property - Owner
	/* R */ inline auto Owner() const { return pOwner; }
	/* W */ inline void Owner(PWObj pOwner) {
		this->pOwner = pOwner;
		_InvalidateInsideArea();
	}
#pragma endregion

};
