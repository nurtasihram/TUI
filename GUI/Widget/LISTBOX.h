#pragma once
#include "GUI_Array.h"
#include "WM.h"

#define LISTBOX_ALL_ITEMS  -1
#define LISTBOX_CI_UNSEL    0
#define LISTBOX_CI_SEL      1
#define LISTBOX_CI_SELFOCUS 2

#define LISTBOX_NOTIFICATION_LOST_FOCUS (WM_NOTIFICATION_WIDGET + 0)
#define LISTBOX_CF_AUTOSCROLLBAR_H   (1<<0)
#define LISTBOX_CF_AUTOSCROLLBAR_V   (1<<1)
#define LISTBOX_CF_MULTISEL          (1<<2)

struct ListBox : public Widget {
public:
	struct Property {
		CFont *pFont = &GUI_Font13_1;
		uint16_t ScrollStepH = 10;
		RGBC aBkColor[4]{
			RGB_WHITE,
			RGB_GRAY,
			RGB_BLUE,
			RGBC_GRAY(0xC0),
		};
		RGBC aTextColor[4]{
			RGB_BLACK,
			RGB_WHITE,
			RGB_WHITE,
			RGB_GRAY
		};
	} static DefaultProps;
	struct Item {
		TString Text;
		uint16_t xSize = 0, ySize = 0;
		uint8_t Status = 0;
		Item(const char *pText) : Text(pText) {}
	};
private:
	Property Props;
	GUI_Array<Item> ItemArray;
	WIDGET_DRAW_ITEM_FUNC *pfDrawItem = nullptr;
	WM_SCROLL_STATE scrollStateV, scrollStateH;
	WObj *pOwner = nullptr;
	uint8_t Flags = 0, ScrollbarWidth = 0;
	int16_t sel = 0;
	uint16_t ItemSpacing = 0;
public:
	ListBox(int x0, int y0, int xsize, int ysize,
			WObj *pParent, uint16_t Id, uint16_t Flags, uint8_t ExFlags,
			int NumItems);
	ListBox(int x0, int y0, int xsize, int ysize,
			WObj *pParent, uint16_t Id, uint16_t Flags, uint8_t ExFlags,
			const char *pItems);
	ListBox(const WM_CREATESTRUCT &wc) : ListBox(
		wc.x, wc.y, wc.xsize, wc.ysize, 
		wc.pParent, wc.Id, wc.Flags, (uint8_t)wc.ExFlags,
		wc.pCaption) {}
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

private:
	void _OnPaint(WM_MSG *pMsg);
	void _OnTouch(const PidState *);
	void _OnMouseOver(const PidState *);
	
	static void _Callback(WM_MSG *);

public:
	bool AddKey(int Key);

	void Add(const char *s);
	void Insert(uint16_t Index, const char *s);
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
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return ItemArray.NumItems(); }
public: // Property - Sel
	/* R */ inline auto Sel() const { return sel; }
	/* W */ void Sel(int NewSel);
public: // Property - MultiSel
	/* R */ inline bool MultiSel() const { return Flags & LISTBOX_CF_MULTISEL; }
	/* W */ inline void MultiSel(bool bEnabled) {
		auto Flags = this->Flags;
		if (bEnabled)
			Flags |= LISTBOX_CF_MULTISEL;
		else 
			Flags &= ~LISTBOX_CF_MULTISEL;
		if (this->Flags == Flags)
			return;
		this->Flags = Flags;
		_InvalidateInsideArea();
	}
public: // Property - ScrollStepH
	/* R */ inline int ScrollStepH() const { return Props.ScrollStepH; }
	/* W */ inline void ScrollStepH(int Value) {
		if (Props.ScrollStepH != Value)
			Props.ScrollStepH = Value;
	}
public: // Property - ScrollBarWidth
	/* R */ inline auto ScrollBarWidth() const { return ScrollbarWidth; }
	/* W */ inline void ScrollBarWidth(uint8_t Width) {
		if (ScrollbarWidth != Width) {
			ScrollbarWidth = Width;
			_SetScrollbarWidth();
			Invalidate();
		}
	}
public: // Property - Spacing
	/* R */ inline auto Spacing() const { return ItemSpacing; }
	/* W */ inline void Spacing(uint16_t Value) {
		if (ItemSpacing != Value) {
			ItemSpacing = Value;
			InvalidateItem(LISTBOX_ALL_ITEMS);
		}
	}
public: // Property - Owner
	/* R */ inline auto Owner() const { return pOwner; }
	/* W */ inline void Owner(WObj *pOwner) {
		this->pOwner = pOwner;
		_InvalidateInsideArea();
	}
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		if (Props.pFont != pFont && pFont) {
			Props.pFont = pFont;
			_InvalidateInsideArea();
		}
	}
public: // Property - BkColor
	/* R */ inline auto BkColor(uint8_t Index) const { return Props.aBkColor[Index]; }
	/* W */ inline void BkColor(uint8_t Index, RGBC Color) {
		if (Props.aBkColor[Index] != Color) {
			Props.aBkColor[Index] = Color;
			_InvalidateInsideArea();
		}
	}
public: // Property - TextColor
	/* R */ inline auto TextColor(uint16_t Index) const { return Props.aTextColor[Index]; }
	/* W */ inline void TextColor(uint16_t Index, RGBC Color) {
		if (Props.aTextColor[Index] != Color) {
			Props.aTextColor[Index] = Color;
			_InvalidateInsideArea();
		}
	}
public: // Property - OwnerDraw
	/* R */ inline void OwnerDraw(WIDGET_DRAW_ITEM_FUNC *pfDrawItem) {
		if (this->pfDrawItem != pfDrawItem) {
			this->pfDrawItem = pfDrawItem;
			InvalidateItem(LISTBOX_ALL_ITEMS);
		}
	}
public: // Property - ItemSel
	/* R */ bool ItemSel(uint16_t Index) const;
	/* W */ void ItemSel(uint16_t Index, bool bSel);
public: // Property - ItemEnabled
	/* R */ bool ItemEnabled(uint16_t Index) const;
	/* W */ void ItemEnabled(uint16_t Index, bool bEnabled);
public: // Property - ItemText
	/* R */ inline const char *ItemText(uint16_t Index) const { return Index < ItemArray.NumItems() ? (const char *)ItemArray[Index].Text : nullptr; }
	/* W */ void ItemText(uint16_t Index, const char *s);
#pragma endregion

};
