#include "GUI.h"

GUI_PANEL GUI;
GUI_PANEL::Property GUI_PANEL::DefaultProps;

#pragma region Basic graphics
void GUI_PANEL::Clear() {
	DispPos(0);
	GUI.Clear({ GUI_XMIN, GUI_YMIN, GUI_XMAX, GUI_YMAX });
}
void GUI_PANEL::Clear(SRect r) {
	r += off;
	r &= GUI.rClip;
	if (!r) return;
	GUI.RevColor();
	LCD_FillRect(r);
	GUI.RevColor();
}
void GUI_PANEL::Fill(SRect r) {
	r += off;
	r &= GUI.rClip;
	if (!r) return;
	LCD_FillRect(r);
}
void GUI_PANEL::DrawFocus(SRect r, int Dist) {
	r /= Dist;
	for (auto i = r.x0; i <= r.x1; i += 2) {
		Fill(Point{ i, r.y0 });
		Fill(Point{ i, r.y1 });
	}
	for (auto i = r.y0; i <= r.y1; i += 2) {
		Fill(Point{ r.x0, i });
		Fill(Point{ r.x1, i });
	}
}
void GUI_PANEL::Outline(SRect r) {
	Fill({ r.x0, r.y0, r.x1, r.y0 });
	Fill({ r.x0, r.y1, r.x1, r.y1 });
	Fill({ r.x0, r.y0, r.x0, r.y1 });
	Fill({ r.x1, r.y0, r.x1, r.y1 });
}

void GUI_PANEL::DrawBitmap(CBitmap &bmp, Point Pos) {
	Pos += off;
	auto &&bc = bmp + Pos;
	if (bc &= GUI.rClip)
		LCD_SetBitmap(bc);
}
void GUI_LCD_SetBitmap(BitmapRect bc) {
	if (bc &= GUI.rClip)
		LCD_SetBitmap(bc);
}
#pragma endregion

#pragma region Cursor Control
static const RGBC _aColorI[]{ RGB_BLUE, RGB_WHITE, RGB_BLACK };
CPalette GUI_CursorPalI{ _aColorI };
static const RGBC _aColor[]{ RGB_BLUE, RGB_BLACK, RGB_WHITE };
CPalette GUI_CursorPal{ _aColor };
void CursorCtl::_Hide() {
	if (_CursorIsVis) {
		_Undraw();
		_CursorIsVis = false;
	}
}
void CursorCtl::_Draw() {
	auto brClipCursor = brCursor & LCD_Rect();
	brClip = (SRect)brClipCursor;
	LCD_GetBitmap(brClip);
	LCD_SetBitmap(brClipCursor);
}
void CursorCtl::_Undraw() {
	LCD_SetBitmap(brClip);
}
bool CursorCtl::_TempHide(const SRect *pRect) {
	if (!_CursorIsVis || !pRect)
		return false;
	if (!(pRect && LCD_Rect()))
		return false;
	_Hide();
	return true;
}
void CursorCtl::_TempShow() {
	if (_CursorOn && !_CursorDeActCnt) {
		_CursorIsVis = true;
		_Draw();
	}
}
CCursor *CursorCtl::operator()(CCursor *pCursor) {
	if (!pCursor)
		pCursor = GUI.DefaultProps.pCursor;
	auto pOldCursor = _pCursor;
	if (pCursor == _pCursor)
		return _pCursor;
	_Hide();
	LCD_AllocBitmap(brClip);
	if (pCursor) {
		_CursorOn = true;
		_pCursor = pCursor;
		brCursor = *pCursor;
		brCursor.move_to(Pos - pCursor->Hot);
		brClip = LCD_AllocBitmap(brCursor);
		_TempShow();
	}
	else
		_CursorOn = false;
	return pOldCursor;
}
void CursorCtl::Activate(bool bActivate) {
	if (bActivate)
		_TempShow();
	else if (!_CursorDeActCnt++)
		_Hide();
}
void CursorCtl::Visible(bool bVisible) {
	if (bVisible && _pCursor) {
		_Hide();
		_CursorOn = true;
		_TempShow();
	}
	else {
		_Hide();
		_CursorOn = false;
	}
}
void CursorCtl::Position(Point nPos) {
	if (Pos == nPos)
		return;
	if (!_CursorOn) {
		Pos = nPos;
		return;
	}
	_Undraw();
	auto dPos = nPos - Pos;
	Pos = nPos;
	brCursor += dPos;
	brClip += dPos;
	_Draw();
}
CCursor *CursorCtl::GetResizeCursor(uint8_t Mode) {
	switch (Mode & SIZE_RESIZE) {
		case SIZE_RESIZE_X:
		case SIZE_RESIZE_X | SIZE_REPOS_X:
			return &GUI_CursorResizeH;
		case SIZE_RESIZE_Y:
		case SIZE_RESIZE_Y | SIZE_REPOS_Y:
			return &GUI_CursorResizeV;
		case SIZE_RESIZE_X | SIZE_REPOS_X | SIZE_RESIZE_Y:
		case SIZE_RESIZE_X | SIZE_RESIZE_Y | SIZE_REPOS_Y:
			return &GUI_CursorResizeDU;
		case SIZE_RESIZE_X | SIZE_RESIZE_Y:
		case SIZE_RESIZE_X | SIZE_REPOS_X | SIZE_RESIZE_Y | SIZE_REPOS_Y:
			return &GUI_CursorResizeDD;
	}
	if (Mode & SIZE_MOUSEMOVE)
		return &GUI_CursorMove;
	return nullptr;
}
#pragma endregion

