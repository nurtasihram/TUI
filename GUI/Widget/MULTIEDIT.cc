#include <string.h>

#include "ScrollBar.h"
#include "MultiEdit.h"

#define MULTIEDIT_FONT_DEFAULT       &GUI_Font13_1
#define MULTIEDIT_BKCOLOR0_DEFAULT   RGB_WHITE
#define MULTIEDIT_BKCOLOR1_DEFAULT   0xC0C0C0
#define MULTIEDIT_TEXTCOLOR0_DEFAULT RGB_BLACK
#define MULTIEDIT_TEXTCOLOR1_DEFAULT RGB_BLACK
#define MULTIEDIT_PASSWORD_CHAR      '*'

#define NUM_DISP_MODES 2

#define INVALID_NUMCHARS (1 << 0)
#define INVALID_NUMLINES (1 << 1)
#define INVALID_TEXTSIZE (1 << 2)
#define INVALID_CURSORXY (1 << 3)
#define INVALID_LINEPOSB (1 << 4)

RGBC MultiEdit::_aDefaultBkColor[2] = {
  MULTIEDIT_BKCOLOR0_DEFAULT,
  MULTIEDIT_BKCOLOR1_DEFAULT,
};
RGBC MultiEdit::_aDefaultColor[2] = {
  MULTIEDIT_TEXTCOLOR0_DEFAULT,
  MULTIEDIT_TEXTCOLOR1_DEFAULT,
};
CFont *MultiEdit::_pDefaultFont = MULTIEDIT_FONT_DEFAULT;

#define MULTIEDIT_REALLOC_SIZE  16

