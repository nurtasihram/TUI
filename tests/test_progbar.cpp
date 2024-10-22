#include "Frame.h"
#include "Button.h"
#include "ProgBar.h"

void _cbDlg(WM_MSG *pMsg) {
	auto pDlg = pMsg->pWin;
	auto pPrg = ((ProgBar *)pDlg->DialogItem(GUI_ID_PROGBAR0));
	switch (pMsg->msgid) {
		case WM_NOTIFY_PARENT:
			switch ((int)pMsg->data) {
				case WM_NOTIFICATION_RELEASED:
					switch (pMsg->pWinSrc->ID()) {
						case 1:
							pPrg->Value(pPrg->Value() + 1);
							break;
						case 2:
							pPrg->Value(pPrg->Value() - 1);
							break;
					}
					break;
			}
			break;
		default:
			break;
	}
}

void MainTask() {
	auto pFrame = new Frame(
		0, 0, 250, 150,
		WObj::Desktop(), WC_VISIBLE, 0, 0,
		"CheckBox & Radio", _cbDlg);
	pFrame->Moveable(true);
	
	auto pProg = new ProgBar(
		50, 50, 150, 20,
		pFrame->Client(), GUI_ID_PROGBAR0);
	pProg->Range(0, 100);
	pProg->Visible(true);

	auto pBtn1 = new Button(
		50, 75, 25, 25,
		pFrame->Client(), 1, 0,
		"+");
	pBtn1->Visible(true);

	auto pBtn2 = new Button(
		100, 75, 25, 25,
		pFrame->Client(), 2, 0,
		"-");
	pBtn2->Visible(true);

	pFrame->Visible(true);

	for (;;)
		WObj::Exec();
}
