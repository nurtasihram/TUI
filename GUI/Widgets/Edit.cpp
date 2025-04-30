#include <memory.h>
#include "Edit.h"

#define EDIT_XOFF 1
#define EDIT_REALLOC_SIZE  16

void Edit::_CursorAtPixel(int xPos) {
	if (!text) return;
	auto pOldFont = GUI.Font();
	GUI.Font(Props.pFont);
	auto xSize = SizeX();
	auto TextWidth = GUI.XDist(text);
	switch (Props.Align & ALIGN_HCENTER) {
		case TEXTALIGN_HCENTER:
			xPos -= (xSize - TextWidth + 1) / 2;
			break;
		case TEXTALIGN_RIGHT:
			xPos -= xSize - TextWidth - (Props.Border + EDIT_XOFF);
			break;
		default:
			xPos -= (Props.Border + EDIT_XOFF) + EffectSize();
	}
	auto NumChars = text.Chars();
	if (xPos < 0)
		_CursorPos(0);
	else if (xPos > TextWidth)
		_CursorPos(NumChars);
	else {
		GUI_ISTRING istr = text;
		int i, x;
		for (i = 0, x = 0; i < NumChars && x < xPos; ++i) {
			auto Char = ++istr;
			auto xLenChar = GUI.Font()->XDist(Char);
			if (xPos < x + xLenChar)
				break;
			x += xLenChar;
		}
		_CursorPos(i);
	}
	GUI.Font(pOldFont);
	Invalidate();
}
bool Edit::_IncrementBuffer(uint16_t AddBytes) {
	auto NewSize = BufferSize + AddBytes;
	char *pNew = (char *)GUI_MEM_Realloc(text, NewSize);
	if (!pNew)
		return false;
	if (!text)
		pNew[0] = '\0';
	BufferSize = NewSize;
	text = pNew;
	return true;
}
bool Edit::_IsSpaceInBuffer(uint16_t BytesNeeded) {
	int NumBytes = 0;
	if (text)
		NumBytes = GUI.BytesChars(text);
	BytesNeeded += NumBytes - BufferSize + 1;
	if (BytesNeeded > 0)
		if (!_IncrementBuffer(BytesNeeded + EDIT_REALLOC_SIZE))
			return false;
	return true;
}
bool Edit::_IsCharsAvailable(uint16_t CharsNeeded) {
	if (CharsNeeded <= 0 || LenMax <= 0)
		return true;
	int NumChars = 0;
	if (text)
		NumChars = GUI.NumChars(text);
	if (CharsNeeded + NumChars > LenMax)
		return false;
	return true;
}
void Edit::_DeleteChar() {
	if (!text) return;
	int CursorOffset = GUI_UC__NumChars2NumBytes(text., CursorPos);
	if (CursorOffset >= GUI.BytesChars(pText))
		return;
	int NumBytes = GUI_UC_GetCharSize(pText);
	char *pstrCur = pText + CursorOffset;
	GUI__strcpy(pstrCur, pstrCur + NumBytes);
	NotifyParent(WN_VALUE_CHANGED);
}
int Edit::_InsertChar(uint16_t Char) {
	if (!_IsCharsAvailable(1))
		return 0;
	int BytesNeeded = GUI_UC__CalcSizeOfChar(Char);
	if (!_IsSpaceInBuffer(BytesNeeded))
		return 0;
	auto CursorOffset = GUI_UC__NumChars2NumBytes(pText, CursorPos);
	pText += CursorOffset;
	memmove(pText + BytesNeeded, pText, GUI__strlen(pText) + 1);
	GUI_UC_Encode(pText, Char);
	NotifyParent(WN_VALUE_CHANGED);
	return 1;
}
uint16_t Edit::_CurrentChar() {
	if (!text) return 0;
	return *text(CursorPos);
}
void Edit::_CursorPos(int CursorPos) {
	if (!text) return;
	int NumChars = text.Chars();
	int Offset = bInsert ? 0 : 1;
	if (CursorPos < 0)
		CursorPos = 0;
	if (CursorPos > NumChars)
		CursorPos = NumChars;
	if (CursorPos > LenMax - Offset)
		CursorPos = LenMax - Offset;
	if (this->CursorPos != CursorPos)
		this->CursorPos = CursorPos;
	SelSize = 0;
}

void Edit::_OnPaint() {
	int CursorWidth;
	SRect rText, rInvert;
	int bEnabled = Enable();
	GUI.BkColor(Props.aBkColor[bEnabled]);
	GUI.PenColor(Props.aTextColor[0]);
	GUI.Font(Props.pFont);
	auto &&r = ClientRect();
	auto &&rFill = InsideRect();
	SRect rInside = rFill;
	rInside.x0 += Props.Border + EDIT_XOFF;
	rInside.x1 -= Props.Border + EDIT_XOFF;
//	GUI__CalcTextRect(pText, &rInside, &rText, Props.Align);
	if (Focussed()) {
		const char *p = text;
		CursorWidth = XSizeCursor > 0 ? XSizeCursor : 1;
		if (text) {
			if (!bInsert || SelSize) {
				int NumChars = text.Chars();
				if (CursorPos < NumChars) {
					if (SelSize) {
						CursorWidth = 0;
						for (int i = CursorPos, ed = CursorPos + SelSize; i < ed; ++i)
							CursorWidth += GUI.Font()->XDist(*text(i));
						if (!CursorWidth)
							CursorWidth = 1;
					}
					else
						CursorWidth = GUI.Font()->XDist(*text(CursorPos));
				}
			}
			rInvert = rText;
			for (int i = 0; i != CursorPos; ++i)
				rInvert.x0 += GUI.Font()->XDist(GUI.NextChar(&p));
		}
	}
	WObj::IVR([&] {
		WObj::UserClip(&rFill);
		GUI.DrawStringIn(text, rText);
		//WIDGET__FillStringInRect(pText, rFill, rInside, rText);
		if (Focussed()) {
			GUI.PenColor(RGB_BLACK);
			GUI.Outline({ rInvert.x0, rInvert.y0, rInvert.x0 + CursorWidth - 1, rInvert.y1 });
		}
		WObj::UserClip(nullptr);
		DrawDown();
	});
}