void MultiEdit::_InvalidateNumChars() {
	this->InvalidFlags |= INVALID_NUMCHARS;
}
int MultiEdit::_GetNumChars() {
	if (this->InvalidFlags & INVALID_NUMCHARS) {
		char *pText = this->pText;
		this->NumChars = GUI__GetNumChars(pText);
		this->InvalidFlags &= ~INVALID_NUMCHARS;
	}
	return this->NumChars;
}
int MultiEdit::_GetXSize() {
	return this->InsideRectScrollBar().xsize() - (this->HBorder + 1) * 2;
}
int MultiEdit::_GetNumCharsInPrompt(const char *pText) {
	int r = 0;
	auto pString = this->pText;
	auto pEndPrompt = pString + GUI_UC__NumChars2NumBytes(pString, this->NumCharsPrompt);
	if (pText < pEndPrompt) {
		r = GUI_UC__NumBytes2NumChars(pText, pEndPrompt - pText);
	}
	return r;
}
int MultiEdit::_NumChars2XSize(const char *pText, int NumChars) {
	int xSize = 0;
	while (--NumChars)
		xSize += GUI.Font()->XDist(GUI_UC__GetCharCodeInc(&pText));
	return xSize;
}
int MultiEdit::_WrapGetNumCharsDisp(const char *pText) {
	int xSize, r;
	xSize = _GetXSize();
	if (this->Flags & MULTIEDIT_CF_PASSWORD) {
		int NumCharsPrompt;
		NumCharsPrompt = _GetNumCharsInPrompt(pText);
		r = GUI__WrapGetNumCharsDisp(pText, xSize, this->WrapMode);
		if (r >= NumCharsPrompt) {
			int x;
			switch (this->WrapMode) {
				case GUI_WRAPMODE_NONE:
					r = GUI__GetNumChars(pText);
					break;
				default:
					r = NumCharsPrompt;
					x = _NumChars2XSize(pText, NumCharsPrompt);
					pText += GUI_UC__NumChars2NumBytes(pText, NumCharsPrompt);
					while (GUI_UC__GetCharCodeInc(&pText) != 0) {
						x += GUI.Font()->XDist(MULTIEDIT_PASSWORD_CHAR);
						if (r && (x > xSize)) {
							break;
						}
						r++;
					}
					break;
			}
		}
	}
	else {
		r = GUI__WrapGetNumCharsDisp(pText, xSize, this->WrapMode);
	}
	return r;
}
int MultiEdit::_WrapGetNumBytesToNextLine(const char *pText) {
	int xSize, r;
	xSize = _GetXSize();
	if (this->Flags & MULTIEDIT_CF_PASSWORD) {
		int NumChars, NumCharsPrompt;
		NumCharsPrompt = _GetNumCharsInPrompt(pText);
		NumChars = _WrapGetNumCharsDisp(pText);
		r = GUI_UC__NumChars2NumBytes(pText, NumChars);
		if (NumChars < NumCharsPrompt) {
			if (*(pText + r) == '\n') {
				r++;
			}
		}
	}
	else {
		r = GUI__WrapGetNumBytesToNextLine(pText, xSize, this->WrapMode);
	}
	return r;
}
int MultiEdit::_GetCharDistX(const char *pText) {
	int r;
	if ((this->Flags & MULTIEDIT_CF_PASSWORD) && (_GetNumCharsInPrompt(pText) == 0)) {
		r = GUI.Font()->XDist(MULTIEDIT_PASSWORD_CHAR);
	}
	else {
		uint16_t c;
		c = GUI_UC_GetCharCode(pText);
		r = GUI.Font()->XDist(c);
	}
	return r;
}
void MultiEdit::_DispString(const char *pText, SRect *pRect) {
	int NumCharsDisp;
	NumCharsDisp = _WrapGetNumCharsDisp(pText);
	if (this->Flags & MULTIEDIT_CF_PASSWORD) {
		int x, NumCharsPrompt, NumCharsLeft = 0;
		NumCharsPrompt = _GetNumCharsInPrompt(pText);
		if (NumCharsDisp < NumCharsPrompt) {
			NumCharsPrompt = NumCharsDisp;
		}
		else {
			NumCharsLeft = NumCharsDisp - NumCharsPrompt;
		}
		GUI_DispStringInRect(pText, pRect, TEXTALIGN_LEFT, NumCharsPrompt);
		x = pRect->x0 + _NumChars2XSize(pText, NumCharsPrompt);
		if (NumCharsLeft) {
			GUI_DispCharAt(MULTIEDIT_PASSWORD_CHAR, { x, pRect->y0 });
			GUI_DispChars(MULTIEDIT_PASSWORD_CHAR, NumCharsLeft - 1);
		}
	}
	else {
		GUI_DispStringInRect(pText, pRect, TEXTALIGN_LEFT, NumCharsDisp);
	}
}
char *MultiEdit::_GetpLine(unsigned LineNumber) {
	char *pText, *pLine;
	pText = this->pText;
	if ((unsigned)this->CacheLineNumber != LineNumber) {
		if (LineNumber > (unsigned)this->CacheLineNumber) {
			int OldNumber = this->CacheLineNumber;
			pLine = pText + this->CacheLinePosByte;
			this->CacheLineNumber = LineNumber;
			LineNumber -= OldNumber;
		}
		else {
			pLine = pText;
			this->CacheLineNumber = LineNumber;
		}
		while (LineNumber--) {
			pLine += _WrapGetNumBytesToNextLine(pLine);
		}
		this->CacheLinePosByte = (uint16_t)(pLine - pText);
	}
	return pText + this->CacheLinePosByte;
}
void MultiEdit::_ClearCache() {
	this->CacheLineNumber = 0;
	this->CacheLinePosByte = 0;
	this->CacheFirstVisibleByte = 0;
	this->CacheFirstVisibleLine = 0;
}
int MultiEdit::_GetCursorLine(const char *pText, int CursorPosChar) {
	const char *pCursor;
	const char *pEndLine;
	int NumChars, ByteOffsetNewCursor, LineNumber = 0;
	ByteOffsetNewCursor = GUI_UC__NumChars2NumBytes(pText, CursorPosChar);
	pCursor = pText + ByteOffsetNewCursor;
	if (this->CacheLinePosByte < ByteOffsetNewCursor) {
		pText += this->CacheLinePosByte;
		LineNumber += this->CacheLineNumber;
	}
	while (*pText && (pCursor > pText)) {
		NumChars = _WrapGetNumCharsDisp(pText);
		pEndLine = pText + GUI_UC__NumChars2NumBytes(pText, NumChars);
		pText += _WrapGetNumBytesToNextLine(pText);
		if (pCursor <= pEndLine) {
			if ((pCursor == pEndLine) && (pEndLine == pText) && *pText) {
				LineNumber++;
			}
			break;
		}
		LineNumber++;
	}
	return LineNumber;
}
void MultiEdit::_GetCursorXY(int *px, int *py) {
	if (this->InvalidFlags & INVALID_CURSORXY) {
		int CursorLine = 0, x = 0;
		GUI.Font(this->pFont);
		if (this->pText) {
			const char *pLine;
			const char *pCursor;
			pLine = this->pText;
			pCursor = pLine + this->CursorPosByte;
			CursorLine = this->CursorLine;
			pLine = _GetpLine(CursorLine);
			while (pLine < pCursor) {
				x += _GetCharDistX(pLine);
				pLine += GUI_UC_GetCharSize(pLine);
			}
		}
		this->CursorPosX = x;
		this->CursorPosY = CursorLine * GUI.Font()->YDist;
		this->InvalidFlags &= ~INVALID_CURSORXY;
	}
	*px = this->CursorPosX;
	*py = this->CursorPosY;
}
void MultiEdit::_InvalidateCursorXY() {
	this->InvalidFlags |= INVALID_CURSORXY;
}
void MultiEdit::_SetScrollState() {
	this->ScrollStateH(this->scrollStateH);
	this->ScrollStateV(this->scrollStateV);
}
void MultiEdit::_CalcScrollPos() {
	int xCursor, yCursor;
	_GetCursorXY(&xCursor, &yCursor);
	yCursor /= GUI_GetYDistOfFont(this->pFont);
	this->scrollStateV.Pos(yCursor);
	this->scrollStateH.Pos(xCursor, 30, 30);
	_SetScrollState();
}
int MultiEdit::_GetTextSizeX() {
	if (this->InvalidFlags & INVALID_TEXTSIZE) {
		this->TextSizeX = 0;
		if (this->pText) {
			int NumChars, xSizeLine;
			char *pText, *pLine;
			GUI.Font(this->pFont);
			pText = this->pText;
			do {
				NumChars = _WrapGetNumCharsDisp(pText);
				xSizeLine = 0;
				pLine = pText;
				while (NumChars--) {
					xSizeLine += _GetCharDistX(pLine);
					pLine += GUI_UC_GetCharSize(pLine);
				}
				if (xSizeLine > this->TextSizeX) {
					this->TextSizeX = xSizeLine;
				}
				pText += _WrapGetNumBytesToNextLine(pText);
			} while (*pText);
		}
		this->InvalidFlags &= ~INVALID_TEXTSIZE;
	}
	return this->TextSizeX;
}
int MultiEdit::_GetNumVisLines() {
	auto &&rect = this->InsideRectScrollBar();
	return (rect.y1 - rect.y0 + 1) / GUI_GetYDistOfFont(this->pFont);
}
int MultiEdit::_GetNumLines() {
	if (this->InvalidFlags & INVALID_NUMLINES) {
		int NumLines = 0;
		if (this->pText) {
			int NumChars, NumBytes;
			char *pText;
			uint16_t Char;
			pText = this->pText;
			GUI.Font(this->pFont);
			do {
				NumChars = _WrapGetNumCharsDisp(pText);
				NumBytes = GUI_UC__NumChars2NumBytes(pText, NumChars);
				Char = GUI_UC_GetCharCode(pText + NumBytes);
				if (Char) {
					NumLines++;
				}
				pText += _WrapGetNumBytesToNextLine(pText);
			} while (Char);
		}
		this->NumLines = NumLines + 1;
		this->InvalidFlags &= ~INVALID_NUMLINES;
	}
	return this->NumLines;
}
void MultiEdit::_InvalidateNumLines() {
	this->InvalidFlags |= INVALID_NUMLINES;
}
void MultiEdit::_InvalidateTextSizeX() {
	this->InvalidFlags |= INVALID_TEXTSIZE;
}
void MultiEdit::_CalcScrollParas() {
	this->scrollStateV.NumItems = _GetNumLines();
	this->scrollStateV.PageSize = _GetNumVisLines();
	this->scrollStateH.NumItems = _GetTextSizeX();
	this->scrollStateH.PageSize = _GetXSize();
	_CalcScrollPos();
}
void MultiEdit::_ManageAutoScrollV() {
	if (this->Flags & MULTIEDIT_CF_AUTOSCROLLBAR_V) {
		char IsRequired = _GetNumVisLines() < _GetNumLines();
		this->ScrollBarV(IsRequired);
		if (!IsRequired) {
			_InvalidateNumLines();
			_InvalidateTextSizeX();
			_InvalidateCursorXY();
			_ClearCache();
		}
	}
}
void MultiEdit::_ManageScrollers() {
	_ManageAutoScrollV();
	if (this->Flags & MULTIEDIT_CF_AUTOSCROLLBAR_H) {
		char IsRequired;
		IsRequired = (_GetXSize() < _GetTextSizeX());
		this->ScrollBarH(IsRequired);
		if (!IsRequired) {
			_ManageAutoScrollV();
		}
	}
	_CalcScrollParas();
}
void MultiEdit::_Invalidate() {
	_ManageScrollers();
	this->Invalidate();
}

