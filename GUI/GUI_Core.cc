#include "WM.h"

GUI_PANEL GUI;
GUI_PANEL::Property GUI_PANEL::DefaultProps;

#pragma region Basic graphics
void GUI_PANEL::Clear() {
	DispPos(0);
	GUI.Clear({ GUI_XMIN, GUI_YMIN, GUI_XMAX, GUI_YMAX });
}
void GUI_PANEL::Clear(SRect r) {
	DRAWMODE PrevDraw = GUI_LCD_SetDrawMode(DRAWMODE_REV);
	r += off;
	GUI_LCD_FillRect(r);
	GUI_LCD_SetDrawMode(PrevDraw);
}
void GUI_PANEL::Fill(SRect r) {
	r += off;
	GUI_LCD_FillRect(r);
}
void GUI_PANEL::OutlineFocus(SRect r, int Dist) {
	r /= Dist;
	r += off;
	for (auto i = r.x0; i <= r.x1; i += 2) {
		GUI_LCD_FillRect(Point{ i, r.y0 });
		GUI_LCD_FillRect(Point{ i, r.y1 });
	}
	for (auto i = r.y0; i <= r.y1; i += 2) {
		GUI_LCD_FillRect(Point{ r.x0, i });
		GUI_LCD_FillRect(Point{ r.x1, i });
	}
}
void GUI_PANEL::Outline(SRect r) {
	r += off;
	GUI_LCD_FillRect({ r.x0, r.y0, r.x1, r.y0 });
	GUI_LCD_FillRect({ r.x0, r.y1, r.x1, r.y1 });
	GUI_LCD_FillRect({ r.x0, r.y0, r.x0, r.y1 });
	GUI_LCD_FillRect({ r.x1, r.y0, r.x1, r.y1 });
}

void GUI_PANEL::DrawBitmap(CBitmap &bmp, Point Pos) {
	Pos += off;
	GUI_LCD_SetBitmap(bmp + Pos);
}
#pragma endregion

#pragma region LCD
BitmapRect::BitmapRect(CBitmap &bmp, Point Pos) :
	SRect(bmp.Rect(Pos)),
	pData(const_cast<void *>(bmp.pData)),
	BytesPerLine(bmp.BytesPerLine),
	BitsPerPixel(bmp.BitsPerPixel),
	HasTrans(bmp.HasTrans),
	pPalEntries(bmp.PalEntries()) {}
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

DRAWMODE GUI_LCD_SetDrawMode(DRAWMODE dm) {
	auto OldDM = GUI.Props.DrawMode;
	if ((GUI.Props.DrawMode ^ dm) & DRAWMODE_REV) {
		auto temp = GUI.BkColor();
		GUI.BkColor(GUI.PenColor());
		GUI.PenColor(temp);
	}
	GUI.Props.DrawMode = dm;
	return OldDM;
}

void GUI_LCD_FillRect(SRect r) {
	if (r &= GUI.rClip)
		LCD_FillRect(r);
}
void GUI_LCD_SetBitmap(BitmapRect bc) {
	bc &= GUI.rClip;
	LCD_SetBitmap(bc);
}
#pragma endregion

#pragma region Cursor Control
static const RGBC _aColorI[]{ RGB_BLUE, RGB_WHITE, RGB_BLACK };
CPalette GUI_CursorPalI{ _aColorI };

static const RGBC _aColor[]{ RGB_BLUE, RGB_BLACK, RGB_WHITE };
CPalette GUI_CursorPal{ _aColor };

