#pragma once
#include "WM.h"

#define SLIDER_STATE_PRESSED WIDGET_STATE_USER0
#define SLIDER_CF_VERTICAL WIDGET_CF_VERTICAL

struct Slider : public Widget {
	struct Property {
		RGBC BkColor = RGBC_GRAY(0xC0);
		RGBC Color = RGBC_GRAY(0xC0);
	} static DefaultProps;
private:
	Property Props;
	int16_t Min, Max, v;
	int16_t nTicks, width = 8;
private:
	void _SliderPressed();
	void _Released();

	void _OnPaint() const;
	void _OnTouch(const PID_STATE *pState);
	void _OnKey(const KEY_STATE *pKeyInfo);
	
	static void _Callback(WObj *pWin, int msgid, WM_PARAM *pData, WObj *pWinSrc);
public:
	Slider(int x0, int y0, int xsize, int ysize,
		   WObj *pParent, uint16_t Id = 0, uint16_t Flags = 0, uint16_t ExFlags = 0,
		   int16_t Min = 0, int16_t Max = 100, int16_t v = 0, int16_t nTicks = -1);
	Slider(const WM_CREATESTRUCT &wc) : Slider(
		wc.x, wc.y, wc.xsize, wc.ysize, wc.pParent, wc.Flags, wc.ExFlags,
		wc.Para.i16_4[0], wc.Para.i16_4[1], wc.Para.i16_4[2], wc.Para.i16_4[3]) {}
public:
	inline void Dec() {
		if (v > Min) {
			--v;
			Invalidate();
			NotifyParent(WN_VALUE_CHANGED);
		}
	}
	inline void Inc() {
		if (v < Max) {
			++v;
			Invalidate();
			NotifyParent(WN_VALUE_CHANGED);
		}
	}

#pragma region Properties
public: // Property - BkColor
	/* R */ inline auto BkColor() const { return Props.BkColor; }
public: // Property - Color
	/* R */ inline auto Color() const { return Props.Color; }
public:  // Propert - Width
	/* R */ inline auto Width() const { return width; }
	/* W */ inline void Width(int Width) {
		if (this->width != Width) {
			this->width = Width;
			Invalidate();
		}
	}
public: // Property - Value
	/* R */ inline auto Value() const { return this->v; }
	/* W */ inline void Value(int16_t v) {
		if (v < Min) v = Min;
		if (v > Max) v = Max;
		if (this->v != v) {
			this->v = v;
			Invalidate();
			NotifyParent(WN_VALUE_CHANGED);
		}
	}
public: // Property - NumTicks
	/* R */ inline auto NumTicks() const { return this->nTicks; }
	/* W */ inline void NumTicks(int16_t nTicks) {
		if (this->nTicks != nTicks) {
			this->nTicks = nTicks;
			Invalidate();
		}
	}
public: // Property - Range
	/* W */ inline void Range(int16_t Min, int16_t Max) {
		if (this->Min != Min || this->Max != Max) {
			if (Max < Min)
				Max = Min;
			this->Min = Min;
			this->Max = Max;
			if (v < Min) v = Min;
			if (v > Max) v = Max;
			Invalidate();
		}
	}
#pragma endregion
};