void Edit::_OnTouch(const PID_STATE *pState) {
	if (pState) {
		static int StartPress = 0;
		if (pState->Pressed) {
			CursorAtPixel(pState->x);
			StartPress = CursorPos;
		}
	}
}
WM_RESULT Edit::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (Edit *)pWin;
	auto bEnabled = pObj->Enable();
	if (!pObj->HandleActive(MsgId, Param))
		return;
	switch (MsgId) {
		case WM_PAINT:
			pObj->_OnPaint();
			return 0;
		case WM_MOUSE_KEY:
			pObj->_OnTouch(Param);
			return 0;
		case WM_DELETE:
			pObj->~Edit();
			return 0;
		case WM_KEY:
			if (!bEnabled)
				break;
			if (auto pKi = (const KEY_STATE *)Param) {
				if (pKi->PressedCnt <= 0)
					break;
				switch (int Key = pKi->Key) {
					case GUI_KEY_TAB:
						break;
					default:
						pObj->AddKey(Key);
						return;
				}
			}
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

void Edit::AddKey(int Key) {
	if (pfAddKeyEx) {
		pfAddKeyEx(Key);
		Invalidate();
		return;
	}
	switch (Key) {
	case GUI_KEY_RIGHT:
		_CursorPos(CursorPos + 1);
		break;
	case GUI_KEY_LEFT:
		_CursorPos(CursorPos - 1);
		break;
	case GUI_KEY_BACKSPACE:
		_CursorPos(CursorPos - 1);
		_DeleteChar();
		break;
	case GUI_KEY_DELETE:
		_DeleteChar();
		break;
	case GUI_KEY_INSERT:
		if (!bInsert)
			bInsert = true;
		else {
			bInsert = false;
			_CursorPos(CursorPos);
		}
		break;
	case GUI_KEY_ENTER:
	case GUI_KEY_ESCAPE:
		break;
	default:
		if (Key < 0x20)
			break;
		if (!bInsert)
			_DeleteChar();
		if (_InsertChar(Key))
			_CursorPos(CursorPos + 1);
	}
	Invalidate();
}
void Edit::Text(const char *s) {
	if (!s) {
		text.~GUI_STRING();
		BufferSize = 0;
		CursorPos = 0;
		Invalidate();
		return;
	}
	int NumBytesOld = 0;
	if (text)
		NumBytesOld = text.Bytes() + 1;
	int NumCharsNew = GUI.NumChars(s);
	if (NumCharsNew > LenMax)
		NumCharsNew = LenMax;
	int NumBytesNew = GUI_UC__NumChars2NumBytes(s, NumCharsNew) + 1;
	if (_IsSpaceInBuffer(NumBytesNew - NumBytesOld)) {
		GUI__memcpy(text, s, NumBytesNew);
		CursorPos = NumBytesNew - 1;
		if (CursorPos == LenMax)
			if (!bInsert)
				CursorPos--;
	}
	Invalidate();
}
void Edit::MaxLen(uint16_t Len) {
	if (Len == this->LenMax)
		return;
	if (Len < this->LenMax) {
		if (text) {
			int NumChars = text.Chars();
			if (NumChars > LenMax)
				text(LenMax) = 0;
		}
	}
	_IncrementBuffer(Len - BufferSize + 1);
	LenMax = Len;
	Invalidate();
}
//void EDIT_SetSel(Edit *pObj, int FirstChar, int LastChar) {
//	if (!pObj)
//		return;
//	if (FirstChar == -1) {
//		pObj->SelSize = 0;
//		return;
//	}
//	if (FirstChar > pObj->BufferSize - 1)
//		FirstChar = pObj->BufferSize - 1;
//	if (LastChar > pObj->BufferSize - 1)
//		LastChar = pObj->BufferSize - 1;
//	if (LastChar == -1)
//		LastChar = EDIT_GetNumChars(pObj);
//	if (LastChar >= FirstChar) {
//		pObj->CursorPos = FirstChar;
//		pObj->SelSize = LastChar - FirstChar + 1;
//	}
//}

Edit::Edit(const SRect &rc = {},
		   PWObj pParent = nullptr, uint16_t Id = 0,
		   WM_CF Flags = WC_HIDE, WC_EX FlagsEx = 0,
		   uint16_t MaxLen = 128) : 
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, FlagsEx),
	LenMax(LenMax == 0 ? 8 : LenMax) {
	_IncrementBuffer(LenMax + 1);
}
