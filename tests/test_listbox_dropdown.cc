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

static void _cbFrame(WObj *pDlg, int msgid, WM_PARAM *pData, WObj *pWinSrc) {
	auto pListBox = (ListBox *)pDlg->DialogItem(GUI_ID_MULTIEDIT0);
	switch (msgid) {
		case WM_INIT_DIALOG:
			pListBox->ScrollStepH(6);
			pListBox->AutoScrollH(true);
			pListBox->AutoScrollV(true);
			pListBox->OwnerDraw(_OwnerDraw);
			((CheckBox *)pDlg->DialogItem(GUI_ID_CHECK1))->CheckState(1);
			break;
		case WM_KEY:
			switch (((KEY_STATE *)pData)->Key) {
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
			switch ((int)*pData) {
				case WN_SEL_CHANGED:
					pListBox->InvalidateItem(LISTBOX_ALL_ITEMS);
					break;
				case WN_RELEASED:
					switch (pWinSrc->ID()) {
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
			WObj::DefCallback(pDlg, msgid, pData, pWinSrc);
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
	for (;;) {
		_bMultiSel = false;
		_bOwnerDrawn = true;
		aDialogCreate->DialogBox(_cbFrame, 0, 0, 0);
	}
}