void MultiEdit::_InvalidateTextArea() {
	_ManageScrollers();
	this->Invalidate(this->InsideRectScrollBar());
}
int MultiEdit::_InvalidateCursorPos() {
	int Value;
	Value = this->CursorPosChar;
	this->CursorPosChar = 0xffff;
	return Value;
}
int MultiEdit::_CalcNextValidCursorPos(int CursorPosChar, int *pCursorPosByte, int *pCursorLine) {
	if (!pText) return 0;
	auto NumChars = _GetNumChars();
	if (CursorPosChar < this->NumCharsPrompt)
		CursorPosChar = this->NumCharsPrompt;
	if (CursorPosChar > NumChars)
		CursorPosChar = NumChars;
	auto CursorPosByte = GUI_UC__NumChars2NumBytes(pText, CursorPosChar);
	auto CursorLine = _GetCursorLine(pText, CursorPosChar);
	auto pCursor = pText + CursorPosByte;
	auto pNextLine = _GetpLine(CursorLine);
	if (pNextLine > pCursor) {
		if (this->CursorPosChar < CursorPosChar)
			pCursor = pNextLine;
		else {
			char *pPrevLine;
			int NumChars;
			pPrevLine = _GetpLine(CursorLine - 1);
			NumChars = _WrapGetNumCharsDisp(pPrevLine);
			pPrevLine += GUI_UC__NumChars2NumBytes(pPrevLine, NumChars);
			pCursor = pPrevLine;
		}
		CursorPosChar = GUI_UC__NumBytes2NumChars(pText, pCursor - pText);
		CursorPosByte = GUI_UC__NumChars2NumBytes(pText, CursorPosChar);
		CursorLine = _GetCursorLine(pText, CursorPosChar);
	}
	if (pCursorPosByte)
		*pCursorPosByte = CursorPosByte;
	if (pCursorLine)
		*pCursorLine = CursorLine;
	return CursorPosChar;
}
void MultiEdit::_SetCursorPos(int CursorPosChar) {
	int CursorPosByte, CursorLine;
	CursorPosChar = _CalcNextValidCursorPos(CursorPosChar, &CursorPosByte, &CursorLine);
	if (this->CursorPosChar != CursorPosChar) {
		this->CursorPosByte = CursorPosByte;
		this->CursorPosChar = CursorPosChar;
		this->CursorLine = CursorLine;
		_InvalidateCursorXY();
		_CalcScrollPos();
	}
}
void MultiEdit::_SetCursorXY(int x, int y) {
	int CursorPosChar = 0;
	if (x < 0 || y < 0)
		return;
	if (pText) {
		GUI.Font(this->pFont);
		auto pLine = _GetpLine(y / GUI.Font()->YDist);
		auto WrapChars = _WrapGetNumCharsDisp(pLine);
		auto Char = GUI_UC__GetCharCode(pLine + GUI_UC__NumChars2NumBytes(pLine, WrapChars));
		if (this->Flags & MULTIEDIT_CF_PASSWORD) {
			if (!Char)
				WrapChars++;
		}
		else if (!Char || (Char == '\n') || ((Char == ' ') && (this->WrapMode == GUI_WRAPMODE_WORD)))
			WrapChars++;
		int SizeX = 0;
		while (--WrapChars > 0) {
			Char = GUI_UC_GetCharCode(pLine);
			SizeX += _GetCharDistX(pLine);
			if (!Char || SizeX > x)
				break;
			pLine += GUI_UC_GetCharSize(pLine);
		}
		CursorPosChar = GUI_UC__NumBytes2NumChars(pText, pLine - pText);
	}
	_SetCursorPos(CursorPosChar);
}
void MultiEdit::_MoveCursorUp() {
	int xPos, yPos;
	_GetCursorXY(&xPos, &yPos);
	yPos -= GUI_GetYDistOfFont(this->pFont);
	_SetCursorXY(xPos, yPos);
}
void MultiEdit::_MoveCursorDown() {
	int xPos, yPos;
	_GetCursorXY(&xPos, &yPos);
	yPos += GUI_GetYDistOfFont(this->pFont);
	_SetCursorXY(xPos, yPos);
}
void MultiEdit::_MoveCursor2NextLine() {
	int xPos, yPos;
	_GetCursorXY(&xPos, &yPos);
	yPos += GUI_GetYDistOfFont(this->pFont);
	_SetCursorXY(0, yPos);
}
void MultiEdit::_MoveCursor2LineEnd() {
	int xPos, yPos;
	_GetCursorXY(&xPos, &yPos);
	_SetCursorXY(0x7FFF, yPos);
}
void MultiEdit::_MoveCursor2LinePos1() {
	int xPos, yPos;
	_GetCursorXY(&xPos, &yPos);
	_SetCursorXY(0, yPos);
}
int MultiEdit::_IsOverwriteAtThisChar() {
	int r = 0;
	if (this->pText && !(this->Flags & MULTIEDIT_CF_INSERT)) {
		const char *pText;
		int CurPos, Line1, Line2;
		uint16_t Char;
		pText = this->pText;
		Line1 = this->CursorLine;
		CurPos = _CalcNextValidCursorPos(this->CursorPosChar + 1, 0, 0);
		Line2 = _GetCursorLine(pText, CurPos);
		pText += this->CursorPosByte;
		Char = GUI_UC_GetCharCode(pText);
		if (Char) {
			if ((Line1 == Line2) || (this->Flags & MULTIEDIT_CF_PASSWORD)) {
				r = 1;
			}
			else {
				if (Char != '\n') {
					if ((Char != ' ') || (this->WrapMode == GUI_WRAPMODE_CHAR)) {
						r = 1;
					}
				}
			}
		}
	}
	return r;
}
int MultiEdit::_GetCursorSizeX() {
	if (_IsOverwriteAtThisChar()) {
		const char *pText;
		pText = this->pText;
		pText += this->CursorPosByte;
		return _GetCharDistX(pText);
	}
	else {
		return 2;
	}
}
int MultiEdit::_IncrementBuffer(unsigned AddBytes) {
	auto NewSize = this->BufferSize + AddBytes;
	if (auto pNew = (char *)GUI_MEM_Realloc(this->pText, NewSize)) {
		if (!this->pText)
			*pNew = 0;
		this->BufferSize = NewSize;
		this->pText = pNew;
		return 1;
	}
	return 0;
}
int MultiEdit::_IsSpaceInBuffer(int BytesNeeded) {
	int NumBytes = GUI__strlen(this->pText);
	BytesNeeded = (BytesNeeded + NumBytes + 1) - this->BufferSize;
	if (BytesNeeded > 0) {
		if (!_IncrementBuffer(BytesNeeded + MULTIEDIT_REALLOC_SIZE)) {
			return 0;
		}
	}
	return 1;
}
int MultiEdit::_IsCharsAvailable(int CharsNeeded) {
	if ((CharsNeeded > 0) && (this->MaxNumChars > 0)) {
		int NumChars = 0;
		if (this->pText) {
			NumChars = _GetNumChars();
		}
		if ((CharsNeeded + NumChars) > this->MaxNumChars) {
			return 0;
		}
	}
	return 1;
}
void MultiEdit::_DeleteChar() {
	if (!this->pText) return;
	if (this->CursorPosByte >= GUI__strlen(this->pText)) return;
	char *pCursor, *pLine, *pEndLine;
	int CursorLine, NumChars;
	pCursor = this->pText + this->CursorPosByte;
	CursorLine = this->CursorLine;
	pLine = _GetpLine(CursorLine);
	NumChars = _WrapGetNumCharsDisp(pLine);
	pEndLine = pLine + GUI_UC__NumChars2NumBytes(pLine, NumChars);
	pLine = pLine + _WrapGetNumBytesToNextLine(pLine);
	auto NumBytes = pCursor == pEndLine ?
		pLine - pEndLine : GUI_UC_GetCharSize(pCursor);
	NumChars = GUI_UC__NumBytes2NumChars(pCursor, NumBytes);
	strcpy(pCursor, pCursor + NumBytes);
	NotifyParent(WM_NOTIFICATION_VALUE_CHANGED);
	this->NumChars -= NumChars;
	_InvalidateNumLines();
	_InvalidateTextSizeX();
	_InvalidateCursorXY();
	_ClearCache();
	this->CursorLine = _GetCursorLine(this->pText, this->CursorPosChar);
}
int MultiEdit::_InsertChar(uint16_t Char) {
	if (_IsCharsAvailable(1)) {
		int BytesNeeded;
		BytesNeeded = GUI_UC__CalcSizeOfChar(Char);
		if (_IsSpaceInBuffer(BytesNeeded)) {
			int CursorOffset;
			char *pText;
			pText = this->pText;
			CursorOffset = this->CursorPosByte;
			pText += CursorOffset;
			memmove(pText + BytesNeeded, pText, strlen(pText) + 1);
			GUI_UC_Encode(pText, Char);
			NotifyParent(WM_NOTIFICATION_VALUE_CHANGED);
			this->NumChars += 1;
			_InvalidateNumLines();
			_InvalidateTextSizeX();
			_ClearCache();
			return 1;
		}
	}
	return 0;
}
void MultiEdit::_OnPaint() {
	int ScrollPosX, ScrollPosY, EffectSize, HBorder;
	int x, y, xOff, yOff, ColorIndex, FontSizeY;
	SRect r, rClip;
	const SRect *prOldClip;
	GUI.Font(this->pFont);
	FontSizeY = GUI.Font()->YDist;
	ScrollPosX = this->scrollStateH.v;
	ScrollPosY = this->scrollStateV.v;
	EffectSize = this->EffectSize();
	HBorder = this->HBorder;
	xOff = EffectSize + HBorder - ScrollPosX;
	yOff = EffectSize - ScrollPosY * FontSizeY;
	ColorIndex = ((this->Flags & MULTIEDIT_CF_READONLY) ? 1 : 0);
	GUI.BkColor(this->aBkColor[ColorIndex]);
	GUI.PenColor(this->aColor[ColorIndex]);
	GUI.Clear();
	rClip.x0 = EffectSize + HBorder;
	rClip.y0 = EffectSize;
	rClip.x1 = this->SizeX() - EffectSize - HBorder - 1;
	rClip.y1 = this->SizeY() - EffectSize - 1;
	prOldClip = WObj::SetUserClipRect(&rClip);
	if (this->pText) {
		const char *pText;
		int Line = 0;
		int xSize = _GetXSize();
		int NumVisLines = _GetNumVisLines();
		pText = this->pText;
		r.x0 = xOff;
		r.y0 = EffectSize;
		r.x1 = xSize + EffectSize + HBorder - 1;
		r.y1 = this->SizeY();
		if (ScrollPosY >= this->CacheFirstVisibleLine) {
			if (this->CacheFirstVisibleByte) {
				pText += this->CacheFirstVisibleByte;
				Line = this->CacheFirstVisibleLine;
			}
		}
		do {
			if (this->CacheFirstVisibleLine != ScrollPosY) {
				if (Line == ScrollPosY) {
					this->CacheFirstVisibleByte = (uint16_t)(pText - this->pText);
					this->CacheFirstVisibleLine = ScrollPosY;
				}
			}
			if ((Line >= ScrollPosY) && ((Line - ScrollPosY) <= NumVisLines)) {
				_DispString(pText, &r);
				r.y0 += FontSizeY;
			}
			pText += _WrapGetNumBytesToNextLine(pText);
			Line++;
		} while (GUI_UC_GetCharCode(pText) && ((Line - ScrollPosY) <= NumVisLines));
	}
	if (this->Focussed()) {
		_GetCursorXY(&x, &y);
		r.x0 = x + xOff;
		r.y0 = y + yOff;
		r.x1 = r.x0 + _GetCursorSizeX() - 1;
		r.y1 = r.y0 + FontSizeY - 1;
		GUI.Outline({ r.x0, r.y0, r.x0 + 4, r.y1 });
	}
	WObj::SetUserClipRect(prOldClip);
	this->DrawDown();
}
void MultiEdit::_OnTouch(WM_MSG *pMsg) {
	int Notification;
	auto pState = (const PidState *)pMsg->data;
	if (pMsg->data) {
		if (pState->Pressed) {
			int Effect, xPos, yPos;
			Effect = this->EffectSize();
			xPos = pState->x + this->scrollStateH.v - Effect - this->HBorder;
			yPos = pState->y + this->scrollStateV.v * GUI_GetYDistOfFont(this->pFont) - Effect;
			_SetCursorXY(xPos, yPos);
			_Invalidate();
			Notification = WM_NOTIFICATION_CLICKED;
		}
		else {
			Notification = WM_NOTIFICATION_RELEASED;
		}
	}
	else {
		Notification = WM_NOTIFICATION_MOVED_OUT;
	}
	NotifyParent(Notification);
}
int MultiEdit::_AddKey(uint16_t Key) {
	int r = 0;
	switch (Key) {
		case GUI_KEY_UP:
			_MoveCursorUp();
			r = 1;
			break;
		case GUI_KEY_DOWN:
			_MoveCursorDown();
			r = 1;
			break;
		case GUI_KEY_RIGHT:
			_SetCursorPos(this->CursorPosChar + 1);
			r = 1;
			break;
		case GUI_KEY_LEFT:
			_SetCursorPos(this->CursorPosChar - 1);
			r = 1;
			break;
		case GUI_KEY_END:
			_MoveCursor2LineEnd();
			r = 1;
			break;
		case GUI_KEY_HOME:
			_MoveCursor2LinePos1();
			r = 1;
			break;
		case GUI_KEY_BACKSPACE:
			if (!(this->Flags & MULTIEDIT_CF_READONLY)) {
				if (this->CursorPosChar > this->NumCharsPrompt) {
					_SetCursorPos(this->CursorPosChar - 1);
					_DeleteChar();
				}
				r = 1;
			}
			break;
		case GUI_KEY_DELETE:
			if (!(this->Flags & MULTIEDIT_CF_READONLY)) {
				_DeleteChar();
				r = 1;
			}
			break;
		case GUI_KEY_INSERT:
			if (!(this->Flags & MULTIEDIT_CF_INSERT)) {
				this->Flags |= MULTIEDIT_CF_INSERT;
			}
			else {
				this->Flags &= ~MULTIEDIT_CF_INSERT;
			}
			r = 1;
			break;
		case GUI_KEY_ENTER:
			if (this->Flags & MULTIEDIT_CF_READONLY)
				_MoveCursor2NextLine();
			else if (_InsertChar((uint8_t)('\n'))) {
				if (this->Flags & MULTIEDIT_CF_PASSWORD)
					_SetCursorPos(this->CursorPosChar + 1);
				else {
					_MoveCursor2NextLine();
				}
			}
			r = 1;
			break;
		case GUI_KEY_ESCAPE:
			break;
		default:
			if (!(this->Flags & MULTIEDIT_CF_READONLY) && (Key >= 0x20)) {
				if (_IsOverwriteAtThisChar()) {
					_DeleteChar();
				}
				if (_InsertChar(Key)) {
					_SetCursorPos(this->CursorPosChar + 1);
				}
				r = 1;
			}
	}
	_InvalidateTextArea();
	return r;
}