#pragma region String 
uint16_t GUI__NumTexts(const char *pTexts) {
	uint16_t NumTexts = 0;
	if (pTexts)
		for (; *pTexts; ++NumTexts)
			while (*pTexts++);
	return NumTexts;
}
const char *GUI__NextText(const char *pTexts) {
	while (*pTexts++);
	return pTexts;
}
uint16_t GUI__NumLines(const char *pText) {
	uint16_t NumLines = 1;
	for (; auto ch = *pText; ++pText)
		if (ch == '\n')
			++NumLines;
	return NumLines;
}
const char *GUI__NextLines(const char *pText) {
	for (; auto ch = *pText; ++pText)
		if (ch == '\n')
			return ++pText;
	return pText;
}
uint16_t GUI__NumCharsLine(const char *pText) {
	uint16_t NumChars = 1;
	for (; auto ch = *pText; ++pText)
		if (ch == '\n')
			break;
		else
			++NumChars;
	return NumChars;
}
uint16_t GUI__NumChars(const char *pText) {
	uint16_t NumChars = 0;
	for (; auto ch = *pText; ++pText)
		++NumChars;
	return NumChars;
}
void GUI__SetText(char **ppText, const char *pText) {
	if (!pText) {
		GUI_MEM_Free(*ppText);
		*ppText = nullptr;
		return;
	}
	auto pString = (char *)GUI_MEM_Alloc(GUI__NumChars(pText) + 1);
	GUI__strcpy(pString, pText);
	GUI_MEM_Free(*ppText);
	*ppText = pString;
}
#pragma endregion

#pragma region GUI_DispString
void GUI_DispString(const char *s) {
	if (!s)
		return;
	auto FontSizeY = GUI.Font()->YDist;
	auto xOrg = GUI.dispPos.x;
	auto yAdjust = GUI_GetYAdjust();
	GUI.dispPos.y -= yAdjust;
	for (; *s; s++) {
		auto LineNumChars = GUI__NumChars(s);
		auto xLineSize = GUI.XDist(s, LineNumChars);
		int xAdjust;
		switch (GUI.TextAlign() & TEXTALIGN_HCENTER) {
			case TEXTALIGN_HCENTER:
				xAdjust = xLineSize / 2;
				break;
			case TEXTALIGN_RIGHT:
				xAdjust = xLineSize;
				break;
			default:
				xAdjust = 0;
		}
		GUI__DispLine(s, LineNumChars, { GUI.dispPos.x - xAdjust, GUI.dispPos.y });
		s += LineNumChars;
		if (*s == '\n' || *s == '\r') {
			switch (GUI.TextAlign() & TEXTALIGN_HCENTER) {
			case TEXTALIGN_HCENTER:
			case TEXTALIGN_RIGHT:
				GUI.dispPos.x = xOrg;
				break;
			default:
				GUI.dispPos.x = 0;
			}
			if (*s == '\n')
				GUI.dispPos.y += FontSizeY;
		}
		else
			GUI.dispPos.x += xLineSize;
		if (*s == 0)
			break;
	}
	GUI.dispPos.y += yAdjust;
	GUI.TextAlign(GUI.TextAlign() & ~TEXTALIGN_HCENTER);
}
void GUI_DispStringAt(const char *s, Point Pos) {
	GUI.DispPos(Pos);
	GUI_DispString(s);
}
#pragma endregion

