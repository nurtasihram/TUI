#pragma once
#include "WM.h"

#define EDIT_CF_LEFT    TEXTALIGN_LEFT
#define EDIT_CF_RIGHT   TEXTALIGN_RIGHT
#define EDIT_CF_HCENTER TEXTALIGN_HCENTER
#define EDIT_CF_VCENTER TEXTALIGN_VCENTER
#define EDIT_CF_TOP     TEXTALIGN_TOP
#define EDIT_CF_BOTTOM  TEXTALIGN_BOTTOM

enum EDIT_CI {
	 EDIT_CI_DISABLED = 0,
	 EDIT_CI_ENABLED
};

class Edit;

typedef void tEDIT_AddKeyEx(int Key);
typedef void tEDIT_UpdateBuffer(Edit *pObj);

class Edit : public Widget {

public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aTextColor[2]{
			RGB_BLACK,
			RGB_BLACK
		};
		RGBC aBkColor[2]{
			RGBC_GRAY(0xC0),
			RGB_WHITE
		};
		TEXTALIGN Align{ TEXTALIGN_LEFT | TEXTALIGN_VCENTER };
		int8_t Border{ 1 };
	} DefaultProps;

private:
	GUI_STRING text;
	uint16_t LenMax;
	uint16_t BufferSize = 0;
	int CursorPos = 0;
	unsigned SelSize = 0;
	bool bInsert = false;
	uint8_t XSizeCursor = 1;
	tEDIT_AddKeyEx *pfAddKeyEx;
	Property Props;

private:
	void _CursorAtPixel(int xPos);
	bool _IncrementBuffer(uint16_t AddBytes);
	bool _IsSpaceInBuffer(uint16_t BytesNeeded);
	bool _IsCharsAvailable(uint16_t CharsNeeded);
	void _DeleteChar();
	int _InsertChar(uint16_t Char);
	uint16_t _CurrentChar();
	void _CursorPos(int CursorPos);

	void _OnPaint();
	void _OnTouch(const PID_STATE *pState);
	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

public:
	Edit(const SRect &rc = {},
		 PWObj pParent = nullptr, uint16_t Id = 0,
		 WM_CF Flags = WC_HIDE, WC_EX FlagsEx = 0,
		 uint16_t MaxLen = 128);
protected:
	~Edit() = default;

public:
	void CursorAtChar(int Pos) {
		_CursorPos(Pos);
		Invalidate();
	}
	void CursorAtPixel(int xPos) {
		_CursorPos(xPos);
		Invalidate();
	}
	void AddKey(int Key);

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		Invalidate();
	}
public: // Property - BkColor
	/* R */ inline auto BkColor(EDIT_CI index) const { return Props.aBkColor[index]; }
	/* W */ inline void BkColor(EDIT_CI index, RGBC Color) {
		Props.aBkColor[index] = Color;
		Invalidate();
	}
public: // Property - TextColor
	/* R */ inline auto TextColor(EDIT_CI index) const { return Props.aBkColor[index]; }
	/* W */ inline void TextColor(EDIT_CI index, RGBC Color) {
		Props.aBkColor[index] = Color;
		Invalidate();
	}
public: // Property - TextAlign
	/* R */ inline auto TextAlign() const { return Props.Align; }
	/* W */ inline void TextAlign(TEXTALIGN Align) {
		Props.Align = Align;
		Invalidate();
	}
public: // Property - Text
	/* R */ inline const char *Text() const { return pText; }
	/* W */ void Text(const char *pText);
public: // Property - InsertMode
	/* R */ inline auto InsertMode() const { return bInsert; }
	/* W */ inline void InsertMode(bool bInsert) { this->bInsert = bInsert; }
public: // Property - MaxLen
	/* R */ inline auto MaxLen() const { return LenMax; }
	/* W */ void MaxLen(uint16_t Len);
#pragma endregion

};
#define EDIT_Create(x0, y0, xsize, ysize, Id, MaxLen, Flags) \
	EDIT_CreateEx(x0, y0, xsize, ysize, nullptr, Flags, 0, Id, MaxLen)
#define EDIT_CreateAsChild(x0, y0, xsize, ysize, pParent, Id, Flags, MaxLen) \
	EDIT_CreateEx(x0, y0, xsize, ysize, pParent, Flags, 0, Id, MaxLen)
Edit *EDIT_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent, uint16_t Flags, uint16_t ExFlags, uint16_t Id, int MaxLen);
