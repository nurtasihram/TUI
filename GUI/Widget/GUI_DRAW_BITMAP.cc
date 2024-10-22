#include "GUI.h"

#include "Widget.h"
static void _DrawBitmap(const GUI_DRAW *pObj, int x, int y) {
	GUI_DrawBitmap((const Bitmap *)pObj->Data.pData, x + pObj->xOff, y + pObj->yOff);
}
static int _GetXSize(const GUI_DRAW *pObj) {
	return ((const Bitmap *)pObj->Data.pData)->XSize;
}
static int _GetYSize(const GUI_DRAW *pObj) {
	return ((const Bitmap *)pObj->Data.pData)->YSize;
}
static const GUI_DRAW_CONSTS _ConstObjData = {
	_DrawBitmap,
	_GetXSize,
	_GetYSize
};
GUI_DRAW *GUI_DRAW_BITMAP_Create(const Bitmap *pBitmap, int x, int y) {
	GUI_DRAW *pObj = (GUI_DRAW *)GUI_ALLOC_AllocZero(sizeof(GUI_DRAW));
	if (!pObj)
		return nullptr;
	pObj->pConsts = &_ConstObjData;
	pObj->Data.pData = (const void *)pBitmap;
	pObj->xOff = x;
	pObj->yOff = y;
	return pObj;
}