void MultiEdit::_Callback(WM_MSG *pMsg) {
	auto pObj = (MultiEdit *)pMsg->pWin;
	if (!pObj->HandleActive(pMsg))
		return;
	switch (pMsg->msgid) {
		case WM_NOTIFY_CLIENTCHANGE:
			pObj->_InvalidateCursorXY();
			pObj->_InvalidateNumLines();
			pObj->_InvalidateTextSizeX();
			pObj->_ClearCache();
			pObj->_CalcScrollParas();
			break;
		case WM_SIZE:
			pObj->_InvalidateCursorXY();
			pObj->_InvalidateNumLines();
			pObj->_InvalidateTextSizeX();
			pObj->_ClearCache();
			pObj->_Invalidate();
			break;
		case WM_NOTIFY_PARENT:
			switch (pMsg->data) {
				case WM_NOTIFICATION_VALUE_CHANGED:
					if (pMsg->pWinSrc == pObj->ScrollBarV()) {
						pObj->scrollStateV.v = ((ScrollBar *)pMsg->pWinSrc)->ScrollState().v;
						pObj->Invalidate();
						pObj->NotifyParent(WM_NOTIFICATION_SCROLL_CHANGED);
					}
					else if (pMsg->pWinSrc == pObj->ScrollBarH()) {
						pObj->scrollStateH.v = ((ScrollBar *)pMsg->pWinSrc)->ScrollState().v;
						pObj->Invalidate();
						pObj->NotifyParent(WM_NOTIFICATION_SCROLL_CHANGED);
					}
					break;
				case WM_NOTIFICATION_SCROLLBAR_ADDED:
					pObj->_SetScrollState();
					break;
			}
			break;
		case WM_PAINT:
			pObj->_OnPaint();
			return;
		case WM_TOUCH:
			pObj->_OnTouch(pMsg);
			break;
		case WM_DELETE:
			GUI_MEM_Free(pObj->pText);
			pObj->pText = nullptr;
			break;
		case WM_KEY:
			if (((const WM_KEY_INFO *)(pMsg->data))->PressedCnt > 0) {
				int Key = ((const WM_KEY_INFO *)(pMsg->data))->Key;
				if (pObj->_AddKey(Key))
					return;
			}
			else if (!(pObj->Flags & MULTIEDIT_CF_READONLY))
				return;
	}
	DefCallback(pMsg);
}

