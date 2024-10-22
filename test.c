#include <stdlib.h>

#include "GUI.h"
#include "DIALOG.h"

#include "touch.h"

void _cbBkWindow(WM_MESSAGE *pMsg) {

	switch (pMsg->MsgId) {

	case WM_PAINT: {

		GUI_SetBkColor(GUI_BLACK);
		GUI_Clear();

		break;

	}
	
	default:
		WM_DefaultProc(pMsg);

	}

}
