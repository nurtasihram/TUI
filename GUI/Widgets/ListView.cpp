#include "ScrollBar.h"
#include "ListView.h"

#define LISTVIEW_ALIGN_DEFAULT (TEXTALIGN_VCENTER | TEXTALIGN_HCENTER)

ListView::Property ListView::DefaultProps;

unsigned LISTVIEW__GetRowDistY(const ListView *pObj) {
	if (pObj->RowDistY)
		return pObj->RowDistY;
	unsigned RowDistY = GUI_GetYDistOfFont(pObj->Props.pFont);
	if (pObj->ShowGrid)
		return RowDistY + 1;
	return RowDistY;
}
static unsigned _GetNumVisibleRows(ListView *pObj) {
	auto RowDistY = LISTVIEW__GetRowDistY(pObj);
	if (!RowDistY)
		return 1;
	auto ySize = pObj->InsideRectScrollBar().ysize() - HEADER_GetHeight(pObj->pHeader);
	unsigned r = ySize / RowDistY;
	return r == 0 ? 1 : r;
}
static void _OnPaint(ListView *pObj, WM_PARAM *pData) {
	int NumColumns = HEADER_GetNumItems(pObj->pHeader),
		NumRows = pObj->RowArray.NumItems(),
		NumVisRows = _GetNumVisibleRows(pObj);
	int RowDistY = LISTVIEW__GetRowDistY(pObj);
	int LBorder = pObj->LBorder,
		RBorder = pObj->RBorder,
		EffectSize = pObj->EffectSize();
	int yPos = HEADER_GetHeight(pObj->pHeader) + EffectSize;
	int EndRow = pObj->scrollStateV.v + (NumVisRows + 1 > NumRows ? NumRows : NumVisRows + 1);
	SRect rClip = *(const SRect *)*pData - pObj->Rect().left_top();
	auto &&rect = pObj->InsideRectScrollBar();
	rClip &= rect;
	GUI.PenColor(pObj->Props.aTextColor[0]);
	GUI.Font(pObj->Props.pFont);
	for (auto i = pObj->scrollStateV.v; i < EndRow; ++i) {
		auto &row = pObj->RowArray[i];
		rect.y0 = yPos;
		if (rect.y0 > rClip.y1)
			break;
		rect.y1 = yPos + RowDistY - 1;
		if (rect.y1 >= rClip.y0) {
			int ColorIndex = i == pObj->Sel ? pObj->Focussed() ? 2 : 1 : 0;
			GUI.BkColor(pObj->Props.aBkColor[ColorIndex]);
			if (pObj->ShowGrid)
				rect.y1--;
			int xPos = EffectSize - pObj->scrollStateH.v;
			for (int j = 0; j < NumColumns; ++j) {
				int Width = HEADER_GetItemWidth(pObj->pHeader, j);
				rect.x0 = xPos;
				if (rect.x0 > rClip.x1)
					break;
				rect.x1 = xPos + Width - 1;
				if (rect.x1 >= rClip.x0) {
					auto &item = row[j];
					auto pItemInfo = item.pItemInfo;
					if (pItemInfo) {
						GUI.BkColor(pItemInfo->aBkColor[ColorIndex]);
						GUI.PenColor(pItemInfo->aTextColor[ColorIndex]);
					}
					else
						GUI.PenColor(pObj->Props.aTextColor[ColorIndex]);
					GUI.Clear(rect);
					rect.x0 += LBorder;
					rect.x1 -= RBorder;
					auto Align = pObj->AlignArray[j];
					GUI_DispStringInRect(item.pText, &rect, Align);
					if (pItemInfo)
						GUI.BkColor(pObj->Props.aBkColor[ColorIndex]);
				}
				xPos += Width;
			}
			if (xPos <= rClip.x1)
				GUI.Clear({ xPos, rect.y0, rClip.x1, rect.y1 });
		}
		yPos += RowDistY;
	}
	if (yPos <= rClip.y1) {
		GUI.BkColor(pObj->Props.aBkColor[0]);
		GUI.Clear({ rClip.x0, yPos, rClip.x1, rClip.y1 });
	}
	if (pObj->ShowGrid) {
		GUI.PenColor(pObj->Props.GridColor);
		yPos = HEADER_GetHeight(pObj->pHeader) + EffectSize - 1;
		for (int i = 0; i < NumVisRows; ++i) {
			yPos += RowDistY;
			if (yPos > rClip.y1)
				break;
			if (yPos >= rClip.y0)
				GUI.DrawLineH(yPos, rClip.x0, rClip.x1);
		}
		int xPos = EffectSize - pObj->scrollStateH.v;
		for (int i = 0; i < NumColumns; ++i) {
			xPos += HEADER_GetItemWidth(pObj->pHeader, i);
			if (xPos > rClip.x1)
				break;
			if (xPos >= rClip.x0)
				GUI.DrawLineV(xPos, rClip.y0, rClip.y1);
		}
	}
	pObj->DrawDown();
}
void LISTVIEW__InvalidateInsideArea(ListView *pObj) {
	auto &&rect = pObj->InsideRectScrollBar();
	rect.y0 += HEADER_GetHeight(pObj->pHeader);
	pObj->Invalidate(rect);
}
void LISTVIEW__InvalidateRow(ListView *pObj, int Sel) {
	if (Sel < 0)
		return;
	int HeaderHeight = HEADER_GetHeight(pObj->pHeader);
	int RowDistY = LISTVIEW__GetRowDistY(pObj);
	auto &&rect = pObj->InsideRectScrollBar();
	rect.y0 += HeaderHeight + (Sel - pObj->scrollStateV.v) * RowDistY;
	rect.y1 = rect.y0 + RowDistY - 1;
	pObj->Invalidate(rect);
}
static void _SetSelFromPos(ListView *pObj, const PID_STATE *pState) {
	auto &&rect = pObj->InsideRectScrollBar();
	int HeaderHeight = HEADER_GetHeight(pObj->pHeader);
	int x = pState->x - rect.x0,
		y = pState->y - rect.y0 - HeaderHeight;
	rect.x1 -= rect.x0;
	rect.y1 -= rect.y0;
	if (x < 0 || x > rect.x1)
		return;
	if (y < 0 || y > rect.y1 - HeaderHeight)
		return;
	unsigned Sel = (y / LISTVIEW__GetRowDistY(pObj)) + pObj->scrollStateV.v;
	if (Sel < pObj->RowArray.NumItems())
		LISTVIEW_SetSel(pObj, Sel);
}
static void _NotifyOwner(ListView *pObj, int Notification) {
	auto pOwner = pObj->pOwner ? pObj->pOwner : pObj->Parent();
	WM_PARAM data;
	msg.MsgId = WM_NOTIFY_PARENT;
	data = Notification;
	msg.pWin = pObj;
	pOwner->SendMessage(MsgId, &data);
}
static void _OnTouch(ListView *pObj, WM_PARAM *pData) {
	int Notification;
	const PID_STATE *pState = (const PID_STATE *)*pData;
	if (pState) {
		if (pState->Pressed) {
			_SetSelFromPos(pObj, pState);
			Notification = WN_CLICKED;
			pObj->Focus();
		}
		else
			Notification = WN_RELEASED;
	}
	else
		Notification = WN_MOVED_OUT;
	_NotifyOwner(pObj, Notification);
}
static int _GetXSize(ListView *pObj) {
	return pObj->InsideRectScrollBar().x1 + 1;
}
static int _GetHeaderWidth(ListView *pObj, Header *pHeader) {
	int i, NumItems = HEADER_GetNumItems(pHeader), r = 1;
	if (NumItems)
		for (i = 0, r = 0; i < NumItems; ++i)
			r += HEADER_GetItemWidth(pHeader, i);
	int w = pObj->scrollStateH.v + pObj->scrollStateH.PageSize;
	return w > r ? w : r;
}
int LISTVIEW__UpdateScrollPos(ListView *pObj) {
	int PrevScrollStateV = pObj->scrollStateV.v;
	if (pObj->Sel >= 0)
		pObj->scrollStateV.Pos(pObj->Sel);
	else
		pObj->scrollStateV.Bound();
	pObj->scrollStateH.Bound();
	pObj->ScrollStateH(pObj->scrollStateH);
	pObj->ScrollStateV(pObj->scrollStateV);
	return pObj->scrollStateV.v - PrevScrollStateV;
}
int LISTVIEW__UpdateScrollParas(ListView *pObj) {
	int NumRows = pObj->RowArray.NumItems();
	pObj->scrollStateV.PageSize = _GetNumVisibleRows(pObj);
	pObj->scrollStateV.NumItems = NumRows ? NumRows : 1;
	pObj->scrollStateH.PageSize = _GetXSize(pObj);
	pObj->scrollStateH.NumItems = _GetHeaderWidth(pObj, pObj->pHeader);
	return LISTVIEW__UpdateScrollPos(pObj);
}
static void _FreeAttached(ListView *pObj) {
	pObj->RowArray.Destruct();
	pObj->AlignArray.Delete();
}
static int _AddKey(ListView *pObj, int Key) {
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
static void _LISTVIEW_Callback(int MsgId, WM_PARAM *pData) {
	ListView *pObj = (ListView *)pWin;
	if (!pObj->HandleActive(MsgId, pData))
		return;
	switch (MsgId) {
	case WM_NOTIFY_CLIENTCHANGE:
	case WM_SIZE:
		LISTVIEW__UpdateScrollParas(pObj);
		return;
	case WM_NOTIFY_PARENT:
		switch (*pData) {
		case WN_CHILD_DELETED:
			if (pData->pWinSrc == pObj->pHeader)
				pObj->pHeader = 0;
			break;
		case WN_VALUE_CHANGED:
			if (pData->pWinSrc == pObj->ScrollBarV()) {
				pObj->scrollStateV.v = ((ScrollBar *)pData->pWinSrc)->ScrollState().v;
				LISTVIEW__InvalidateInsideArea(pObj);
				_NotifyOwner(pObj, WN_SCROLL_CHANGED);
			}
			else if (pData->pWinSrc == pObj->ScrollBarH()) {
				pObj->scrollStateH.v = ((ScrollBar *)pData->pWinSrc)->ScrollState().v;
				LISTVIEW__UpdateScrollParas(pObj);
				HEADER_SetScrollPos(pObj->pHeader, pObj->scrollStateH.v);
				_NotifyOwner(pObj, WN_SCROLL_CHANGED);
			}
			break;
		case WN_SCROLLBAR_ADDED:
			LISTVIEW__UpdateScrollParas(pObj);
			break;
		}
		return;
	case WM_PAINT:
		_OnPaint(pObj, pData);
		return;
	case WM_TOUCH:
		_OnTouch(pObj, pData);
		return;
	case WM_KEY:
	{
		const WM_KEY_INFO *ki = (const WM_KEY_INFO *)*pData;
		if (ki->PressedCnt > 0)
			if (_AddKey(pObj, ki->Key))
				return;
		break;
	}
	case WM_DELETE:
		_FreeAttached(pObj);
		break;
	}
	DefCallback(pObj, MsgId, pData, pWinSrc);
}
ListView *LISTVIEW_CreateEx(
	int x0, int y0, int xsize, int ysize, WObj *pParent,
	uint16_t Flags, uint16_t ExFlags, uint16_t Id) {
	//if (xsize == 0 && ysize == 0 && x0 == 0 && y0 == 0) {
	//	auto &&rect = pParent->ClientRect();
	//	xsize = rect.x1 - rect.x0 + 1;
	//	ysize = rect.y1 - rect.y0 + 1;
	//}
	//auto pObj = (ListView *)WObj::Create(
	//	x0, y0, xsize, ysize, pParent, Flags, &_LISTVIEW_Callback,
	//	sizeof(ListView) - sizeof(WObj));
	//if (!pObj)
	//	return nullptr;
	//pObj->Init(Id, WC_FOCUSSABLE);
	//pObj->Props = ListView::DefaultProps;
	//pObj->ShowGrid = 0;
	//pObj->RowDistY = 0;
	//pObj->Sel = -1;
	//pObj->LBorder = 1;
	//pObj->RBorder = 1;
	//pObj->pHeader = HEADER_CreateEx(0, 0, 0, 0, pObj, WC_VISIBLE, 0, 0);
	//LISTVIEW__UpdateScrollParas(pObj);
	//return pObj;
}
void LISTVIEW_IncSel(ListView *pObj) {
	LISTVIEW_SetSel(pObj, LISTVIEW_GetSel(pObj) + 1);
}
void LISTVIEW_DecSel(ListView *pObj) {
	if (int Sel = LISTVIEW_GetSel(pObj))
		LISTVIEW_SetSel(pObj, Sel - 1);
}
void LISTVIEW_AddColumn(ListView *pObj, int Width, const char *s, int Align) {
	if (!pObj)
		return;
	HEADER_AddItem(pObj->pHeader, Width, s, Align);
	pObj->AlignArray.Add(Align);
	auto NumRows = LISTVIEW_GetNumRows(pObj);
	if (!NumRows)
		return;
	for (auto i = 0; i < NumRows; ++i)
		pObj->RowArray[i].Add({});
	LISTVIEW__UpdateScrollParas(pObj);
	LISTVIEW__InvalidateInsideArea(pObj);
}
void LISTVIEW_AddRow(ListView *pObj, const char **ppText) {
	if (!pObj)
		return;
	auto NumRows = pObj->RowArray.NumItems();
	pObj->RowArray.Add({});
	for (int i = 0, NumColumns = HEADER_GetNumItems(pObj->pHeader); i < NumColumns; ++i) {
		auto &Row = pObj->RowArray[NumRows];
		const char *s = ppText ? *ppText++ : nullptr;
		if (s == 0)
			ppText = 0;
		int NumBytes = GUI__strlen(s) + 1;
		Row.Add({});
		GUI__SetText(&Row[i].pText, s);
	}
	LISTVIEW__UpdateScrollParas(pObj);
	LISTVIEW__InvalidateRow(pObj, NumRows);
}
void LISTVIEW_DeleteColumn(ListView *pObj, unsigned Index) {
	if (!pObj)
		return;
	if (Index >= pObj->AlignArray.NumItems())
		return;
	HEADER_DeleteItem(pObj->pHeader, Index);
	pObj->AlignArray.Delete(Index);
	for (int i = 0, NumRows = pObj->RowArray.NumItems(); i < NumRows; ++i) {
		auto &Row = pObj->RowArray[i];
		auto &Item = Row[Index];
		if (auto pItemInfo = Item.pItemInfo)
			GUI_MEM_Free(pItemInfo);
		Row.Delete(Index);
	}
	LISTVIEW__UpdateScrollParas(pObj);
	LISTVIEW__InvalidateInsideArea(pObj);
}
RGBC LISTVIEW_GetBkColor(ListView *pObj, unsigned Index) {
	if (!pObj)
		return RGB_INVALID_COLOR;
	if (Index >= GUI_COUNTOF(pObj->Props.aBkColor))
		return RGB_INVALID_COLOR;
	return pObj->Props.aBkColor[Index];
}
CFont *LISTVIEW_GetFont(ListView *pObj) {
	if (!pObj)
		return nullptr;
	return pObj->Props.pFont;
}
Header *LISTVIEW_GetHeader(ListView *pObj) {
	return pObj ? pObj->pHeader : 0;
}
unsigned LISTVIEW_GetNumColumns(ListView *pObj) {
	if (!pObj)
		return 0;
	return pObj->AlignArray.NumItems();
}
unsigned LISTVIEW_GetNumRows(ListView *pObj) {
	if (!pObj)
		return 0;
	return pObj->RowArray.NumItems();
}
int LISTVIEW_GetSel(ListView *pObj) {
	if (!pObj)
		return -1;
	return pObj->Sel;
}
RGBC LISTVIEW_GetTextColor(ListView *pObj, unsigned Index) {
	if (!pObj)
		return RGB_INVALID_COLOR;
	if (Index >= GUI_COUNTOF(pObj->Props.aTextColor))
		return RGB_INVALID_COLOR;
	return pObj->Props.aTextColor[Index];
}
void LISTVIEW_SetBkColor(ListView *pObj, unsigned int Index, RGBC Color) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->Props.aBkColor))
		return;
	if (Color == pObj->Props.aBkColor[Index])
		return;
	pObj->Props.aBkColor[Index] = Color;
	LISTVIEW__InvalidateInsideArea(pObj);
}
void LISTVIEW_SetColumnWidth(ListView *pObj, unsigned int Index, int Width) {
	if (!pObj)
		return;
	HEADER_SetItemWidth(pObj->pHeader, Index, Width);
}
void LISTVIEW_SetFont(ListView *pObj, CFont *pFont) {
	if (!pObj)
		return;
	if (pFont == pObj->Props.pFont)
		return;
	pObj->Props.pFont = pFont;
	LISTVIEW__UpdateScrollParas(pObj);
	LISTVIEW__InvalidateInsideArea(pObj);
}
int LISTVIEW_SetGridVis(ListView *pObj, int Show) {
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
void LISTVIEW_SetItemText(ListView *pObj, unsigned Column, unsigned Row, const char *s) {
	if (!pObj)
		return;
	if (Column >= LISTVIEW_GetNumColumns(pObj) || Row >= LISTVIEW_GetNumRows(pObj))
		return;
	GUI__SetText(&pObj->RowArray[Row][Column].pText, s);
	LISTVIEW__InvalidateRow(pObj, Row);
}
void LISTVIEW_SetLBorder(ListView *pObj, unsigned BorderSize) {
	if (!pObj)
		return;
	if (pObj->LBorder == BorderSize)
		return;
	pObj->LBorder = BorderSize;
	LISTVIEW__InvalidateInsideArea(pObj);
}
void LISTVIEW_SetRBorder(ListView *pObj, unsigned BorderSize) {
	if (!pObj)
		return;
	if (pObj->RBorder == BorderSize)
		return;
	pObj->RBorder = BorderSize;
	LISTVIEW__InvalidateInsideArea(pObj);
}
unsigned LISTVIEW_SetRowHeight(ListView *pObj, unsigned RowHeight) {
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
void LISTVIEW_SetSel(ListView *pObj, int NewSel) {
	if (!pObj)
		return;
	int MaxSel = pObj->RowArray.NumItems() - 1;
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
	pObj->NotifyParent(WN_SEL_CHANGED);
}
void LISTVIEW_SetTextAlign(ListView *pObj, unsigned int Index, int Align) {
	if (Index >= pObj->AlignArray.NumItems())
		return;
	auto &OldAlign = pObj->AlignArray[Index];
	if (Align == OldAlign)
		return;
	OldAlign = Align;
	LISTVIEW__InvalidateInsideArea(pObj);
}
void LISTVIEW_SetTextColor(ListView *pObj, unsigned int Index, RGBC Color) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->Props.aTextColor))
		return;
	if (Color == pObj->Props.aTextColor[Index])
		return;
	pObj->Props.aTextColor[Index] = Color;
	LISTVIEW__InvalidateInsideArea(pObj);
}
static LISTVIEW_ITEM_INFO *_GetpItemInfo(ListView *pObj, unsigned Column, unsigned Row, unsigned int Index) {
	if (!pObj)
		return nullptr;
	LISTVIEW_ITEM_INFO *pItemInfo;
	if (Index >= GUI_COUNTOF(pItemInfo->aTextColor))
		return nullptr;
	if (Column >= LISTVIEW_GetNumColumns(pObj) || Row >= LISTVIEW_GetNumRows(pObj))
		return nullptr;
	auto &Item = pObj->RowArray[Row][Column];
	pItemInfo = Item.pItemInfo;
	if (pItemInfo)
		return pItemInfo;
	Item.pItemInfo = (LISTVIEW_ITEM_INFO *)GUI_MEM_AllocZero(sizeof(LISTVIEW_ITEM_INFO));
	pItemInfo = Item.pItemInfo;
	for (int i = 0; i < GUI_COUNTOF(pItemInfo->aTextColor); ++i) {
		pItemInfo->aTextColor[i] = LISTVIEW_GetTextColor(pObj, i);
		pItemInfo->aBkColor[i] = LISTVIEW_GetBkColor(pObj, i);
	}
	return pItemInfo;
}
void LISTVIEW_SetItemTextColor(ListView *pObj, unsigned Column, unsigned Row, unsigned int Index, RGBC Color) {
	LISTVIEW_ITEM_INFO *pItemInfo = _GetpItemInfo(pObj, Column, Row, Index);
	if (pItemInfo)
		pItemInfo->aTextColor[Index] = Color;
}
void LISTVIEW_SetItemBkColor(ListView *pObj, unsigned Column, unsigned Row, unsigned int Index, RGBC Color) {
	LISTVIEW_ITEM_INFO *pItemInfo = _GetpItemInfo(pObj, Column, Row, Index);
	if (pItemInfo)
		pItemInfo->aBkColor[Index] = Color;
}
static void _InvalidateRowAndBelow(ListView *pObj, int Sel) {
	if (Sel < 0)
		return;
	auto &&rect = pObj->InsideRectScrollBar();
	rect.y0 += HEADER_GetHeight(pObj->pHeader) + (Sel - pObj->scrollStateV.v) * LISTVIEW__GetRowDistY(pObj);
	pObj->Invalidate(rect);
}
void LISTVIEW_DeleteRow(ListView *pObj, unsigned Index) {
	if (!pObj)
		return;
	unsigned NumRows = pObj->RowArray.NumItems();
	if (Index >= NumRows)
		return;
	auto &Row = pObj->RowArray[Index];
	Row.Destruct();
	pObj->RowArray[Index];
	if (pObj->Sel == (signed int)Index)
		pObj->Sel = -1;
	if (pObj->Sel > (signed int)Index)
		pObj->Sel--;
	if (LISTVIEW__UpdateScrollParas(pObj))
		LISTVIEW__InvalidateInsideArea(pObj);
	else
		_InvalidateRowAndBelow(pObj, Index);
}
