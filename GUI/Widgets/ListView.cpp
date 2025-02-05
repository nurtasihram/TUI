#include "ScrollBar.h"
#include "ListView.h"

ListView::Property ListView::DefaultProps;

uint16_t ListView::_GetNumVisibleRows() const {
	auto RowDistY = _GetRowDistY();
	if (!RowDistY)
		return 1;
	auto ySize = InsideRect().ysize() - pHeader->Height();
	unsigned r = ySize / RowDistY;
	return r == 0 ? 1 : r;
}
uint16_t ListView::_GetRowDistY() const {
	if (RowDistY)
		return RowDistY;
	auto RowDistY = Props.pFont->YDist;
	if (StatusEx & LISTVIEW_CF_GRIDLINE)
		return RowDistY + 1;
	return RowDistY;
}
int ListView::_GetHeaderWidth() const {
	int r = 1;
	if (auto NumItems = pHeader->NumItems())
		for (int i = 0, r = 0; i < NumItems; ++i)
			r += pHeader->ItemWidth(i);
	int w = scrollStateH.v + scrollStateH.PageSize;
	return w > r ? w : r;
}

void ListView::_InvalidateInsideArea() {
	auto &&rInside = InsideRect();
	rInside.y0 += pHeader->Height();
	Invalidate(rInside);
}
void ListView::_InvalidateRow(int sel) {
	if (sel < 0)
		return;
	auto HeaderHeight = pHeader->Height();
	auto RowDistY = _GetRowDistY();
	auto &&rect = InsideRect();
	rect.y0 += HeaderHeight + (sel - scrollStateV.v) * RowDistY;
	rect.y1 = rect.y0 + RowDistY - 1;
	Invalidate(rect);
}
void ListView::_InvalidateRowAndBelow(int sel) {
	if (sel < 0)
		return;
	auto &&rect = InsideRect();
	rect.y0 += pHeader->Height() + (sel - scrollStateV.v) * _GetRowDistY();
	Invalidate(rect);
}
void ListView::_SelFromPos(const PID_STATE *pState) {
	auto &&rect = InsideRect();
	int HeaderHeight = pHeader->Height();
	int x = pState->x - rect.x0,
		y = pState->y - rect.y0 - HeaderHeight;
	rect.x1 -= rect.x0;
	rect.y1 -= rect.y0;
	if (x < 0 || x > rect.x1)
		return;
	if (y < 0 || y > rect.y1 - HeaderHeight)
		return;
	unsigned sel = (y / _GetRowDistY()) + scrollStateV.v;
	if (sel < RowArray.NumItems())
		Sel(sel);
}
void ListView::_NotifyOwner(int Notification) {
	auto pOwner = this->pOwner ? this->pOwner : Parent();
	pOwner->SendMessage(WM_NOTIFY_PARENT, Notification);
}

int ListView::_UpdateScrollPos() {
	int PrevScrollStateV = scrollStateV.v;
	if (sel >= 0)
		scrollStateV.Pos(sel);
	else
		scrollStateV.Bound();
	scrollStateH.Bound();
	ScrollStateH(scrollStateH);
	ScrollStateV(scrollStateV);
	return scrollStateV.v - PrevScrollStateV;
}
int ListView::_UpdateScrollParas() {
	int NumRows = RowArray.NumItems();
	scrollStateV.PageSize = _GetNumVisibleRows();
	scrollStateV.NumItems = NumRows ? NumRows : 1;
	scrollStateH.PageSize = _GetXSize();
	scrollStateH.NumItems = _GetHeaderWidth();
	return _UpdateScrollPos();
}