#pragma region Char
void GUI_DispNextLine() {
	GUI.dispPos.y += GUI.Font()->YDist;
	GUI.dispPos.x = 0;
}
int GUI_GetYAdjust() {
	switch (GUI.TextAlign() & TEXTALIGN_VCENTER) {
		case TEXTALIGN_VCENTER:
			return GUI.Font()->YSize / 2;
		case TEXTALIGN_BOTTOM:
			return GUI.Font()->YSize - 1;
	}
	return 0;
}

#include "WM.h"
SRect GUI_PANEL::DispString(const char *s, SRect rText) {
	if (!s) return {};
	Point start;
	auto pOldClipRect = WObj::UserClip(&rText);
	if (pOldClipRect) {
		auto &&r = rText & *pOldClipRect;
		WObj::UserClip(&r);
	}
	rText += off;
	uint16_t NumLines = GUI__NumLines(s);
	uint16_t DistY = GUI__NumLines(s) * Font()->YSize;
	switch (Props.TextAlign & TEXTALIGN_VCENTER) {
		case TEXTALIGN_VCENTER:
			dispPos.y = rText.y0 + (rText.dy() - DistY) / 2;
			break;
		case TEXTALIGN_BOTTOM:
			dispPos.y = rText.y1 - DistY;
			break;
		default:
			dispPos.y = rText.y0;
	}
	start.y = dispPos.y;
	uint16_t DistX = 0;
	for (int i = 0; i < NumLines; ++i) {
		auto NumCharsLine = GUI__NumCharsLine(s);
		switch (Props.TextAlign & TEXTALIGN_HCENTER) {
			case TEXTALIGN_HCENTER:
				dispPos.x = rText.x0 + (rText.dx() - XDist(s, NumCharsLine)) / 2;
				break;
			case TEXTALIGN_RIGHT:
				dispPos.x = rText.x1 - XDist(s, NumCharsLine);
				break;
			default:
				dispPos.x = rText.x0;
		}
		uint16_t xDist = 0, dispX = dispPos.x;
		while (--NumCharsLine)
			xDist += Font()->DispChar(*s++);
		if (DistX < xDist) {
			DistX = xDist;
			start.x = dispX;
		}
		auto ch = *s;
		if (ch == '\0')
			break;
		else if (ch == '\n')
			++s;
		dispPos.y += Font()->YDist;
	}
	WObj::UserClip(pOldClipRect);
	return SRect::left_top(start - off, { DistX, DistY });
}
void GUI_DispChar(uint16_t c) {
	auto &&r = SRect::left_top(GUI.dispPos,
							   { GUI.Font()->XDist(c),
								 GUI.Font()->YSize });
	WObj::IVR(r, [&] {
		if (c == '\n')
			GUI_DispNextLine();
		else if (c != '\r')
			GUI.Font()->DispChar(c);
	});
	if (c != '\n')
		GUI.dispPos.x = r.x1 + 1;
	GUI.dispPos -= GUI.off;
}
void GUI__DispLine(const char *s, int MaxNumChars, Point Off) {
	GUI.dispPos = Off + GUI.off;
	while (MaxNumChars--)
		GUI.Font()->DispChar(*s++);
	GUI.dispPos -= GUI.off;
}
#pragma endregion

