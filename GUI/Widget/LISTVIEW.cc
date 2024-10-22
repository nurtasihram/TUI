#include "GUI_ARRAY.h"


#include "LISTVIEW.h"
#define LISTVIEW_FONT_DEFAULT       &GUI_Font13_1
#define LISTVIEW_BKCOLOR0_DEFAULT   GUI_WHITE     /* Not selected */
#define LISTVIEW_BKCOLOR1_DEFAULT   GUI_GRAY      /* Selected, no focus */
#define LISTVIEW_BKCOLOR2_DEFAULT   GUI_BLUE      /* Selected, focus */
#define LISTVIEW_TEXTCOLOR0_DEFAULT GUI_BLACK   /* Not selected */
#define LISTVIEW_TEXTCOLOR1_DEFAULT GUI_WHITE   /* Selected, no focus */
#define LISTVIEW_TEXTCOLOR2_DEFAULT GUI_WHITE   /* Selected, focus */
#define LISTVIEW_GRIDCOLOR_DEFAULT  GUI_LIGHTGRAY
#define LISTVIEW_ALIGN_DEFAULT      (GUI_TA_VCENTER | GUI_TA_HCENTER)
LISTVIEW_PROPS LISTVIEW_DefaultProps = {
	LISTVIEW_BKCOLOR0_DEFAULT,
	LISTVIEW_BKCOLOR1_DEFAULT,
	LISTVIEW_BKCOLOR2_DEFAULT,
	LISTVIEW_TEXTCOLOR0_DEFAULT,
	LISTVIEW_TEXTCOLOR1_DEFAULT,
	LISTVIEW_TEXTCOLOR2_DEFAULT,
	LISTVIEW_GRIDCOLOR_DEFAULT,
	LISTVIEW_FONT_DEFAULT
};
unsigned LISTVIEW__GetRowDistY(const ListView_Obj *pObj) {
	if (pObj->RowDistY)
		return pObj->RowDistY;
	unsigned RowDistY = GUI_GetYDistOfFont(pObj->Props.pFont);
	if (pObj->ShowGrid)
		return RowDistY + 1;
	return RowDistY;
}
static unsigned _GetNumVisibleRows(ListView_Obj *pObj) {
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	unsigned ySize = rect.y1 - rect.y0 + 1 - HEADER_GetHeight(pObj->pHeader),
			 RowDistY = LISTVIEW__GetRowDistY(pObj);
	if (!RowDistY)
		return 1;
	unsigned r = ySize / RowDistY;
	
	return r == 0 ? 1 : r;
}
static void _Paint(ListView_Obj *pObj, WM_MESSAGE *pMsg) {
	int NumColumns = HEADER_GetNumItems(pObj->pHeader),
		NumRows = GUI_ARRAY_GetNumItems(&pObj->RowArray),
		NumVisRows = _GetNumVisibleRows(pObj);
	int RowDistY = LISTVIEW__GetRowDistY(pObj);
	int LBorder = pObj->LBorder,
		RBorder = pObj->RBorder,
		EffectSize = pObj->pEffect->EffectSize;
	
	int yPos = HEADER_GetHeight(pObj->pHeader) + EffectSize;
	int EndRow = pObj->ScrollStateV.v + (((NumVisRows + 1) > NumRows) ? NumRows : NumVisRows + 1);
	SRect ClipRect = *(const SRect *)pMsg->Data - pObj->Rect().left_top();
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	ClipRect &= rect;
	GUI.PenColor(pObj->Props.aTextColor[0]);
	GUI.Font(pObj->Props.pFont);
	GUI_SetTextMode(DRAWMODE_TRANS);
	int i;
	for (i = pObj->ScrollStateV.v; i < EndRow; i++) {
		const GUI_ARRAY *pRow = (const GUI_ARRAY *)GUI_ARRAY_GetItem(&pObj->RowArray, i);
		if (!pRow)
			continue;
		rect.y0 = yPos;
		if (rect.y0 > ClipRect.y1)
			break;
		rect.y1 = yPos + RowDistY - 1;
		if (rect.y1 >= ClipRect.y0) {
			int ColorIndex = i == pObj->Sel ?
				(pObj->State & WIDGET_STATE_FOCUS) ? 2 : 1 : 0;
			
			GUI.BkColor(pObj->Props.aBkColor[ColorIndex]);
			
			if (pObj->ShowGrid)
				rect.y1--;
			int xPos = EffectSize - pObj->ScrollStateH.v;
			int j;
			for (j = 0; j < NumColumns; j++) {
				int Width = HEADER_GetItemWidth(pObj->pHeader, j);
				rect.x0 = xPos;
				if (rect.x0 > ClipRect.x1)
					break;
				rect.x1 = xPos + Width - 1;
				if (rect.x1 >= ClipRect.x0) {
					LISTVIEW_ITEM *pItem = (LISTVIEW_ITEM *)GUI_ARRAY_GetItem(pRow, j);
					LISTVIEW_ITEM_INFO *pItemInfo = pItem->pItemInfo;
					if (pItemInfo) {
						GUI.BkColor(pItemInfo->aBkColor[ColorIndex]);
						GUI.PenColor(pItemInfo->aTextColor[ColorIndex]);
					}
					else
						GUI.PenColor(pObj->Props.aTextColor[ColorIndex]);
					GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
					rect.x0 += LBorder;
					rect.x1 -= RBorder;
					
					int Align = *((int *)GUI_ARRAY_GetItem(&pObj->AlignArray, j));
					GUI_DispStringInRect(pItem->acText, &rect, Align);
					if (pItemInfo)
						GUI.BkColor(pObj->Props.aBkColor[ColorIndex]);
				}
				xPos += Width;
			}
			if (xPos <= ClipRect.x1)
				GUI_ClearRect(xPos, rect.y0, ClipRect.x1, rect.y1);
		}
		yPos += RowDistY;
	}
	if (yPos <= ClipRect.y1) {
		GUI.BkColor(pObj->Props.aBkColor[0]);
		GUI_ClearRect(ClipRect.x0, yPos, ClipRect.x1, ClipRect.y1);
	}
	if (pObj->ShowGrid) {
		GUI.PenColor(pObj->Props.GridColor);
		yPos = HEADER_GetHeight(pObj->pHeader) + EffectSize - 1;
		for (i = 0; i < NumVisRows; i++) {
			yPos += RowDistY;
			if (yPos > ClipRect.y1)
				break;
			if (yPos >= ClipRect.y0)
				GUI_DrawHLine(yPos, ClipRect.x0, ClipRect.x1);
		}
		int xPos = EffectSize - pObj->ScrollStateH.v;
		int i;
		for (i = 0; i < NumColumns; i++) {
			xPos += HEADER_GetItemWidth(pObj->pHeader, i);
			if (xPos > ClipRect.x1)
				break;
			if (xPos >= ClipRect.x0)
				GUI_DrawVLine(xPos, ClipRect.y0, ClipRect.y1);
		}
	}
	
	WIDGET__EFFECT_DrawDown(pObj);
}
void LISTVIEW__InvalidateInsideArea(ListView_Obj *pObj) {
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	rect.y0 += HEADER_GetHeight(pObj->pHeader);
	pObj->Invalidate(&rect);
}
void LISTVIEW__InvalidateRow(ListView_Obj *pObj, int Sel) {
	if (Sel < 0)
		return;
	int HeaderHeight = HEADER_GetHeight(pObj->pHeader);
	int RowDistY = LISTVIEW__GetRowDistY(pObj);
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	rect.y0 += HeaderHeight + (Sel - pObj->ScrollStateV.v) * RowDistY;
	rect.y1 = rect.y0 + RowDistY - 1;
	pObj->Invalidate(&rect);
}
static void _SetSelFromPos(ListView_Obj *pObj, const PidState *pState) {
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	int HeaderHeight = HEADER_GetHeight(pObj->pHeader);
	int x = pState->x - rect.x0,
		y = pState->y - rect.y0 - HeaderHeight;
	rect.x1 -= rect.x0;
	rect.y1 -= rect.y0;
	if (x < 0 || x > rect.x1)
		return;
	if (y < 0 || y > rect.y1 - HeaderHeight)
		return;
	unsigned Sel = (y / LISTVIEW__GetRowDistY(pObj)) + pObj->ScrollStateV.v;
	if (Sel < GUI_ARRAY_GetNumItems(&pObj->RowArray))
		LISTVIEW_SetSel(pObj, Sel);
}
static void _NotifyOwner(ListView_Obj *pObj, int Notification) {
	auto pOwner = pObj->pOwner ? pObj->pOwner : pObj->Parent();
	WM_MESSAGE msg = { 0 };
	msg.MsgId = WM_NOTIFY_PARENT;
	msg.Data = Notification;
	msg.pWin = pObj;
	pOwner->SendMessage(&msg);
}
static void _OnTouch(ListView_Obj *pObj, WM_MESSAGE *pMsg) {
	int Notification;
	const PidState *pState = (const PidState *)pMsg->Data;
	if (pState) {
		if (pState->Pressed) {
			_SetSelFromPos(pObj, pState);
			Notification = WM_NOTIFICATION_CLICKED;
			pObj->Focus();
		}
		else
			Notification = WM_NOTIFICATION_RELEASED;
	}
	else
		Notification = WM_NOTIFICATION_MOVED_OUT;
	_NotifyOwner(pObj, Notification);
}
static int _GetXSize(ListView_Obj *pObj) {
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	return rect.x1 + 1;
}
static int _GetHeaderWidth(ListView_Obj *pObj, Header_Obj *pHeader) {
	int i, NumItems = HEADER_GetNumItems(pHeader), r = 1;
	if (NumItems)
		for (i = 0, r = 0; i < NumItems; i++)
			r += HEADER_GetItemWidth(pHeader, i);
	int w = pObj->ScrollStateH.v + pObj->ScrollStateH.PageSize;
	return w > r ? w : r;
}
int LISTVIEW__UpdateScrollPos(ListView_Obj *pObj) {
	int PrevScrollStateV = pObj->ScrollStateV.v;
	if (pObj->Sel >= 0)
		WM_CheckScrollPos(&pObj->ScrollStateV, pObj->Sel, 0, 0);
	else
		WM_CheckScrollBounds(&pObj->ScrollStateV);
	WM_CheckScrollBounds(&pObj->ScrollStateH);
	WIDGET__SetScrollState(pObj, &pObj->ScrollStateV, &pObj->ScrollStateH);
	return pObj->ScrollStateV.v - PrevScrollStateV;
}
int LISTVIEW__UpdateScrollParas(ListView_Obj *pObj) {
	
	int NumRows = GUI_ARRAY_GetNumItems(&pObj->RowArray);
	pObj->ScrollStateV.PageSize = _GetNumVisibleRows(pObj);
	pObj->ScrollStateV.NumItems = NumRows ? NumRows : 1;
	
	pObj->ScrollStateH.PageSize = _GetXSize(pObj);
	pObj->ScrollStateH.NumItems = _GetHeaderWidth(pObj, pObj->pHeader);
	return LISTVIEW__UpdateScrollPos(pObj);
}
static void _FreeAttached(ListView_Obj *pObj) {
	int i, NumRows = GUI_ARRAY_GetNumItems(&pObj->RowArray);
	for (i = 0; i < NumRows; i++) {
		GUI_ARRAY *pRow = (GUI_ARRAY *)GUI_ARRAY_GetItem(&pObj->RowArray, i);
		int j, NumColumns = GUI_ARRAY_GetNumItems(&pObj->AlignArray);
		for (j = 0; j < NumColumns; j++) {
			LISTVIEW_ITEM *pItem = (LISTVIEW_ITEM *)GUI_ARRAY_GetItem(pRow, j);
			LISTVIEW_ITEM_INFO *pItemInfo = pItem->pItemInfo;
			if (pItemInfo) 
				GUI_ALLOC_Free(pItemInfo);
		}
		GUI_ARRAY_Delete(pRow);
	}
	GUI_ARRAY_Delete(&pObj->AlignArray);
	GUI_ARRAY_Delete(&pObj->RowArray);
}
static int _AddKey(ListView_Obj *pObj, int Key) {
	switch (Key) {
	case GUI_KEY_DOWN:
		LISTVIEW_IncSel(pObj);
		return 1;
	case GUI_KEY_UP:
		LISTVIEW_DecSel(pObj);
		return 1;
	}
	return 0;
}
static void _LISTVIEW_Callback(WM_MESSAGE *pMsg) {
	ListView_Obj *pObj = (ListView_Obj *)pMsg->pWin;
	if (WIDGET_HandleActive(pObj, pMsg) == 0)
		return;
	switch (pMsg->MsgId) {
	case WM_NOTIFY_CLIENTCHANGE:
	case WM_SIZE:
		LISTVIEW__UpdateScrollParas(pObj);
		return;
	case WM_NOTIFY_PARENT:
		switch (pMsg->Data) {
		case WM_NOTIFICATION_CHILD_DELETED:
			if (pMsg->pWinSrc == pObj->pHeader)
				pObj->pHeader = 0;
			break;
		case WM_NOTIFICATION_VALUE_CHANGED:
		{
			WM_SCROLL_STATE ScrollState;
			if (pMsg->pWinSrc == (WObj *)WM_GetScrollbarV(pObj)) {
				WM_GetScrollState(pMsg->pWinSrc, &ScrollState);
				pObj->ScrollStateV.v = ScrollState.v;
				LISTVIEW__InvalidateInsideArea(pObj);
				_NotifyOwner(pObj, WM_NOTIFICATION_SCROLL_CHANGED);
			}
			else if (pMsg->pWinSrc == (WObj *)WM_GetScrollbarH(pObj)) {
				WM_GetScrollState(pMsg->pWinSrc, &ScrollState);
				pObj->ScrollStateH.v = ScrollState.v;
				LISTVIEW__UpdateScrollParas(pObj);
				HEADER_SetScrollPos(pObj->pHeader, pObj->ScrollStateH.v);
				_NotifyOwner(pObj, WM_NOTIFICATION_SCROLL_CHANGED);
			}
			break;
		}
		case WM_NOTIFICATION_SCROLLBAR_ADDED:
			LISTVIEW__UpdateScrollParas(pObj);
			break;
		}
		return;
	case WM_PAINT:
		_Paint(pObj, pMsg);
		return;
	case WM_TOUCH:
		_OnTouch(pObj, pMsg);
		return;
	case WM_KEY:
	{
		const WM_KEY_INFO *ki = (const WM_KEY_INFO *)pMsg->Data;
		if (ki->PressedCnt > 0)
			if (_AddKey(pObj, ki->Key))
				return;
		break;
	}
	case WM_DELETE:
		_FreeAttached(pObj);
		break;
	}
	WM_DefaultProc(pMsg);
}
ListView_Obj *LISTVIEW_CreateEx(
	int x0, int y0, int xsize, int ysize, WObj *pParent,
	int WinFlags, int ExFlags, int Id) {
	if (xsize == 0 && ysize == 0 && x0 == 0 && y0 == 0) {
		auto &&rect = pParent->ClientRect();
		xsize = rect.x1 - rect.x0 + 1;
		ysize = rect.y1 - rect.y0 + 1;
	}
	auto pObj = (ListView_Obj *)WObj::Create(
		x0, y0, xsize, ysize, pParent, WinFlags, &_LISTVIEW_Callback,
		sizeof(ListView_Obj) - sizeof(WObj));
	if (!pObj) {
		return nullptr;
	}
	WIDGET__Init(pObj, Id, WIDGET_STATE_FOCUSSABLE);
	pObj->Props = LISTVIEW_DefaultProps;
	pObj->ShowGrid = 0;
	pObj->RowDistY = 0;
	pObj->Sel = -1;
	pObj->LBorder = 1;
	pObj->RBorder = 1;
	pObj->pHeader = HEADER_CreateEx(0, 0, 0, 0, pObj, WC_VISIBLE, 0, 0);
	LISTVIEW__UpdateScrollParas(pObj);
	return pObj;
}
void LISTVIEW_IncSel(ListView_Obj *pObj) {
	LISTVIEW_SetSel(pObj, LISTVIEW_GetSel(pObj) + 1);
}
void LISTVIEW_DecSel(ListView_Obj *pObj) {
	int Sel = LISTVIEW_GetSel(pObj);
	if (Sel)
		LISTVIEW_SetSel(pObj, Sel - 1);
}
void LISTVIEW_AddColumn(ListView_Obj *pObj, int Width, const char *s, int Align) {
	if (!pObj)
		return;
	HEADER_AddItem(pObj->pHeader, Width, s, Align);   /* Modify header */
	GUI_ARRAY_AddItem(&pObj->AlignArray, &Align, sizeof(int));
	unsigned NumRows = LISTVIEW_GetNumRows(pObj);
	if (!NumRows)
		return;
	unsigned i;
	for (i = 0; i < NumRows; i++) {
		GUI_ARRAY *pRow = (GUI_ARRAY *)GUI_ARRAY_GetItem(&pObj->RowArray, i);
		GUI_ARRAY_AddItem(pRow, nullptr, sizeof(LISTVIEW_ITEM) + 1);
	}
	LISTVIEW__UpdateScrollParas(pObj);
	LISTVIEW__InvalidateInsideArea(pObj);
}
void LISTVIEW_AddRow(ListView_Obj *pObj, const char **ppText) {
	if (!pObj)
		return;
	int NumRows = GUI_ARRAY_GetNumItems(&pObj->RowArray);
	if (GUI_ARRAY_AddItem(&pObj->RowArray, nullptr, sizeof(GUI_ARRAY)))
		return;
	int i, NumColumns = HEADER_GetNumItems(pObj->pHeader);
	for (i = 0; i < NumColumns; i++) {
		GUI_ARRAY *pRow = (GUI_ARRAY *)GUI_ARRAY_GetItem(&pObj->RowArray, NumRows);
		const char *s = ppText ? *ppText++ : nullptr;
		if (s == 0)
			ppText = 0;
		int NumBytes = GUI__strlen(s) + 1;
		GUI_ARRAY_AddItem(pRow, nullptr, sizeof(LISTVIEW_ITEM) + NumBytes);
		LISTVIEW_ITEM *pItem = (LISTVIEW_ITEM *)GUI_ARRAY_GetItem(pRow, i);
		if (NumBytes > 1)
			GUI__strcpy(pItem->acText, s);
	}
	LISTVIEW__UpdateScrollParas(pObj);
	LISTVIEW__InvalidateRow(pObj, NumRows);
}
CFont *LISTVIEW_SetDefaultFont(CFont *pFont) {
	CFont *pOldFont = LISTVIEW_DefaultProps.pFont;
	LISTVIEW_DefaultProps.pFont = pFont;
	return pOldFont;
}
RGBC LISTVIEW_SetDefaultTextColor(unsigned Index, RGBC Color) {
	RGBC OldColor = 0;
	if (Index < GUI_COUNTOF(LISTVIEW_DefaultProps.aTextColor)) {
		OldColor = LISTVIEW_DefaultProps.aTextColor[Index];
		LISTVIEW_DefaultProps.aTextColor[Index] = Color;
	}
	return OldColor;
}
RGBC LISTVIEW_SetDefaultBkColor(unsigned Index, RGBC Color) {
	RGBC OldColor = 0;
	if (Index < GUI_COUNTOF(LISTVIEW_DefaultProps.aBkColor)) {
		OldColor = LISTVIEW_DefaultProps.aBkColor[Index];
		LISTVIEW_DefaultProps.aBkColor[Index] = Color;
	}
	return OldColor;
}
RGBC LISTVIEW_SetDefaultGridColor(RGBC Color) {
	RGBC OldColor = LISTVIEW_DefaultProps.GridColor;
	LISTVIEW_DefaultProps.GridColor = Color;
	return OldColor;
}
void LISTVIEW_DeleteColumn(ListView_Obj *pObj, unsigned Index) {
	if (!pObj)
		return;
	if (Index >= GUI_ARRAY_GetNumItems(&pObj->AlignArray))
		return;
	HEADER_DeleteItem(pObj->pHeader, Index);
	GUI_ARRAY_DeleteItem(&pObj->AlignArray, Index);
	unsigned i, NumRows = GUI_ARRAY_GetNumItems(&pObj->RowArray);
	for (i = 0; i < NumRows; i++) {
		GUI_ARRAY *pRow = (GUI_ARRAY *)GUI_ARRAY_GetItem(&pObj->RowArray, i);
		LISTVIEW_ITEM *pItem = (LISTVIEW_ITEM *)GUI_ARRAY_GetItem(pRow, Index);
		LISTVIEW_ITEM_INFO *pItemInfo = pItem->pItemInfo;
		if (pItemInfo)
			GUI_ALLOC_Free(pItemInfo);
		GUI_ARRAY_DeleteItem(pRow, Index);
	}
	LISTVIEW__UpdateScrollParas(pObj);
	LISTVIEW__InvalidateInsideArea(pObj);
}
RGBC LISTVIEW_GetBkColor(ListView_Obj *pObj, unsigned Index) {
	if (!pObj)
		return GUI_INVALID_COLOR;
	if (Index >= GUI_COUNTOF(pObj->Props.aBkColor))
		return GUI_INVALID_COLOR;
	return pObj->Props.aBkColor[Index];
}
CFont *LISTVIEW_GetFont(ListView_Obj *pObj) {
	if (!pObj)
		return nullptr;
	return pObj->Props.pFont;
}
Header_Obj *LISTVIEW_GetHeader(ListView_Obj *pObj) {
	return pObj ? pObj->pHeader : 0;
}
unsigned LISTVIEW_GetNumColumns(ListView_Obj *pObj) {
	if (!pObj)
		return 0;
	return GUI_ARRAY_GetNumItems(&pObj->AlignArray);
}
unsigned LISTVIEW_GetNumRows(ListView_Obj *pObj) {
	if (!pObj)
		return 0;
	return GUI_ARRAY_GetNumItems(&pObj->RowArray);
}
int LISTVIEW_GetSel(ListView_Obj *pObj) {
	if (!pObj)
		return -1;
	return pObj->Sel;
}
RGBC LISTVIEW_GetTextColor(ListView_Obj *pObj, unsigned Index) {
	if (!pObj)
		return GUI_INVALID_COLOR;
	if (Index >= GUI_COUNTOF(pObj->Props.aTextColor))
		return GUI_INVALID_COLOR;
	return pObj->Props.aTextColor[Index];
}
void LISTVIEW_SetBkColor(ListView_Obj *pObj, unsigned int Index, RGBC Color) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->Props.aBkColor))
		return;
	if (Color == pObj->Props.aBkColor[Index])
		return;
	pObj->Props.aBkColor[Index] = Color;
	LISTVIEW__InvalidateInsideArea(pObj);
}
void LISTVIEW_SetColumnWidth(ListView_Obj *pObj, unsigned int Index, int Width) {
	if (!pObj)
		return;
	HEADER_SetItemWidth(pObj->pHeader, Index, Width);
}
void LISTVIEW_SetFont(ListView_Obj *pObj, CFont *pFont) {
	if (!pObj)
		return;
	if (pFont == pObj->Props.pFont)
		return;
	pObj->Props.pFont = pFont;
	LISTVIEW__UpdateScrollParas(pObj);
	LISTVIEW__InvalidateInsideArea(pObj);
}
int LISTVIEW_SetGridVis(ListView_Obj *pObj, int Show) {
	if (!pObj)
		return 0;
	int ShowGrid = pObj->ShowGrid;
	if (Show == ShowGrid)
		return ShowGrid;
	pObj->ShowGrid = Show;
	LISTVIEW__UpdateScrollParas(pObj);
	LISTVIEW__InvalidateInsideArea(pObj);
	return ShowGrid;
}
void LISTVIEW_SetItemText(ListView_Obj *pObj, unsigned Column, unsigned Row, const char *s) {
	if (!pObj)
		return;
	if (Column >= LISTVIEW_GetNumColumns(pObj) || Row >= LISTVIEW_GetNumRows(pObj))
		return;
	int NumBytes = GUI__strlen(s) + 1;
	LISTVIEW_ITEM *pItem = (LISTVIEW_ITEM *)GUI_ARRAY_ResizeItem((GUI_ARRAY *)GUI_ARRAY_GetItem(&pObj->RowArray, Row), Column, sizeof(LISTVIEW_ITEM) + NumBytes);
	if (NumBytes > 1)
		GUI__strcpy(pItem->acText, s);
	LISTVIEW__InvalidateRow(pObj, Row);
}
void LISTVIEW_SetLBorder(ListView_Obj *pObj, unsigned BorderSize) {
	if (!pObj)
		return;
	if (pObj->LBorder == BorderSize)
		return;
	pObj->LBorder = BorderSize;
	LISTVIEW__InvalidateInsideArea(pObj);
}
void LISTVIEW_SetRBorder(ListView_Obj *pObj, unsigned BorderSize) {
	if (!pObj)
		return;
	if (pObj->RBorder == BorderSize)
		return;
	pObj->RBorder = BorderSize;
	LISTVIEW__InvalidateInsideArea(pObj);
}
unsigned LISTVIEW_SetRowHeight(ListView_Obj *pObj, unsigned RowHeight) {
	if (!pObj)
		return 0;
	unsigned r = pObj->RowDistY;
	if (RowHeight == r)
		return r;
	pObj->RowDistY = RowHeight;
	LISTVIEW__UpdateScrollParas(pObj);
	LISTVIEW__InvalidateInsideArea(pObj);
	return r;
}
void LISTVIEW_SetSel(ListView_Obj *pObj, int NewSel) {
	if (!pObj)
		return;
	int MaxSel = GUI_ARRAY_GetNumItems(&pObj->RowArray) - 1;
	if (NewSel > MaxSel)
		NewSel = MaxSel;
	if (NewSel < 0)
		NewSel = -1;
	if (NewSel == pObj->Sel)
		return;
	int OldSel = pObj->Sel;
	pObj->Sel = NewSel;
	if (LISTVIEW__UpdateScrollPos(pObj))
		LISTVIEW__InvalidateInsideArea(pObj);
	else {
		LISTVIEW__InvalidateRow(pObj, OldSel);
		LISTVIEW__InvalidateRow(pObj, NewSel);
	}
	WM_NotifyParent(pObj, WM_NOTIFICATION_SEL_CHANGED);
}
void LISTVIEW_SetTextAlign(ListView_Obj *pObj, unsigned int Index, int Align) {
	if (Index >= GUI_ARRAY_GetNumItems(&pObj->AlignArray))
		return;
	int *pAlign = (int *)GUI_ARRAY_GetItem(&pObj->AlignArray, Index);
	if (Align == *pAlign)
		return;
	*pAlign = Align;
	LISTVIEW__InvalidateInsideArea(pObj);
}
void LISTVIEW_SetTextColor(ListView_Obj *pObj, unsigned int Index, RGBC Color) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->Props.aTextColor))
		return;
	if (Color == pObj->Props.aTextColor[Index])
		return;
	pObj->Props.aTextColor[Index] = Color;
	LISTVIEW__InvalidateInsideArea(pObj);
}
static LISTVIEW_ITEM_INFO *_GetpItemInfo(ListView_Obj *pObj, unsigned Column, unsigned Row, unsigned int Index) {
	if (!pObj)
		return nullptr;
	LISTVIEW_ITEM_INFO *pItemInfo;
	if (Index >= GUI_COUNTOF(pItemInfo->aTextColor))
		return nullptr;
	if (Column >= LISTVIEW_GetNumColumns(pObj) || Row >= LISTVIEW_GetNumRows(pObj))
		return nullptr;
	LISTVIEW_ITEM *pItem = (LISTVIEW_ITEM *)GUI_ARRAY_GetItem((GUI_ARRAY *)GUI_ARRAY_GetItem(&pObj->RowArray, Row), Column);
	pItemInfo = pItem->pItemInfo;
	if (pItemInfo)
		return pItemInfo;
	pItem->pItemInfo = (LISTVIEW_ITEM_INFO *)GUI_ALLOC_AllocZero(sizeof(LISTVIEW_ITEM_INFO));
	pItemInfo = pItem->pItemInfo;
	int i;
	for (i = 0; i < GUI_COUNTOF(pItemInfo->aTextColor); i++) {
		pItemInfo->aTextColor[i] = LISTVIEW_GetTextColor(pObj, i);
		pItemInfo->aBkColor[i] = LISTVIEW_GetBkColor(pObj, i);
	}
	return pItemInfo;
}
void LISTVIEW_SetItemTextColor(ListView_Obj *pObj, unsigned Column, unsigned Row, unsigned int Index, RGBC Color) {
	LISTVIEW_ITEM_INFO *pItemInfo = _GetpItemInfo(pObj, Column, Row, Index);
	if (pItemInfo)
		pItemInfo->aTextColor[Index] = Color;
}
void LISTVIEW_SetItemBkColor(ListView_Obj *pObj, unsigned Column, unsigned Row, unsigned int Index, RGBC Color) {
	LISTVIEW_ITEM_INFO *pItemInfo = _GetpItemInfo(pObj, Column, Row, Index);
	if (pItemInfo)
		pItemInfo->aBkColor[Index] = Color;
}
static void _InvalidateRowAndBelow(ListView_Obj *pObj, int Sel) {
	if (Sel < 0)
		return;
	SRect rect;
	WM_GetInsideRectExScrollbar(pObj, &rect);
	rect.y0 += HEADER_GetHeight(pObj->pHeader) + (Sel - pObj->ScrollStateV.v) * LISTVIEW__GetRowDistY(pObj);
	pObj->Invalidate(&rect);
}
void LISTVIEW_DeleteRow(ListView_Obj *pObj, unsigned Index) {
	if (!pObj)
		return;
	unsigned NumRows = GUI_ARRAY_GetNumItems(&pObj->RowArray);
	if (Index >= NumRows)
		return;
	GUI_ARRAY *pRow = (GUI_ARRAY *)GUI_ARRAY_GetItem(&pObj->RowArray, Index);
	unsigned i, NumColumns = GUI_ARRAY_GetNumItems(pRow);
	for (i = 0; i < NumColumns; i++) {
		LISTVIEW_ITEM *pItem = (LISTVIEW_ITEM *)GUI_ARRAY_GetItem(pRow, i);
		LISTVIEW_ITEM_INFO *pItemInfo = pItem->pItemInfo;
		if (pItemInfo)
			GUI_ALLOC_Free(pItemInfo);
	}
	GUI_ARRAY_Delete(pRow);
	GUI_ARRAY_DeleteItem(&pObj->RowArray, Index);
	if (pObj->Sel == (signed int)Index)
		pObj->Sel = -1;
	if (pObj->Sel > (signed int)Index)
		pObj->Sel--;
	if (LISTVIEW__UpdateScrollParas(pObj))
		LISTVIEW__InvalidateInsideArea(pObj);
	else
		_InvalidateRowAndBelow(pObj, Index);
}