void ListView::_OnPaint(SRect rClip) const {
	int NumColumns = pHeader->NumItems(),
		NumRows = RowArray.NumItems(),
		NumVisRows = _GetNumVisibleRows();
	int EffectSize = this->EffectSize();
	int yPos = pHeader->Height() + EffectSize;
	int EndRow = scrollStateV.v + NumVisRows + 1;
	if (EndRow > NumRows)
		EndRow = NumRows;
	int RowDistY = _GetRowDistY();
	auto &&rect = InsideRect();
	rClip &= rect;
	GUI.PenColor(Props.aTextColor[0]);
	GUI.Font(Props.pFont);
	for (auto i = scrollStateV.v; i < EndRow; ++i) {
		auto &row = RowArray[i];
		rect.y0 = yPos;
		if (rect.y0 > rClip.y1)
			break;
		rect.y1 = yPos + RowDistY - 1;
		if (rect.y1 >= rClip.y0) {
			auto ColorIndex =
				i == sel ?
				Focussed() ?
				LISTVIEW_CI_SELFOCUS :
				LISTVIEW_CI_SEL :
				LISTVIEW_CI_UNSEL;
			GUI.BkColor(Props.aBkColor[ColorIndex]);
			if (StatusEx & LISTVIEW_CF_GRIDLINE)
				rect.y1--;
			auto xPos = EffectSize - scrollStateH.v;
			for (int j = 0; j < NumColumns; ++j) {
				auto Width = pHeader->ItemWidth(j);
				rect.x0 = xPos;
				if (rect.x0 > rClip.x1)
					break;
				rect.x1 = xPos + Width - 1;
				if (rect.x1 >= rClip.x0) {
					auto &item = row[j];
					GUI.PenColor(Props.aTextColor[ColorIndex]);
					GUI.Clear(rect);
					rect.x0 += lBorder;
					rect.x1 -= rBorder;
					GUI.TextAlign(pHeader->ItemAlign(j));
					GUI.DispString(item.Text, rect);
				}
				xPos += Width;
			}
			if (xPos <= rClip.x1)
				GUI.Clear({ xPos, rect.y0, rClip.x1, rect.y1 });
		}
		yPos += RowDistY;
	}
	if (yPos <= rClip.y1) {
		GUI.BkColor(Props.aBkColor[0]);
		GUI.Clear({ rClip.x0, yPos, rClip.x1, rClip.y1 });
	}
	if (StatusEx & LISTVIEW_CF_GRIDLINE) {
		GUI.PenColor(Props.GridColor);
		yPos = pHeader->Height() + EffectSize - 1;
		for (int i = 0; i < NumVisRows; ++i) {
			yPos += RowDistY;
			if (yPos > rClip.y1)
				break;
			if (yPos >= rClip.y0)
				GUI.DrawLineH(rClip.x0, yPos, rClip.x1);
		}
		auto xPos = EffectSize - scrollStateH.v;
		for (int i = 0; i < NumColumns; ++i) {
			xPos += pHeader->ItemWidth(i);
			if (xPos > rClip.x1)
				break;
			if (xPos >= rClip.x0)
				GUI.DrawLineV(xPos, rClip.y0, rClip.y1);
		}
	}
	DrawDown();
}
void ListView::_OnTouch(const PID_STATE *pState) {
	int Notification;
	if (pState) {
		if (pState->Pressed) {
			_SelFromPos(pState);
			Notification = WN_CLICKED;
			Focus();
		}
		else
			Notification = WN_RELEASED;
	}
	else
		Notification = WN_MOVED_OUT;
	_NotifyOwner(Notification);
}
bool ListView::_OnKey(int Key) {
	switch (Key) {
		case GUI_KEY_DOWN:
			Inc();
			return true;
		case GUI_KEY_UP:
			Dec();
			return true;
	}
	return false;
}

