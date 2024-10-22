#include "GUI.h"

static RGBC         *_pBuffer;
static SRect          _Rect;
static bool          _CursorIsVis;
static bool          _CursorOn;
static const Cursor *_pCursor;
static uint8_t       _CursorDeActCnt;
static int           _AllocSize;
static int           _x, _y;              /* Position of hot spot */
static SRect          _ClipRect;
static RGBC          _ColorIndex[4];      /* Color-Cache */
static void _SetPixel(int x, int y, RGBC Color) {
	if (y >= _ClipRect.y0 && y <= _ClipRect.y1)
		if (x >= _ClipRect.x0 && x <= _ClipRect.x1)
			LCD_SetPixel(x, y, Color);
}
static RGBC _GetPixel(int x, int y) {
	if (y >= _ClipRect.y0 && y <= _ClipRect.y1)
		if (x >= _ClipRect.x0 && x <= _ClipRect.x1)
			return LCD_GetPixel(x, y);
	return 0;
}
static void _Undraw(void) {
	if (!_pBuffer)
		return;
	RGBC *pData = _pBuffer;
	int xSize = _Rect.x1 - _Rect.x0 + 1,
		ySize = _Rect.y1 - _Rect.y0 + 1;
	int y;
	for (y = 0; y < ySize; y++) {
		int x;
		for (x = 0; x < xSize; x++)
			_SetPixel(x + _Rect.x0, y + _Rect.y0, *(pData + x));
		pData += _pCursor->pBitmap->XSize;
	}
}
static int _Log2Phys(int Index) {
	if (Index < 4)
		return _ColorIndex[Index];
	return _pCursor->pBitmap->pPal->pPalEntries[Index];
}
static void _Draw(void) {
	if (!_pBuffer)
		return;
	const Bitmap *pBM = _pCursor->pBitmap;
	RGBC *pData = _pBuffer;
	int xSize = _Rect.x1 - _Rect.x0 + 1,
		ySize = _Rect.y1 - _Rect.y0 + 1;
	int y;
	for (y = 0; y < ySize; y++) {
		int x;
		for (x = 0; x < xSize; x++) {
			RGBC BitmapPixel = GUI_GetBitmapPixel(pBM, x, y);
			pData[x] = _GetPixel(_Rect.x0 + x, _Rect.y0 + y);
			if (BitmapPixel)
				_SetPixel(_Rect.x0 + x, _Rect.y0 + y, _Log2Phys(BitmapPixel));
		}
		pData += pBM->XSize;
	}
}
static void _CalcRect(void) {
	if (!_pCursor)
		return;
	_Rect.x0 = _x - _pCursor->xHot;
	_Rect.y0 = _y - _pCursor->yHot;
	_Rect.x1 = _Rect.x0 + _pCursor->pBitmap->XSize - 1;
	_Rect.y1 = _Rect.y0 + _pCursor->pBitmap->YSize - 1;
}
static void _Hide(void) {
	if (!_CursorIsVis)
		return;
	_Undraw();
	_CursorIsVis = false;
}
static void _Show(void) {
	if (!_CursorOn || _CursorDeActCnt)
		return;
	_CursorIsVis = true;
	_Draw();
}
bool GUI_CURSOR__TempHide(const SRect *pRect) {
	if (!_CursorIsVis || !pRect)
		return false;
	if (!(pRect && _Rect))
		return false;
	_Hide();
	return true;
}
void GUI_CURSOR__TempUnhide(void) {
	_Show();
}
void GUI_CURSOR_Activate(void) {
	if (!--_CursorDeActCnt)
		_Show();
}
void GUI_CURSOR_Deactivate(void) {
	if (!_CursorDeActCnt++)
		_Hide();
}
const Cursor *GUI_CURSOR_Select(const Cursor *pCursor) {
	const Cursor *pOldCursor = _pCursor;
	if (pCursor == _pCursor)
		return _pCursor;
	const Bitmap *pBM = pCursor->pBitmap;
	int i = pBM->pPal->NumEntries > 4 ? 4 : pBM->pPal->NumEntries;
	while (i--)
		_ColorIndex[i] = pBM->pPal->pPalEntries[i];
	_Hide();
	int AllocSize = pBM->XSize * pBM->YSize * sizeof(RGBC);
	if (AllocSize != _AllocSize) {
		GUI_ALLOC_Free(_pBuffer);
		_pBuffer = nullptr;
	}
	_pBuffer = (RGBC *)GUI_ALLOC_AllocZero(AllocSize);
	_CursorOn = true;
	_pCursor = pCursor;
	_CalcRect();
	_Show();
	return pOldCursor;
}
void GUI_CURSOR_Hide(void) {
	_Hide();
	_CursorOn = false;
}
void GUI_CURSOR_Show(void) {
	_ClipRect = LCD_Rect();
	_Hide();
	_CursorOn = true;
	if (!_pCursor)
		GUI_CURSOR_Select(GUI_DEFAULT_CURSOR);
	else
		_Show();
}
void GUI_CURSOR_SetPosition(int xNewPos, int yNewPos) {
	if (!_pBuffer)
		return;
	if (_x == xNewPos && _y == yNewPos)
		return;
	if (!_CursorOn) {
		_x = xNewPos;
		_y = yNewPos;
		_CalcRect();
		return;
	}
	const Bitmap *pBM = _pCursor->pBitmap;
	RGBC *pData = _pBuffer;
	int xSize = _pCursor->pBitmap->XSize;
	int xStart, xStep, xEnd, xOverlapMin, xOverlapMax;
	int xOff = xNewPos - _x;
	if (xOff > 0) {
		xStep = 1;
		xStart = 0;
		xEnd = _pCursor->pBitmap->XSize;
		xOverlapMax = xEnd - 1;
		xOverlapMin = xOff;
	}
	else {
		xStep = -1;
		xStart = xSize - 1;
		xEnd = -1;
		xOverlapMin = 0;
		xOverlapMax = xStart + xOff;
	}
	int yStart, yStep, yEnd, yOverlapMin, yOverlapMax;
	int yOff = yNewPos - _y;
	if (yOff > 0) {
		yStep = 1;
		yStart = 0;
		yEnd = _pCursor->pBitmap->YSize;
		yOverlapMax = yEnd - 1;
		yOverlapMin = yOff;
	}
	else {
		yStep = -1;
		yStart = _pCursor->pBitmap->YSize - 1;
		yEnd = -1;
		yOverlapMin = 0;
		yOverlapMax = yStart + yOff;
	}
	int y;
	for (y = yStart; y != yEnd; y += yStep) {
		int yNew = y + yOff;
		char yOverlaps = y >= yOverlapMin && y <= yOverlapMax,
			 yNewOverlaps = yNew >= yOverlapMin && yNew <= yOverlapMax;
		int x;
		for (x = xStart; x != xEnd; x += xStep) {
			RGBC *pSave = pData + x + y * xSize;
			int xNew = x + xOff;
			
			RGBC BitmapPixel = GUI_GetBitmapPixel(pBM, x, y);
			
			char xyOverlaps = x >= xOverlapMin && x <= xOverlapMax && yOverlaps,
				 xyNewOverlaps = xNew >= xOverlapMin && xNew <= xOverlapMax && yNewOverlaps;
			if (BitmapPixel)
				if (!xyOverlaps || (GUI_GetBitmapPixel(pBM, x - xOff, y - yOff) == 0))
					_SetPixel(x + _Rect.x0, y + _Rect.y0, *(pSave));
			RGBC Pixel = xyNewOverlaps ?
				*(pData + xNew + yNew * xSize) :
				_GetPixel(_Rect.x0 + xNew, _Rect.y0 + yNew);
			*pSave = Pixel;
			if (BitmapPixel) {
				_SetPixel(_Rect.x0 + xNew, _Rect.y0 + yNew, _Log2Phys(BitmapPixel));
			}
		}
	}
	_x = xNewPos;
	_y = yNewPos;
	_CalcRect();
}