#pragma region Font
int Font::XDist(const char *pString, int NumChars) const {
	if (!pString) return 0;
	int DistX = 0;
	for (; NumChars; --NumChars)
		 DistX += GUI.Font()->XDist(*pString++);
	return DistX;
}
Point Font::Size(const char *pText) const {
	uint16_t DistX = 0;
	auto NumLines = GUI__NumLines(pText);
	for (int i = 0; i < NumLines; ++i) {
		auto NumCharsLien = GUI__NumCharsLine(pText);
		auto xDist = XDist(pText, NumCharsLien);
		if (DistX < xDist)
			DistX = xDist;
	}
	return{ DistX, NumLines * YDist };
}
#pragma region Proportional Font
const FontProp::Prop *FontProp::_FindChar(uint16_t c) const {
	for (auto pProp = &prop; pProp; pProp = pProp->pNext)
		if (c >= pProp->First && c <= pProp->Last)
			return pProp;
	return nullptr;
}
int FontProp::DispChar(uint16_t c) const {
	auto pProp = _FindChar(c);
	if (!pProp)
		return 0;
	auto &ci = pProp->paCharInfo[c - pProp->First];
	auto &&Pal = GUI.Palette();
	GUI_LCD_SetBitmap({ {
			/* Size */ { ci.XSize, GUI.Font()->YSize },
			/* BytesPerLine */ ci.BytesPerLine,
			/* BitsPerPixel */ 1,
			/* Bits */ ci.pData,
			/* Palette */ &Pal,
			/* Transparent */ GUI.BkColor() == RGB_INVALID_COLOR
			}, GUI.dispPos });
	GUI.dispPos.x += ci.XDist;
	return ci.XDist;
}
int FontProp::XDist(uint16_t c) const {
	auto pProp = _FindChar(c);
	return pProp ? pProp->paCharInfo[c - pProp->First].XSize : 0;
}
bool FontProp::IsInFont(uint16_t c) const
{ return _FindChar(c); }
#pragma endregion
#pragma region Monospace Font
int FontMono::DispChar(uint16_t c) const {
	int c0 = -1, c1 = -1;
	const uint8_t *pd;
	auto FirstChar = this->FirstChar;
	if (FirstChar <= c && c <= LastChar) {
		pd = (const uint8_t *)pData;
		c0 = ((int)c) - FirstChar;
	}
	else {
		pd = (const uint8_t *)pTransData;
		if (pTrans) {
			FirstChar = pTrans->FirstChar;
			if (FirstChar <= c && c <= pTrans->LastChar) {
				auto ptl = pTrans->pList + c - pTrans->FirstChar;
				c0 = ptl->c0;
				c1 = ptl->c1;
			}
		}
	}
	if (c0 == -1)
		return 0;
	auto BytesPerChar = YSize * BytesPerLine;
	auto &&Pal = GUI.Palette();
	GUI_LCD_SetBitmap({ {
			/* Size */ { xSize, YSize },
			/* BytesPerLine */ BytesPerLine,
			/* BitsPerPixel */ 1,
			/* Bits */ pd + c0 * BytesPerChar,
			/* Palette */ &Pal,
			/* Transparent */ GUI.BkColor() == RGB_INVALID_COLOR
		}, GUI.dispPos });
	if (c1 != -1)
		GUI_LCD_SetBitmap({ {
				/* Size */ { xSize, YSize },
				/* BytesPerLine */ BytesPerLine,
				/* BitsPerPixel */ 1,
				/* Bits */ pd + c1 * BytesPerChar,
				/* Palette */ &Pal,
				/* Transparent */ true
			}, GUI.dispPos });
	GUI.dispPos.x += xDist;
	return xDist;
}
int FontMono::XDist(uint16_t) const { return xDist; }
bool FontMono::IsInFont(uint16_t c) const {
	if (FirstChar <= c && c <= LastChar)
		return true;
	if (pTrans)
		return FirstChar <= c && c <= LastChar;
	return false;
}
#pragma endregion
#pragma endregion

