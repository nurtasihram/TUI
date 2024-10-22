#pragma once
#include "WM.h"

#define MULTIEDIT_CF_READONLY        (1 << 0)
#define MULTIEDIT_CF_INSERT          (1 << 2)
#define MULTIEDIT_CF_AUTOSCROLLBAR_V (1 << 3)
#define MULTIEDIT_CF_AUTOSCROLLBAR_H (1 << 4)
#define MULTIEDIT_CF_PASSWORD        (1 << 5)

#define MULTIEDIT_CI_EDIT     0
#define MULTIEDIT_CI_READONLY 1

#define NUM_DISP_MODES 2

struct MultiEdit : public Widget {
	static RGBC _aDefaultBkColor[2];
	static RGBC _aDefaultColor[2];
	static CFont *_pDefaultFont;
public:
	RGBC aBkColor[NUM_DISP_MODES];
	RGBC aColor[NUM_DISP_MODES];
	char *pText;
	uint16_t MaxNumChars;         /* Maximum number of characters including the prompt */
	uint16_t NumChars;            /* Number of characters (text and prompt) in object */
	uint16_t NumCharsPrompt;      /* Number of prompt characters */
	uint16_t NumLines;            /* Number of text lines needed to show all data */
	uint16_t TextSizeX;           /* Size in X of text depending of wrapping mode */
	uint16_t BufferSize;
	uint16_t CursorLine;          /* Number of current cursor line */
	uint16_t CursorPosChar;       /* Character offset number of cursor */
	uint16_t CursorPosByte;       /* Byte offset number of cursor */
	uint16_t CursorPosX;          /* Cursor position in X */
	uint16_t CursorPosY;          /* Cursor position in Y */
	uint16_t CacheLinePosByte;    /*  */
	uint16_t CacheLineNumber;     /*  */
	uint16_t CacheFirstVisibleLine;
	uint16_t CacheFirstVisibleByte;
	WM_SCROLL_STATE scrollStateV, scrollStateH;
	CFont *pFont;
	uint8_t Flags;
	uint8_t InvalidFlags;         /* Flags to save validation status */
	uint8_t EditMode;
	uint8_t HBorder;
	GUI_WRAPMODE WrapMode;
	//private:
	void _InvalidateNumChars();
	int _GetNumChars();
	int _GetXSize();
	int _GetNumCharsInPrompt(const char *pText);
	int _NumChars2XSize(const char *pText, int NumChars);
	int _WrapGetNumCharsDisp(const char *pText);
	int _WrapGetNumBytesToNextLine(const char *pText);
	int _GetCharDistX(const char *pText);
	void _DispString(const char *pText, SRect *pRect);
	char *_GetpLine(unsigned LineNumber);
	void _ClearCache();
	int _GetCursorLine(const char *pText, int CursorPosChar);
	void _GetCursorXY(int *px, int *py);
	void _InvalidateCursorXY();
	void _SetScrollState();
	void _CalcScrollPos();
	int _GetTextSizeX();
	int _GetNumVisLines();
	int _GetNumLines();
	void _InvalidateNumLines();
	void _InvalidateTextSizeX();
	void _CalcScrollParas();
	void _ManageAutoScrollV();
	void _ManageScrollers();
	void _Invalidate();

	void _InvalidateTextArea();
	int _InvalidateCursorPos();
	int _CalcNextValidCursorPos(int CursorPosChar, int *pCursorPosByte, int *pCursorLine);
	void _SetCursorPos(int CursorPosChar);
	void _SetCursorXY(int x, int y);
	void _MoveCursorUp();
	void _MoveCursorDown();
	void _MoveCursor2NextLine();
	void _MoveCursor2LineEnd();
	void _MoveCursor2LinePos1();
	int _IsOverwriteAtThisChar();
	int _GetCursorSizeX();
	int _IncrementBuffer(unsigned AddBytes);
	int _IsSpaceInBuffer(int BytesNeeded);
	int _IsCharsAvailable(int CharsNeeded);
	void _DeleteChar();
	int _InsertChar(uint16_t Char);
	void _OnPaint();
	void _OnTouch(WM_MSG *pMsg);
	int _AddKey(uint16_t Key);

	static void _Callback(WM_MSG *pMsg);

	void _SetFlag(bool bEnable, uint8_t Flag);
	void _SetWrapMode(GUI_WRAPMODE WrapMode);

public:
	inline int AddKey(uint16_t Key) { return _AddKey(Key); }

	inline void WrapWord() { _SetWrapMode(GUI_WRAPMODE_WORD); }
	inline void WrapChar() { _SetWrapMode(GUI_WRAPMODE_CHAR); }
	inline void WrapNone() { _SetWrapMode(GUI_WRAPMODE_NONE); }

public:
	static MultiEdit *Create(int x0, int y0, int xsize, int ysize,
							 WObj *pParent, uint16_t Flags, uint16_t ExFlags,
							 uint16_t Id, int BufferSize, const char *pText);

	static inline auto Create(int x0, int y0, int xsize, int ysize,
							  WObj *pParent, uint16_t Id,
							  uint16_t Flags, uint16_t ExFlags, const char *pText, int MaxLen) {
		return Create(x0, y0, xsize, ysize, pParent, Flags, ExFlags, Id, MaxLen, pText);
	}

#pragma region Properties
public: // Property - Font
	void Font(CFont *pFont);
	inline auto Font() const { return pFont; }
public: // Property - BkColor
	/* W */ void BkColor(unsigned Index, RGBC color) {
		if (aBkColor[Index] != color) {
			aBkColor[Index] = color;
			_InvalidateTextArea();
		}
	}
	/* R */ inline auto BkColor(unsigned Index) const { return aBkColor[Index]; }
public: // Property - BorderH
	/* W */ inline void BorderH(uint8_t HBorder) {
		if (this->HBorder != HBorder) {
			this->HBorder = HBorder;
			_Invalidate();
		}
	}
	/* R */ inline auto BorderH() const { return HBorder; }
public: // Property - Text
	/* W */ void Text(const char *);
	/* R */ inline auto Text() const { return pText; }
public: // Property - InsertMode
	inline void InsertMode(bool bEnable) { _SetFlag(bEnable, MULTIEDIT_CF_INSERT); }
public: // Property - ReadOnly
	inline void ReadOnly(bool bEnable) { _SetFlag(bEnable, MULTIEDIT_CF_READONLY); }
public: // Property - PasswordMode
	void PasswordMode(bool bEnable);
public: // Property - AutoScrollV
	inline void AutoScrollV(bool bEnable) { _SetFlag(bEnable, MULTIEDIT_CF_AUTOSCROLLBAR_V); }
public: // Property - AutoScrollH
	inline void AutoScrollH(bool bEnable) { _SetFlag(bEnable, MULTIEDIT_CF_AUTOSCROLLBAR_H); }
#pragma endregion

};
