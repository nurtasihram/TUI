#include <memory.h>

#include "EDIT.h"
#define EDIT_FONT_DEFAULT       &GUI_Font13_1
#define EDIT_ALIGN_DEFAULT      GUI_TA_LEFT | GUI_TA_VCENTER
#define EDIT_BKCOLOR0_DEFAULT   0xC0C0C0
#define EDIT_BKCOLOR1_DEFAULT   GUI_WHITE
#define EDIT_TEXTCOLOR0_DEFAULT GUI_BLACK
#define EDIT_TEXTCOLOR1_DEFAULT GUI_BLACK
#define EDIT_BORDER_DEFAULT     1
#define EDIT_XOFF               1
#define EDIT_REALLOC_SIZE  16
EDIT_PROPS EDIT__DefaultProps = {
	EDIT_ALIGN_DEFAULT,
	EDIT_BORDER_DEFAULT,
	EDIT_FONT_DEFAULT,
	EDIT_TEXTCOLOR0_DEFAULT,
	EDIT_TEXTCOLOR1_DEFAULT,
	EDIT_BKCOLOR0_DEFAULT,
	EDIT_BKCOLOR1_DEFAULT
};
static void _Paint(Edit_Obj *pObj) {
	int CursorWidth;
	SRect rFillRect, r, rText, rInvert;
	int IsEnabled = pObj->Enable();
	const char *pText = pObj->pText;
	GUI.BkColor(pObj->Props.aBkColor[IsEnabled]);
	GUI.PenColor(pObj->Props.aTextColor[0]);
	GUI.Font(pObj->Props.pFont);
	WM__GetClientRectWin(pObj, &r);
	WIDGET__GetInsideRect(pObj, &rFillRect);
	SRect rInside = rFillRect;
	rInside.x0 += pObj->Props.Border + EDIT_XOFF;
	rInside.x1 -= pObj->Props.Border + EDIT_XOFF;
	GUI__CalcTextRect(pText, &rInside, &rText, pObj->Props.Align);
	if (pObj->State & WIDGET_STATE_FOCUS) {
		const char *p = pText;
		
		CursorWidth = pObj->XSizeCursor > 0 ? pObj->XSizeCursor : 1;
		if (pText) {
			uint16_t Char;
			int i;
			if ((pObj->EditMode != GUI_EDIT_MODE_INSERT) || (pObj->SelSize)) {
				int NumChars = GUI__GetNumChars(pText), CursorOffset;
				if (pObj->CursorPos < NumChars) {
					if (pObj->SelSize) {
						CursorWidth = 0;
						for (i = pObj->CursorPos; i < (int)(pObj->CursorPos + pObj->SelSize); i++) {
							CursorOffset = GUI_UC__NumChars2NumBytes(pText, i);
							Char = GUI_UC_GetCharCode(pText + CursorOffset);
							CursorWidth += GUI_GetCharDistX(Char);
						}
						if (!CursorWidth)
							CursorWidth = 1;
					}
					else {
						CursorOffset = GUI_UC__NumChars2NumBytes(pText, pObj->CursorPos);
						Char = GUI_UC_GetCharCode(pText + CursorOffset);
						CursorWidth = GUI_GetCharDistX(Char);
					}
				}
			}
			rInvert = rText;
			for (i = 0; i != pObj->CursorPos; i++) {
				Char = GUI_UC__GetCharCodeInc(&p);
				rInvert.x0 += GUI_GetCharDistX(Char);
			}
		}
	}
	WM_ITERATE_START() {
		WObj::SetUserClipRect(&rFillRect);
		WIDGET__FillStringInRect(pText, &rFillRect, &rInside, &rText);
		if (pObj->State & WIDGET_STATE_FOCUS) {
			GUI.PenColor(GUI_BLACK);
			GUI_DrawRect(rInvert.x0, rInvert.y0, rInvert.x0 + CursorWidth - 1, rInvert.y1);
		}
		WObj::SetUserClipRect(nullptr);
		WIDGET__EFFECT_DrawDown(pObj);
	} WM_ITERATE_END();
}
static void _Delete(Edit_Obj *pObj) {
	GUI_ALLOC_Free(pObj->pText);
	pObj->pText = nullptr;
}
void EDIT_SetCursorAtPixel(Edit_Obj *pObj, int xPos) {
	if (!pObj)
		return;
	const char *pText = pObj->pText;
	if (!pText)
		return;
	auto pOldFont = GUI.Font();
	GUI.Font(pObj->Props.pFont);
	int xSize = pObj->SizeX();
	int TextWidth = GUI_GetStringDistX(pText);
	int NumChars;
	switch (pObj->Props.Align & GUI_TA_HORIZONTAL) {
	case GUI_TA_HCENTER:
		xPos -= (xSize - TextWidth + 1) / 2;
		break;
	case GUI_TA_RIGHT:
		xPos -= xSize - TextWidth - (pObj->Props.Border + EDIT_XOFF);
		break;
	default:
		xPos -= (pObj->Props.Border + EDIT_XOFF) + pObj->pEffect->EffectSize;
	}
	NumChars = GUI__GetNumChars(pText);
	if (xPos < 0)
		EDIT__SetCursorPos(pObj, 0);
	else if (xPos > TextWidth)
		EDIT__SetCursorPos(pObj, NumChars);
	else {
		int i, x;
		for (i = 0, x = 0; (i < NumChars) && (x < xPos); i++) {
			uint16_t Char = GUI_UC__GetCharCodeInc(&pText);
			int xLenChar = GUI_GetCharDistX(Char);
			if (xPos < (x + xLenChar))
				break;
			x += xLenChar;
		}
		EDIT__SetCursorPos(pObj, i);
	}
	GUI.Font(pOldFont);
	EDIT_Invalidate(pObj);
}
static int _IncrementBuffer(Edit_Obj *pObj, unsigned AddBytes) {
	int NewSize = pObj->BufferSize + AddBytes;
	char *pNew = (char *)GUI_ALLOC_Realloc(pObj->pText, NewSize);
	if (!pNew)
		return 0;
	if (!pObj->pText)
		pNew[0] = '\0';
	pObj->BufferSize = NewSize;
	pObj->pText = pNew;
	return 1;
}
static int _IsSpaceInBuffer(Edit_Obj *pObj, int BytesNeeded) {
	int NumBytes = 0;
	if (pObj->pText)
		NumBytes = (int)GUI__strlen(pObj->pText);
	BytesNeeded = (BytesNeeded + NumBytes + 1) - pObj->BufferSize;
	if (BytesNeeded > 0)
		if (!_IncrementBuffer(pObj, BytesNeeded + EDIT_REALLOC_SIZE))
			return 0;
	return 1;
}
static int _IsCharsAvailable(Edit_Obj *pObj, int CharsNeeded) {
	if (CharsNeeded <= 0 || pObj->MaxLen <= 0)
		return 1;
	int NumChars = 0;
	if (pObj->pText)
		NumChars = GUI__GetNumChars(pObj->pText);
	if (CharsNeeded + NumChars > pObj->MaxLen)
		return 0;
	return 1;
}
static void _DeleteChar(Edit_Obj *pObj) {
	char *pText = pObj->pText;
	if (!pText)
		return;
	int CursorOffset = GUI_UC__NumChars2NumBytes(pText, pObj->CursorPos);
	if (CursorOffset >= GUI__strlen(pText))
		return;
	int NumBytes = GUI_UC_GetCharSize(pText);
	pText += CursorOffset;
	GUI__strcpy(pText, pText + NumBytes);
	WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
}
static int _InsertChar(Edit_Obj *pObj, uint16_t Char) {
	if (!_IsCharsAvailable(pObj, 1))
		return 0;
	int BytesNeeded = GUI_UC__CalcSizeOfChar(Char);
	if (!_IsSpaceInBuffer(pObj, BytesNeeded))
		return 0;
	char *pText = pObj->pText;
	int CursorOffset = GUI_UC__NumChars2NumBytes(pText, pObj->CursorPos);
	pText += CursorOffset;
	memmove(pText + BytesNeeded, pText, GUI__strlen(pText) + 1);
	GUI_UC_Encode(pText, Char);
	WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
	return 1;
}
uint16_t EDIT__GetCurrentChar(Edit_Obj *pObj) {
	const char *pText = pObj->pText;
	if (!pText)
		return 0;
	pText += GUI_UC__NumChars2NumBytes(pText, pObj->CursorPos);
	return GUI_UC_GetCharCode(pText);
}
void EDIT__SetCursorPos(Edit_Obj *pObj, int CursorPos) {
	char *pText = pObj->pText;
	if (!pText)
		return;
	int NumChars = GUI__GetNumChars(pText);
	int Offset = (pObj->EditMode == GUI_EDIT_MODE_INSERT) ? 0 : 1;
	if (CursorPos < 0)
		CursorPos = 0;
	if (CursorPos > NumChars)
		CursorPos = NumChars;
	if (CursorPos > (pObj->MaxLen - Offset))
		CursorPos = pObj->MaxLen - Offset;
	if (pObj->CursorPos != CursorPos)
		pObj->CursorPos = CursorPos;
	pObj->SelSize = 0;
}
static void _OnTouch(Edit_Obj *pObj, WM_MESSAGE *pMsg) {
	const PidState *pState = (const PidState *)pMsg->Data;
	if (!pState) {
		return;
	}
	static int StartPress = 0;
	if (!pState->Pressed) {
		return;
	}
	EDIT_SetCursorAtPixel(pObj, pState->x);
	StartPress = pObj->CursorPos;
}
static void EDIT__Callback(WM_MESSAGE *pMsg) {
	Edit_Obj *pObj = (Edit_Obj *)pMsg->pWin;
	int IsEnabled = pObj->Enable();
	if (WIDGET_HandleActive(pObj, pMsg) == 0)
		return;
	switch (pMsg->MsgId) {
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		break;
	case WM_PAINT:
		_Paint(pObj);
		return;
	case WM_DELETE:
		_Delete(pObj);
		break;
	case WM_KEY:
		if (!IsEnabled)
			break;
		const WM_KEY_INFO *ki = (const WM_KEY_INFO *)pMsg->Data;
		if (ki->PressedCnt <= 0)
			break;
		int Key = ki->Key;
		switch (Key) {
		case GUI_KEY_TAB:
			break;
		default:
			EDIT_AddKey(pObj, Key);
			return;
		}
	}
	WM_DefaultProc(pMsg);
}
void EDIT_AddKey(Edit_Obj *pObj, int Key) {
	if (!pObj)
		return;
	if (pObj->pfAddKeyEx) {
		pObj->pfAddKeyEx(pObj, Key);
		EDIT_Invalidate(pObj);
		return;
	}
	switch (Key) {
	case GUI_KEY_UP:
	{
		char *pText = pObj->pText;
		if (!pText)
			break;
		pText += GUI_UC__NumChars2NumBytes(pText, pObj->CursorPos);
		uint16_t Char = GUI_UC_GetCharCode(pText);
		if (Char < 0x7f) {
			pText[0] = Char + 1;
			WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
		}
		break;
	}
	case GUI_KEY_DOWN:
	{
		char *pText = pObj->pText;
		if (!pText)
			break;
		pText += GUI_UC__NumChars2NumBytes(pText, pObj->CursorPos);
		uint16_t Char = GUI_UC_GetCharCode(pText);
		if (Char > 0x20) {
			pText[0] = Char - 1;
			WM_NotifyParent(pObj, WM_NOTIFICATION_VALUE_CHANGED);
		}
		break;
	}
	case GUI_KEY_RIGHT:
		EDIT__SetCursorPos(pObj, pObj->CursorPos + 1);
		break;
	case GUI_KEY_LEFT:
		EDIT__SetCursorPos(pObj, pObj->CursorPos - 1);
		break;
	case GUI_KEY_BACKSPACE:
		EDIT__SetCursorPos(pObj, pObj->CursorPos - 1);
		_DeleteChar(pObj);
		break;
	case GUI_KEY_DELETE:
		_DeleteChar(pObj);
		break;
	case GUI_KEY_INSERT:
		if (pObj->EditMode == GUI_EDIT_MODE_OVERWRITE)
			pObj->EditMode = GUI_EDIT_MODE_INSERT;
		else {
			pObj->EditMode = GUI_EDIT_MODE_OVERWRITE;
			EDIT__SetCursorPos(pObj, pObj->CursorPos);
		}
		break;
	case GUI_KEY_ENTER:
	case GUI_KEY_ESCAPE:
		break;
	default:
		if (Key < 0x20)
			break;
		if (pObj->EditMode != GUI_EDIT_MODE_INSERT)
			_DeleteChar(pObj);
		if (_InsertChar(pObj, Key))
			EDIT__SetCursorPos(pObj, pObj->CursorPos + 1);
	}
	EDIT_Invalidate(pObj);
}
void EDIT_SetFont(Edit_Obj *pObj, CFont *pfont) {
	if (!pObj)
		return;
	pObj->Props.pFont = pfont;
	EDIT_Invalidate(pObj);
}
void EDIT_SetBkColor(Edit_Obj *pObj, unsigned int Index, RGBC color) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->Props.aBkColor))
		return;
	pObj->Props.aBkColor[Index] = color;
	EDIT_Invalidate(pObj);
}
void EDIT_SetTextColor(Edit_Obj *pObj, unsigned int Index, RGBC color) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->Props.aTextColor))
		return;
	pObj->Props.aTextColor[Index] = color;
	EDIT_Invalidate(pObj);
}
void EDIT_SetText(Edit_Obj *pObj, const char *s) {
	if (!pObj)
		return;
	if (!s) {
		GUI_ALLOC_Free(pObj->pText);
		pObj->pText = nullptr;
		pObj->BufferSize = 0;
		pObj->CursorPos = 0;
		EDIT_Invalidate(pObj);
		return;
	}
	char *pText = pObj->pText;
	int NumBytesOld = 0;
	if (pObj->pText)
		NumBytesOld = (int)GUI__strlen(pText) + 1;
	int NumCharsNew = GUI__GetNumChars(s);
	if (NumCharsNew > pObj->MaxLen)
		NumCharsNew = pObj->MaxLen;
	int NumBytesNew = GUI_UC__NumChars2NumBytes(s, NumCharsNew) + 1;
	if (_IsSpaceInBuffer(pObj, NumBytesNew - NumBytesOld)) {
		GUI__memcpy(pText, s, NumBytesNew);
		pObj->CursorPos = NumBytesNew - 1;
		if (pObj->CursorPos == pObj->MaxLen)
			if (pObj->EditMode == GUI_EDIT_MODE_OVERWRITE)
				pObj->CursorPos--;
	}
	EDIT_Invalidate(pObj);
}
void EDIT_GetText(Edit_Obj *pObj, char *sDest, int MaxLen) {
	if (!pObj || !sDest)
		return;
	*sDest = 0;
	char *pText = pObj->pText;
	if (!pText)
		return;
	int NumChars = GUI__GetNumChars(pText);
	if (NumChars > MaxLen)
		NumChars = MaxLen;
	int NumBytes = GUI_UC__NumChars2NumBytes(pText, NumChars);
	GUI__memcpy(sDest, pText, NumBytes);
	sDest[NumBytes] = '\0';
}
void EDIT_SetMaxLen(Edit_Obj *pObj, int MaxLen) {
	if (!pObj)
		return;
	if (MaxLen == pObj->MaxLen)
		return;
	if (MaxLen < pObj->MaxLen) {
		char *pText = pObj->pText;
		if (pText) {
			int NumChars = GUI__GetNumChars(pText);
			if (NumChars > MaxLen)
				pText[GUI_UC__NumChars2NumBytes(pText, MaxLen)] = 0;
		}
	}
	_IncrementBuffer(pObj, MaxLen - pObj->BufferSize + 1);
	pObj->MaxLen = MaxLen;
	EDIT_Invalidate(pObj);
}
void EDIT_SetTextAlign(Edit_Obj *pObj, int Align) {
	if (!pObj)
		return;
	pObj->Props.Align = Align;
	EDIT_Invalidate(pObj);
}
void EDIT_SetSel(Edit_Obj *pObj, int FirstChar, int LastChar) {
	if (!pObj)
		return;
	if (FirstChar == -1) {
		pObj->SelSize = 0;
		return;
	}
	if (FirstChar > pObj->BufferSize - 1)
		FirstChar = pObj->BufferSize - 1;
	if (LastChar > pObj->BufferSize - 1)
		LastChar = pObj->BufferSize - 1;
	if (LastChar == -1)
		LastChar = EDIT_GetNumChars(pObj);
	if (LastChar >= FirstChar) {
		pObj->CursorPos = FirstChar;
		pObj->SelSize = LastChar - FirstChar + 1;
	}
}
Edit_Obj *EDIT_CreateEx(int x0, int y0,
						  int xsize, int ysize,
						  WObj *pParent, int WinFlags, int ExFlags,
						  int Id, int MaxLen) {
	auto pObj = (Edit_Obj *)WObj::Create(
		x0, y0, xsize, ysize,
		pParent, WC_VISIBLE | WC_LATE_CLIP | WinFlags, EDIT__Callback,
		sizeof(Edit_Obj) - sizeof(WObj));
	if (!pObj)
		return nullptr;
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE);
	pObj->Props = EDIT__DefaultProps;
	pObj->XSizeCursor = 1;
	pObj->MaxLen = (MaxLen == 0) ? 8 : MaxLen;
	pObj->BufferSize = 0;
	pObj->pText = nullptr;
	if (_IncrementBuffer(pObj, pObj->MaxLen + 1) == 0) {
		pObj->Delete();
		return nullptr;
	}
	return pObj;
}
Edit_Obj *EDIT_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo,
								WObj *pParent,
								int x0, int y0, WM_CALLBACK *cb) {
	Edit_Obj *pEdit = EDIT_CreateEx(
		pCreateInfo->x0 + x0, pCreateInfo->y0 + y0,
		pCreateInfo->xSize, pCreateInfo->ySize,
		pParent, 0, pCreateInfo->Flags, pCreateInfo->Id, pCreateInfo->Para);
	if (pEdit)
		EDIT_SetTextAlign(pEdit, pCreateInfo->Flags);
	return pEdit;
}
void EDIT_SetDefaultFont(CFont *pFont) { EDIT__DefaultProps.pFont = pFont; }
CFont *EDIT_GetDefaultFont(void) { return EDIT__DefaultProps.pFont; }
void EDIT_SetDefaultTextAlign(int Align) { EDIT__DefaultProps.Align = Align; }
int EDIT_GetDefaultTextAlign(void) { return EDIT__DefaultProps.Align; }
void EDIT_SetDefaultTextColor(unsigned int Index, RGBC Color) {
	if (Index < GUI_COUNTOF(EDIT__DefaultProps.aTextColor))
		EDIT__DefaultProps.aTextColor[Index] = Color;
}
void EDIT_SetDefaultBkColor(unsigned int Index, RGBC Color) {
	if (Index < GUI_COUNTOF(EDIT__DefaultProps.aBkColor))
		EDIT__DefaultProps.aBkColor[Index] = Color;
}
RGBC EDIT_GetDefaultTextColor(unsigned int Index) {
	return Index < GUI_COUNTOF(EDIT__DefaultProps.aTextColor) ?
		EDIT__DefaultProps.aTextColor[Index] : 0;
}
RGBC EDIT_GetDefaultBkColor(unsigned int Index) {
	return Index < GUI_COUNTOF(EDIT__DefaultProps.aBkColor) ?
		EDIT__DefaultProps.aBkColor[Index] : 0;
}
int EDIT_GetNumChars(Edit_Obj *pObj) {
	if (!pObj)
		return 0;
	char *pText = pObj->pText;
	if (!pText)
		return 0;
	return GUI__GetNumChars(pText);
}
void EDIT_SetCursorAtChar(Edit_Obj *pObj, int Pos) {
	if (!pObj)
		return;
	EDIT__SetCursorPos(pObj, Pos);
	EDIT_Invalidate(pObj);
}
int EDIT_SetInsertMode(Edit_Obj *pObj, int OnOff) {
	if (!pObj)
		return 0;
	int PrevMode = pObj->EditMode;
	pObj->EditMode = OnOff ? GUI_EDIT_MODE_INSERT : GUI_EDIT_MODE_OVERWRITE;
	return PrevMode;
}
void EDIT_SetpfAddKeyEx(Edit_Obj *pObj, tEDIT_AddKeyEx *pfAddKeyEx) {
	if (!pObj)
		return;
	pObj->pfAddKeyEx = pfAddKeyEx;
}
void EDIT_SetpfUpdateBuffer(Edit_Obj *pObj, tEDIT_UpdateBuffer *pfUpdateBuffer) {
	if (!pObj)
		return;
	pObj->pfUpdateBuffer = pfUpdateBuffer;
}
