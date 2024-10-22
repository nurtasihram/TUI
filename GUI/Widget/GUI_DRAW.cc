#include "GUI.h"

#include "Widget.h"
void GUI_DRAW__Draw(GUI_DRAW *pDrawObj, int x, int y) {
	if (pDrawObj)
		pDrawObj->pConsts->pfDraw(pDrawObj, x, y);
}
int GUI_DRAW__GetXSize(GUI_DRAW *pDrawObj) {
	return pDrawObj->pConsts->pfGetXSize(pDrawObj);
}
int GUI_DRAW__GetYSize(GUI_DRAW *pDrawObj) {
	return pDrawObj->pConsts->pfGetYSize(pDrawObj);
}