void MultiEdit::_SetFlag(bool bEnable, uint8_t Flag) {
	if (bEnable)
		this->Flags |= Flag;
	else
		this->Flags &= ~Flag;
	_InvalidateTextArea();
}
void MultiEdit::_SetWrapMode(GUI_WRAPMODE WrapMode) {
	if (this->WrapMode != WrapMode) {
		this->WrapMode = WrapMode;
		_ClearCache();
		_InvalidateNumLines();
		_InvalidateTextSizeX();
		_InvalidateTextArea();
		_SetCursorPos(_InvalidateCursorPos());
	}
}

MultiEdit *MultiEdit::Create(
	int x0, int y0, int xsize, int ysize, WObj *pParent, uint16_t Flags, uint16_t ExFlags,
	uint16_t Id, int BufferSize, const char *pText) {
	//if ((xsize == 0) && (ysize == 0) && (x0 == 0) && (y0 == 0)) {
	//	auto &&rect = pParent->ClientRect();
	//	xsize = rect.x1 - rect.x0 + 1;
	//	ysize = rect.y1 - rect.y0 + 1;
	//}
	//auto pObj = (MultiEdit *)WObj::Create(
	//	x0, y0, xsize, ysize, pParent, Flags, &_Callback,
	//	sizeof(MultiEdit) - sizeof(WObj));
	//if (!pObj) return nullptr;
	//pObj->Init(Id, WIDGET_STATE_FOCUSSABLE);
	//for (int i = 0; i < NUM_DISP_MODES; ++i) {
	//	pObj->aBkColor[i] = _aDefaultBkColor[i];
	//	pObj->aColor[i] = _aDefaultColor[i];
	//}
	//pObj->pFont = _pDefaultFont;
	//pObj->Flags = ExFlags;
	//pObj->CursorPosChar = 0;
	//pObj->CursorPosByte = 0;
	//pObj->HBorder = 1;
	//pObj->MaxNumChars = 0;
	//pObj->NumCharsPrompt = 0;
	//pObj->BufferSize = 0;
	//pObj->pText = 0;
	//if (BufferSize > 0) {
	//	auto pText = (char *)GUI_MEM_AllocZero(BufferSize);
	//	if (pText) {
	//		pObj->BufferSize = BufferSize;
	//		pObj->pText = pText;
	//	}
	//	else {
	//		pObj->Delete();
	//		return nullptr;
	//	}
	//}
	//pObj->Text(pText);
	//pObj->_ManageScrollers();
	//return pObj;
}

