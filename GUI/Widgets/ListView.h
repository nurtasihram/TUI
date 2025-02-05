#pragma once
#include "GUI_Array.h"
#include "Header.h"

enum LISTVIEW_CI {
	 LISTVIEW_CI_UNSEL = 0,
	 LISTVIEW_CI_SEL,
	 LISTVIEW_CI_SELFOCUS
};
constexpr WC_EX LISTVIEW_CF_HEADER_DRAG = HEADER_CF_DRAG,
				LISTVIEW_CF_GRIDLINE = LISTVIEW_CF_HEADER_DRAG << 1;

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
	struct Item {
		TString Text;
		Item() {}
	};
	struct Column : public GUI_Array<Item> {
	};
private:
	GUI_Array<Column> RowArray;
	Property Props;
	SCROLL_STATE scrollStateV;
	int16_t sel = -1;
	SCROLL_STATE scrollStateH;
	uint16_t RowDistY = 0;
	uint16_t lBorder = 1, rBorder = 1;
	WObj *pOwner = nullptr;
	Header *pHeader = nullptr;
private:
	inline auto _GetXSize() const { return InsideRect().x1 + 1; }
	uint16_t _GetNumVisibleRows() const;
	uint16_t _GetRowDistY() const;
	int _GetHeaderWidth() const;
	int _UpdateScrollParas();
	void _InvalidateInsideArea();
	void _InvalidateRow(int sel);
	void _InvalidateRowAndBelow(int sel);
	int _UpdateScrollPos();
	void _NotifyOwner(int Notification);
	void _SelFromPos(const PID_STATE *pState);

	void _OnTouch(const PID_STATE *pState);
	void _OnPaint(SRect rClip) const;
	bool _OnKey(int Key);

	static WM_RESULT _Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);
public:
	ListView(int x0, int y0, int xsize, int ysize,
			 WObj *pParent, uint16_t Id,
			 uint16_t Flags, uint16_t ExFlags);

public:

	inline void Inc() { Sel(sel + 1); }
	inline void Dec() {
		if (auto sel = Sel())
			Sel(sel - 1);
	}

	void AddRow(const char *pTexts);
	void AddColumn(const char *pText, uint16_t Width = 0, TEXTALIGN Align = TEXTALIGN_LEFT);
	void DeleteRow(uint16_t Index);
	void DeleteColumn(uint16_t Index);

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		_UpdateScrollParas();
		_InvalidateInsideArea();
	}
public: // Property - BkColor
	/* R */ inline auto BkColor(LISTVIEW_CI Index) const { return Props.aBkColor[Index]; }
	/* W */ inline void BkColor(LISTVIEW_CI Index, RGBC Color) {
		Props.aBkColor[Index] = Color;
		_InvalidateInsideArea();
	}
public: // Property - TextColor
	/* R */ inline auto TextColor(LISTVIEW_CI Index) const { return Props.aTextColor[Index]; }
	/* W */ inline void TextColor(LISTVIEW_CI Index, RGBC Color) {
		Props.aTextColor[Index] = Color;
		_InvalidateInsideArea();
	}
public: // Property - LBorder
	/* R */ inline auto LBorder() const { return lBorder; }
	/* W */ inline void LBorder(uint16_t BorderSize) {
		lBorder = BorderSize;
		_InvalidateInsideArea();
	}
public: // Property - RBorder
	/* R */ inline auto RBorder() const { return rBorder; }
	/* W */ inline void RBorder(uint16_t BorderSize) {
		rBorder = BorderSize;
		_InvalidateInsideArea();
	}
public: // Property - RowHeight
	/* R */ inline auto RowHeight() const { return RowDistY; }
	/* W */ inline void RowHeight(uint16_t height) {
		RowDistY = height;
		_UpdateScrollParas();
		_InvalidateInsideArea();
	}
public: // Proeprty - Sel
	/* R */ inline int16_t Sel() const { return sel; }
	/* W */ void Sel(int16_t sel);
public: // Property - GridVis
	/* R */ inline bool GridVis() const { return StatusEx & LISTVIEW_CF_GRIDLINE; }
	/* W */ inline void GridVis(bool bShow) {
		if (bShow)
			StatusEx |= LISTVIEW_CF_GRIDLINE;
		else
			StatusEx &= ~LISTVIEW_CF_GRIDLINE;
		_UpdateScrollParas();
		_InvalidateInsideArea();
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
		 GUI__SetText(&RowArray[Row][Column].Text, s);
		 _InvalidateRow(Row);
	 }
#pragma endregion

};
