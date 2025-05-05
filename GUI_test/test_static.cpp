#include "Static.h"
#include "Radio.h"


void MainTask() {
	static auto pText = new Static(
		SRect::left_top({ 25, 35 }, { 290, 145 }),
		nullptr, 0,
		WC_VISIBLE, TEXTALIGN_LEFT,
		"Read two news stories every day.\n"
		"All short news stories \n"
		" are written for quick readability and learning.");
	pText->BkColor(RGB_CYAN);
	static auto pHorizon = new Radio(
		SRect::left_top({ 25, 35 + 145 + 5 }, { 60, 15 * 3 }),
		nullptr, GUI_ID_RADIO0,
		WC_VISIBLE, 0,
		"Left\0"
		"HCenter\0"
		"Right\0", 15);
	static auto pVertical = new Radio(
		SRect::left_top({ 25 + 60, 35 + 145 + 5 }, { 60, 15 * 3 }),
		nullptr, GUI_ID_RADIO1,
		WC_VISIBLE, 0,
		"Top\0"
		"VCenter\0"
		"Bottom\0", 15);
	WObj::Desktop()->Callback([](PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) -> WM_RESULT {
		if (MsgId == WM_NOTIFY_CHILD)
			if ((int)Param == WN_VALUE_CHANGED)
				switch (pSrc->ID()) {
					case GUI_ID_RADIO0:
					case GUI_ID_RADIO1: {
						TEXTALIGN align = 0;
						switch (pVertical->Sel()) {
							case 0:
								align = TEXTALIGN_TOP;
								break;
							case 1:
								align = TEXTALIGN_VCENTER;
								break;
							case 2:
								align = TEXTALIGN_BOTTOM;
								break;
						}
						switch (pHorizon->Sel()) {
							case 0:
								align |= TEXTALIGN_LEFT;
								break;
							case 1:
								align |= TEXTALIGN_HCENTER;
								break;
							case 2:
								align |= TEXTALIGN_RIGHT;
								break;
						}
						pText->TextAlign(align);
						break;
					}
				}
		return WObj::DesktopCallback(pWin, MsgId, Param, pSrc);
	});
	for (;;)
		WObj::Exec();
}
