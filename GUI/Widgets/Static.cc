#include "Static.h"

void Static::_OnPaint() const {
	GUI.BkColor(BkColorProp(Props.bkColor));
	GUI.Clear();
	if (!text) return;
	GUI.TextMode(DRAWMODE_TRANS);
	GUI.PenColor(Props.textColor);
	GUI.Font(Props.pFont);
	GUI_DispStringInRect(text, ClientRect(), Props.Align);
}
void Static::_Callback(WObj *pWin, int msgid, WM_PARAM *pData, WObj *pWinSrc) {
	auto pObj = (Static *)pWin;
	if (!pObj->HandleActive(msgid, pData))
		return;
	switch (msgid) {
	case WM_PAINT:
		pObj->_OnPaint();
		return;
	case WM_DELETE:
		pObj->~Static();
		break;
	}
	DefCallback(pObj, msgid, pData, pWinSrc);
}

Static::Static(int x0, int y0, int xsize, int ysize,
			   WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
			   const char *pText) :
	Widget(x0, y0, xsize, ysize, _Callback, pParent, Id, Flags),
	text(pText) {}
