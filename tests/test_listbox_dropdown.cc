#include "Frame.h"
#include "CheckBox.h"
#include "ListBox.h"
#include "Radio.h"

const RGBC ColorsSmilie0[]{
	/* __ */ RGB_INVALID_COLOR,
	/* XX */ RGB_BLACK,
	/* oo */ RGB_RED
};
CPalette PalSmilie0{ ColorsSmilie0 };
const BPP2_DAT acSmilie0[]{
	________,XXXXXXXX,XX______,________,
	______XX,oooooooo,ooXX____,________,
	____XXoo,oooooooo,ooooXX__,________,
	__XXooXX,XXoooooo,XXXXooXX,________,
	XXooooXX,XXoooooo,XXXXoooo,XX______,
	XXoooooo,ooooXXoo,oooooooo,XX______,
	XXoooooo,ooooXXoo,oooooooo,XX______,
	XXoooooo,ooooXXoo,oooooooo,XX______,
	XXoooooo,oooooooo,oooooooo,XX______,
	__XXoooo,ooXXXXXX,ooooooXX,________,
	____XXoo,XXoooooo,XXooXX__,________,
	______XX,oooooooo,ooXX____,________,
	________,XXXXXXXX,XX______,________
};
CBitmap bmSmilie0{
	/* Size */ { 13, 13 },
	/* BytesPerLine */ 4,
	/* Bits */ acSmilie0,
	/* Palette */ &PalSmilie0,
	/* Transparent */ true
};

const RGBC ColorsSmilie1[]{
	/* __ */ RGB_INVALID_COLOR,
	/* XX */ RGB_BLACK,
	/* oo */ RGB_YELLOW
};
CPalette PalSmilie1{ ColorsSmilie1 };
const BPP2_DAT acSmilie1[]{
	________,XXXXXXXX,XX______,________,
	______XX,oooooooo,ooXX____,________,
	____XXoo,oooooooo,ooooXX__,________,
	__XXooXX,XXoooooo,XXXXooXX,________,
	XXooooXX,XXoooooo,XXXXoooo,XX______,
	XXoooooo,ooooXXoo,oooooooo,XX______,
	XXoooooo,ooooXXoo,oooooooo,XX______,
	XXoooooo,ooooXXoo,oooooooo,XX______,
	XXoooooo,oooooooo,oooooooo,XX______,
	__XXoooo,XXoooooo,XXooooXX,________,
	____XXoo,ooXXXXXX,ooooXX__,________,
	______XX,oooooooo,ooXX____,________,
	________,XXXXXXXX,XX______,________,
};
CBitmap bmSmilie1{
	/* Size */ { 13, 13 },
	/* BytesPerLine */ 4,
	/* Bits */ acSmilie1,
	/* Palette */ &PalSmilie1,
	/* Transparent */ true
};

static int _GetItemSizeX(ListBox *pWin, int ItemIndex) {
	return GUI_GetStringDistX(pWin->ItemText(ItemIndex)) 
		+ bmSmilie0.Size.x + 16;
}
static int _GetItemSizeY(ListBox *pWin, int ItemIndex) {
	int DistY = GUI.Font()->YDist + 1;
	if (pWin->MultiSel()) {
		if (pWin->ItemSel(ItemIndex))
			DistY += 8;
	}
	else if (pWin->Sel() == ItemIndex)
		DistY += 8;
	return DistY;
}

