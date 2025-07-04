#include "ScrollBar.h"
#include "ListView.h"

ListView::Property ListView::DefaultProps;

uint16_t ListView::_HeaderHeight() const {
	if (pHeader)
		if (pHeader->Visible())
			return pHeader->Height();
	return 0;
}
uint16_t ListView::_GetNumVisibleRows() const {
	if (auto RowDistY = _GetRowDistY()) {
		unsigned r = InsideRect().ysize() / RowDistY;
		return r == 0 ? 1 : r;
	}
	return 1;
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
	int i, r = 1;
	if (pHeader)
		if (auto NumItems = pHeader->NumItems())
			for (i = 0, r = 0; i < NumItems; i++)
				r += pHeader->ItemWidth(i);
	auto Width = scrollStateH.v + scrollStateH.PageSize - r;
	if (Width > 0)
		r += Width;
	return r;
}
void ListView::_InvalidateRow(int sel) {
	if (sel < 0)
		return;
	auto RowDistY = _GetRowDistY();
	auto &&rsWin = InsideRect();
	rsWin.y0 += (sel - scrollStateV.v) * RowDistY;
	rsWin.y1 = rsWin.y0 + RowDistY - 1;
	Invalidate(rsWin);
}
void ListView::_InvalidateRowAndBelow(int sel) {
	if (sel < 0)
		return;
	auto &&rsWin = InsideRect();
	rsWin.y0 += (sel - scrollStateV.v) * _GetRowDistY();
	Invalidate(rsWin);
}
void ListView::_SelFromPos(Point Pos) {
	auto &&rInside = InsideRect();
	if (rInside <= Pos) {
		auto sel = (Pos.y - rInside.y0) / _GetRowDistY() + scrollStateV.v;
		if (sel < RowArray.NumItems())
			Sel(sel);
	}
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
	int NumColumns = pHeader ? pHeader->NumItems() : 0,
		NumRows = RowArray.NumItems(),
		NumVisRows = _GetNumVisibleRows();
	int EffectSize = this->EffectSize();
	int yPos = (pHeader ? pHeader->Height() : 0) + EffectSize;
	int EndRow = scrollStateV.v + NumVisRows + 1;
	if (EndRow > NumRows)
		EndRow = NumRows;
	int RowDistY = _GetRowDistY();
	auto &&rsWin = InsideRect();
	rClip &= rsWin;
	GUI.PenColor(Props.aTextColor[0]);
	GUI.Font(Props.pFont);
	for (auto i = scrollStateV.v; i < EndRow; ++i) {
		auto &row = RowArray[i];
		rsWin.y0 = yPos;
		if (rsWin.y0 > rClip.y1)
			break;
		rsWin.y1 = yPos + RowDistY - 1;
		if (rsWin.y1 >= rClip.y0) {
			auto ColorIndex =
				i == sel ?
				Focussed() ?
				LISTVIEW_CI_SELFOCUS :
				LISTVIEW_CI_SEL :
				LISTVIEW_CI_UNSEL;
			GUI.BkColor(Props.aBkColor[ColorIndex]);
			if (StatusEx & LISTVIEW_CF_GRIDLINE)
				rsWin.y1--;
			auto xPos = EffectSize - scrollStateH.v;
			for (int j = 0; j < NumColumns; ++j) {
				auto Width = pHeader ? pHeader->ItemWidth(j) : 0;
				rsWin.x0 = xPos;
				if (rsWin.x0 > rClip.x1)
					break;
				rsWin.x1 = xPos + Width - 1;
				if (rsWin.x1 >= rClip.x0) {
					auto &item = row[j];
					GUI.PenColor(Props.aTextColor[ColorIndex]);
					GUI.Clear(rsWin);
					rsWin.x0 += lBorder;
					rsWin.x1 -= rBorder;
					GUI.TextAlign(pHeader ? pHeader->ItemAlign(j) : TEXTALIGN_LEFT);
					GUI.DrawStringIn(item.Text, rsWin);
				}
				xPos += Width;
			}
			if (xPos <= rClip.x1)
				GUI.Clear({ xPos, rsWin.y0, rClip.x1, rsWin.y1 });
		}
		yPos += RowDistY;
	}
	if (yPos <= rClip.y1) {
		GUI.BkColor(Props.aBkColor[0]);
		GUI.Clear({ rClip.x0, yPos, rClip.x1, rClip.y1 });
	}
	if (StatusEx & LISTVIEW_CF_GRIDLINE) {
		GUI.PenColor(Props.GridColor);
		yPos = (pHeader ? pHeader->Height() : 0) + EffectSize - 1;
		for (int i = 0; i < NumVisRows; ++i) {
			yPos += RowDistY;
			if (yPos > rClip.y1)
				break;
			if (yPos >= rClip.y0)
				GUI.DrawLineH(rClip.x0, yPos, rClip.x1);
		}
		auto xPos = EffectSize - scrollStateH.v;
		for (int i = 0; i < NumColumns; ++i) {
			xPos += pHeader ? pHeader->ItemWidth(i) : 30; ///// 
			if (xPos > rClip.x1)
				break;
			if (xPos >= rClip.x0)
				GUI.DrawLineV(xPos, rClip.y0, rClip.y1);
		}
	}
	DrawDown();
}
void ListView::_OnMouse(MOUSE_STATE State) {
	if (State) {
		if (State.Pressed) {
			_SelFromPos(State);
			Focus();
			NotifyOwner(WN_CLICKED);
		}
		else
			NotifyOwner(WN_RELEASED);
	}
	else
		NotifyOwner(WN_MOVED_OUT);
}
bool ListView::_OnKey(KEY_STATE State) {
	if (State.PressedCnt <= 0)
		return false;
	switch (State.Key) {
		case GUI_KEY_DOWN:
			Inc();
			return true;
		case GUI_KEY_UP:
			Dec();
			return true;
		case GUI_KEY_LEFT:
			scrollStateH.Value(scrollStateH.v - 1);
			_UpdateScrollParas();
			if (pHeader)
				pHeader->ScrollPos(scrollStateH.v);
			Invalidate();
			return true;
		case GUI_KEY_RIGHT:
			scrollStateH.Value(scrollStateH.v + 1);
			_UpdateScrollParas();
			if (pHeader)
				pHeader->ScrollPos(scrollStateH.v);
			Invalidate();
			return true;
	}
	return false;
}

