#pragma once
#include "WM.h"

#define SCROLLBAR_STATE_PRESSED   WC_USER(0)

#define SCROLLBAR_CF_VERTICAL     WC_EX_VERTICAL
#define SCROLLBAR_CF_FOCUSSABLE   WC_FOCUSSABLE

enum SCROLLBAR_CI {
	SCROLLBAR_CI_
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
	Property Props;
	SCROLL_STATE state{ 100, 0, 10 };
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
	void _DrawTriangle(int x, int y, int Size, int Inc) const;
	void _InvalidatePartner();
	void _ScrollbarPressed();
	void _ScrollbarReleased();
	void _OnTouch(const PID_STATE *pState);
	void _OnKey(const KEY_STATE *pKeyInfo);
	void _OnPaint();
	
	static WM_RESULT _Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);

protected:
	SRect _AutoSize(WObj *pParent, uint16_t ExFlags) const;
public:
	ScrollBar(int x0, int y0, int xsize, int ysize,
			  WObj *pParent, uint16_t Id,
			  WM_CF Flags, uint16_t ExFlags);
	ScrollBar(WObj *pParent, int SpecialFlags);
	ScrollBar(const WM_CREATESTRUCT &wc) :
		ScrollBar(wc.x, wc.y, wc.xsize, wc.ysize,
				  wc.pParent, wc.Id, wc.Flags, wc.ExFlags) {}
public:
	inline void Dec() { AddValue(-1); }
	inline void Inc() { AddValue(1); }
	inline void AddValue(int Add) { Value(state.v + Add); }

	ScrollBar *Partner();

#pragma region Properties
public: // Property - Value
	/* R */ inline auto Value() const { return state.v; }
	/* W */ inline void Value(int v) {
		if (state.v != v) {
			state.Value(v);
			Invalidate();
			NotifyParent(WN_VALUE_CHANGED);
		}
	}
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return state.NumItems; }
	/* W */ inline void NumItems(int NumItems) {
		if (state.NumItems != NumItems) {
			state.NumItems = NumItems;
			Invalidate();
		}
	}
public: // Property - PageSize
	/* R */ inline auto PageSize() const { return state.PageSize; }
	/* W */ inline void PageSize(int PageSize) {
		if (state.PageSize != PageSize) {
			state.PageSize = PageSize;
			Invalidate();
		}
	}
public: // Property - ScrollState
	/* R */ inline auto ScrollState() const { return state; }
	/* W */ inline void ScrollState(const SCROLL_STATE &s) {
		if (state != s) {
			state = s;
			Invalidate();
			NotifyParent(WN_VALUE_CHANGED);
		}
	}
public: // Property - Width
	/* R */ inline auto Width() const { return SizeX(); }
	/* W */ inline void Width(int Width) {
		Widget::SizeX(Width);
		_InvalidatePartner();
	}
#pragma endregion

};
