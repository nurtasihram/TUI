#include <memory.h>
#include "Edit.h"

#define EDIT_XOFF 1
#define EDIT_REALLOC_SIZE  16

void Edit::_CursorAtPixel(int xPos) {
	if (!pText)
		return;
	auto pOldFont = GUI.Font();
	GUI.Font(Props.pFont);
	auto xSize = SizeX();
	auto TextWidth = GUI.XDist(pText);
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
	auto NumChars = GUI__GetNumChars(pText);
	if (xPos < 0)
		_CursorPos(0);
	else if (xPos > TextWidth)
		_CursorPos(NumChars);
	else {
		int i, x;
		for (i = 0, x = 0; i < NumChars && x < xPos; ++i) {
			auto Char = GUI_UC__GetCharCodeInc(&pText);
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
	char *pNew = (char *)GUI_MEM_Realloc(pText, NewSize);
	if (!pNew)
		return false;
	if (!pText)
		pNew[0] = '\0';
	BufferSize = NewSize;
	pText = pNew;
	return true;
}
bool Edit::_IsSpaceInBuffer(uint16_t BytesNeeded) {
	int NumBytes = 0;
	if (pText)
		NumBytes = (int)GUI__strlen(pObj->pText);
	BytesNeeded = (BytesNeeded + NumBytes + 1) - >BufferSize;
	if (BytesNeeded > 0)
		if (!_IncrementBuffer(BytesNeeded + EDIT_REALLOC_SIZE))
			return false;
	return true;
}
bool Edit::_IsCharsAvailable(uint16_t CharsNeeded) {
	if (CharsNeeded <= 0 || LenMax <= 0)
		return true;
	int NumChars = 0;
	if (pText)
		NumChars = GUI__GetNumChars(pText);
	if (CharsNeeded + NumChars > LenMax)
		return false;
	return true;
}
void Edit::_DeleteChar() {
	if (!pText)
		return;
	int CursorOffset = GUI_UC__NumChars2NumBytes(pText, pObj->CursorPos);
	if (CursorOffset >= GUI__strlen(pText))
		return;
	int NumBytes = GUI_UC_GetCharSize(pText);
	char *pstrCur = pText + CursorOffset;
	GUI__strcpy(pstrCur, pstrCur + NumBytes);
	pObj->NotifyParent(WN_VALUE_CHANGED);
}
int Edit::_InsertChar(uint16_t Char) {
	if (!_IsCharsAvailable(1))
		return 0;
	int BytesNeeded = GUI_UC__CalcSizeOfChar(Char);
	if (!_IsSpaceInBuffer(BytesNeeded))
		return 0;
	char *pText = pObj->pText;
	int CursorOffset = GUI_UC__NumChars2NumBytes(pText, pObj->CursorPos);
	pText += CursorOffset;
	memmove(pText + BytesNeeded, pText, GUI__strlen(pText) + 1);
	GUI_UC_Encode(pText, Char);
	NotifyParent(WN_VALUE_CHANGED);
	return 1;
}
uint16_t Edit::_CurrentChar() {
	if (!pText)
		return 0;
	pText += GUI_UC__NumChars2NumBytes(pText, pObj->CursorPos);
	return GUI_UC_GetCharCode(pText);
}
void Edit::_CursorPos(int CursorPos) {
	if (!pText)
		return;
	int NumChars = GUI__GetNumChars(pText);
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
	GUI__CalcTextRect(pText, &rInside, &rText, Props.Align);
	if (Focussed()) {
		const char *p = pText;
		CursorWidth = XSizeCursor > 0 ? XSizeCursor : 1;
		if (pText) {
			uint16_t Char;
			int i;
			if (!bInsert || SelSize) {
				int NumChars = GUI__GetNumChars(pText), CursorOffset;
				if (CursorPos < NumChars) {
					if (SelSize) {
						CursorWidth = 0;
						for (i = CursorPos; i < (int)(CursorPos + SelSize); ++i) {
							CursorOffset = GUI_UC__NumChars2NumBytes(pText, i);
							Char = GUI_UC_GetCharCode(pText + CursorOffset);
							CursorWidth += GUI.Font()->XDist(Char);
						}
						if (!CursorWidth)
							CursorWidth = 1;
					}
					else {
						CursorOffset = GUI_UC__NumChars2NumBytes(pText, CursorPos);
						Char = GUI_UC_GetCharCode(pText + CursorOffset);
						CursorWidth = GUI.Font()->XDist(Char);
					}
				}
			}
			rInvert = rText;
			for (i = 0; i != CursorPos; ++i) {
				Char = GUI_UC__GetCharCodeInc(&p);
				rInvert.x0 += GUI.Font()->XDist(Char);
			}
		}
	}
	WObj::IVR([&] {
		WObj::UserClip(&rFill);
		WIDGET__FillStringInRect(pText, rFill, rInside, rText);
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
		case WM_TOUCH:
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
		pText.~GUI_STRING();
		BufferSize = 0;
		CursorPos = 0;
		Invalidate();
		return;
	}
	int NumBytesOld = 0;
	if (pText)
		NumBytesOld = (int)GUI__strlen(pText) + 1;
	int NumCharsNew = GUI__GetNumChars(s);
	if (NumCharsNew > LenMax)
		NumCharsNew = LenMax;
	int NumBytesNew = GUI_UC__NumChars2NumBytes(s, NumCharsNew) + 1;
	if (_IsSpaceInBuffer(NumBytesNew - NumBytesOld)) {
		GUI__memcpy(pText, s, NumBytesNew);
		CursorPos = NumBytesNew - 1;
		if (CursorPos == LenMax)
			if (!EditMode == GUI_EDIT_MODE_OVERWRITE)
				CursorPos--;
	}
	Invalidate();
}
void Edit::MaxLen(uint16_t Len) {
	if (Len == this->LenMax)
		return;
	if (Len < this->LenMax) {
		if (pText) {
			int NumChars = GUI__GetNumChars(pText);
			if (NumChars > LenMax)
				pText[GUI_UC__NumChars2NumBytes(pText, LenMax)] = 0;
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
