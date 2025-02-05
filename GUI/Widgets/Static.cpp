#include "Static.h"

void Static::_OnPaint() const {
	GUI.BkColor(BkColorProp(Props.BkColor));
	GUI.Clear();
	if (!text) return;
	GUI.PenColor(Props.Color);
	GUI.Font(Props.pFont);
	GUI.TextAlign(Props.Align);
	GUI.DispString(text, ClientRect());
}
WM_RESULT Static::_Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc) {
	auto pObj = (Static *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	switch (MsgId) {
		case WM_PAINT:
			pObj->_OnPaint();
			return 0;
		case WM_DELETE:
			pObj->~Static();
			return 0;
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

Static::Static(int x0, int y0, int xsize, int ysize,
			   WObj *pParent, uint16_t Id,
			   WM_CF Flags, uint16_t ExFlags,
			   const char *pText) :
	Widget(x0, y0, xsize, ysize,
		   _Callback,
		   pParent, Id,
		   Flags),
	text(pText)
{ Props.Align = (TEXTALIGN)ExFlags; }
