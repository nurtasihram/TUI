#include "GUI.h"

static const RGBC _aColorI[]{ RGB_BLUE, RGB_WHITE, RGB_BLACK };
CPalette GUI_CursorPalI{ _aColorI };

static const RGBC _aColor[]{ RGB_BLUE, RGB_BLACK, RGB_WHITE };
CPalette GUI_CursorPal{ _aColor };

#pragma region Cursor Control
void CursorCtl::_SetPixel(int x, int y, RGBC Color) {
	if (y >= _ClipRect.y0 && y <= _ClipRect.y1)
		if (x >= _ClipRect.x0 && x <= _ClipRect.x1)
			LCD_SetPixel({ x, y }, Color);
}
RGBC CursorCtl::_GetPixel(int x, int y) {
	if (y >= _ClipRect.y0 && y <= _ClipRect.y1)
		if (x >= _ClipRect.x0 && x <= _ClipRect.x1)
			return LCD_GetPixel({ x, y });
	return 0;
}
void CursorCtl::_Draw() {
	if (!_pBuffer)
		return;
	auto pBM = _pCursor;
	auto pData = _pBuffer;
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
		pData += pBM->Size.x;
	}
}
void CursorCtl::_Undraw() {
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
		pData += _pCursor->Size.x;
	}
}

bool CursorCtl::_TempHide(const SRect *pRect) {
	if (!_CursorIsVis || !pRect)
		return false;
	if (!(pRect && _Rect))
		return false;
	_Hide();
	return true;
}
void CursorCtl::_TempShow() {
	_Show();
}

CCursor *CursorCtl::Select(CCursor *pCursor) {
	auto pOldCursor = _pCursor;
	if (pCursor == _pCursor)
		return _pCursor;
	auto i = pCursor->pPal->NumEntries > 4 ? 4 : pCursor->pPal->NumEntries;
	while (i--)
		_ColorIndex[i] = pCursor->pPal->pPalEntries[i];
	_Hide();
	int AllocSize = pCursor->Size.x * pCursor->Size.y * sizeof(RGBC);
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

void CursorCtl::Activate(bool bActivate)  {
	if (bActivate) {
		if (!--_CursorDeActCnt)
			_Show();
	}
	else if (!_CursorDeActCnt++)
		_Hide();
}
void CursorCtl::Visible(bool bVisible) {
	if (bVisible) {
		_ClipRect = LCD_Rect();
		_Hide();
		_CursorOn = true;
		if (!_pCursor)
			Select(GUI_DEFAULT_CURSOR);
		else
			_Show();
	}
	else {
		_Hide();
		_CursorOn = false;
	}
}
void CursorCtl::Position(Point nPos) {
	if (!_pBuffer)
		return;
	if (Pos == nPos)
		return;
	if (!_CursorOn) {
		Pos = nPos;
		_CalcRect();
		return;
	}
	auto pData = _pBuffer;
	int xSize = _pCursor->Size.x;
	int xStart, xStep, xEnd, xOverlapMin, xOverlapMax;
	int yStart, yStep, yEnd, yOverlapMin, yOverlapMax;
	auto Off = nPos - Pos;
	if (Off.x > 0) {
		xStep = 1;
		xStart = 0;
		xEnd = _pCursor->Size.x;
		xOverlapMax = xEnd - 1;
		xOverlapMin = Off.x;
	}
	else {
		xStep = -1;
		xStart = xSize - 1;
		xEnd = -1;
		xOverlapMin = 0;
		xOverlapMax = xStart + Off.x;
	}
	if (Off.y > 0) {
		yStep = 1;
		yStart = 0;
		yEnd = _pCursor->Size.y;
		yOverlapMax = yEnd - 1;
		yOverlapMin = Off.y;
	}
	else {
		yStep = -1;
		yStart = _pCursor->Size.y - 1;
		yEnd = -1;
		yOverlapMin = 0;
		yOverlapMax = yStart + Off.y;
	}
	int y;
	for (y = yStart; y != yEnd; y += yStep) {
		int yNew = y + Off.y;
		char yOverlaps = y >= yOverlapMin && y <= yOverlapMax,
			 yNewOverlaps = yNew >= yOverlapMin && yNew <= yOverlapMax;
		int x;
		for (x = xStart; x != xEnd; x += xStep) {
			RGBC *pSave = pData + x + y * xSize;
			int xNew = x + Off.x;
			RGBC BitmapPixel = GUI_GetBitmapPixel(_pCursor, x, y);
			char xyOverlaps = x >= xOverlapMin && x <= xOverlapMax && yOverlaps,
				 xyNewOverlaps = xNew >= xOverlapMin && xNew <= xOverlapMax && yNewOverlaps;
			if (BitmapPixel)
				if (!xyOverlaps || (GUI_GetBitmapPixel(_pCursor, x - Off.x, y - Off.y) == 0))
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
	Pos = nPos;
	_CalcRect();
}
#pragma endregion
