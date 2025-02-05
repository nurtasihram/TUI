#pragma once
#include "GUI_Array.h"
#include "Header.h"

enum LISTVIEW_CI {
	 LISTVIEW_CI_UNSEL = 0,
	 LISTVIEW_CI_SEL,
	 LISTVIEW_CI_SELFOCUS
};
constexpr WC_EX LISTVIEW_CF_HEADER_DRAG = HEADER_CF_DRAG;

struct LISTVIEW_ITEM_INFO {
	RGBC aBkColor[3];
	RGBC aTextColor[3];
};
struct ListView : public Widget {
public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aBkColor[3]{
			/* Not selected */ RGB_WHITE,
			/* Selected, no focus */ RGB_GRAY,
			/* Selected, focus */ RGB_BLUE
		};
		RGBC aTextColor[3]{
			/* Not selected */ RGB_BLACK,
			/* Selected, no focus */ RGB_WHITE,
			/* Selected, focus */ RGB_WHITE
		};
		RGBC GridColor{ RGB_LIGHTGRAY };
	} static DefaultProps;
	struct Item {
		LISTVIEW_ITEM_INFO *pItemInfo = nullptr;
		TString text;
		~Item() {
			GUI_MEM_Free(pItemInfo);
			pItemInfo = nullptr;
		}
	};
private:
	GUI_Array<GUI_Array<Item>> RowArray;
	GUI_Array<TEXTALIGN> AlignArray;
	Property Props;
	SCROLL_STATE scrollStateV;
	int16_t sel = -1;
	SCROLL_STATE scrollStateH;
	uint16_t RowDistY = 0;
	uint16_t lBorder = 1, rBorder = 1;
	bool bShowGrid = true;
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
protected:
	~ListView();

public:

	inline void Inc() { Sel(sel + 1); }
	inline void Dec() {
		if (auto sel = Sel())
			Sel(sel - 1);
	}

	void AddRow(const char *pTexts);
	void AddColumn(const char *s, uint16_t Width = 0, TEXTALIGN Align = TEXTALIGN_LEFT);
	void DeleteColumn(unsigned Index);
	void DeleteRow(unsigned Index);

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
	/* R */ inline auto GridVis() const { return bShowGrid; }
	/* W */ inline void GridVis(int Show) {
		bShowGrid = Show;
		_UpdateScrollParas();
		_InvalidateInsideArea();
	}
public: // Property - NumColumns
	/* R */ inline auto NumColumns() const { return pHeader->NumItems(); }
public: // Property - NumRows
	/* R */ inline auto NumRows() const { return RowArray.NumItems(); }
public: // Property - ColumnWidth
	/* R */ inline auto ColumnWidth(unsigned Index) const { return pHeader->ItemWidth(Index); }
	/* W */ inline void ColumnWidth(unsigned Index, uint16_t Width) { pHeader->ItemWidth(Index, Width); }
public: // Property - TextAlign
	/* R */ inline auto TextAlign(unsigned Index) const { return AlignArray[Index]; }
	/* W */ inline void TextAlign(unsigned Index, TEXTALIGN Align) {
		AlignArray[Index] = Align;
		_InvalidateInsideArea();
	}
public: // Property - ItemColor
	/* W */ inline void ItemColor(unsigned Column, unsigned Row, RGBC Color) {
		if (Column >= NumColumns() || Row >= NumRows())
			return;
	}
 public: // Property - ItemText
	 /* W */ inline void ItemText(unsigned Column, unsigned Row, const char *s) {
		 if (Column >= NumColumns() || Row >= NumRows())
			 return;
		 GUI__SetText(&RowArray[Row][Column].text, s);
		 _InvalidateRow(Row);
	 }
#pragma endregion

};
