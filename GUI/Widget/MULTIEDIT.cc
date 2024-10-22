#include <memory.h>
#include "MULTIEDIT.h"
#define MULTIEDIT_FONT_DEFAULT       &GUI_Font13_1
#define MULTIEDIT_BKCOLOR0_DEFAULT   GUI_WHITE
#define MULTIEDIT_BKCOLOR1_DEFAULT   0xC0C0C0
#define MULTIEDIT_TEXTCOLOR0_DEFAULT GUI_BLACK
#define MULTIEDIT_TEXTCOLOR1_DEFAULT GUI_BLACK
#define MULTIEDIT_PASSWORD_CHAR      '*'
#define INVALID_NUMCHARS (1 << 0)
#define INVALID_NUMLINES (1 << 1)
#define INVALID_TEXTSIZE (1 << 2)
#define INVALID_CURSORXY (1 << 3)
#define MULTIEDIT_REALLOC_SIZE 16
static RGBC _aDefaultBkColor[2] = {
	MULTIEDIT_BKCOLOR0_DEFAULT,
	MULTIEDIT_BKCOLOR1_DEFAULT,
};
static RGBC _aDefaultColor[2] = {
	MULTIEDIT_TEXTCOLOR0_DEFAULT,
	MULTIEDIT_TEXTCOLOR1_DEFAULT,
};
static CFont *_pDefaultFont = MULTIEDIT_FONT_DEFAULT;
static void _InvalidateNumChars(MultiEdit_Obj *pObj) {
	pObj->InvalidFlags |= INVALID_NUMCHARS;
}
static int _GetNumChars(MultiEdit_Obj *pObj) {
	if (pObj->InvalidFlags & INVALID_NUMCHARS) {
		pObj->NumChars = GUI__GetNumChars(pObj->pText);
		pObj->InvalidFlags &= ~INVALID_NUMCHARS;
	}
	return pObj->NumChars;
}
static int _GetXSize(MultiEdit_Obj *pObj) {
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	return rect.x1 - rect.x0 - (pObj->HBorder * 2) - 1;
}
static int _GetNumCharsInPrompt(const MultiEdit_Obj *pObj, const char *pText) {
	char *pString = pObj->pText;
	char *pEndPrompt = pString + GUI_UC__NumChars2NumBytes(pString, pObj->NumCharsPrompt);
	return pText < pEndPrompt ?
		GUI_UC__NumBytes2NumChars(pText, pEndPrompt - pText) : 0;
}
static int _NumChars2XSize(const char *pText, int NumChars) {
	int xSize = 0;
	while (NumChars--) {
		uint16_t Char = GUI_UC__GetCharCodeInc(&pText);
		xSize += GUI_GetCharDistX(Char);
	}
	return xSize;
}
static int _WrapGetNumCharsDisp(MultiEdit_Obj *pObj, const char *pText) {
	int xSize = _GetXSize(pObj);
	if (!(pObj->Flags & MULTIEDIT_CF_PASSWORD))
		return GUI__WrapGetNumCharsDisp(pText, xSize, pObj->WrapMode);
	int NumCharsPrompt = _GetNumCharsInPrompt(pObj, pText);
	int r = GUI__WrapGetNumCharsDisp(pText, xSize, pObj->WrapMode);
	if (r < NumCharsPrompt)
		return r;
	int x;
	switch (pObj->WrapMode) {
	case GUI_WRAPMODE_NONE:
		return GUI__GetNumChars(pText);
	default:
		r = NumCharsPrompt;
		x = _NumChars2XSize(pText, NumCharsPrompt);
		pText += GUI_UC__NumChars2NumBytes(pText, NumCharsPrompt);
		while (GUI_UC__GetCharCodeInc(&pText) != 0) {
			x += GUI_GetCharDistX(MULTIEDIT_PASSWORD_CHAR);
			if (r && x > xSize)
				return r;
			r++;
		}
		break;
	}
	return r;
}
static int _WrapGetNumBytesToNextLine(MultiEdit_Obj *pObj, const char *pText) {
	int xSize = _GetXSize(pObj);
	if (!(pObj->Flags & MULTIEDIT_CF_PASSWORD))
		return GUI__WrapGetNumBytesToNextLine(pText, xSize, pObj->WrapMode);
	int NumCharsPrompt = _GetNumCharsInPrompt(pObj, pText);
	int NumChars = _WrapGetNumCharsDisp(pObj, pText);
	int r = GUI_UC__NumChars2NumBytes(pText, NumChars);
	if (NumChars < NumCharsPrompt)
		if (*(pText + r) == '\n')
			r++;
	return r;
}
static int _GetCharDistX(const MultiEdit_Obj *pObj, const char *pText) {
	if ((pObj->Flags & MULTIEDIT_CF_PASSWORD) && _GetNumCharsInPrompt(pObj, pText) == 0)
		return GUI_GetCharDistX(MULTIEDIT_PASSWORD_CHAR);
	uint16_t c = GUI_UC_GetCharCode(pText);
	return GUI_GetCharDistX(c);
}
static void _DispString(MultiEdit_Obj *pObj, const char *pText, SRect *pRect) {
	int NumCharsDisp = _WrapGetNumCharsDisp(pObj, pText);
	if (!(pObj->Flags & MULTIEDIT_CF_PASSWORD)) {
		GUI_DispStringInRectMax(pText, pRect, GUI_TA_LEFT, NumCharsDisp);
		return;
	}
	int NumCharsLeft = 0;
	int NumCharsPrompt = _GetNumCharsInPrompt(pObj, pText);
	if (NumCharsDisp < NumCharsPrompt)
		NumCharsPrompt = NumCharsDisp;
	else
		NumCharsLeft = NumCharsDisp - NumCharsPrompt;
	GUI_DispStringInRectMax(pText, pRect, GUI_TA_LEFT, NumCharsPrompt);
	int x = pRect->x0 + _NumChars2XSize(pText, NumCharsPrompt);
	if (NumCharsLeft) {
		GUI_DispCharAt(MULTIEDIT_PASSWORD_CHAR, x, pRect->y0);
		GUI_DispChars(MULTIEDIT_PASSWORD_CHAR, NumCharsLeft - 1);
	}
}
static char *_GetpLine(MultiEdit_Obj *pObj, unsigned LineNumber) {
	char *pText = pObj->pText;
	if ((unsigned)pObj->CacheLineNumber != LineNumber)
		return pText + pObj->CacheLinePosByte;
	char *pLine;
	if (LineNumber > (unsigned)pObj->CacheLineNumber) {
		int OldNumber = pObj->CacheLineNumber;
		pLine = pText + pObj->CacheLinePosByte;
		pObj->CacheLineNumber = LineNumber;
		LineNumber -= OldNumber;
	}
	else {
		pLine = pText;
		pObj->CacheLineNumber = LineNumber;
	}
	while (LineNumber--)
		pLine += _WrapGetNumBytesToNextLine(pObj, pLine);
	pObj->CacheLinePosByte = pLine - pText;
	return pText + pObj->CacheLinePosByte;
}
static void _ClearCache(MultiEdit_Obj *pObj) {
	pObj->CacheLineNumber = 0;
	pObj->CacheLinePosByte = 0;
	pObj->CacheFirstVisibleByte = 0;
	pObj->CacheFirstVisibleLine = 0;
}
static int _GetCursorLine(MultiEdit_Obj *pObj, const char *pText, int CursorPosChar) {
	int ByteOffsetNewCursor = GUI_UC__NumChars2NumBytes(pText, CursorPosChar);
	const char *pCursor = pText + ByteOffsetNewCursor;
	int LineNumber = 0;
	if (pObj->CacheLinePosByte < ByteOffsetNewCursor) {
		pText += pObj->CacheLinePosByte;
		LineNumber += pObj->CacheLineNumber;
	}
	while (*pText && pCursor > pText) {
		int NumChars = _WrapGetNumCharsDisp(pObj, pText);
		const char *pEndLine = pText + GUI_UC__NumChars2NumBytes(pText, NumChars);
		pText += _WrapGetNumBytesToNextLine(pObj, pText);
		if (pCursor <= pEndLine) {
			if ((pCursor == pEndLine) && (pEndLine == pText) && *pText)
				LineNumber++;
			return LineNumber;
		}
		LineNumber++;
	}
	return LineNumber;
}
static void _GetCursorXY(MultiEdit_Obj *pObj, int *px, int *py) {
	if (!(pObj->InvalidFlags & INVALID_CURSORXY)) {
		*px = pObj->CursorPosX;
		*py = pObj->CursorPosY;
		return;
	}
	int CursorLine = 0, x = 0;
	GUI.Font(pObj->pFont);
	const char *pLine = pObj->pText;
	if (pLine) {
		const char *pCursor = pLine + pObj->CursorPosByte;
		CursorLine = pObj->CursorLine;
		pLine = _GetpLine(pObj, CursorLine);
		while (pLine < pCursor) {
			x += _GetCharDistX(pObj, pLine);
			pLine += GUI_UC_GetCharSize(pLine);
		}
	}
	pObj->CursorPosX = x;
	pObj->CursorPosY = CursorLine * GUI_GetFontDistY();
	pObj->InvalidFlags &= ~INVALID_CURSORXY;
}
static void _InvalidateCursorXY(MultiEdit_Obj *pObj) {
	pObj->InvalidFlags |= INVALID_CURSORXY;
}
static void _SetScrollState(MultiEdit_Obj *pObj) {
	WIDGET__SetScrollState(pObj, &pObj->ScrollStateV, &pObj->ScrollStateH);
}
static void _CalcScrollPos(MultiEdit_Obj *pObj) {
	int xCursor, yCursor;
	_GetCursorXY(pObj, &xCursor, &yCursor);
	yCursor /= GUI_GetYDistOfFont(pObj->pFont);
	WM_CheckScrollPos(&pObj->ScrollStateV, yCursor, 0, 0);       /* Vertical */
	WM_CheckScrollPos(&pObj->ScrollStateH, xCursor, 30, 30);     /* Horizontal */
	_SetScrollState(pObj);
}
static int _GetTextSizeX(MultiEdit_Obj *pObj) {
	if (!(pObj->InvalidFlags & INVALID_TEXTSIZE))
		return pObj->TextSizeX;
	pObj->TextSizeX = 0;
	char *pText = pObj->pText;
	if (pText) {
		GUI.Font(pObj->pFont);
		do {
			int NumChars = _WrapGetNumCharsDisp(pObj, pText);
			char *pLine = pText;
			int xSizeLine = 0;
			while (NumChars--) {
				xSizeLine += _GetCharDistX(pObj, pLine);
				pLine += GUI_UC_GetCharSize(pLine);
			}
			if (xSizeLine > pObj->TextSizeX)
				pObj->TextSizeX = xSizeLine;
			pText += _WrapGetNumBytesToNextLine(pObj, pText);
		} while (*pText);
	}
	pObj->InvalidFlags &= ~INVALID_TEXTSIZE;
	return pObj->TextSizeX;
}
static int _GetNumVisLines(MultiEdit_Obj *pObj) {
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	return (rect.y1 - rect.y0 + 1) / GUI_GetYDistOfFont(pObj->pFont);
}
static int _GetNumLines(MultiEdit_Obj *pObj) {
	if (!(pObj->InvalidFlags & INVALID_NUMLINES))
		return pObj->NumLines;
	int NumLines = 0;
	char *pText = pObj->pText;
	if (pText) {
		uint16_t Char;
		GUI.Font(pObj->pFont);
		do {
			int NumChars = _WrapGetNumCharsDisp(pObj, pText);
			int NumBytes = GUI_UC__NumChars2NumBytes(pText, NumChars);
			Char = GUI_UC_GetCharCode(pText + NumBytes);
			if (Char)
				NumLines++;
			pText += _WrapGetNumBytesToNextLine(pObj, pText);
		} while (Char);
	}
	pObj->NumLines = NumLines + 1;
	pObj->InvalidFlags &= ~INVALID_NUMLINES;
	return pObj->NumLines;
}
static void _InvalidateNumLines(MultiEdit_Obj *pObj) {
	pObj->InvalidFlags |= INVALID_NUMLINES;
}
static void _InvalidateTextSizeX(MultiEdit_Obj *pObj) {
	pObj->InvalidFlags |= INVALID_TEXTSIZE;
}
static void _CalcScrollParas(MultiEdit_Obj *pObj) {
	pObj->ScrollStateV.NumItems = _GetNumLines(pObj);
	pObj->ScrollStateV.PageSize = _GetNumVisLines(pObj);
	pObj->ScrollStateH.NumItems = _GetTextSizeX(pObj);
	pObj->ScrollStateH.PageSize = _GetXSize(pObj);
	_CalcScrollPos(pObj);
}
static void _ManageAutoScrollV(MultiEdit_Obj *pObj) {
	if (!(pObj->Flags & MULTIEDIT_CF_AUTOSCROLLBAR_V))
		return;
	char IsRequired = _GetNumVisLines(pObj) < _GetNumLines(pObj);
	if (WM_SetScrollbarV(pObj, IsRequired) == IsRequired)
		return;
	_InvalidateNumLines(pObj);
	_InvalidateTextSizeX(pObj);
	_InvalidateCursorXY(pObj);
	_ClearCache(pObj);
}
static void _ManageScrollers(MultiEdit_Obj *pObj) {
	_ManageAutoScrollV(pObj);
	if (pObj->Flags & MULTIEDIT_CF_AUTOSCROLLBAR_H) {
		char IsRequired = (_GetXSize(pObj) < _GetTextSizeX(pObj));
		if (WM_SetScrollbarH(pObj, IsRequired) != IsRequired)
			_ManageAutoScrollV(pObj);
	}
	_CalcScrollParas(pObj);
}
static void _Invalidate(MultiEdit_Obj *pObj) {
	_ManageScrollers(pObj);
	pObj->Invalidate();
}
static void _InvalidateTextArea(MultiEdit_Obj *pObj) {
	SRect rInsideRect;
	_ManageScrollers(pObj);
	WM_GetInsideRectExScrollbar(pObj, &rInsideRect);
	pObj->Invalidate(&rInsideRect);
}
static int _InvalidateCursorPos(MultiEdit_Obj *pObj) {
	int Value = pObj->CursorPosChar;
	pObj->CursorPosChar = 0xffff;
	return Value;
}
static void _SetFlag(MultiEdit_Obj *pObj, int OnOff, uint8_t Flag) {
	if (!pObj)
		return;
	if (OnOff)
		pObj->Flags |= Flag;
	else
		pObj->Flags &= ~Flag;
	_InvalidateTextArea(pObj);
}
static int _CalcNextValidCursorPos(MultiEdit_Obj *pObj, int CursorPosChar, int *pCursorPosByte, int *pCursorLine) {
	char *pText = pObj->pText;
	if (!pText)
		return 0;
	int NumChars = _GetNumChars(pObj);
	if (CursorPosChar < pObj->NumCharsPrompt)
		CursorPosChar = pObj->NumCharsPrompt;
	if (CursorPosChar > NumChars)
		CursorPosChar = NumChars;
	int CursorPosByte = GUI_UC__NumChars2NumBytes(pText, CursorPosChar);
	int CursorLine = _GetCursorLine(pObj, pText, CursorPosChar);
	char *pCursor = pText + CursorPosByte;
	char *pNextLine = _GetpLine(pObj, CursorLine);
	if (pNextLine > pCursor) {
		if (pObj->CursorPosChar < CursorPosChar)
			pCursor = pNextLine;
		else {
			char *pPrevLine = _GetpLine(pObj, CursorLine - 1);
			int NumChars = _WrapGetNumCharsDisp(pObj, pPrevLine);
			pPrevLine += GUI_UC__NumChars2NumBytes(pPrevLine, NumChars);
			pCursor = pPrevLine;
		}
		CursorPosChar = GUI_UC__NumBytes2NumChars(pText, pCursor - pText);
		CursorPosByte = GUI_UC__NumChars2NumBytes(pText, CursorPosChar);
		CursorLine = _GetCursorLine(pObj, pText, CursorPosChar);
	}
	if (pCursorPosByte)
		*pCursorPosByte = CursorPosByte;
	if (pCursorLine)
		*pCursorLine = CursorLine;
	return CursorPosChar;
}
static void _SetCursorPos(MultiEdit_Obj *pObj, int CursorPosChar) {
	int CursorPosByte, CursorLine;
	CursorPosChar = _CalcNextValidCursorPos(pObj, CursorPosChar, &CursorPosByte, &CursorLine);
	if (pObj->CursorPosChar == CursorPosChar)
		return;
	pObj->CursorPosByte = CursorPosByte;
	pObj->CursorPosChar = CursorPosChar;
	pObj->CursorLine = CursorLine;
	_InvalidateCursorXY(pObj);
	_CalcScrollPos(pObj);
}
static int _SetWrapMode(MultiEdit_Obj *pObj, GUI_WRAPMODE WrapMode) {
	if (!pObj)
		return 0;
	int r = pObj->WrapMode;
	if (pObj->WrapMode == WrapMode)
		return r;
	pObj->WrapMode = WrapMode;
	_ClearCache(pObj);
	_InvalidateNumLines(pObj);
	_InvalidateTextSizeX(pObj);
	_InvalidateTextArea(pObj);
	_SetCursorPos(pObj, _InvalidateCursorPos(pObj));
	return r;
}
static void _SetCursorXY(MultiEdit_Obj *pObj, int x, int y) {
	if (x < 0 || y < 0)
		return;
	char *pText = pObj->pText;
	if (!pText) {
		_SetCursorPos(pObj, 0);
		return;
	}
	GUI.Font(pObj->pFont);
	int CursorLine = y / GUI_GetFontDistY();
	char *pLine = _GetpLine(pObj, CursorLine);
	int WrapChars = _WrapGetNumCharsDisp(pObj, pLine);
	uint16_t Char = GUI_UC__GetCharCode(pLine + GUI_UC__NumChars2NumBytes(pLine, WrapChars));
	if (pObj->Flags & MULTIEDIT_CF_PASSWORD) {
		if (!Char)
			WrapChars++;
	}
	else if (!Char || Char == '\n' ||
			 (Char == ' ' && pObj->WrapMode == GUI_WRAPMODE_WORD))
		WrapChars++;
	int SizeX = 0;
	while (--WrapChars > 0) {
		Char = GUI_UC_GetCharCode(pLine);
		SizeX += _GetCharDistX(pObj, pLine);
		if (!Char || SizeX > x)
			break;
		pLine += GUI_UC_GetCharSize(pLine);
	}
	_SetCursorPos(pObj, GUI_UC__NumBytes2NumChars(pText, pLine - pText));
}
static void _MoveCursorUp(MultiEdit_Obj *pObj) {
	int xPos, yPos;
	_GetCursorXY(pObj, &xPos, &yPos);
	yPos -= GUI_GetYDistOfFont(pObj->pFont);
	_SetCursorXY(pObj, xPos, yPos);
}
static void _MoveCursorDown(MultiEdit_Obj *pObj) {
	int xPos, yPos;
	_GetCursorXY(pObj, &xPos, &yPos);
	yPos += GUI_GetYDistOfFont(pObj->pFont);
	_SetCursorXY(pObj, xPos, yPos);
}
static void _MoveCursor2NextLine(MultiEdit_Obj *pObj) {
	int xPos, yPos;
	_GetCursorXY(pObj, &xPos, &yPos);
	yPos += GUI_GetYDistOfFont(pObj->pFont);
	_SetCursorXY(pObj, 0, yPos);
}
static void _MoveCursor2LineEnd(MultiEdit_Obj *pObj) {
	int xPos, yPos;
	_GetCursorXY(pObj, &xPos, &yPos);
	_SetCursorXY(pObj, 0x7FFF, yPos);
}
static void _MoveCursor2LinePos1(MultiEdit_Obj *pObj) {
	int xPos, yPos;
	_GetCursorXY(pObj, &xPos, &yPos);
	_SetCursorXY(pObj, 0, yPos);
}
static int _IsOverwriteAtThisChar(MultiEdit_Obj *pObj) {
	const char *pText = pObj->pText;
	if (!pText)
		return 0;
	if (pObj->Flags & MULTIEDIT_CF_INSERT)
		return 0;
	int Line1 = pObj->CursorLine,
		CurPos = _CalcNextValidCursorPos(pObj, pObj->CursorPosChar + 1, 0, 0),
		Line2 = _GetCursorLine(pObj, pText, CurPos);
	pText += pObj->CursorPosByte;
	uint16_t Char = GUI_UC_GetCharCode(pText);
	if (!Char)
		return 0;
	if (Line1 == Line2 || (pObj->Flags & MULTIEDIT_CF_PASSWORD))
		return 1;
	if (Char != '\n')
		if ((Char != ' ') || (pObj->WrapMode == GUI_WRAPMODE_CHAR))
			return 0;
	return 0;
}
static int _GetCursorSizeX(MultiEdit_Obj *pObj) {
	if (!_IsOverwriteAtThisChar(pObj))
		return 2;
	const char *pText = pObj->pText;
	pText += pObj->CursorPosByte;
	return _GetCharDistX(pObj, pText);
}
static int _IncrementBuffer(MultiEdit_Obj *pObj, unsigned AddBytes) {
	int NewSize = pObj->BufferSize + AddBytes;
	char *pNew = (char *)GUI_ALLOC_Realloc(pObj->pText, NewSize);
	if (!pNew)
		return 0;
	if (!pObj->pText)
		*pNew = '\0';
	pObj->BufferSize = NewSize;
	pObj->pText = pNew;
	return 1;
}
static int _IsSpaceInBuffer(MultiEdit_Obj *pObj, int BytesNeeded) {
	int NumBytes = 0;
	if (pObj->pText)
		NumBytes = GUI__strlen(pObj->pText);
	BytesNeeded = (BytesNeeded + NumBytes + 1) - pObj->BufferSize;
	if (BytesNeeded <= 0)
		return 1;
	return _IncrementBuffer(pObj, BytesNeeded + MULTIEDIT_REALLOC_SIZE);
}
static int _IsCharsAvailable(MultiEdit_Obj *pObj, int CharsNeeded) {
	if (CharsNeeded <= 0)
		return 1;
	if (pObj->MaxNumChars <= 0)
		return 1;
	int NumChars = 0;
	if (pObj->pText)
		NumChars = _GetNumChars(pObj);
	return CharsNeeded + NumChars <= pObj->MaxNumChars;
}
static void _DeleteChar(MultiEdit_Obj *pObj) {
	char *s = pObj->pText;
	if (!s)
		return;
	uint16_t CursorOffset = pObj->CursorPosByte;
	if (CursorOffset >= GUI__strlen(s))
		return;
	char *pCursor = s + CursorOffset;
	int CursorLine = pObj->CursorLine;
	char *pLine = _GetpLine(pObj, CursorLine);
	int NumChars = _WrapGetNumCharsDisp(pObj, pLine);
	char *pEndLine = pLine + GUI_UC__NumChars2NumBytes(pLine, NumChars);
	pLine += _WrapGetNumBytesToNextLine(pObj, pLine);
	int NumBytes = pCursor == pEndLine ?
		pLine - pEndLine : GUI_UC_GetCharSize(pCursor);
	NumChars = GUI_UC__NumBytes2NumChars(pCursor, NumBytes);
	GUI__strcpy(pCursor, pCursor + NumBytes);
	WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
	pObj->NumChars -= NumChars;
	_InvalidateNumLines(pObj);
	_InvalidateTextSizeX(pObj);
	_InvalidateCursorXY(pObj); /* Invalidate X/Y position */
	_ClearCache(pObj);
	pObj->CursorLine = _GetCursorLine(pObj, s, pObj->CursorPosChar);
}
static int _InsertChar(MultiEdit_Obj *pObj, uint16_t Char) {
	if (!_IsCharsAvailable(pObj, 1))
		return 0;
	int BytesNeeded = GUI_UC__CalcSizeOfChar(Char);
	if (!_IsSpaceInBuffer(pObj, BytesNeeded))
		return 0;
	char *pText = pObj->pText;
	int CursorOffset = pObj->CursorPosByte;
	pText += CursorOffset;
	memmove(pText + BytesNeeded, pText, GUI__strlen(pText) + 1);
	GUI_UC_Encode(pText, Char);
	WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
	pObj->NumChars += 1;
	_InvalidateNumLines(pObj);
	_InvalidateTextSizeX(pObj);
	_ClearCache(pObj);
	return 1;
}
static void _MULTIEDIT_Paint(MultiEdit_Obj *pObj) {
	GUI.Font(pObj->pFont);
	auto FontSizeY = GUI_GetFontDistY();
	auto ScrollPosX = pObj->ScrollStateH.v,
		 ScrollPosY = pObj->ScrollStateV.v;
	auto EffectSize = pObj->pEffect->EffectSize,
		 Border = EffectSize + pObj->HBorder;
	auto xOff = Border - ScrollPosX,
		 yOff = EffectSize - ScrollPosY * FontSizeY;
	auto ColorIndex = pObj->Flags & MULTIEDIT_CF_READONLY ? 1 : 0;
	GUI.BkColor(pObj->aBkColor[ColorIndex]);
	GUI.PenColor(pObj->aColor[ColorIndex]);
	GUI_Clear();
	SRect rClip = { { Border, EffectSize }, pObj->Size() - 1 };
	rClip.right_bottom(rClip.right_bottom() - rClip.left_top());
	auto prOldClip = WObj::SetUserClipRect(&rClip);
	if (auto pText = pObj->pText) {
		int Line = 0;
		auto NumVisLines = _GetNumVisLines(pObj);
		SRect r = {
			xOff, EffectSize,
			_GetXSize(pObj) - 1 + Border, pObj->rect.ysize() };
		if (ScrollPosY >= pObj->CacheFirstVisibleLine) {
			if (pObj->CacheFirstVisibleByte) {
				pText += pObj->CacheFirstVisibleByte;
				Line = pObj->CacheFirstVisibleLine;
			}
		}
		do {
			if (pObj->CacheFirstVisibleLine != ScrollPosY) {
				if (Line == ScrollPosY) {
					pObj->CacheFirstVisibleByte = pText - pObj->pText;
					pObj->CacheFirstVisibleLine = ScrollPosY;
				}
			}
			if ((Line >= ScrollPosY) && ((Line - ScrollPosY) <= NumVisLines)) {
				_DispString(pObj, pText, &r);
				r.y0 += FontSizeY;  /* Next line */
			}
			pText += _WrapGetNumBytesToNextLine(pObj, pText);
			Line++;
		} while (GUI_UC_GetCharCode(pText) && ((Line - ScrollPosY) <= NumVisLines));
	}
	if (pObj->Focussed()) {
		int x, y;
		_GetCursorXY(pObj, &x, &y);
		SRect r;
		r.x0 = x + xOff;
		r.y0 = y + yOff;
		r.x1 = r.x0 + _GetCursorSizeX(pObj) - 1;
		r.y1 = r.y0 + FontSizeY - 1;
		GUI_DrawRect(r.x0, r.y0, r.x0 + 4, r.y1);
	}
	WObj::SetUserClipRect(prOldClip);
	WIDGET__EFFECT_DrawDown(pObj);
}
static void _OnTouch(MultiEdit_Obj *pObj, WM_MESSAGE *pMsg) {
	int Notification;
	const PidState *pState = (const PidState *)pMsg->Data;
	if (pState) {
		if (pState->Pressed) {
			int EffectSize = pObj->pEffect->EffectSize;
			int xPos = pState->x + pObj->ScrollStateH.v - EffectSize - pObj->HBorder,
				yPos = pState->y + pObj->ScrollStateV.v * GUI_GetYDistOfFont(pObj->pFont) - EffectSize;
			_SetCursorXY(pObj, xPos, yPos);
			_Invalidate(pObj);
			Notification = WM_NOTIFICATION_CLICKED;
		}
		else
			Notification = WM_NOTIFICATION_RELEASED;
	}
	else
		Notification = WM_NOTIFICATION_MOVED_OUT;
	WM_NotifyParent(pObj, Notification);
}
static int _AddKey(MultiEdit_Obj *pObj, uint16_t Key) {
	int r = 0;
	switch (Key) {
	case GUI_KEY_UP:
		_MoveCursorUp(pObj);
		r = 1;
		break;
	case GUI_KEY_DOWN:
		_MoveCursorDown(pObj);
		r = 1;
		break;
	case GUI_KEY_RIGHT:
		_SetCursorPos(pObj, pObj->CursorPosChar + 1);
		r = 1;
		break;
	case GUI_KEY_LEFT:
		_SetCursorPos(pObj, pObj->CursorPosChar - 1);
		r = 1;
		break;
	case GUI_KEY_END:
		_MoveCursor2LineEnd(pObj);
		r = 1;
		break;
	case GUI_KEY_HOME:
		_MoveCursor2LinePos1(pObj);
		r = 1;
		break;
	case GUI_KEY_BACKSPACE:
		if (!(pObj->Flags & MULTIEDIT_CF_READONLY)) {
			if (pObj->CursorPosChar > pObj->NumCharsPrompt) {
				_SetCursorPos(pObj, pObj->CursorPosChar - 1);
				_DeleteChar(pObj);
			}
			r = 1;
		}
		break;
	case GUI_KEY_DELETE:
		if (!(pObj->Flags & MULTIEDIT_CF_READONLY)) {
			_DeleteChar(pObj);
			r = 1;
		}
		break;
	case GUI_KEY_INSERT:
		if (pObj->Flags & MULTIEDIT_CF_INSERT)
			pObj->Flags &= ~MULTIEDIT_CF_INSERT;
		else
			pObj->Flags |= MULTIEDIT_CF_INSERT;
		r = 1;
		break;
	case GUI_KEY_ENTER:
		if (pObj->Flags & MULTIEDIT_CF_READONLY)
			_MoveCursor2NextLine(pObj);
		else if (_InsertChar(pObj, (uint8_t)('\n'))) {
			if (pObj->Flags & MULTIEDIT_CF_PASSWORD)
				_SetCursorPos(pObj, pObj->CursorPosChar + 1);
			else
				_MoveCursor2NextLine(pObj);
		}
		r = 1;
		break;
	case GUI_KEY_ESCAPE:
		break;
	default:
		if (!(pObj->Flags & MULTIEDIT_CF_READONLY) && (Key >= 0x20)) {
			if (_IsOverwriteAtThisChar(pObj))
				_DeleteChar(pObj);
			if (_InsertChar(pObj, Key))
				_SetCursorPos(pObj, pObj->CursorPosChar + 1);
			r = 1;
		}
	}
	_InvalidateTextArea(pObj);
	return r;
}
static void _MULTIEDIT_Callback(WM_MESSAGE *pMsg) {
	MultiEdit_Obj *pObj = (MultiEdit_Obj *)pMsg->pWin;
	if (WIDGET_HandleActive(pObj, pMsg) == 0)
		return;
	switch (pMsg->MsgId) {
	case WM_NOTIFY_CLIENTCHANGE:
		_InvalidateCursorXY(pObj);
		_InvalidateNumLines(pObj);
		_InvalidateTextSizeX(pObj);
		_ClearCache(pObj);
		_CalcScrollParas(pObj);
		break;
	case WM_SIZE:
		_InvalidateCursorXY(pObj);
		_InvalidateNumLines(pObj);
		_InvalidateTextSizeX(pObj);
		_ClearCache(pObj);
		_Invalidate(pObj);
		break;
	case WM_NOTIFY_PARENT:
		switch (pMsg->Data) {
		case WM_NOTIFICATION_VALUE_CHANGED:
		{
			WM_SCROLL_STATE ScrollState;
			if (pMsg->pWinSrc == (WObj *)WM_GetScrollbarV(pObj)) {
				WM_GetScrollState(pMsg->pWinSrc, &ScrollState);
				pObj->ScrollStateV.v = ScrollState.v;
				pObj->Invalidate();
				WM_NotifyParent(pObj, WM_NOTIFICATION_SCROLL_CHANGED);
			}
			else if (pMsg->pWinSrc == (WObj *)WM_GetScrollbarH(pObj)) {
				WM_GetScrollState(pMsg->pWinSrc, &ScrollState);
				pObj->ScrollStateH.v = ScrollState.v;
				pObj->Invalidate();
				WM_NotifyParent(pObj, WM_NOTIFICATION_SCROLL_CHANGED);
			}
			break;
		}
		case WM_NOTIFICATION_SCROLLBAR_ADDED:
			_SetScrollState(pObj);
			break;
		}
		break;
	case WM_PAINT:
		_MULTIEDIT_Paint(pObj);
		return;
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		break;
	case WM_DELETE:
		GUI_ALLOC_Free(pObj->pText);
		pObj->pText = nullptr;
		break;
	case WM_KEY:
	{
		const WM_KEY_INFO *ki = (const WM_KEY_INFO *)pMsg->Data;
		if (ki->PressedCnt > 0) {
			int Key = ki->Key;
			if (_AddKey(pObj, Key))
				return;
			break;
		}
		if (pObj->Flags & MULTIEDIT_CF_READONLY)
			break;
		return;
	}
	}
	WM_DefaultProc(pMsg);
}
MultiEdit_Obj *MULTIEDIT_CreateEx(
	int x0, int y0, int xsize, int ysize,
	WObj *pParent, int WinFlags, int ExFlags,
	int Id, int BufferSize, const char *pText) {
	if (xsize == 0 && ysize == 0 && x0 == 0 && y0 == 0) {
		SRect rect = pParent->ClientRect();
		xsize = rect.x1 - rect.x0 + 1;
		ysize = rect.y1 - rect.y0 + 1;
	}
	auto pObj = (MultiEdit_Obj *)WObj::Create(
		x0, y0, xsize, ysize, pParent, WinFlags, &_MULTIEDIT_Callback,
		sizeof(MultiEdit_Obj) - sizeof(WObj));
	if (!pObj) {
		return 0;
	}
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE);
	int i;
	for (i = 0; i < NUM_DISP_MODES; i++) {
		pObj->aBkColor[i] = _aDefaultBkColor[i];
		pObj->aColor[i] = _aDefaultColor[i];
	}
	pObj->pFont = _pDefaultFont;
	pObj->Flags = ExFlags;
	pObj->CursorPosChar = 0;
	pObj->CursorPosByte = 0;
	pObj->HBorder = 1;
	pObj->MaxNumChars = 0;
	pObj->NumCharsPrompt = 0;
	pObj->BufferSize = 0;
	pObj->pText = nullptr;
	if (BufferSize > 0) {
		char *pText = (char *)GUI_ALLOC_AllocZero(BufferSize);
		if (!pText) {
			pObj->Delete();
			return 0;
		}
		pObj->BufferSize = BufferSize;
		pObj->pText = pText;
	}
	MULTIEDIT_SetText(pObj, pText);
	_ManageScrollers(pObj);
	return pObj;
}
MultiEdit_Obj *MULTIEDIT_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb) {
	return MULTIEDIT_CreateEx(pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,
							  pCreateInfo->xSize, pCreateInfo->ySize,
							  pParent, 0, pCreateInfo->Flags,
							  pCreateInfo->Id, pCreateInfo->Para, nullptr);
}
int MULTIEDIT_AddKey(MultiEdit_Obj *pObj, uint16_t Key) {
	return _AddKey(pObj, Key);
}
void MULTIEDIT_SetText(MultiEdit_Obj *pObj, const char *pNew) {
	if (!pObj)
		return;
	int NumCharsNew = 0, NumCharsOld = 0;
	int NumBytesNew = 0, NumBytesOld = 0;
	char *pText = pObj->pText;
	if (pText) {
		pText += GUI_UC__NumChars2NumBytes(pText, pObj->NumCharsPrompt);
		NumCharsOld = GUI__GetNumChars(pText);
		NumBytesOld = GUI_UC__NumChars2NumBytes(pText, NumCharsOld);
	}
	if (pNew) {
		NumCharsNew = GUI__GetNumChars(pNew);
		NumBytesNew = GUI_UC__NumChars2NumBytes(pNew, NumCharsNew);
	}
	if (!_IsCharsAvailable(pObj, NumCharsNew - NumCharsOld))
		return;
	if (!_IsSpaceInBuffer(pObj, NumBytesNew - NumBytesOld))
		return;
	pText = pObj->pText;
	pText += GUI_UC__NumChars2NumBytes(pText, pObj->NumCharsPrompt);
	if (pNew)
		GUI__strcpy(pText, pNew);
	else
		*pText = 0;
	_SetCursorPos(pObj, pObj->NumCharsPrompt);
	_InvalidateTextArea(pObj);
	_InvalidateNumChars(pObj);
	_InvalidateNumLines(pObj);
	_InvalidateTextSizeX(pObj);
}
void MULTIEDIT_GetText(MultiEdit_Obj *pObj, char *sDest, int MaxLen) {
	if (!pObj)
		return;
	char *pText = pObj->pText;
	pText += GUI_UC__NumChars2NumBytes(pText, pObj->NumCharsPrompt);
	int Len = GUI__strlen(pText);
	if (Len > (MaxLen - 1))
		Len = MaxLen - 1;
	GUI__memcpy(sDest, pText, Len);
	sDest[Len] = '\0';
}
void MULTIEDIT_GetPrompt(MultiEdit_Obj *pObj, char *sDest, int MaxLen) {
	if (!pObj)
		return;
	char *sSource = pObj->pText;
	int Len = GUI_UC__NumChars2NumBytes(sSource, pObj->NumCharsPrompt);
	if (Len > MaxLen - 1)
		Len = MaxLen - 1;
	GUI__memcpy(sDest, sSource, Len);
	sDest[Len] = '\0';
}
void MULTIEDIT_SetPasswordMode(MultiEdit_Obj *pObj, int OnOff) {
	if (!pObj)
		return;
	_SetFlag(pObj, OnOff, MULTIEDIT_CF_PASSWORD);
	_InvalidateCursorXY(pObj);
	_InvalidateNumLines(pObj);
	_InvalidateTextSizeX(pObj);
}
void MULTIEDIT_SetWrapWord(MultiEdit_Obj *pObj) {
	_SetWrapMode(pObj, GUI_WRAPMODE_WORD);
}
void MULTIEDIT_SetWrapChar(MultiEdit_Obj *pObj) {
	_SetWrapMode(pObj, GUI_WRAPMODE_CHAR);
}
void MULTIEDIT_SetWrapNone(MultiEdit_Obj *pObj) {
	_SetWrapMode(pObj, GUI_WRAPMODE_NONE);
}
void MULTIEDIT_SetInsertMode(MultiEdit_Obj *pObj, int OnOff) {
	_SetFlag(pObj, OnOff, MULTIEDIT_CF_INSERT);
}
void MULTIEDIT_SetReadOnly(MultiEdit_Obj *pObj, int OnOff) {
	_SetFlag(pObj, OnOff, MULTIEDIT_CF_READONLY);
}
void MULTIEDIT_SetAutoScrollV(MultiEdit_Obj *pObj, int OnOff) {
	_SetFlag(pObj, OnOff, MULTIEDIT_CF_AUTOSCROLLBAR_V);
}
void MULTIEDIT_SetAutoScrollH(MultiEdit_Obj *pObj, int OnOff) {
	_SetFlag(pObj, OnOff, MULTIEDIT_CF_AUTOSCROLLBAR_H);
}
void MULTIEDIT_SetHBorder(MultiEdit_Obj *pObj, unsigned HBorder) {
	if (!pObj)
		return;
	if ((unsigned)pObj->HBorder == HBorder)
		return;
	pObj->HBorder = HBorder;
	_Invalidate(pObj);
}
void MULTIEDIT_SetFont(MultiEdit_Obj *pObj, CFont *pFont) {
	if (!pObj)
		return;
	if (pObj->pFont == pFont)
		return;
	pObj->pFont = pFont;
	_InvalidateTextArea(pObj);
	_InvalidateCursorXY(pObj);
	_InvalidateNumLines(pObj);
	_InvalidateTextSizeX(pObj);
}
void MULTIEDIT_SetBkColor(MultiEdit_Obj *pObj, unsigned Index, RGBC color) {
	if (!pObj)
		return;
	if (Index >= NUM_DISP_MODES)
		return;
	pObj->aBkColor[Index] = color;
	_InvalidateTextArea(pObj);
}
void MULTIEDIT_SetCursorOffset(MultiEdit_Obj *pObj, int Offset) {
	if (!pObj)
		return;
	_SetCursorPos(pObj, Offset);
	pObj->Invalidate();
}
void MULTIEDIT_SetTextColor(MultiEdit_Obj *pObj, unsigned Index, RGBC color) {
	if (!pObj)
		return;
	if (Index >= NUM_DISP_MODES)
		return;
	pObj->aColor[Index] = color;
	pObj->Invalidate();
}
void MULTIEDIT_SetPrompt(MultiEdit_Obj *pObj, const char *pPrompt) {
	if (!pObj)
		return;
	int NumCharsNew = 0, NumCharsOld = 0;
	int NumBytesNew = 0, NumBytesOld = 0;
	char *pText = pObj->pText;
	if (pText) {
		NumCharsOld = pObj->NumCharsPrompt;
		NumBytesOld = GUI_UC__NumChars2NumBytes(pText, NumCharsOld);
	}
	if (pPrompt) {
		NumCharsNew = GUI__GetNumChars(pPrompt);
		NumBytesNew = GUI_UC__NumChars2NumBytes(pPrompt, NumCharsNew);
	}
	if (!_IsCharsAvailable(pObj, NumCharsNew - NumCharsOld))
		return;
	if (!_IsSpaceInBuffer(pObj, NumBytesNew - NumBytesOld))
		return;
	memmove(pText + NumBytesNew, pText + NumBytesOld, GUI__strlen(pText + NumBytesOld) + 1);
	if (pPrompt)
		GUI__memcpy(pText, pPrompt, NumBytesNew);
	pObj->NumCharsPrompt = NumCharsNew;
	_SetCursorPos(pObj, NumCharsNew);
	_InvalidateTextArea(pObj);
	_InvalidateNumChars(pObj);
	_InvalidateNumLines(pObj);
	_InvalidateTextSizeX(pObj);
}
void MULTIEDIT_SetBufferSize(MultiEdit_Obj *pObj, int BufferSize) {
	if (!pObj)
		return;
	char *pText = (char *)GUI_ALLOC_AllocZero(BufferSize);
	if (!pText) {
		_InvalidateTextArea(pObj);
		return;
	}
	GUI_ALLOC_Free(pObj->pText);
	pObj->pText = pText;
	pObj->BufferSize = BufferSize;
	pObj->NumCharsPrompt = 0;
	_SetCursorPos(pObj, 0);
	_InvalidateNumChars(pObj);
	_InvalidateCursorXY(pObj);
	_InvalidateNumLines(pObj);
	_InvalidateTextSizeX(pObj);
	_InvalidateTextArea(pObj);
}
void MULTIEDIT_SetMaxNumChars(MultiEdit_Obj *pObj, unsigned MaxNumChars) {
	if (!pObj)
		return;
	pObj->MaxNumChars = MaxNumChars;
	if (MaxNumChars < (unsigned)pObj->NumCharsPrompt)
		pObj->NumCharsPrompt = MaxNumChars;
	char *pText = pObj->pText;
	if (!pText)
		return;
	if (!MaxNumChars)
		return;
	int Offset = GUI_UC__NumChars2NumBytes(pText, MaxNumChars);
	if (Offset >= pObj->BufferSize)
		return;
	pText[Offset] = 0;
	_SetCursorPos(pObj, Offset);
	_InvalidateTextArea(pObj);
	_InvalidateNumChars(pObj);
}
int MULTIEDIT_GetTextSize(MultiEdit_Obj *pObj) {
	if (!pObj)
		return 0;
	const char *s = pObj->pText;
	if (!s)
		return 0;
	s += GUI_UC__NumChars2NumBytes(s, pObj->NumCharsPrompt);
	return 1 + GUI__strlen(s);
}
