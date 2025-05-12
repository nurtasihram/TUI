#pragma once
#include "WM.h"

using		SCROLLBAR_CF = WC_EX;
constexpr	SCROLLBAR_CF
			SCROLLBAR_CF__PRESSED	= WC_EX_USER(0),
			SCROLLBAR_CF_VERTICAL	= WC_EX_VERTICAL,
			SCROLLBAR_CF_FOCUSSABLE	= WC_FOCUSSABLE;
enum SCROLLBAR_CI {
	SCROLLBAR_CI_RAIL = 0,
	SCROLLBAR_CI_BUTTON,
};

class ScrollBar : public Widget {

public:
	struct Property {
		RGBC aBkColor[2]{
			RGBC_GRAY(0x80),
			RGB_BLACK
		};
		RGBC aColor{ RGBC_GRAY(0xC0) };
	} static DefaultProps;
	static const int16_t DefaultWidth = 12;

private:
	Property Props = DefaultProps;
	SCROLL_STATE state;

private:
	struct Positions {
		int16_t x0_LeftArrow = 0,
				x1_LeftArrow = 0;
		int16_t x0_RightArrow = 0,
				x1_RightArrow = 0;
		int16_t x0_Thumb = 0,
				x1_Thumb = 0;
		int16_t ThumbSize = 0;
		int16_t xSizeMoveable = 0;
		int16_t x1 = 0;
	};

	int _GetArrowSize() const;
	Positions _CalcPositions() const;
	SRect _AutoSize(PWObj pParent, uint16_t FlagsEx) const;

	void _DrawTriangle(int x, int y, int Size, int Inc) const;
	void _InvalidatePartner();
	void _ScrollbarPressed();
	void _ScrollbarReleased();

	void _OnMouse(MOUSE_STATE);
	bool _OnKey(KEY_STATE);
	void _OnPaint();
	
	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	ScrollBar(const SRect &rc,
			  PWObj pParent = nullptr, uint16_t Id = 0,
			  WM_CF Flags = WC_HIDE, SCROLLBAR_CF FlagsEx = 0,
			  SCROLL_STATE State = { 100, 0, 10 });
	ScrollBar(PWObj pParent, uint16_t Id);
	ScrollBar(const WM_CREATESTRUCT &wc) : ScrollBar(
		wc.rect(),
		wc.pParent, wc.Id,
		wc.Flags, wc.FlagsEx,
		{ wc.Para.i16_4[0], wc.Para.i16_4[1], wc.Para.i16_4[2] }) {}
protected:
	~ScrollBar();

public:
	inline void Dec() { AddValue(-1); }
	inline void Inc() { AddValue(1); }
	inline void AddValue(int Add) { Value(state.v + Add); }

	ScrollBar *Partner();

#pragma region Properties
public: // Property - Value
	/* R */ inline auto Value() const { return state.v; }
	/* W */ inline void Value(int v) {
		state.Value(v);
		Invalidate();
		NotifyOwner(WN_VALUE_CHANGED);
	}
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return state.NumItems; }
	/* W */ inline void NumItems(int NumItems) {
		state.NumItems = NumItems;
		Invalidate();
	}
public: // Property - PageSize
	/* R */ inline auto PageSize() const { return state.PageSize; }
	/* W */ inline void PageSize(int PageSize) {
		state.PageSize = PageSize;
		Invalidate();
	}
public: // Property - ScrollState
	/* R */ inline auto ScrollState() const { return state; }
	/* W */ inline void ScrollState(const SCROLL_STATE &s) {
		state = s;
		Invalidate();
	}
public: // Property - Width
	/* R */ inline auto Width() const { return StatusEx & SCROLLBAR_CF_VERTICAL ? SizeY() : SizeX(); }
	/* W */ inline void Width(int Width) {
		if (StatusEx & SCROLLBAR_CF_VERTICAL)
			Widget::SizeY(Width);
		else
			Widget::SizeX(Width);
		_InvalidatePartner();
	}
#pragma endregion

};
