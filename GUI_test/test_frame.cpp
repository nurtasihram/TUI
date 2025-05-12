#include "Widgets/Widgets.inl"

enum {
	ID_LEFT = GUI_ID_BUTTON0,
	ID_HCENTER,
	ID_RIGHT,
	ID_TOP,
	ID_VCENTER,
	ID_BOTTOM
};

Frame *pFrm1, *pFrm2;
bool _cbFrame(PWObj pDlg, int MsgId, WM_PARAM &Param, PWObj pSrc) {
	switch (MsgId) {
		case WM_NOTIFY_CHILD:
			switch ((int)Param) {
				case WN_RELEASED: {
					auto &&rcFrm1 = pFrm1->Rect();
					auto &&rcFrm2 = pFrm2->Rect();
					switch (pSrc->ID()) {
						case ID_LEFT:
							rcFrm1.AlignTo(ALIGN_LEFT, rcFrm2);
							break;
						case ID_HCENTER:
							rcFrm1.AlignTo(ALIGN_HCENTER, rcFrm2);
							break;
						case ID_RIGHT:
							rcFrm1.AlignTo(ALIGN_RIGHT, rcFrm2);
							break;
						case ID_TOP:
							rcFrm1.AlignTo(ALIGN_TOP, rcFrm2);
							break;
						case ID_VCENTER:
							rcFrm1.AlignTo(ALIGN_VCENTER, rcFrm2);
							break;
						case ID_BOTTOM:
							rcFrm1.AlignTo(ALIGN_BOTTOM, rcFrm2);
							break;
						default:
							return 0;
					}
					pFrm1->Position(rcFrm1.left_top());
					break;
				}
			}
			break;
		default:
			return false;
	}
	return true;
}

static const WM_CREATESTRUCT aDialogCreate[]{
/*    Class             , x		, y		, xsize	, ysize	, Caption	 , Id			, Flags			*/
	{ WCLS_FRAME		, 0		, 0		, 145	, 100	, "Align"	 , 0			, WC_VISIBLE  	},
	{ WCLS_BUTTON		, 5		, 5		, 40	, 30	, "Left"	 , ID_LEFT						},
	{ WCLS_BUTTON		, 50	, 5		, 40	, 30	, "HCenter"	 , ID_HCENTER					},
	{ WCLS_BUTTON		, 95	, 5		, 40	, 30	, "Right"	 , ID_RIGHT						},
	{ WCLS_BUTTON		, 5		, 40	, 40	, 30	, "Top"		 , ID_TOP						},
	{ WCLS_BUTTON		, 50	, 40	, 40	, 30	, "VCenter"	 , ID_VCENTER					},
	{ WCLS_BUTTON		, 95	, 40	, 40	, 30	, "Bottom"	 , ID_BOTTOM					},
	{}
};

void MainTask() {

	new Button(SRect::left_top(0, { 30, 80 }), WObj::Desktop(), GUI_ID_BUTTON9 + 1, WC_VISIBLE);
	WObj::Desktop()->Callback([](PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) -> WM_RESULT {
		if (MsgId == WM_NOTIFY_CHILD)
			if ((int)Param == WN_RELEASED)
				switch (pSrc->ID()) {
					case GUI_ID_BUTTON9 + 1:
						WObj::Desktop()->Destroy();
						return 0;
				}
		return WObj::DesktopCallback(pWin, MsgId, Param, pSrc);
	});

	pFrm1 = new Frame(
		SRect::left_top({ 150, 100 }, { 100, 120 }),
		WObj::Desktop(), 0,
		WC_VISIBLE, FRAME_CF_RESIZEABLE,
		"Frame 1");
	pFrm2 = new Frame(
		SRect::left_top({ 197, 95 }, { 150, 80 }),
		WObj::Desktop(), 0,
		WC_VISIBLE, FRAME_CF_RESIZEABLE,
		"Frame 2");
	pFrm1->AddMaxButton();
	pFrm1->AddMinButton();
	aDialogCreate->CreateDialog(_cbFrame);
	while (WObj::Online())
		WObj::Exec();
}
