#pragma once
#include "GUI_Array.h"
#include "WM.h"

constexpr WC_EX HEADER_CF_DRAG = WC_USER(0);

class Header : public Widget {
	struct Column {
		GUI_DRAW_BASE *pDrawObj = nullptr;
		TString Text;
		uint16_t Width = 0;
		TEXTALIGN Align = TEXTALIGN_LEFT;
		Column() {}
		~Column() {
			GUI_MEM_Free(pDrawObj);
			pDrawObj = nullptr;
		}
	};
public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC BkColor{ RGBC_GRAY(0xAA) };
		RGBC TextColor{ RGB_BLACK };
	} static DefaultProps;
private:
	GUI_Array<Column> Columns;
	Property Props;
	int16_t CapturePosX = -1;
	int16_t CaptureItem = -1;
	uint16_t scrollPos = 0;
private:
	int _GetItemIndex(Point Pos);
	void _HandlePID(PID_STATE State);
	bool _HandleDrag(int MsgId, const PID_STATE *pState);

	void _OnPaint();

	static WM_RESULT _Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);

public:
	Header(int x0, int y0, int xsize, int ysize,
		   WObj *pParent, uint16_t Id,
		   uint16_t Flags, uint16_t ExFlags = 0);

public:
	void Add(const char *pText, uint16_t Width = 0, TEXTALIGN Align= TEXTALIGN_LEFT);

	void DeleteItem(unsigned Index);

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		Invalidate();
	}
public: // Property - TextColor
	/* R */ inline auto TextColor() const { return Props.TextColor; }
	/* W */ inline void TextColor(RGBC Color) {
		Props.TextColor = Color;
		Invalidate();
	}
public: // Property - BkColor
	/* R */ inline auto BkColor() const { return Props.BkColor; }
	/* W */ inline void BkColor(RGBC Color) {
		Props.BkColor = Color;
		Invalidate();
	}
public: // Property - ItemWidth
	/* R */ inline uint16_t ItemWidth(uint16_t Index) const {
		if (Index < Columns.NumItems())
			return Columns[Index].Width;
		return 0;
	}
	/* W */ void ItemWidth(uint16_t Index, int Width);
public: // Property - ItemAlign
	/* R */ inline TEXTALIGN ItemAlign(uint16_t Index) const {
		if (Index < Columns.NumItems())
			return Columns[Index].Align;
		return 0;
	}
	/* W */ inline void ItemAlign(uint16_t Index, TEXTALIGN Align) {
		if (Index < Columns.NumItems()) {
			Columns[Index].Align = Align;
			Invalidate();
		}
	}
public: // Property - ItemText
	/* R */ inline const char *ItemText(uint16_t Index) const { return Columns[Index].Text; }
	/* W */ inline void ItemText(uint16_t Index, const char *pText) {
		if (Index < Columns.NumItems())
			Columns[Index].Text = pText;
	}
public: // Property - ScrollPos
	/* R */ inline auto ScrollPos() const { return scrollPos; }
	/* W */ void ScrollPos(uint16_t ScrollPos) {
		scrollPos = ScrollPos;
		Invalidate();
		Parent()->Invalidate();
	}
public: // Property - Height
	/* R */ inline auto Height() const { return ClientRect().ysize(); }
	/* W */ void Height(uint16_t height);
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return Columns.NumItems(); }
#pragma endregion

};