void MultiEdit::Font(CFont *pFont) {
	if (this->pFont != pFont) {
		this->pFont = pFont;
		_InvalidateTextArea();
		_InvalidateCursorXY();
		_InvalidateNumLines();
		_InvalidateTextSizeX();
	}
}
void MultiEdit::Text(const char *pNew) {
	int NumCharsNew = 0, NumCharsOld = 0;
	int NumBytesNew = 0, NumBytesOld = 0;
	if (pText) {
		auto pText = this->pText + GUI_UC__NumChars2NumBytes(this->pText, NumCharsPrompt);
		NumCharsOld = GUI__GetNumChars(pText);
		NumBytesOld = GUI_UC__NumChars2NumBytes(pText, NumCharsOld);
	}
	if (pNew) {
		NumCharsNew = GUI__GetNumChars(pNew);
		NumBytesNew = GUI_UC__NumChars2NumBytes(pNew, NumCharsNew);
	}
	if (_IsCharsAvailable(NumCharsNew - NumCharsOld)) {
		if (_IsSpaceInBuffer(NumBytesNew - NumBytesOld)) {
			auto pText = this->pText + GUI_UC__NumChars2NumBytes(this->pText, NumCharsPrompt);
			if (pNew)
				strcpy(pText, pNew);
			else
				*pText = 0;
			_SetCursorPos(NumCharsPrompt);
			_InvalidateTextArea();
			_InvalidateNumChars();
			_InvalidateNumLines();
			_InvalidateTextSizeX();
		}
	}
}

