#include "ScrollBar.h"
#include "Header.h"

Header::Property Header::DefaultProps;

constexpr int16_t _DefaultBorderH = 0;
constexpr int16_t _DefaultBorderV = 2;


int Header::_GetItemIndex(Point Pos) {
	if (0 > Pos.y || Pos.y >= SizeY())
		return -1;
	int xPos = 0;
	for (int i = 0, NumColumns = Columns.NumItems(); i < NumColumns; ++i) {
		auto &Col = Columns[i];
		xPos += Col.Width;
		if (xPos > Pos.x)
			break;
		if (Pos.x - 4 <= xPos && xPos <= Pos.x + 4)
			return i;
	}
	return -1;
}

void Header::_HandlePID(PID_STATE State) {
	State.x += scrollPos;
	int Hit = _GetItemIndex(State);
	/* set capture position () */
	if (State.Pressed == 1 && Hit >= 0 && CapturePosX == -1) {
		CapturePosX = State.x;
		CaptureItem = Hit;
	}
	/* set mouse cursor and capture () */
	if (Hit >= 0 && !Captured()) {
		Capture(true);
		GUI.Cursor(&GUI_CursorHeader);
	}
	/* modify header */
	if (CapturePosX >= 0 && State.x != CapturePosX && State.Pressed == 1) {
		auto NewSize = ItemWidth(CaptureItem) + State.x - CapturePosX;
		if (NewSize >= 0) {
			ItemWidth(CaptureItem, NewSize);
			CapturePosX = State.x;
		}
	}
	/* release capture & restore cursor */
	if (State.Pressed <= 0)
		if (Hit == -1)
			CaptureRelease();
}
bool Header::_HandleDrag(int MsgId, const PID_STATE *pState) {
	if (!(StatusEx & HEADER_CF_DRAG))
		return false;
	switch (MsgId) {
		case WM_TOUCH: {
			int Notification;
			if (pState) {
				_HandlePID(*pState);
				Notification = pState->Pressed ? WN_CLICKED : WN_RELEASED;
			}
			else
				Notification = WN_MOVED_OUT;
			NotifyParent(Notification);
			return true;
		}
		case WM_MOUSEOVER:
			if (pState)
				_HandlePID({ *pState, -1 });
			return true;
		case WM_CAPTURE_RELEASED:
			CapturePosX = -1;
			return true;
	}
	return false;
}

void Header::_OnPaint() {
	int xPos = -scrollPos;
	int EffectSize = this->EffectSize();
	GUI.BkColor(Props.BkColor);
	GUI.Font(Props.pFont);
	GUI.Clear();
	for (int i = 0, NumItems = Columns.NumItems(); i < NumItems; ++i) {
		auto &Col = Columns[i];
		auto &&rClient = ClientRect();
		rClient.x0 = xPos;
		rClient.x1 = rClient.x0 + Col.Width;
		if (Col.pDrawObj) {
			Point Off;
			auto &&size = Col.pDrawObj->Size();
			switch (Col.Align & TEXTALIGN_HCENTER) {
				case TEXTALIGN_HCENTER:
					Off.x = (Col.Width - size.x) / 2;
					break;
				case TEXTALIGN_RIGHT:
					Off.x =  Col.Width - size.x;
					break;
			}
			switch (Col.Align & TEXTALIGN_VCENTER) {
				case TEXTALIGN_VCENTER:
					Off.y = (rClient.ysize() - size.y) / 2;
					break;
				case TEXTALIGN_BOTTOM:
					Off.y =  rClient.ysize() - size.y;
					break;
			}
			Off.x += xPos;
			WObj::UserClip(&rClient);
			Col.pDrawObj->Draw({ Off, 0 });
			WObj::UserClip(nullptr);
		}
		DrawUp(rClient);
		xPos += rClient.dx();
		rClient.x0 += EffectSize + _DefaultBorderH;
		rClient.y0 += EffectSize + _DefaultBorderV;
		rClient.x1 -= EffectSize + _DefaultBorderH;
		rClient.y1 -= EffectSize + _DefaultBorderV;
		GUI.PenColor(Props.TextColor);
		GUI.TextAlign(Col.Align);
		GUI.DispString(Col.Text, rClient);
	}
	auto &&rClient = ClientRect();
	rClient.x0 = xPos;
	DrawUp(rClient);
}

WM_RESULT Header::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (Header *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	if (pObj->_HandleDrag(MsgId, Param))
		return 0;
	switch (MsgId) {
		case WM_PAINT:
			pObj->_OnPaint();
			return 0;
		case WM_DELETE:
			pObj->~Header();
			return 0;
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

Header::Header(const SRect &rc,
			   PWObj pParent, uint16_t Id,
			   WM_CF Flags, uint16_t FlagsEx,
			   const char *pTexts,
			   const uint16_t *pacWidth) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags | WC_ANCHOR_HORIZONTAL,
		   FlagsEx) {
	if (!rc.x0 || !rc.y0)
		Move(Parent()->InsideRect().left_top());
	if (rc.x1 <= rc.x0)
		SizeX(Parent()->InsideRect().xsize());
	if (rc.y1 <= rc.y0)
		SizeY(Props.pFont->YDist +
			  2 * _DefaultBorderV +
			  2 * Effect()->EffectSize);
	if (auto NumItems = GUI__NumTexts(pTexts)) {
		Columns.Resize(NumItems);
		for (auto &&i : Columns) {
			i.Text = pTexts;
			i.Width = pacWidth ? *pacWidth++ : 0;
			pTexts = GUI__NextText(pTexts);
		}
	}
}

void Header::Add(const char *pText, int Width, TEXTALIGN Align) {
	if (Width < 0)
		Width = Props.pFont->Size(pText).x +
		2 * (EffectSize() + _DefaultBorderH);
	auto &Col = Columns.Add();
	Col.Width = Width;
	Col.Align = Align;
	Col.pDrawObj = nullptr;
	Col.Text = pText;
	Invalidate();
	Parent()->Invalidate();
}
void Header::Delete(unsigned Index) {
	if (Index < Columns.NumItems()) {
		Columns.Delete(Index);
		Invalidate();
		Parent()->Invalidate();
	}
}

void Header::Height(uint16_t height) {
	SizeY(height);
	Parent()->Invalidate();
}
void Header::ItemWidth(uint16_t Index, int Width) {
	if (Index >= Columns.NumItems())
		return;
	auto &Item = Columns[Index];
	if (Width < 0)
		Width = Props.pFont->Size(Item).x +
			2 * (EffectSize() + _DefaultBorderH);
	Item.Width = Width;
	Invalidate();
	Parent()->SendMessage(WM_NOTIFY_CLIENTCHANGE);
	Parent()->Invalidate();
}

//void Header::DrawObj(unsigned Index, GUI_DRAW_BASE * pDrawObj) {
//	if (Index < Columns.NumItems()) {
//		auto &Col = Columns[Index];
//		GUI_MEM_Free(Col.pDrawObj);
//		Col.pDrawObj = pDrawObj;
//	}
//}
//void HEADER_SetBitmapEx(Header *pObj, unsigned Index, CBitmap* pBitmap, int x, int y) {
//	HEADER__SetDrawObj(pObj, Index, GUI_DRAW_BMP::Create(pBitmap));
//	pObj->Invalidate();
//}
