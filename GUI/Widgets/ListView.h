#pragma once
#include "GUI_Array.h"
#include "Header.h"

using		LISTVIEW_CF = WC_EX;
constexpr	LISTVIEW_CF
			LISTVIEW_CF_HEADER_DRAG = HEADER_CF_DRAG,
			LISTVIEW_CF_GRIDLINE	= LISTVIEW_CF_HEADER_DRAG << 1;
using		LISTVIEW_IF = uint8_t;
constexpr	LISTVIEW_IF
			LISTVIEW_ITEM_LEFT		= ALIGN_LEFT,
			LISTVIEW_ITEM_WIDGET	= 1 << 0;
enum LISTVIEW_CI {
	LISTVIEW_CI_UNSEL = 0,
	LISTVIEW_CI_SEL,
	LISTVIEW_CI_SELFOCUS
};

struct ListView : public Widget {

public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aBkColor[3]{
			/* Unselect */ RGB_WHITE,
			/* Selected */ RGB_GRAY,
			/* Selected focussed */ RGB_DARKBLUE
		};
		RGBC aTextColor[3]{
			/* Unselect */ RGB_BLACK,
			/* Selected */ RGB_WHITE,
			/* Selected focussed */ RGB_WHITE
		};
		RGBC GridColor{ RGB_LIGHTGRAY };
	} static DefaultProps;

private:
	struct Item {
		PWObj pItem = nullptr;
		GUI_STRING Text;
		Item() {}
	};
	struct Column : public GUI_Array<Item> {
	};
	GUI_Array<Column> RowArray;
	Property Props;
	SCROLL_STATE scrollStateV;
	int16_t sel = -1;
	SCROLL_STATE scrollStateH;
	uint16_t RowDistY = 0;
	uint16_t lBorder = 1, rBorder = 1;
	PWObj pOwner = nullptr;
	Header *pHeader = nullptr;
private:
	uint16_t _HeaderHeight() const;
	inline auto _GetXSize() const { return InsideRect().x1 + 1; }
	uint16_t _GetNumVisibleRows() const;
	uint16_t _GetRowDistY() const;
	int _GetHeaderWidth() const;
	int _UpdateScrollParas();
	void _InvalidateRow(int sel);
	void _InvalidateRowAndBelow(int sel);
	int _UpdateScrollPos();
	void _NotifyOwner(int Notification);
	void _SelFromPos(Point Pos);

	void _OnTouch(const PID_STATE *pState);
	void _OnPaint(SRect rClip) const;
	bool _OnKey(int Key);

	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	ListView(const SRect &rc = {},
			 PWObj pParent = nullptr, uint16_t Id = 0,
			 WM_CF Flags = WC_HIDE, LISTVIEW_CF FlagsEx = 0);
protected:
	~ListView() = default;

public:
	inline void Inc() { Sel(sel + 1); }
	inline void Dec() {
		if (auto sel = Sel())
			Sel(sel - 1);
	}

	void AddRow(const char *pTexts);
	void AddColumn(const char *pText, int Width = -1, TEXTALIGN Align = TEXTALIGN_LEFT);
	void DeleteRow(uint16_t Index);
	void DeleteColumn(uint16_t Index);

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		_UpdateScrollParas();
		Invalidate();
	}
public: // Property - BkColor
	/* R */ inline auto BkColor(LISTVIEW_CI Index) const { return Props.aBkColor[Index]; }
	/* W */ inline void BkColor(LISTVIEW_CI Index, RGBC Color) {
		Props.aBkColor[Index] = Color;
		Invalidate();
	}
public: // Property - TextColor
	/* R */ inline auto TextColor(LISTVIEW_CI Index) const { return Props.aTextColor[Index]; }
	/* W */ inline void TextColor(LISTVIEW_CI Index, RGBC Color) {
		Props.aTextColor[Index] = Color;
		Invalidate();
	}
public: // Property - LBorder
	/* R */ inline auto LBorder() const { return lBorder; }
	/* W */ inline void LBorder(uint16_t BorderSize) {
		lBorder = BorderSize;
		Invalidate();
	}
public: // Property - RBorder
	/* R */ inline auto RBorder() const { return rBorder; }
	/* W */ inline void RBorder(uint16_t BorderSize) {
		rBorder = BorderSize;
		Invalidate();
	}
public: // Property - RowHeight
	/* R */ inline auto RowHeight() const { return RowDistY; }
	/* W */ inline void RowHeight(uint16_t height) {
		RowDistY = height;
		_UpdateScrollParas();
		Invalidate();
	}
public: // Proeprty - Sel
	/* R */ inline int16_t Sel() const { return sel; }
	/* W */ void Sel(int16_t sel);
public: // Property - GridLine
	/* R */ inline bool GridLine() const { return StatusEx & LISTVIEW_CF_GRIDLINE; }
	/* W */ inline void GridLine(bool bShow) {
		if (bShow)
			StatusEx |= LISTVIEW_CF_GRIDLINE;
		else
			StatusEx &= ~LISTVIEW_CF_GRIDLINE;
		_UpdateScrollParas();
		Invalidate();
	}
public: // Property - NumColumns
	/* R */ inline auto NumColumns() const { return pHeader->NumItems(); }
public: // Property - NumRows
	/* R */ inline auto NumRows() const { return RowArray.NumItems(); }
public: // Property - ColumnWidth
	/* R */ inline auto ColumnWidth(uint16_t Index) const { return pHeader->ItemWidth(Index); }
	/* W */ inline void ColumnWidth(uint16_t Index, uint16_t Width) { pHeader->ItemWidth(Index, Width); }
 public: // Property - ItemText
	 /* W */ inline void ItemText(uint16_t Column, uint16_t Row, const char *s) {
		 if (Column >= NumColumns() || Row >= NumRows())
			 return;
		 RowArray[Row][Column].Text = s;
		 _InvalidateRow(Row);
	 }
#pragma endregion

};