WM_RESULT  ListView::_Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc) {
	auto pObj = (ListView *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	switch (MsgId) {
		case WM_PAINT:
			pObj->_OnPaint(Param);
			return 0;
		case WM_TOUCH:
			pObj->_OnTouch(Param);
			return 0;
		case WM_DELETE:
			pObj->~ListView();
			return 0;
		case WM_NOTIFY_CLIENTCHANGE:
		case WM_SIZE:
			pObj->_UpdateScrollParas();
			return 0;
		case WM_NOTIFY_PARENT:
			switch ((int)Param) {
				case WN_CHILD_DELETED:
					if (pSrc == pObj->pHeader)
						pObj->pHeader = nullptr;
					break;
				case WN_VALUE_CHANGED:
					if (pSrc == (WObj *)pObj->ScrollBarV()) {
						pObj->scrollStateV.v = ((ScrollBar *)pSrc)->ScrollState().v;
						pObj->_InvalidateInsideArea();
						pObj->_NotifyOwner(WN_SCROLL_CHANGED);
					}
					else if (pSrc == (WObj *)pObj->ScrollBarH()) {
						pObj->scrollStateH.v = ((ScrollBar *)pSrc)->ScrollState().v;
						pObj->_UpdateScrollParas();
						pObj->pHeader->ScrollPos(pObj->scrollStateH.v);
						pObj->_NotifyOwner(WN_SCROLL_CHANGED);
					}
					break;
				case WN_SCROLLBAR_ADDED:
					pObj->_UpdateScrollParas();
					break;
			}
			return 0;
		case WM_KEY:
			if (const KEY_STATE *pKi = Param)
				if (pKi->PressedCnt > 0)
					if (pObj->_OnKey(pKi->Key))
						return 0;
			break;
	}
	return DefCallback(pWin, MsgId, Param, pSrc);
}

ListView::ListView(int x0, int y0, int xsize, int ysize,
				   WObj *pParent, uint16_t Id,
				   uint16_t Flags, uint16_t ExFlags) : 
	Widget(x0, y0, xsize, ysize,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, ExFlags & LISTVIEW_CF_GRIDLINE) {
	if (xsize == 0 && ysize == 0 && x0 == 0 && y0 == 0)
		Size(Parent()->ClientRect().size());
	pHeader = new Header(0, 0, 0, 0, this, 0, WC_VISIBLE, ExFlags & LISTVIEW_CF_HEADER_DRAG);
	_UpdateScrollParas();
}

void ListView::AddColumn(const char *s, uint16_t Width, TEXTALIGN Align) {
	pHeader->Add(s, Width, Align);
	auto NumRows = this->NumRows();
	if (!NumRows)
		return;
	for (auto i = 0; i < NumRows; ++i)
		RowArray[i].Add();
	_UpdateScrollParas();
	_InvalidateInsideArea();
}
void ListView::AddRow(const char *pTexts) {
	auto NumRows = this->NumRows();
	auto &Row = RowArray.Add();
	for (int i = 0, NumColumns = this->NumColumns(); i < NumColumns; ++i) {
		Row.Add().Text = pTexts;
		pTexts = GUI__NextText(pTexts);
	}
	_UpdateScrollParas();
	_InvalidateRow(NumRows);
}

void ListView::DeleteColumn(uint16_t Index) {
	if (Index >= pHeader->NumItems())
		return;
	pHeader->DeleteItem(Index);
	for (int i = 0, NumRows = RowArray.NumItems(); i < NumRows; ++i) {
		auto &Row = RowArray[i];
		auto &Item = Row[Index];
		Row.Delete(Index);
	}
	_UpdateScrollParas();
	_InvalidateInsideArea();
}
void ListView::DeleteRow(uint16_t Index) {
	auto NumRows = RowArray.NumItems();
	if (Index >= NumRows)
		return;
	RowArray[Index].Delete();
	if (sel == Index)
		sel = -1;
	if (sel > Index)
		sel--;
	if (_UpdateScrollParas())
		_InvalidateInsideArea();
	else
		_InvalidateRowAndBelow(Index);
}

void ListView::Sel(int16_t NewSel) {
	int MaxSel = RowArray.NumItems() - 1;
	if (NewSel > MaxSel)
		NewSel = MaxSel;
	if (NewSel < 0)
		NewSel = -1;
	if (NewSel == sel)
		return;
	int OldSel = sel;
	sel = NewSel;
	if (_UpdateScrollPos())
		_InvalidateInsideArea();
	else {
		_InvalidateRow(OldSel);
		_InvalidateRow(NewSel);
	}
	NotifyParent(WN_SEL_CHANGED);
}