void GUI_PANEL::Init() {
	ClipRect();
	Cursor(Props.pCursor);
	WObj::Init();
}

#pragma region GUI_DRAW_BASE Classes
#include <memory>
void GUI_DRAW_BMP::Draw(SRect r) { GUI.DrawBitmap(bmp, r.left_top()); }
GUI_DRAW_BMP *GUI_DRAW_BMP::Create(CBitmap &bmp) { return new(GUI_MEM_AllocZero(sizeof(GUI_DRAW_BMP))) GUI_DRAW_BMP(bmp); }
void GUI_DRAW_SELF::Draw(SRect r) { pfDraw(r); }
GUI_DRAW_SELF *GUI_DRAW_SELF::Create(GUI_DRAW_CB *pcb) { return new(GUI_MEM_AllocZero(sizeof(GUI_DRAW_SELF))) GUI_DRAW_SELF(pcb); }
GUI_DRAW::~GUI_DRAW() {
	GUI_MEM_Free(pDrawObj);
	pDrawObj = nullptr;
}
#pragma endregion

#pragma region Key
static struct {
	int Key;
	int PressedCnt;
} _KeyMsg;
static int _KeyMsgCnt;
static int _Key;
int GUI_GetKey() {
	int r = _Key;
	_Key = 0;
	return r;
}
void GUI_StoreKey(int Key) {
	if (!_Key)
		_Key = Key;
}
void GUI_StoreKeyMsg(int Key, int PressedCnt) {
	_KeyMsg.Key = Key;
	_KeyMsg.PressedCnt = PressedCnt;
	_KeyMsgCnt = 1;
}
bool GUI_PollKeyMsg() {
	if (!_KeyMsgCnt)
		return false;
	int Key = _KeyMsg.Key;
	_KeyMsgCnt--;
	WObj::OnKey(Key, _KeyMsg.PressedCnt);
	if (_KeyMsg.PressedCnt == 1)
		GUI_StoreKey(Key);
	return true;
}
void GUI_SendKeyMsg(int Key, int PressedCnt) {
	if (!WObj::OnKey(Key, PressedCnt))
		GUI_StoreKeyMsg(Key, PressedCnt);
}
#pragma endregion

#pragma region LCD
BitmapRect::BitmapRect(CBitmap &bmp, Point Pos) :
	SRect(bmp.Rect(Pos)),
	pData(const_cast<void *>(bmp.pData)),
	pPalEntries(bmp.PalEntries()),
	BytesPerLine(bmp.BytesPerLine),
	BitsPerPixel(bmp.BitsPerPixel),
	HasTrans(bmp.HasTrans) {}
BitmapRect &BitmapRect::operator&=(const SRect &rClip) {
	auto rNew = SRect::operator&(rClip);
	auto off = rNew.left_top() - left_top();
	if (off.x > 0) {
		auto xoff_bits = off.x * BitsPerPixel;
		BitsXOff = xoff_bits & 7;
		(const uint8_t *&)pData += xoff_bits >> 3;
	}
	if (off.y > 0)
		(const uint8_t *&)pData += off.y * BytesPerLine;
	SRect::operator=(rNew);
	return*this;
}
SRect &SRect::AlignTo(ALIGN a, const SRect &r2) {
	if (a == ALIGN_HCENTER)
		xmove((r2.x0 + r2.x1 - x0 - x1) / 2);
	else if (a & ALIGN_RIGHT) {
		x0 += r2.x1 - x1;
		x1 = r2.x1;
	}
	else /* if (a & ALIGN_LEFT) */ {
		x1 += r2.x0 - x0;
		x0 = r2.x0;
	}
	if (a == ALIGN_VCENTER)
		ymove((r2.y0 + r2.y1 - y0 - y1) / 2);
	else if (a & ALIGN_BOTTOM) {
		y0 += r2.y1 - y1;
		y1 = r2.y1;
	}
	else /* if (a & ALIGN_TOP) */ {
		y1 += r2.y0 - y0;
		y0 = r2.y0;
	}
	return*this;
}
#pragma endregion
