#include "GUI.h"

#include "Widget.h"
static void _Draw(const GUI_DRAW *pObj, int x, int y) {
	(*pObj->Data.pfDraw)();
}
static int _GetXSize(const GUI_DRAW *pObj) {
	return 0;
}
static int _GetYSize(const GUI_DRAW *pObj) {
	return 0;
}
static const GUI_DRAW_CONSTS _ConstObjData = {
	_Draw,
	_GetXSize,
	_GetYSize
};
GUI_DRAW *GUI_DRAW_SELF_Create(GUI_DRAW_SELF_CB *pfDraw, int x, int y) {
	GUI_DRAW *pObj = (GUI_DRAW *)GUI_ALLOC_AllocZero(sizeof(GUI_DRAW));
	if (!pObj)
		return 0;
	pObj->pConsts = &_ConstObjData;
	pObj->Data.pfDraw = pfDraw;
	pObj->xOff = x;
	pObj->yOff = y;
	return pObj;
}
