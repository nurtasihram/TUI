#pragma once
#include "ListBox.h"

using		DROPDOWN_CF = WC_EX;
constexpr	DROPDOWN_CF
			DROPDOWN_CF_AUTOSCROLLBAR_H = LISTBOX_CF_AUTOSCROLLBAR_H,
			DROPDOWN_CF_AUTOSCROLLBAR_V = LISTBOX_CF_AUTOSCROLLBAR_V,
			DROPDOWN_CF_AUTOSCROLLBAR	= LISTBOX_CF_AUTOSCROLLBAR,
			DROPDOWN_CF_UP				= WC_EX_USER(2);

class DropDown : public Widget {

private:
	ListBox *pListbox = nullptr;
	SCROLL_STATE ScrollState;
	int16_t ySizeEx = 0;

private:
	bool _SelectByKey(int Key);
	void _AdjustHeight();
	SRect _ListBoxRect() const;

	void _OnPaint() const;
	bool _OnMouse(MOUSE_STATE);
	bool _OnKey(KEY_STATE);

	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	DropDown(const SRect &rc = {},
			 PWObj pParent = nullptr, uint16_t Id = 0,
			 WM_CF Flags = WC_HIDE, DROPDOWN_CF FlagsEx = 0,
			 GUI_PCSTR pItems = nullptr);
	DropDown(const WM_CREATESTRUCT &wc) :
		DropDown(wc.rect(),
				 wc.pParent, wc.Id,
				 wc.Flags, wc.FlagsEx,
				 wc.pCaption) {}
protected:
	~DropDown() { pListbox->Destroy(); }

public:
	inline void Inc() { Sel(Sel() + 1); }
	inline void Dec() { if (Sel() > 0) Sel(Sel() - 1); }

	inline void Add(GUI_PCSTR s) {
		pListbox->Add(s);
		Invalidate();
	}
	inline void Insert(uint16_t Index, GUI_PCSTR s) {
		pListbox->Insert(Index, s);
		Invalidate();
	}
	inline void Delete(uint16_t Index) {
		pListbox->Delete(Index);
		Invalidate();
	}

#pragma region Properties
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return pListbox->NumItems(); }
public: // Property - Expend
	/* R */ inline bool Expand() const { return pListbox->Visible(); }
	/* W */ void Expand(bool bExpand);
public: // Property - Sel
	/* R */ inline int16_t Sel() const { return pListbox->Sel(); }
	/* W */ void Sel(int16_t sel);
#pragma endregion

};
