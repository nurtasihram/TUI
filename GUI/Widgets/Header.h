#pragma once
#include "GUI_Array.h"
#include "WM.h"

using		HEADER_CF = WC_EX;
constexpr	HEADER_CF
			HEADER_CF_DRAG = WC_EX_USER(0);

class Header : public Widget {

public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC BkColor{ RGBC_GRAY(0xAA) };
		RGBC TextColor{ RGB_BLACK };
	} static DefaultProps;

private:
	struct Column {
		GUI_DRAW_BASE *pDrawObj = nullptr;
		GUI_STRING Text;
		uint16_t Width = 0;
		TEXTALIGN Align = TEXTALIGN_LEFT;
		Column() {}
		~Column() {
			GUI_MEM_Free(pDrawObj);
			pDrawObj = nullptr;
		}
		inline operator GUI_PCSTR () const {
			return Text;
		}
	};
	GUI_Array<Column> Columns;
	Property Props = DefaultProps;
	int16_t CapturePosX = -1;
	int16_t CaptureItem = -1;
	uint16_t scrollPos = 0;

private:
	int _GetItemIndex(Point Pos);

	void _HandleMouse(MOUSE_STATE);
	bool _HandleDrag(int MsgId, MOUSE_STATE);

	void _OnPaint();

	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	Header(const SRect &rc,
		   PWObj pParent, uint16_t Id,
		   WM_CF Flags, HEADER_CF FlagsEx = 0,
		   GUI_PCSTR pTexts = nullptr,
		   const uint16_t *pacWidth = nullptr);
protected:
	~Header() = default;

public:
	void Add(GUI_PCSTR pText, int Width = -1, TEXTALIGN Align = TEXTALIGN_LEFT);

	void Delete(unsigned Index);

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
	/* R */ inline GUI_PCSTR ItemText(uint16_t Index) const { return Columns[Index].Text; }
	/* W */ inline void ItemText(uint16_t Index, GUI_PCSTR pText) {
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
public: // Property - Drag
	/* R */ inline bool Drag() const { return StatusEx & HEADER_CF_DRAG; }
	/* W */ inline void Drag(bool bDrag) {
		if (bDrag)
			StatusEx |= HEADER_CF_DRAG;
		else
			StatusEx &= ~HEADER_CF_DRAG;
	}
#pragma endregion

};
