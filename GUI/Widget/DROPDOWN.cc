
#include "DROPDOWN.h"
#define DROPDOWN_FONT_DEFAULT       &GUI_Font13_1
#define DROPDOWN_BKCOLOR0_DEFAULT   GUI_WHITE     /* Not selected */
#define DROPDOWN_BKCOLOR1_DEFAULT   GUI_GRAY      /* Selected, no focus */
#define DROPDOWN_BKCOLOR2_DEFAULT   GUI_BLUE      /* Selected, focus */
#define DROPDOWN_TEXTCOLOR0_DEFAULT GUI_BLACK     /* Not selected */
#define DROPDOWN_TEXTCOLOR1_DEFAULT GUI_WHITE     /* Selected, no focus */
#define DROPDOWN_TEXTCOLOR2_DEFAULT GUI_WHITE     /* Selected, focus */
#define DROPDOWN_BORDER_DEFAULT     2
#define DROPDOWN_ALIGN_DEFAULT      GUI_TA_LEFT   /* Default text alignment */
DROPDOWN_PROPS DROPDOWN__DefaultProps = {
	DROPDOWN_FONT_DEFAULT,
	DROPDOWN_BKCOLOR0_DEFAULT,
	DROPDOWN_BKCOLOR1_DEFAULT,
	DROPDOWN_BKCOLOR2_DEFAULT,
	DROPDOWN_TEXTCOLOR0_DEFAULT,
	DROPDOWN_TEXTCOLOR1_DEFAULT,
	DROPDOWN_TEXTCOLOR2_DEFAULT,
	DROPDOWN_BORDER_DEFAULT,
	DROPDOWN_ALIGN_DEFAULT
};
static int _GetNumItems(Dropdown_Obj *pObj) {
	return pObj->Handles.NumItems;
}
static void _DrawTriangleDown(int x, int y, int Size) {
	for (; Size >= 0; Size--, y++)
		GUI_DrawHLine(y, x - Size, x + Size);
}
static const char *_GetpItem(Dropdown_Obj *pObj, int Index) {
	return (const char*)GUI_ARRAY_GetItem(&pObj->Handles, Index);
}
static int _Tolower(int Key) {
	return Key >= 0x41 && Key <= 0x5a ?
		Key + 0x20 : Key;
}
static void _SelectByKey(Dropdown_Obj *pObj, int Key) {
	int i;
	Key = _Tolower(Key);
	for (i = 0; i < _GetNumItems(pObj); i++) {
		char c = _Tolower(*_GetpItem(pObj, i));
		if (c == Key) {
			DROPDOWN_SetSel(pObj, i);
			break;
		}
	}
}
static void _FreeAttached(Dropdown_Obj *pObj) {
	GUI_ARRAY_Delete(&pObj->Handles);
	pObj->pListWin->Delete();
}
static void _Paint(Dropdown_Obj *pObj) {
	SRect r;
	const char *s;
	int InnerSize, ColorIndex;
	/* Do some initial calculations */
	int Border = pObj->pEffect->EffectSize;
	int TextBorderSize = pObj->Props.TextBorderSize;
	GUI.Font(pObj->Props.pFont);
	ColorIndex = (pObj->State & WIDGET_STATE_FOCUS) ? 2 : 1;
	s = _GetpItem(pObj, pObj->Sel);
	WM_GetClientRect(&r);
	r /= Border;
	InnerSize = r.y1 - r.y0 + 1;
	/* Draw the 3D effect (if configured) */
	WIDGET__EFFECT_DrawDown(pObj);
	/* Draw the outer text frames */
	r.x1 -= InnerSize;     /* Spare square area to the right */
	GUI.PenColor(pObj->Props.aBackColor[ColorIndex]);
	/* Draw the text */
	GUI.BkColor(pObj->Props.aBackColor[ColorIndex]);
	GUI_FillRectEx(&r);
	r.x0 += TextBorderSize;
	r.x1 -= TextBorderSize;
	GUI.PenColor(pObj->Props.aTextColor[ColorIndex]);
	GUI_DispStringInRect(s, &r, pObj->Props.Align);/**/
	/* Draw arrow */
	WM_GetClientRect(&r);
	r /= Border;
	r.x0 = r.x1 + 1 - InnerSize;
	GUI.PenColor(0xc0c0c0);
	GUI_FillRectEx(&r);
	GUI.PenColor(GUI_BLACK);
	_DrawTriangleDown((r.x1 + r.x0) / 2, r.y0 + 5, (r.y1 - r.y0 - 8) / 2);
	WIDGET__EFFECT_DrawUpRect(pObj, &r);
}
static int _OnTouch(Dropdown_Obj *pObj, WM_MESSAGE *pMsg) {
	const PidState *pState = (const PidState *)pMsg->Data;
	if (pState) {
		if (pState->Pressed)
			WM_NotifyParent(pObj, WM_NOTIFICATION_CLICKED);
		else
			WM_NotifyParent(pObj, WM_NOTIFICATION_RELEASED);
	}
	else
		WM_NotifyParent(pObj, WM_NOTIFICATION_MOVED_OUT);
	return 0;
}
void DROPDOWN__AdjustHeight(Dropdown_Obj *pObj) {
	int Height = pObj->TextHeight;
	if (!Height)
		Height = GUI_GetYDistOfFont(pObj->Props.pFont);
	Height += pObj->pEffect->EffectSize + 2 * pObj->Props.TextBorderSize;
	pObj->SizeY(Height);
}
static void _DROPDOWN_Callback(WM_MESSAGE *pMsg) {
	Dropdown_Obj *pObj = (Dropdown_Obj *)pMsg->pWin;
	char IsExpandedBeforeMsg = pObj->pListWin ? 1 : 0;
	if (!WIDGET_HandleActive(pObj, pMsg))
		return;
	switch (pMsg->MsgId) {
	case WM_NOTIFY_PARENT:
		switch (pMsg->Data) {
		case WM_NOTIFICATION_SCROLL_CHANGED:
			WM_NotifyParent(pObj, WM_NOTIFICATION_SCROLL_CHANGED);
			break;
		case WM_NOTIFICATION_CLICKED:
			DROPDOWN_SetSel(pObj, LISTBOX_GetSel(pObj->pListWin));
			pObj->Focus();
			break;
		case LISTBOX_NOTIFICATION_LOST_FOCUS:
			DROPDOWN_Collapse(pObj);
			break;
		}
		break;
	case WM_PID_STATE_CHANGED:
		if (IsExpandedBeforeMsg == 0) {    /* Make sure we do not react a second time */
			const WM_PID_STATE_CHANGED_INFO *pInfo = (const WM_PID_STATE_CHANGED_INFO *)pMsg->Data;
			if (pInfo->State)
				DROPDOWN_Expand(pObj);
		}
		break;
	case WM_TOUCH:
		if (_OnTouch(pObj, pMsg) == 0)
			return;
		break;
	case WM_PAINT:
		_Paint(pObj);
		break;
	case WM_DELETE:
		_FreeAttached(pObj);
		break;       /* No return here ... WM_DefaultProc needs to be called */
	case WM_KEY:
	{
		const WM_KEY_INFO *ki = (const WM_KEY_INFO *)pMsg->Data;
		if (ki->PressedCnt > 0) {
			int Key = ki->Key;
			switch (Key) {
			case GUI_KEY_TAB:
				break;                    /* Send to parent by not doing anything */
			default:
				DROPDOWN_AddKey(pObj, Key);
				return;
			}
		}
		break;
	}
	}
	WM_DefaultProc(pMsg);
}
Dropdown_Obj *DROPDOWN_CreateEx(int x0, int y0,
								int xsize, int ysize, WObj *pParent,
								int WinFlags, int ExFlags, int Id) {
	auto pObj = (Dropdown_Obj *)WObj::Create(
		x0, y0,
		xsize, -1, pParent, WinFlags, _DROPDOWN_Callback,
		sizeof(Dropdown_Obj) - sizeof(WObj));
	if (!pObj)
		return 0;
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE);
	pObj->Flags = ExFlags;
	pObj->Props = DROPDOWN__DefaultProps;
	pObj->ScrollbarWidth = 0;
	pObj->ySizeEx = ysize;
	DROPDOWN__AdjustHeight(pObj);
	return pObj;
}
void DROPDOWN_Collapse(Dropdown_Obj *pObj) {
	if (!pObj)
		return;
	if (!pObj->pListWin)
		return;
	pObj->pListWin->Delete();
	pObj->pListWin = nullptr;
}
void DROPDOWN_Expand(Dropdown_Obj *pObj) {
	if (!pObj)
		return;
	if (pObj->pListWin)
		return;
	WObj *pParent = pObj->Parent();
	int xSize = pObj->SizeX(),
		ySize = pObj->ySizeEx;
	int NumItems = _GetNumItems(pObj);
	/* Get coordinates of window in client coordiantes of parent */
	auto &&r = pObj->rect - pParent->Rect().left_top();
	if (pObj->Flags & DROPDOWN_CF_UP)
		r.y0 -= ySize;
	else
		r.y0 = r.y1;
	ListBox_Obj *pList = LISTBOX_CreateAsChild(
		nullptr, pObj->Parent(),
		r.x0, r.y0,
		xSize, ySize, WC_VISIBLE);
	if (pObj->Flags & DROPDOWN_CF_AUTOSCROLLBAR) {
		LISTBOX_SetScrollbarWidth(pList, pObj->ScrollbarWidth);
		LISTBOX_SetAutoScrollV(pList, 1);
	}
	int i;
	for (i = 0; i < NumItems; i++)
		LISTBOX_AddString(pList, _GetpItem(pObj, i));
	for (i = 0; i < GUI_COUNTOF(pObj->Props.aBackColor); i++)
		LISTBOX_SetBkColor(pList, i, pObj->Props.aBackColor[i]);
	for (i = 0; i < GUI_COUNTOF(pObj->Props.aTextColor); i++)
		LISTBOX_SetTextColor(pList, i, pObj->Props.aTextColor[i]);
	LISTBOX_SetItemSpacing(pList, pObj->ItemSpacing);
	LISTBOX_SetFont(pList, pObj->Props.pFont);
	pList->Focus();
	pObj->pListWin = pList;
	LISTBOX_SetOwner(pList, pObj);
	LISTBOX_SetSel(pList, pObj->Sel);
	WM_NotifyParent(pObj, WM_NOTIFICATION_CLICKED);
}
void DROPDOWN_AddKey(Dropdown_Obj *pObj, int Key) {
	if (!pObj)
		return;
	switch (Key) {
	case GUI_KEY_DOWN:
		DROPDOWN_IncSel(pObj);
		break;
	case GUI_KEY_UP:
		DROPDOWN_DecSel(pObj);
		break;
	default:
		_SelectByKey(pObj, Key);
		break;
	}
}
void DROPDOWN_AddString(Dropdown_Obj *pObj, const char *s) {
	if (!pObj || !s)
		return;
	GUI_ARRAY_AddItem(&pObj->Handles, s, (int)GUI__strlen(s) + 1);
	DROPDOWN_Invalidate(pObj);
}
int DROPDOWN_GetNumItems(Dropdown_Obj *pObj) {
	if (!pObj)
		return 0;
	return _GetNumItems(pObj);
}
void DROPDOWN_SetFont(Dropdown_Obj *pObj, CFont *pfont) {
	if (!pObj)
		return;
	int OldHeight = GUI_GetYDistOfFont(pObj->Props.pFont);
	pObj->Props.pFont = pfont;
	DROPDOWN__AdjustHeight(pObj);
	DROPDOWN_Invalidate(pObj);
	if (pObj->pListWin) {
		if (OldHeight != GUI_GetYDistOfFont(pObj->Props.pFont)) {
			DROPDOWN_Collapse(pObj);
			DROPDOWN_Expand(pObj);
		}
		LISTBOX_SetFont(pObj->pListWin, pfont);
	}
}
void DROPDOWN_SetBkColor(Dropdown_Obj *pObj, unsigned int Index, RGBC color) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->Props.aBackColor))
		return;
	pObj->Props.aBackColor[Index] = color;
	DROPDOWN_Invalidate(pObj);
	if (pObj->pListWin)
		LISTBOX_SetBkColor(pObj->pListWin, Index, color);
}
void DROPDOWN_SetTextColor(Dropdown_Obj *pObj, unsigned int Index, RGBC color) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->Props.aBackColor))
		return;
	pObj->Props.aTextColor[Index] = color;
	DROPDOWN_Invalidate(pObj);
	if (pObj->pListWin)
		LISTBOX_SetTextColor(pObj->pListWin, Index, color);
}
void DROPDOWN_SetSel(Dropdown_Obj *pObj, int Sel) {
	if (!pObj)
		return;
	int NumItems = _GetNumItems(pObj);
	int MaxSel = NumItems ? NumItems - 1 : 0;
	if (Sel > MaxSel)
		Sel = MaxSel;
	if (Sel == pObj->Sel)
		return;
	pObj->Sel = Sel;
	DROPDOWN_Invalidate(pObj);
	WM_NotifyParent(pObj, WM_NOTIFICATION_SEL_CHANGED);
}
void DROPDOWN_IncSel(Dropdown_Obj *pObj) {
	DROPDOWN_SetSel(pObj, DROPDOWN_GetSel(pObj) + 1);
}
void DROPDOWN_DecSel(Dropdown_Obj *pObj) {
	int Sel = DROPDOWN_GetSel(pObj);
	if (Sel)
		Sel--;
	DROPDOWN_SetSel(pObj, Sel);
}
int DROPDOWN_GetSel(Dropdown_Obj *pObj) {
	if (!pObj)
		return 0;
	return pObj->Sel;
}
void DROPDOWN_SetScrollbarWidth(Dropdown_Obj *pObj, unsigned Width) {
	if (!pObj)
		return;
	if (Width == pObj->ScrollbarWidth)
		return;
	pObj->ScrollbarWidth = Width;
	if (pObj->pListWin)
		LISTBOX_SetScrollbarWidth(pObj->pListWin, Width);
}
void DROPDOWN_SetDefaultFont(CFont *pFont) {
	DROPDOWN__DefaultProps.pFont = pFont;
}
CFont *DROPDOWN_GetDefaultFont(void) {
	return DROPDOWN__DefaultProps.pFont;
}
void DROPDOWN_SetTextHeight(Dropdown_Obj *pObj, unsigned TextHeight) {
	if (!pObj)
		return;
	pObj->TextHeight = TextHeight;
	DROPDOWN__AdjustHeight(pObj);
	pObj->Invalidate();
};
void DROPDOWN_DeleteItem(Dropdown_Obj *pObj, unsigned int Index) {
	if (!pObj)
		return;
	unsigned int NumItems = DROPDOWN_GetNumItems(pObj);
	if (Index >= NumItems)
		return;
	GUI_ARRAY_DeleteItem(&pObj->Handles, Index);
	pObj->Invalidate();
	if (pObj->pListWin)
		LISTBOX_DeleteItem(pObj->pListWin, Index);
};
void DROPDOWN_InsertString(Dropdown_Obj *pObj, const char *s, unsigned int Index) {
	if (!pObj || !s)
		return;
	unsigned int NumItems = DROPDOWN_GetNumItems(pObj);
	if (Index >= NumItems) {
		DROPDOWN_AddString(pObj, s);
		if (pObj->pListWin)
			LISTBOX_AddString(pObj->pListWin, s);
		return;
	}
	char *pBuffer = (char *)GUI_ARRAY_InsertItem(&pObj->Handles, Index, (int)GUI__strlen(s) + 1);
	if (pBuffer)
		GUI__strcpy(pBuffer, s);
	pObj->Invalidate();
	if (pObj->pListWin)
		LISTBOX_InsertString(pObj->pListWin, s, Index);
}
void DROPDOWN_SetItemSpacing(Dropdown_Obj *pObj, unsigned Value) {
	if (!pObj)
		return;
	pObj->ItemSpacing = Value;
	if (pObj->pListWin)
		LISTBOX_SetItemSpacing(pObj->pListWin, Value);
}
unsigned DROPDOWN_GetItemSpacing(Dropdown_Obj *pObj) {
	if (!pObj)
		return 0;
	return pObj->ItemSpacing;
}
void DROPDOWN_SetAutoScroll(Dropdown_Obj *pObj, int OnOff) {
	if (!pObj)
		return;
	char Flags = pObj->Flags & (~DROPDOWN_CF_AUTOSCROLLBAR);
	if (OnOff)
		Flags |= DROPDOWN_CF_AUTOSCROLLBAR;
	if (pObj->Flags == Flags)
		return;
	pObj->Flags = Flags;
	if (pObj->pListWin)
		LISTBOX_SetAutoScrollV(pObj->pListWin, (Flags & DROPDOWN_CF_AUTOSCROLLBAR) ? 1 : 0);
}
void DROPDOWN_SetTextAlign(Dropdown_Obj *pObj, int Align) {
	if (!pObj)
		return;
	pObj->Props.Align = Align;
	pObj->Invalidate();
}
