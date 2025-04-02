#include "Static.h"

void Static::_OnPaint() const {
	GUI.BkColor(BkColorProp(Props.BkColor));
	GUI.Clear();
	if (!text) return;
	GUI.PenColor(Props.Color);
	GUI.Font(Props.pFont);
	GUI.TextAlign(Props.Align);
	GUI.DrawStringIn(text, ClientRect());
}
WM_RESULT Static::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
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
		case WM_GET_CLASS:
			return ClassNames[WCLS_STATIC];
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

Static::Static(const SRect &rc,
			   PWObj pParent, uint16_t Id,
			   WM_CF Flags, STATIC_CF FlagsEx,
			   const char *pText) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags),
	text(pText)
{ Props.Align = (TEXTALIGN)FlagsEx; }
