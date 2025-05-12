#include "Widgets/Frame.h"
#include "Widgets/CheckBox.h"
#include "Widgets/ListBox.h"
#include "Widgets/DropDown.h"
#include "Widgets/Radio.h"

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
	return GUI.XDist(pWin->ItemText(ItemIndex)) 
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

static int _OwnerDraw(PWObj pWin, WIDGET_ITEM_CMD Cmd, int16_t ItemIndex, SRect rItem) {
	auto pObj = (ListBox *)pWin;
	switch (Cmd) {
		case WIDGET_ITEM_GET_XSIZE:
			return _GetItemSizeX(pObj, ItemIndex);
		case WIDGET_ITEM_GET_YSIZE:
			return _GetItemSizeY(pObj, ItemIndex);
		case WIDGET_ITEM_DRAW:
		{
			auto pBm = pObj->MultiSel() ?
				pObj->ItemSel(ItemIndex) ? &bmSmilie1 : &bmSmilie0 :
				ItemIndex == pObj->Sel() ? &bmSmilie1 : &bmSmilie0;
			Point posBmp{ rItem.x0 + 2, rItem.y0 + (rItem.ysize() - pBm->Size.y) / 2 };
			rItem.x0 = posBmp.x + pBm->Size.x;
			GUI.TextAlign(TEXTALIGN_VCENTER);
			ListBox::DefOwnerDraw(pObj, Cmd, ItemIndex, rItem);
			GUI.DrawBitmap(*pBm, posBmp);
			break;
		}
		default:
			return ListBox::DefOwnerDraw(pObj, Cmd, ItemIndex, rItem);
	}
	return 0;
}

static bool _bMultiSel = false;
static bool _bOwnerDrawn = true;

static bool _cbFrame(PWObj pDlg, int MsgId, WM_PARAM &Param, PWObj pSrc) {
	switch (MsgId) {
		case WM_INIT_DIALOG: {
			auto pListBox = (ListBox *)pDlg->DialogItem(GUI_ID_MULTIEDIT0);
			pListBox->ScrollStepH(6);
			pListBox->OwnerDraw(_OwnerDraw);
			((CheckBox *)pDlg->DialogItem(GUI_ID_CHECK1))->CheckState(1);
			break;
		}
		case WM_KEY:
			switch (((KEY_STATE)Param).Key) {
				case GUI_KEY_ESCAPE:
					pDlg->DialogEnd(0);
					Param = 1;
					break;
				case GUI_KEY_ENTER:
					pDlg->DialogEnd(1);
					Param = 1;
					break;
				default:
					Param = 0;
					break;
			}
			break;
		case WM_NOTIFY_CHILD: {
			auto pListBox = (ListBox *)pDlg->DialogItem(GUI_ID_MULTIEDIT0);
			switch ((int)Param) {
				case WN_SEL_CHANGED:
					pListBox->InvalidateItem(LISTBOX_ALL_ITEMS);
					break;
				case WN_RELEASED:
					switch (pSrc->ID()) {
						case GUI_ID_OK:
							pDlg->DialogEnd(0);
							break;
						case GUI_ID_CANCEL:
							pDlg->DialogEnd(1);
							break;
						case GUI_ID_CHECK0:
							pListBox->MultiSel(_bMultiSel = !_bMultiSel);
							pListBox->InvalidateItem(LISTBOX_ALL_ITEMS);
							break;
						case GUI_ID_CHECK1:
							if ((_bOwnerDrawn = !_bOwnerDrawn))
								pListBox->OwnerDraw(_OwnerDraw);
							else
								pListBox->OwnerDraw(nullptr);
							pListBox->InvalidateItem(LISTBOX_ALL_ITEMS);
							break;
					}
					break;
				}
			}
			break;
		default:
			return false;
	}
	return true;
}

GUI_PCSTR pLang{
	"English\0"
	"Deutsch\0"
	"Francis\0"
	"Japanese\0"
	"Italiano\0"
	"Espan\0"
	"Greek\0"
	"Hebrew\0"
	"Dutch\0"
	"Other language ...\0"
};

static const WM_CREATESTRUCT aDialogCreate[]{
/*    Class             , x		, y		, xsize	, ysize	, Caption				 , Id				, Flags	, FlagsEx				, Para	*/
	{ WCLS_FRAME		, 50	, 50	, 220	, 165	, "Owner drawn list box" , 0				, 0		,							 	},
	{ WCLS_LISTBOX		, 10	, 10	, 100	, 100	,  pLang				 , GUI_ID_MULTIEDIT0, 0		, LISTBOX_CF_AUTOSCROLLBAR		},
	{ WCLS_CHECKBOX		, 120	, 10	, 85	, 18	, "Multi select"		 , GUI_ID_CHECK0											},
	{ WCLS_CHECKBOX		, 120	, 35	, 85	, 18	, "Owner drawn"			 , GUI_ID_CHECK1											},
	{ WCLS_BUTTON		, 120	, 65	, 85	, 20	, "OK"					 , GUI_ID_OK												},
	{ WCLS_BUTTON		, 120	, 90	, 85	, 20	, "Cancel"				 , GUI_ID_CANCEL											},
	{ WCLS_DROPDOWN		, 10	, 110	, 100	, 50	, pLang					 , 0				, 0		, DROPDOWN_CF_AUTOSCROLLBAR		},
	{}
};

void MainTask() {
	for (;;) {
		_bMultiSel = false;
		_bOwnerDrawn = true;
		aDialogCreate->DialogBox(_cbFrame);
	}
}