void CursorCtl::_Draw() {
	LCD_GetBitmap(brClip);
	LCD_SetBitmap(brCursor & LCD_Rect());
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

CCursor *CursorCtl::Select(CCursor *pCursor) {
	auto pOldCursor = _pCursor;
	if (pCursor == _pCursor)
		return _pCursor;
	_Hide();
	if (brClip.pData)
		GUI_MEM_Free(brClip.pData),
		brClip.pData = nullptr;
	if (pCursor) {
		_CursorOn = true;
		_pCursor = pCursor;
		brCursor = *pCursor;
		auto &&nPos = Pos - pCursor->Hot;
		brCursor.move_to(nPos);
		brClip = pCursor->Rect(nPos);
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
#pragma endregion

static PID_STATE _State;
int GUI_PID_GetState(PID_STATE *pState) {
	*pState = _State;
	return pState->Pressed != 0;
}
void GUI_PID_StoreState(const PID_STATE *pState) {
	if (_State != *pState)
		_State = *pState;
}

void GUI_Init() {
	GUI.ClipRect();
	GUI.Cursor.Select(GUI.Props.pCursor);
	WObj::Init();
}

#pragma region String
void GUI_DispString(const char *s) {
	if (!s)
		return;
	auto FontSizeY = GUI.Font()->YDist;
	auto xOrg = GUI.dispPos.x;
	auto yAdjust = GUI_GetYAdjust();
	GUI.dispPos.y -= yAdjust;
	for (; *s; s++) {
		int LineNumChars = GUI__GetLineNumChars(s, 0x7fff);
		int xLineSize = GUI__GetLineDistX(s, LineNumChars);
		int xAdjust;
		switch (GUI.TextAlign() & TEXTALIGN_HORIZONTAL) {
			case TEXTALIGN_HCENTER:
				xAdjust = xLineSize / 2;
				break;
			case TEXTALIGN_RIGHT:
				xAdjust = xLineSize;
				break;
			default:
				xAdjust = 0;
		}
		SRect r;
		r.x0 = GUI.dispPos.x -= xAdjust;
		r.x1 = r.x0 + xLineSize - 1;
		r.y0 = GUI.dispPos.y;
		r.y1 = r.y0 + FontSizeY - 1;
		GUI__DispLine(s, LineNumChars, r);
		GUI.dispPos.y = r.y0;
		s += GUI_UC__NumChars2NumBytes(s, LineNumChars);
		if (*s == '\n' || *s == '\r') {
			switch (GUI.TextAlign() & TEXTALIGN_HORIZONTAL) {
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
			GUI.dispPos.x = r.x0 + xLineSize;
		if (*s == 0)
			break;
	}
	GUI.dispPos.y += yAdjust;
	GUI.TextAlign(GUI.TextAlign() & ~TEXTALIGN_HORIZONTAL);
}
void GUI__DispStringInRect(const char *s, const SRect &rText, int TextAlign, int MaxNumChars) {
	int y = 0;
	const char *sOrg = s;
	int xLine = 0;
	auto FontYSize = GUI.Font()->YSize;
	auto r = rText;
	/* handle vertical alignment */
	if ((TextAlign & TEXTALIGN_VCENTER) == TEXTALIGN_TOP)
		y = r.y0;
	else {
		int NumLines = 1;
		/* Count the number of lines */
		for (int NumCharsRem = MaxNumChars; NumCharsRem; ++NumLines) {
			auto LineLen = GUI__GetLineNumChars(s, NumCharsRem);
			NumCharsRem -= LineLen;
			s += GUI_UC__NumChars2NumBytes(s, LineLen);
			if (GUI__HandleEOLine(&s))
				break;
		}
		/* Do the vertical alignment */
		switch (TextAlign & TEXTALIGN_VCENTER) {
		case TEXTALIGN_BASELINE:
		case TEXTALIGN_BOTTOM:
			y = r.y1 - NumLines * FontYSize + 1;
			break;
		case TEXTALIGN_VCENTER:
			y = r.y0 + (r.ysize() - NumLines * FontYSize) / 2;
			break;
		}
	}
	/* Output string */
	s = sOrg;
	for (int NumCharsRem = MaxNumChars; NumCharsRem;) {
		auto LineLen = GUI__GetLineNumChars(s, NumCharsRem);
		NumCharsRem -= LineLen;
		auto xLineSize = GUI__GetLineDistX(s, LineLen);
		switch (TextAlign & TEXTALIGN_HORIZONTAL) {
		case TEXTALIGN_HCENTER:
			xLine = r.x0 + (r.x1 - r.x0 - xLineSize) / 2; break;
		case TEXTALIGN_LEFT:
			xLine = r.x0; break;
		case TEXTALIGN_RIGHT:
			xLine = r.x1 - xLineSize + 1;
		}
		SRect rLine;
		rLine.x0 = GUI.dispPos.x = xLine;
		rLine.x1 = rLine.x0 + xLineSize - 1;
		rLine.y0 = GUI.dispPos.y = y;
		rLine.y1 = y + FontYSize - 1;
		GUI__DispLine(s, LineLen, rLine);
		s += GUI_UC__NumChars2NumBytes(s, LineLen);
		y += GUI.Font()->YDist;
		if (GUI__HandleEOLine(&s))
			break;
	}
}
void GUI_DispStringInRect(const char *s, const SRect &rText, int TextAlign, int MaxLen) {
	if (!s)
		return;
	auto pOldClipRect = WObj::UserClip(&rText);
	if (pOldClipRect) {
		auto &&r = rText & *pOldClipRect;
		WObj::UserClip(&r);
	}
	GUI__DispStringInRect(s, rText, TextAlign, MaxLen);
	WObj::UserClip(pOldClipRect);
}
void GUI_DispStringInRect(const char *s, const SRect &rText, int TextAlign) {
	GUI_DispStringInRect(s, rText, TextAlign, 0x7fff);
}
void GUI_DispStringAt(const char *s, Point Pos) {
	GUI.DispPos(Pos);
	GUI_DispString(s);
}
void GUI_DispStringLen(const char *s, int MaxNumChars) {
	uint16_t Char;
	while (MaxNumChars && ((Char = GUI_UC__GetCharCodeInc(&s)) != 0)) {
		GUI_DispChar(Char);
		MaxNumChars--;
	}
	while (MaxNumChars--)
		GUI_DispChar(' ');
}
void GUI_DispChars(uint16_t c, int NumChars) {
	while (--NumChars >= 0)
		GUI_DispChar(c);
}
void GUI_DispChar(uint16_t c) {
	auto &&r = SRect::left_top(GUI.dispPos,
							   { GUI.Font()->XDist(c),
								 GUI.Font()->YSize });
	WObj::IVR(r, [&] {
		GL_DispChar(c);
	});
	if (c != '\n')
		GUI.dispPos.x = r.x1 + 1;
	GUI.dispPos -= GUI.off;
}
void GUI_DispCharAt(uint16_t c, Point Pos) {
	GUI.DispPos(Pos);
	GUI_DispChar(c);
}
#pragma endregion

#pragma region Char
void GUI_DispNextLine() {
	GUI.dispPos.y += GUI.Font()->YDist;
	GUI.dispPos.x = 0;
}
void GL_DispChar(uint16_t c) {
	if (c == '\n')
		GUI_DispNextLine();
	else if (c != '\r') {
		GUI.Font()->DispChar(c);
		GUI__DrawTextStyle(c);
	}
}
int GUI_GetYAdjust() {
	switch (GUI.TextAlign() & TEXTALIGN_VCENTER) {
		case TEXTALIGN_BOTTOM:
			return GUI.Font()->YSize - 1;
		case TEXTALIGN_VCENTER:
			return GUI.Font()->YSize / 2;
		case TEXTALIGN_BASELINE:
			return GUI.Font()->YSize / 2;
	}
	return 0;
}
int GUI__GetLineNumChars(const char *s, int MaxNumChars) {
	if (!s)
		return 0;
	if (GUI.Font()->pafEncode)
		return GUI.Font()->pafEncode->pfGetLineLen(s, MaxNumChars);
	int NumChars;
	for (NumChars = 0; NumChars < MaxNumChars; ++NumChars) {
		auto data = GUI_UC__GetCharCodeInc(&s);
		if (data == 0 || data == '\n')
			break;
	}
	return NumChars;
}
int GUI__GetLineDistX(const char *s, int MaxNumChars) {
	if (!s)
		return 0;
	if (GUI.Font()->pafEncode)
		return GUI.Font()->pafEncode->pfGetLineDistX(s, MaxNumChars);
	int Dist = 0;
	while (--MaxNumChars >= 0)
		Dist += GUI.Font()->XDist(GUI_UC__GetCharCodeInc(&s));
	return Dist;
}

#include "WM.h"
void GUI__DispLine(const char *s, int MaxNumChars, SRect r) {
	r += GUI.off;
	WObj::IVR(r, [&] {
		GUI.dispPos = r.left_top();
		if (GUI.pClipRect_HL)
			if (!(*GUI.pClipRect_HL && r))
				return;
		if (GUI.Font()->pafEncode)
			GUI.Font()->pafEncode->pfDispLine(s, MaxNumChars);
		else while (--MaxNumChars >= 0) {
			auto Char = GUI_UC__GetCharCodeInc(&s);
			GUI.Font()->DispChar(Char);
			GUI__DrawTextStyle(Char);
		}
	});
	GUI.dispPos -= GUI.off;
}

#pragma region Proportional Font
const FontProp::Prop *FontProp::_FindChar(uint16_t c) const {
	for (auto pProp = &prop; pProp; pProp = pProp->pNext)
		if (c >= pProp->First && c <= pProp->Last)
			return pProp;
	return nullptr;
}
void FontProp::DispChar(uint16_t c) const {
	auto pProp = _FindChar(c);
	if (!pProp)
		return;
	auto &ci = pProp->paCharInfo[c - pProp->First];
	auto &&Pal = GUI.Palette();
	GUI_LCD_SetBitmap({ {
			/* Size */ { ci.XSize, GUI.Font()->YSize },
			/* BytesPerLine */ ci.BytesPerLine,
			/* BitsPerPixel */ 1,
			/* Bits */ ci.pData,
			/* Palette */ &Pal,
			/* Transparent */ (GUI.TextMode() & DRAWMODE_TRANS) == DRAWMODE_TRANS
			}, GUI.dispPos });
	if (GUI.Font()->YDist > GUI.Font()->YSize) {
		int YDist = GUI.Font()->YDist;
		int YSize = GUI.Font()->YSize;
		if (GUI.TextMode() != DRAWMODE_TRANS) {
			RGBC OldColor = GUI.PenColor();
			GUI.PenColor(GUI.BkColor());
			GUI_LCD_FillRect({ GUI.dispPos.x,
							   GUI.dispPos.y + YSize,
							   GUI.dispPos.x + ci.XSize,
							   GUI.dispPos.y + YDist });
			GUI.PenColor(OldColor);
		}
	}
	GUI.dispPos.x += ci.XDist;
}
int FontProp::XDist(uint16_t c) const {
	auto pProp = _FindChar(c);
	return pProp ? pProp->paCharInfo[c - pProp->First].XSize : 0;
}
bool FontProp::IsInFont(uint16_t c) const {
	return _FindChar(c);
}
#pragma endregion

#pragma region Monospace Font
void FontMono::DispChar(uint16_t c) const {
	int c0, c1;
	const uint8_t *pd;
	int x = GUI.dispPos.x;
	int y = GUI.dispPos.y;
	auto FirstChar = this->FirstChar;
	if (FirstChar <= c && c <= LastChar) {
		pd = (const uint8_t *)pData;
		c0 = ((int)c) - FirstChar;
		c1 = -1;
	}
	else {
		auto pti = pTrans;
		pd = (const uint8_t *)pTransData;
		if (pti) {
			FirstChar = pti->FirstChar;
			if (c >= FirstChar && c <= pti->LastChar) {
				c -= pti->FirstChar;
				auto ptl = pti->pList;
				ptl += c;
				c0 = ptl->c0;
				c1 = ptl->c1;
			}
			else
				c0 = c1 = -1;
		}
		else
			c0 = c1 = -1;
	}
	if (c0 != -1) {
		auto BytesPerChar = YSize * BytesPerLine;
		auto &&Pal = GUI.Palette();
		GUI_LCD_SetBitmap({ {
				/* Size */ { xSize, YSize },
				/* BytesPerLine */ BytesPerLine,
				/* BitsPerPixel */ 1,
				/* Bits */ pd + c0 * BytesPerChar,
				/* Palette */ &Pal,
				/* Transparent */ (GUI.TextMode() & DRAWMODE_TRANS) == DRAWMODE_TRANS
			}, { x, y } });
		if (c1 != -1) {
			GUI_LCD_SetBitmap({ {
					/* Size */ { xSize, YSize },
					/* BytesPerLine */ BytesPerLine,
					/* BitsPerPixel */ 1,
					/* Bits */ pd + c1 * BytesPerChar,
					/* Palette */ &Pal,
					/* Transparent */ true
				}, { x, y } });
		}
		if (YDist > YSize) {
			if (GUI.DrawMode() != DRAWMODE_TRANS) {
				auto OldMode = GUI_LCD_SetDrawMode(GUI.DrawMode() ^ DRAWMODE_REV);  /* Reverse so we can fill with BkColor */
				GUI_LCD_FillRect({ x,
								   y + GUI.Font()->YSize * GUI.Font()->YDist,
								   x + xSize,
								   y + GUI.Font()->YDist });
				GUI_LCD_SetDrawMode(OldMode);
			}
		}
	}
	GUI.dispPos.x += xDist;
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

int GUI_GetStringDistX(const char *s) { return GUI__GetLineDistX(s, GUI__strlen(s)); }
void GUI_GetTextExtend(SRect *pRect, const char *s, int MaxNumChars) {
	int xMax = 0, NumLines = 0, LineSizeX = 0;
	pRect->left_top(GUI.DispPos());
	while (MaxNumChars--) {
		uint16_t Char = GUI_UC__GetCharCodeInc(&s);
		if (Char == '\n' || Char == 0) {
			if (LineSizeX > xMax)
				xMax = LineSizeX;
			LineSizeX = 0;
			NumLines++;
			if (!Char)
				break;
		}
		else
			LineSizeX += GUI.Font()->XDist(Char);
	}
	if (LineSizeX > xMax)
		xMax = LineSizeX;
	if (!NumLines)
		NumLines = 1;
	pRect->x1 = pRect->x0 + xMax - 1;
	pRect->y1 = pRect->y0 + GUI.Font()->YSize * NumLines - 1;
}
bool GUI__SetText(char **ppText, const char *s) {
	if (!s) {
		GUI_MEM_Free(*ppText);
		return true;
	}
	auto pString = (char *)GUI_MEM_Alloc(GUI__strlen(s) + 1);
	GUI__strcpy(pString, s);
	GUI_MEM_Free(*ppText);
	*ppText = pString;
	return true;
}

int GUI__GetNumChars(const char *s) {
	if (!s)
		return 0;
	int NumChars = 0;
	while (GUI_UC__GetCharCodeInc(&s))
		++NumChars;
	return NumChars;
}
void GUI__CalcTextRect(const char *pText, const SRect *pTextRectIn, SRect *pTextRectOut, int TextAlign) {
	if (!pText) {
		*pTextRectOut = *pTextRectIn;
		return;
	}
	int xPos, yPos;
	int TextWidth = GUI_GetStringDistX(pText);
	switch (TextAlign & TEXTALIGN_HORIZONTAL) {
	case TEXTALIGN_HCENTER:
		xPos = pTextRectIn->x0 + ((pTextRectIn->x1 - pTextRectIn->x0 + 1) - TextWidth) / 2;
		break;
	case TEXTALIGN_RIGHT:
		xPos = pTextRectIn->x1 - TextWidth + 1;
		break;
	default:
		xPos = pTextRectIn->x0;
	}
	int TextHeight = GUI.Font()->YDist;
	switch (TextAlign & TEXTALIGN_VCENTER) {
	case TEXTALIGN_VCENTER:
		yPos = pTextRectIn->y0 + ((pTextRectIn->y1 - pTextRectIn->y0 + 1) - TextHeight) / 2;
		break;
	case TEXTALIGN_BOTTOM:
		yPos = pTextRectIn->y1 - TextHeight + 1;
		break;
	case TEXTALIGN_BASELINE:
	default:
		yPos = pTextRectIn->y0;
	}
	pTextRectOut->x0 = xPos;
	pTextRectOut->y0 = yPos;
	pTextRectOut->x1 = xPos + TextWidth - 1;
	pTextRectOut->y1 = yPos + TextHeight - 1;
}

void GUI__DrawTextStyle(uint16_t Char) {
	int x1 = GUI.dispPos.x - 1,
		x0 = x1 - GUI.Font()->XDist(Char) + 1;
	if (GUI.TextStyle() & TS_UNDERLINE) {
		int yOff = GUI.Font()->Baseline;
		if (yOff >= GUI.Font()->YSize)
			yOff = GUI.Font()->YSize - 1;
		GUI_LCD_FillRect({ GUI.dispPos.y + yOff, x0, x1, x0 });
	}
	if (GUI.TextStyle() & TS_STRIKETHRU)
		GUI_LCD_FillRect({
			GUI.dispPos.y + GUI.Font()->Baseline - (GUI.Font()->CHeight + 1) / 2,
			x0, x1, x0 });
	if (GUI.TextStyle() & TS_OVERLINE) {
		int yOff = GUI.Font()->Baseline - GUI.Font()->CHeight - 1;
		if (yOff < 0)
			yOff = 0;
		GUI_LCD_FillRect({ GUI.dispPos.y + yOff, x0, x1, x0 });
	}
}

#pragma region Wrap
static int _IsLineEnd(uint16_t Char) {
	return !Char || Char == '\n';
}
static int _GetWordWrap(const char *s, int xSize) {
	int xDist = 0, NumChars = 0, WordWrap = 0;
	uint16_t PrevChar = 0;
	for (;;) {
		uint16_t Char = GUI_UC__GetCharCodeInc(&s);   /* Similar to:  *s++ */
		/* Let's first check if the line end is reached. In this case we are done. */
		if (_IsLineEnd(Char)) {
			WordWrap = NumChars;
			break;
		}
		/* If current character is a space, we found a wrap position */
		if (Char == ' ' && Char != PrevChar)
			WordWrap = NumChars;
		PrevChar = Char;
		xDist += GUI.Font()->XDist(Char);
		if ((xDist <= xSize) || (NumChars == 0))
			NumChars++;
		else
			break;
	}
	if (!WordWrap)
		WordWrap = NumChars;
	return WordWrap;
}
static int _GetCharWrap(const char *s, int xSize) {
	int xDist = 0, NumChars = 0;
	uint16_t Char;
	while ((Char = GUI_UC__GetCharCodeInc(&s)) != 0) {
		xDist += GUI.Font()->XDist(Char);
		if ((NumChars && (xDist > xSize)) || (Char == '\n'))
			break;
		NumChars++;
	}
	return NumChars;
}
static int _GetNoWrap(const char *s) {
	return GUI__GetLineNumChars(s, 0x7FFF);
}
int GUI__WrapGetNumCharsDisp(const char *pText, int xSize, GUI_WRAPMODE WrapMode) {
	switch (WrapMode) {
	case GUI_WRAPMODE_WORD:
		return _GetWordWrap(pText, xSize);
	case GUI_WRAPMODE_CHAR:
		return _GetCharWrap(pText, xSize);
	default:
		break;
	}
	return _GetNoWrap(pText);
}
int GUI__WrapGetNumCharsToNextLine(const char *pText, int xSize, GUI_WRAPMODE WrapMode) {
	int NumChars = GUI__WrapGetNumCharsDisp(pText, xSize, WrapMode);
	pText += GUI_UC__NumChars2NumBytes(pText, NumChars);
	uint16_t Char = GUI_UC__GetCharCodeInc(&pText);
	if (Char == '\n')
		NumChars++;
	else if (WrapMode == GUI_WRAPMODE_WORD)
		while (Char == ' ') {
			Char = GUI_UC__GetCharCodeInc(&pText);
			++NumChars;
		}
	return NumChars;
}
int GUI__WrapGetNumBytesToNextLine(const char *pText, int xSize, GUI_WRAPMODE WrapMode) {
	return GUI_UC__NumChars2NumBytes(pText, GUI__WrapGetNumCharsToNextLine(pText, xSize, WrapMode));
}
int GUI__HandleEOLine(const char **ps) {
	const char *s = *ps;
	char c = *s++;
	if (c == 0)
		return 1;
	if (c == '\n')
		*ps = s;
	return 0;
}
int GUI__DivideRound(int a, int b) {
	return b ? (a + b / 2) / b : 0;
}
#pragma endregion

#pragma region GUI_DRAW_BASE Classes
#include <memory>

void GUI_DRAW_BASE::Paint(SRect r) { Draw(r); }

void GUI_DRAW_BMP::Draw(SRect r) { GUI.DrawBitmap(*pBitmap, r.left_top()); }
Point GUI_DRAW_BMP::Size() const { return pBitmap->Size; }
GUI_DRAW_BMP *GUI_DRAW_BMP::Create(CBitmap *pBitmap) { return new(GUI_MEM_AllocZero(sizeof(GUI_DRAW_BMP))) GUI_DRAW_BMP(pBitmap); }

void GUI_DRAW_SELF::Draw(SRect r) { pfDraw(r); }
GUI_DRAW_SELF *GUI_DRAW_SELF::Create(GUI_DRAW_CB *pfDraw) { return new(GUI_MEM_AllocZero(sizeof(GUI_DRAW_SELF))) GUI_DRAW_SELF(pfDraw); }

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
void GUI_ClearKeyBuffer() {
	while (GUI_GetKey()) {}
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

#pragma region UC
int GUI_UC_GetCharSize(const char *s) {
	return GUI.pUC_API->pfGetCharSize(s);
}
uint16_t GUI_UC_GetCharCode(const char *s) {
	return GUI.pUC_API->pfGetCharCode(s);
}
int GUI_UC_Encode(char *s, uint16_t Char) {
	return GUI.pUC_API->pfEncode(s, Char);
}
int GUI_UC__CalcSizeOfChar(uint16_t Char) {
	return GUI.pUC_API->pfCalcSizeOfChar(Char);
}
uint16_t GUI_UC__GetCharCodeInc(const char **ps) {
	const char *s = *ps;
	uint16_t r = GUI_UC__GetCharCode(s);
	s += GUI_UC__GetCharSize(s);
	*ps = s;

	return r;
}
int GUI_UC__NumChars2NumBytes(const char *s, int NumChars) {
	int NumBytes = 0;
	while (NumChars--) {
		int CharSize = GUI_UC__GetCharSize(s);
		s += CharSize;
		NumBytes += CharSize;
	}
	return NumBytes;
}
int GUI_UC__NumBytes2NumChars(const char *s, intptr_t NumBytes) {
	int Chars = 0, Bytes = 0;
	while (NumBytes > Bytes) {
		int CharSize = GUI_UC__GetCharSize(s + Bytes);
		Bytes += CharSize;
		Chars++;
	}
	return Chars;
}


static uint16_t _GetCharCode(const char *s) {
	return *(const uint8_t *)s;
}
static int _GetCharSize(const char *s) {
	return 1;
}
static int _CalcSizeOfChar(uint16_t Char) {
	return 1;
}
static int _Encode(char *s, uint16_t Char) {
	*s = (uint8_t)(Char);
	return 1;
}
const GUI_UC_ENC_APILIST GUI__API_TableNone = {
	_GetCharCode,     /*  return character code as uint16_t */
	_GetCharSize,     /*  return size of character: 1 */
	_CalcSizeOfChar,  /*  return size of character: 1 */
	_Encode           /*  Encode character */
};
#pragma endregion