static int _OwnerDraw(const WIDGET_ITEM_DRAW_INFO *pDrawItemInfo) {
	auto pWin = (ListBox *)pDrawItemInfo->pWin;
	auto Index = pDrawItemInfo->ItemIndex;
	switch (pDrawItemInfo->Cmd) {
		case WIDGET_ITEM_GET_XSIZE:
			return _GetItemSizeX(pWin, Index);
		case WIDGET_ITEM_GET_YSIZE:
			return _GetItemSizeY(pWin, Index);
		case WIDGET_ITEM_DRAW:
		{
			RGBC aColor[4]{ RGB_BLACK, RGB_WHITE, RGB_WHITE, RGB_GRAY };
			RGBC aBkColor[4]{ RGB_WHITE, RGB_GRAY, RGB_BLUE, RGBC_GRAY(0xC0) };
			auto bEnabled = pWin->ItemEnabled(pDrawItemInfo->ItemIndex);
			auto bSelected = pWin->ItemSel(Index);
			auto bMultiSel = pWin->MultiSel();
			auto Sel = pWin->Sel();
			auto YSize = _GetItemSizeY(pWin, Index);
			auto ColorIndex =
				 bMultiSel ? bEnabled ? bSelected ? 2 : 0 : 3 :
				 bEnabled ? pDrawItemInfo->ItemIndex == Sel ?
							pDrawItemInfo->pWin->Focussed() ? 2 : 1 : 0 : 3;
			GUI.BkColor(aBkColor[ColorIndex]);
			GUI.PenColor(aColor[ColorIndex]);
			GUI.Clear();
			auto FontDistY = GUI.Font()->YDist;
			GUI_DispStringAt(pWin->ItemText(pDrawItemInfo->ItemIndex),
							 { pDrawItemInfo->x0 + bmSmilie0.Size.x + 16,
							   pDrawItemInfo->y0 + (YSize - FontDistY) / 2 });
			auto pBm = bMultiSel ? bSelected ?
				&bmSmilie1 : &bmSmilie0 :
					pDrawItemInfo->ItemIndex == Sel ?
						&bmSmilie1 : &bmSmilie0;
			GUI.DrawBitmap(
				*pBm,
				{ pDrawItemInfo->x0 + 7,
				  pDrawItemInfo->y0 + (YSize - pBm->Size.y) / 2 });
			if (bMultiSel && pDrawItemInfo->ItemIndex == Sel) {
				SRect rFocus{
					pDrawItemInfo->x0,
					pDrawItemInfo->y0,
					pDrawItemInfo->pWin->InsideRect().x1,
					pDrawItemInfo->y0 + YSize - 1
				};
				GUI.PenColor(RGB_WHITE - aBkColor[ColorIndex]);
				GUI.OutlineFocus(rFocus);
			}
			break;
		}
		default:
			return ListBox::OwnerDrawProc(pDrawItemInfo);
	}
	return 0;
}

static bool _bMultiSel = false;
static bool _bOwnerDrawn = true;

static void _cbFrame(WM_MSG *pMsg) {
	auto pDlg = pMsg->pWin;
	auto pListBox = (ListBox *)pDlg->DialogItem(GUI_ID_MULTIEDIT0);
	switch (pMsg->msgid) {
		case WM_INIT_DIALOG:
			pListBox->ScrollStepH(6);
			pListBox->AutoScrollH(true);
			pListBox->AutoScrollV(true);
			pListBox->OwnerDraw(_OwnerDraw);
			((CheckBox *)pDlg->DialogItem(GUI_ID_CHECK1))->CheckState(1);
			break;
		case WM_KEY:
			switch (((WM_KEY_INFO *)(pMsg->data))->Key) {
				case GUI_KEY_ESCAPE:
					pDlg->DialogEnd(1);
					break;
				case GUI_KEY_ENTER:
					pDlg->DialogEnd(0);
					break;
			}
			break;
		case WM_TOUCH_CHILD:
			pListBox->Focus();
			break;
		case WM_NOTIFY_PARENT:
			switch ((int)pMsg->data) {
				case WM_NOTIFICATION_SEL_CHANGED:
					pListBox->InvalidateItem(LISTBOX_ALL_ITEMS);
					break;
				case WM_NOTIFICATION_RELEASED:
					switch (pMsg->pWinSrc->ID()) {
						case GUI_ID_OK:
							pDlg->DialogEnd(0);
							break;
						case GUI_ID_CANCEL:
							pDlg->DialogEnd(1);
							break;
						case GUI_ID_CHECK0:
							pListBox->MultiSel(_bMultiSel = !_bMultiSel);
							pListBox->Focus();
							pListBox->InvalidateItem(LISTBOX_ALL_ITEMS);
							break;
						case GUI_ID_CHECK1:
							if (_bOwnerDrawn = !_bOwnerDrawn)
								pListBox->OwnerDraw(_OwnerDraw);
							else
								pListBox->OwnerDraw(nullptr);
							pListBox->InvalidateItem(LISTBOX_ALL_ITEMS);
							break;
					}
					break;
			}
			break;
		default:
			WObj::DefCallback(pMsg);
	}
}

