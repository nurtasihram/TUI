#include "Widgets/ProgBar.h"
#include "Widgets/Slider.h"

WM_RESULT _cbDlg(PWObj pDlg, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pPrg = ((ProgBar *)pDlg->DialogItem(GUI_ID_PROGBAR0));
	auto pSlr = ((Slider *)pDlg->DialogItem(GUI_ID_SLIDER0));
	switch (MsgId) {
		case WM_NOTIFY_CHILD:
			switch ((int)Param) {
				case WN_VALUE_CHANGED:
					switch (pSrc->ID()) {
						case GUI_ID_SLIDER0:
							pPrg->Value(pSlr->Value());
							break;
					}
					break;
			}
			return 0;
	}
	return WObj::DesktopCallback(pDlg, MsgId, Param, pSrc);
}

void MainTask() {
	auto pProg = new ProgBar(
		SRect::left_top({ 50, 50 }, { 150, 20 }),
		WObj::Desktop(), GUI_ID_PROGBAR0,
		WC_VISIBLE);
	auto pSlider = new Slider(
		SRect::left_top({ 50, 80 }, { 150, 20 }),
		WObj::Desktop(), GUI_ID_SLIDER0,
		WC_VISIBLE, 0);
	WObj::Desktop()->Callback(_cbDlg);
	for (;;)
		WObj::Exec();
}
