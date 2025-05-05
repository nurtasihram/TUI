#pragma once
#include "WM.h"

using		SLIDER_CF = WC_EX;
constexpr	SLIDER_CF 
			SLIDER_CF__PRESSED = WC_EX_USER(0),
			SLIDER_CF_VERTICAL = WC_EX_VERTICAL;

class Slider : public Widget {
public:
	struct Property {
		RGBC BkColor{ RGBC_GRAY(0xC0) };
		RGBC Color{ RGBC_GRAY(0xC0) };
	} static DefaultProps;
private:
	Property Props = DefaultProps;
	int16_t Min = 0, Max = 100, v = 0;
	int16_t nTicks = 0, width = 8;

private:
	void _SliderPressed();
	void _Released();

	void _OnPaint() const;
	void _OnMouse(const MOUSE_STATE *pState);
	bool _OnKey(KEY_STATE);
	
	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	Slider(const SRect &rc = {},
		   PWObj pParent = nullptr, uint16_t Id = 0,
		   WM_CF Flags = WC_HIDE, SLIDER_CF FlagsEx = 0,
		   int16_t Min = 0, int16_t Max = 100, int16_t v = 0, int16_t nTicks = -1);
	Slider(const WM_CREATESTRUCT &wc) :
		Slider(wc.rect(),
			   wc.pParent, wc.Id,
			   wc.Flags, wc.FlagsEx,
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
		this->width = Width;
		Invalidate();
	}
public: // Property - Value
	/* R */ inline auto Value() const { return this->v; }
	/* W */ inline void Value(int16_t v) {
		if (v < Min) v = Min;
		if (v > Max) v = Max;
		this->v = v;
		Invalidate();
		NotifyParent(WN_VALUE_CHANGED);
	}
public: // Property - NumTicks
	/* R */ inline auto NumTicks() const { return this->nTicks; }
	/* W */ inline void NumTicks(int16_t nTicks) {
		this->nTicks = nTicks;
		Invalidate();
	}
public: // Property - Range
	/* W */ inline void Range(int16_t Min, int16_t Max) {
		if (Max < Min)
			Max = Min;
		this->Min = Min;
		this->Max = Max;
		if (v < Min) v = Min;
		if (v > Max) v = Max;
		Invalidate();
	}
#pragma endregion
};
