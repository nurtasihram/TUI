#pragma once
#include "GUI_Array.h"
#include "WM.h"

enum RADIO_BI {
	RADIO_BI_INACTIV = 0,
	RADIO_BI_ACTIV,
	RADIO_BI_CHECK
};

class Radio : public Widget {

public:
	static CBitmap abmRadio[3];
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		CBitmap *apBm[3]{
			/* Inactive */ &abmRadio[0],
			/* Active */ &abmRadio[1],
			/* Checked */ &abmRadio[2]
		};
		RGBC BkColor{ RGB_INVALID_COLOR };
		RGBC TextColor{ RGB_BLACK };
		uint8_t Border{ 2 };
	} static DefaultProps;

private:
	Property Props = DefaultProps;
	GUI_Array<GUI_STRING> TextArray;
	uint16_t height, spacing;
	int16_t sel = 0;
	uint8_t groupId = 0;

private:
	void _SetValue(int v);
	void _ClearSelection(uint8_t GroupId);
	void _HandleSetValue(int16_t v);

	void _OnPaint() const;
	void _OnMouse(const MOUSE_STATE *pState);
	bool _OnKey(KEY_STATE);

	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	Radio(const SRect &rc,
		  PWObj pParent, uint16_t Id,
		  WM_CF Flags, uint16_t FlagsEx,
		  int NumItems, int Spacing);
	Radio(const SRect &rc,
		  PWObj pParent, uint16_t Id,
		  WM_CF Flags, uint16_t FlagsEx,
		  GUI_PCSTR pItems, int Spacing = 0);
	Radio(const WM_CREATESTRUCT &wc) :
		Radio(wc.rect(),
			  wc.pParent, wc.Id,
			  wc.Flags, wc.FlagsEx,
			  wc.Para.pString) {}
protected:
	~Radio() = default;

public:
	inline void Add(int d) { Sel(sel + d); }
	inline void Inc() { Add(1); }
	inline void Dec() { Add(-1); }

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		if (TextArray.NumItems())
			Invalidate();
	}
public: // Property - TextColor
	/* R */ inline auto BkColor() const { return Props.BkColor; }
	/* W */ inline void BkColor(RGBC Color) {
		Props.BkColor = Color;
		Invalidate();
	}
public: // Property - TextColor
	/* R */ inline auto TextColor() const { return Props.TextColor; }
	/* W */ inline void TextColor(RGBC Color) {
		Props.TextColor = Color;
		if (TextArray.NumItems())
			Invalidate();
	}
public: // Property - Text
	/* R */ inline GUI_PCSTR Text(unsigned Index) const {
		if (Index < (unsigned)TextArray.NumItems())
			return TextArray[Index];
		return nullptr;
	}
	/* W */ void Text(unsigned Index, GUI_PCSTR pText) {
		if (Index < TextArray.NumItems()) {
			TextArray[Index] = pText;
			Invalidate();
		}
	}
public: // Property - Image
	/* R */ inline auto Image(RADIO_BI Index) const { return Props.apBm[Index]; }
	/* W */ inline void Image(RADIO_BI Index, CBitmap *pBitmap) {
		if (!pBitmap) pBitmap = DefaultProps.apBm[Index];
		Props.apBm[Index] = pBitmap;
		Invalidate();
	}
public: // Property - Sel
	/* R */ inline auto Sel() const { return sel; }
	/* W */ void Sel(int16_t);
public: // Property - GroupId
	/* R */ inline auto GroupId() const { return groupId; }
	/* W */ void GroupId(uint8_t groupId);
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return TextArray.NumItems(); }
#pragma endregion

};