WM_RESULT  ListView::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (ListView *)pWin;
	switch (MsgId) {
	case WM_PAINT:
		pObj->_OnPaint(Param);
		return 0;
	case WM_MOUSE:
		pObj->_OnMouse(Param);
		return 0;
	case WM_KEY:
		if (pObj->_OnKey(Param))
			return true;
		break;
	case WM_DELETE:
		pObj->~ListView();
		return 0;
	case WM_NOTIFY_CLIENT_CHANGE:
	case WM_SIZED:
		pObj->_UpdateScrollParas();
		return 0;
	case WM_NOTIFY_CHILD:
		switch ((int)Param) {
			case WN_CHILD_DELETED:
				if (pObj->pHeader == pSrc)
					pObj->pHeader = nullptr;
				return 0;
			case WN_VALUE_CHANGED:
				if (pSrc == (PWObj)pObj->ScrollBarV()) {
					pObj->scrollStateV.v = ((ScrollBar *)pSrc)->Value();
					pObj->Invalidate();
					pObj->NotifyOwner(WN_SCROLL_CHANGED);
				}
				else if (pSrc == (PWObj)pObj->ScrollBarH()) {
					pObj->scrollStateH.v = ((ScrollBar *)pSrc)->Value();
					pObj->_UpdateScrollParas();
					if (pObj->pHeader)
						pObj->pHeader->ScrollPos(pObj->scrollStateH.v);
					pObj->NotifyOwner(WN_SCROLL_CHANGED);
				}
				return 0;
			case WN_SCROLLBAR_ADDED:
				pObj->_UpdateScrollParas();
				return 0;
		}
		return 0;
	case WM_GET_INSIDE_RECT: {
		pObj->HandleActive(MsgId, Param);
		SRect &&rInside = Param;
		if (auto pHeader = pObj->pHeader)
			if (pHeader->Visible())
				rInside.y0 += pHeader->Height();
		return rInside;
	}
	case WM_GET_CLASS:
		return ClassNames[WCLS_LISTVIEW];
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

ListView::ListView(const SRect &rc,
				   PWObj pParent, uint16_t Id,
				   WM_CF Flags, LISTVIEW_CF FlagsEx) : 
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | WC_FOCUSSABLE, FlagsEx & LISTVIEW_CF_GRIDLINE) {
	if (!rc)
		Rect(Parent()->ClientRect());
	pHeader = new Header({}, this, 0, WC_VISIBLE, FlagsEx & LISTVIEW_CF_HEADER_DRAG);
	_UpdateScrollParas();
}

void ListView::AddColumn(GUI_PCSTR s, int Width, TEXTALIGN Align) {
	if (pHeader)
		pHeader->Add(s, Width, Align);
	auto NumRows = this->NumRows();
	if (!NumRows)
		return;
	for (auto i = 0; i < NumRows; ++i)
		RowArray[i].Add();
	_UpdateScrollParas();
	Invalidate();
}
bool ListView::AddRow(GUI_PCSTR pTexts) {
	auto NumRows = this->NumRows();
	auto &Row = RowArray.Add();
	if (GUI.NumTexts(pTexts) < this->NumColumns())
		return false;
	for (int i = 0, NumColumns = this->NumColumns(); i < NumColumns; ++i) {
		Row.Add().Text = pTexts;
		pTexts = GUI.NextText(pTexts);
	}
	_UpdateScrollParas();
	_InvalidateRow(NumRows);
	return true;
}

void ListView::DeleteColumn(uint16_t Index) {
	if (Index >= NumColumns())
		return;
	if (pHeader)
		pHeader->Delete(Index);
	for (int i = 0, NumRows = RowArray.NumItems(); i < NumRows; ++i)
		RowArray[i].Delete();
	_UpdateScrollParas();
	Invalidate();
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
		Invalidate();
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
		Invalidate();
	else {
		_InvalidateRow(OldSel);
		_InvalidateRow(NewSel);
	}
	NotifyOwner(WN_SEL_CHANGED);
}