static const WM_CREATESTRUCT aDialogCreate[]{
/*    Class             , x		, y		, xsize	, ysize	, Caption				 , Id				, Flags	, ExFlags				, Para	*/
	{ WCLS_FRAME		, 50	, 50	, 220	, 180	, "Owner drawn list box" , 0				, 0		, FRAME_CF_MOVEABLE			 	},
	{ WCLS_LISTBOX		, 10	, 10	, 100	, 100	, "English\0"
														  "Deutsch\0"
														  "Francis\0"
														  "Japanese\0"
														  "Italiano\0"
														  "Espan\0"
														  "Greek\0"
														  "Hebrew\0"
														  "Dutch\0"
														  "Other language ...\0" , GUI_ID_MULTIEDIT0, 0		, LISTBOX_CF_AUTOSCROLLBAR_H	},
	{ WCLS_CHECKBOX		, 120	, 10	, 80	, 15	, "Multi select"		 , GUI_ID_CHECK0											},
	{ WCLS_CHECKBOX		, 120	, 35	, 80	, 15	, "Owner drawn"			 , GUI_ID_CHECK1											},
	{ WCLS_BUTTON		, 120	, 65	, 80	, 20	, "OK"					 , GUI_ID_OK												},
	{ WCLS_BUTTON		, 120	, 90	, 80	, 20	, "Cancel"				 , GUI_ID_CANCEL											},
	{ WCLS_DROPDOWN		, 10	, 100	, 100	, 20	, "Alfa\0Beta\0"		 , 0														},
	{}
};

void MainTasfk() {
	for (;;)
		WObj::Exec();
}

void MainTask() {
	_bMultiSel = false;
	_bOwnerDrawn = true;
	auto pDlg = aDialogCreate->CreateDialog(_cbFrame, 0, 0, 0);

	auto pFrame = new Frame(
		0, 0, 250, 250,
		WObj::Desktop(), WC_VISIBLE,
		0, 0, "CheckBox & Radio");
	pFrame->Moveable(true);
	auto pCheck0 = new CheckBox(50, 50, 145, 15, pFrame->Client(), 0, 0, "CheckBox 0");
	pCheck0->NumStates(3);
	pCheck0->Visible(true);
	auto pCheck1 = new CheckBox(50, 50 + 15, 145, 15, pFrame->Client(), 0, 0, "CheckBox 1");
	pCheck1->Visible(true);
	auto pCheck2 = new CheckBox(50, 50 + 15 * 2, 145, 15, pFrame->Client(), 0, 0, "CheckBox 2");
	pCheck2->Visible(true);
	auto pRadios = new Radio(50, 50 + 15 * 3, 145, 15 * 3, pFrame->Client(), 0, 0, 0, 3, 15);
	pRadios->Text(0, "Radio 0");
	pRadios->Text(1, "Radio 1");
	pRadios->Text(2, "Radio 2");
	pRadios->Visible(true);
	auto pBtn = new Button(35, 170, 40, 30, pFrame->Client(), 0, 0, "Click");
	pBtn->Visible(true);
	pFrame->Visible(true);

	for (;;) {
		WObj::Exec();
	}
}
