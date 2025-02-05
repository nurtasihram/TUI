#include "ScrollBar.h"
#include "Header.h"

Header::Property Header::DefaultProps;

static int _DefaultBorderH = 0;
static int _DefaultBorderV = 2;

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
			switch (Col.Align & TEXTALIGN_HORIZONTAL) {
			case TEXTALIGN_RIGHT:
				Off.x =  Col.Width - size.x;
				break;
			case TEXTALIGN_HCENTER:
				Off.x = (Col.Width - size.x) / 2;
				break;
			}
			switch (Col.Align & TEXTALIGN_VCENTER) {
			case TEXTALIGN_BOTTOM:
				Off.y =  rClient.ysize() - size.y;
				break;
			case TEXTALIGN_VCENTER:
				Off.y = (rClient.ysize() - size.y) / 2;
				break;
			}
			Off.x += xPos;
			WObj::UserClip(&rClient);
			Col.pDrawObj->Draw({ Off, 0 });
			WObj::UserClip(nullptr);
		}
		DrawUp(rClient);
		xPos += rClient.x1 - rClient.x0;
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

WM_RESULT Header::_Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc) {
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

Header::Header(int x0, int y0, int xsize, int ysize,
			   WObj *pParent, uint16_t Id,
			   WM_CF Flags, uint16_t ExFlags) :
	Widget(x0, y0, xsize, ysize,
		   _Callback,
		   pParent, Id,
		   Flags | WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT,
		   ExFlags) {
	if (x0 == 0 && y0 == 0)
		Move(Parent()->InsideRect().left_top());
	if (xsize == 0)
		SizeX(Parent()->InsideRect().xsize());
	if (ysize == 0)
		SizeY(Props.pFont->YDist +
			2 * _DefaultBorderV +
			2 * Effect()->EffectSize);
}

void Header::Add(const char *pText, uint16_t Width, TEXTALIGN Align) {
	if (!Width)
		Width =
			Props.pFont->XDist(pText, strlen(pText)) +
			2 * (EffectSize() + _DefaultBorderH);
	auto &Col = Columns.Add();
	Col.Width = Width;
	Col.Align = Align;
	Col.pDrawObj = nullptr;
	Col.Text = pText;
	Invalidate();
	Parent()->Invalidate();
}
void Header::DeleteItem(unsigned Index) {
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
	if (Width < 0)
		return;
	if (Index < Columns.NumItems()) {
		Columns[Index].Width = Width;
		Invalidate();
		Parent()->SendMessage(WM_NOTIFY_CLIENTCHANGE);
		Parent()->Invalidate();
	}
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
