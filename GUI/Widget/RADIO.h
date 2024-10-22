#pragma once
#include "GUI_Array.h"
#include "WM.h"

#define RADIO_BI_INACTIV 0
#define RADIO_BI_ACTIV   1
#define RADIO_BI_CHECK   2

#define RADIO_TEXTPOS_RIGHT 0
#define RADIO_TEXTPOS_LEFT  WIDGET_STATE_USER0  /* Not implemented, TBD */

class Radio : public Widget {
public:
	static CBitmap abmRadio[2];
	static CBitmap bmCheck;
	struct Property {
		CFont *pFont = &GUI_Font13_1;
		CBitmap *apBmRadio[2]{
			&abmRadio[0],
			&abmRadio[1]
		};
		CBitmap *pBmCheck = &bmCheck;
		RGBC BkColor = RGB_INVALID_COLOR;
		RGBC TextColor = RGB_BLACK;
		uint8_t Border = 2;
	} static DefaultProps;
private:
	Property Props;
	GUI_Array<TString> TextArray;
	uint16_t height, spacing;
	int16_t sel = 0;
	uint8_t groupId = 0;
private:
	void _OnPaint() const;
	void _OnTouch(PidState *);
	bool _OnKey(const WM_KEY_INFO *);

	void _SetValue(int v);
	void _ClearSelection(uint8_t GroupId);
	void _HandleSetValue(int16_t v);

	static void _Callback(WM_MSG *pMsg);
public:
	Radio(int x0, int y0, int xSize, int ySize,
		  WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
		  int NumItems, int Spacing);
	Radio(int x0, int y0, int xSize, int ySize,
		  WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
		  const char *pItems, int Spacing = 0);
	Radio(const WM_CREATESTRUCT &wc) : Radio(
		wc.x, wc.y, wc.xsize, wc.ysize,
		wc.pParent, wc.Id, wc.Flags, wc.ExFlags,
		wc.Para.pString) {}
protected:
	~Radio() { TextArray.Destruct(); }
public:
	inline void Add(int d) { Sel(sel + d); }
	inline void Inc() { Add(1); }
	inline void Dec() { Add(-1); }

#pragma region Properties
public: // Property - Font
	/* W */ inline void Font(CFont *pFont) {
		if (Props.pFont != pFont && pFont) {
			Props.pFont  = pFont;
			if (TextArray.NumItems())
				Invalidate();
		}
	}
	/* R */ inline auto Font() const { return Props.pFont; }
public: // Property - TextColor
	/* W */ inline void BkColor(RGBC Color) {
		if (Props.BkColor != Color) {
			Props.BkColor = Color;
			Invalidate();
		}
	}
	/* R */ inline RGBC BkColor() const { return Props.BkColor; }
public: // Property - TextColor
	/* W */ inline void TextColor(RGBC Color) {
		if (Props.TextColor != Color) {
			Props.TextColor = Color;
			if (TextArray.NumItems())
				Invalidate();
		}
	}
	/* R */ inline auto TextColor() const { return Props.TextColor; }
public: // Property - Text
	/* W */ void Text(unsigned Index, const char *pText) {
		if (Index < TextArray.NumItems()) {
			TextArray[Index] = pText;
			Invalidate();
		}
	}
	/* R */ inline const char *Text(unsigned Index) const {
		if (Index < (unsigned)TextArray.NumItems())
			return TextArray[Index];
		return nullptr;
	}
public: // Property - Sel
	/* W */ void Sel(int16_t);
	/* R */ inline auto Sel() const { return sel; }
public: // Property - GroupId
	/* W */ void GroupId(uint8_t groupId);
	/* R */ inline auto GroupId() const { return groupId; }
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return TextArray.NumItems(); }
#pragma endregion

};