void MultiEdit::PasswordMode(bool bEnable) {
	_SetFlag(bEnable, MULTIEDIT_CF_PASSWORD);
	_InvalidateCursorXY();
	_InvalidateNumLines();
	_InvalidateTextSizeX();
}

//void MULTIEDIT_GetPrompt(MultiEdit *pObj, char *sDest, int MaxLen) {
//	if (pObj) {
//		char *sSource = pObj->pText;
//		int Len = GUI_UC__NumChars2NumBytes(sSource, pObj->NumCharsPrompt);
//		if (Len > (MaxLen - 1)) {
//			Len = MaxLen - 1;
//		}
//		memcpy(sDest, sSource, Len);
//		*(sDest + Len) = 0;
//	}
//}
//void MULTIEDIT_SetCursorOffset(MultiEdit *pObj, int Offset) {
//	_SetCursorPos(pObj, Offset);
//	pObj->Invalidate();
//}
//void MULTIEDIT_SetTextColor(MultiEdit *pObj, unsigned Index, RGBC color) {
//	if (Index < NUM_DISP_MODES) {
//		pObj->aColor[Index] = color;
//		pObj->Invalidate();
//	}
//}
//void MULTIEDIT_SetPrompt(MultiEdit *pObj, const char *pPrompt) {
//	int NumCharsNew = 0, NumCharsOld = 0;
//	int NumBytesNew = 0, NumBytesOld = 0;
//	char *pText;
//	if (pObj->pText) {
//		pText = pObj->pText;
//		NumCharsOld = pObj->NumCharsPrompt;
//		NumBytesOld = GUI_UC__NumChars2NumBytes(pText, NumCharsOld);
//	}
//	if (pPrompt) {
//		NumCharsNew = GUI__GetNumChars(pPrompt);
//		NumBytesNew = GUI_UC__NumChars2NumBytes(pPrompt, NumCharsNew);
//	}
//	if (_IsCharsAvailable(pObj, NumCharsNew - NumCharsOld)) {
//		if (_IsSpaceInBuffer(pObj, NumBytesNew - NumBytesOld)) {
//			pText = pObj->pText;
//			memmove(pText + NumBytesNew, pText + NumBytesOld, strlen(pText + NumBytesOld) + 1);
//			if (pPrompt) {
//				memcpy(pText, pPrompt, NumBytesNew);
//			}
//			pObj->NumCharsPrompt = NumCharsNew;
//			_SetCursorPos(pObj, NumCharsNew);
//			_InvalidateTextArea(pObj);
//			_InvalidateNumChars(pObj);
//			_InvalidateNumLines(pObj);
//			_InvalidateTextSizeX(pObj);
//		}
//	}
//}
//void MULTIEDIT_SetBufferSize(MultiEdit *pObj, int BufferSize) {
//	auto pText = (char *)GUI_MEM_AllocZero(BufferSize);
//	GUI_MEM_Free(pObj->pText);
//	pObj->pText = pText;
//	pObj->BufferSize = BufferSize;
//	pObj->NumCharsPrompt = 0;
//	_SetCursorPos(pObj, 0);
//	_InvalidateNumChars(pObj);
//	_InvalidateCursorXY(pObj);
//	_InvalidateNumLines(pObj);
//	_InvalidateTextSizeX(pObj);
//	_InvalidateTextArea(pObj);
//}
//void MULTIEDIT_SetMaxNumChars(MultiEdit *pObj, unsigned MaxNumChars) {
//	pObj->MaxNumChars = MaxNumChars;
//	if (MaxNumChars < (unsigned)pObj->NumCharsPrompt)
//		pObj->NumCharsPrompt = MaxNumChars;
//	if (!pObj->pText || !MaxNumChars) return;
//	auto Offset = GUI_UC__NumChars2NumBytes(pObj->pText, MaxNumChars);
//	if (Offset >= pObj->BufferSize) return;
//	pObj->pText[Offset] = 0;
//	_SetCursorPos(pObj, Offset);
//	_InvalidateTextArea(pObj);
//	_InvalidateNumChars(pObj);
//}
