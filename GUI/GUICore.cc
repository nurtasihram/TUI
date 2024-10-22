#include "WM.h"

GUI_CONTEXT GUI;
void (*GUI_pfDispCharStyle)(uint16_t Char) = nullptr;
static void _InitContext(GUI_CONTEXT *pContext) {
	pContext->pClipRect_HL = &GUI.rClip;
	pContext->pAFont = GUI_DEFAULT_FONT;
	pContext->pClipRect_HL = &GUI.rClip;
	pContext->pUC_API = &GUI__API_TableNone;
	pContext->rClip = LCD_Rect();
	GUI.BkColor(GUI_DEFAULT_BKCOLOR);
	GUI.PenColor(GUI_DEFAULT_COLOR);
}

#pragma region Basic graphics
void GUI_CONTEXT::Clear() {
	DispPos(0);
	GUI.Clear({ GUI_XMIN, GUI_YMIN, GUI_XMAX, GUI_YMAX });
}
void GUI_CONTEXT::Clear(SRect r) {
	DRAWMODE PrevDraw = GUI_LCD_SetDrawMode(DRAWMODE_REV);
	r += off;
	WObj::IVR(r, [&] {
		GUI_LCD_FillRect(r);
	});
	GUI_LCD_SetDrawMode(PrevDraw);
}
void GUI_CONTEXT::FillRect(SRect r) {
	r += off;
	WObj::IVR(r, [&] {
		GUI_LCD_FillRect(r);
	});
}
void GUI_CONTEXT::DrawFocus(SRect r, int Dist) {
	r /= Dist;
	r += off;
	WObj::IVR(r, [&] {
		for (auto i = r.x0; i <= r.x1; i += 2) {
			GUI_LCD_SetPixel({ i, r.y0 });
			GUI_LCD_SetPixel({ i, r.y1 });
		}
		for (auto i = r.y0; i <= r.y1; i += 2) {
			GUI_LCD_SetPixel({ r.x0, i });
			GUI_LCD_SetPixel({ r.x1, i });
		}
	});
}
void GUI_CONTEXT::DrawRect(SRect r) {
	r += off;
	SRect r1{ r.x0, r.y0, r.x1, r.y0 },
		  r2{ r.x0, r.y1, r.x1, r.y1 },
		  r3{ r.x0, r.y0, r.x0, r.y1 },
		  r4{ r.x1, r.y0, r.x1, r.y1 };
	WObj::IVR(r, [&] {
		GUI_LCD_FillRect(r1);
		GUI_LCD_FillRect(r2);
		GUI_LCD_FillRect(r3);
		GUI_LCD_FillRect(r4);
	});
}

void GUI_CONTEXT::DrawBitmap(CBitmap &bmp, Point Pos) {
	Pos += off;
	BitmapRect bc{ bmp, Pos };
	WObj::IVR(bc, [&] {
		GUI_LCD_DrawBitmap(bc);
	});
}
#pragma endregion

void GUI_Init(void) {
	_InitContext(&GUI);
	GUI_LCD_SetClipRectMax();
	GUI_LCD_SetDrawMode(DRAWMODE_REV);
	GUI_LCD_FillRect(LCD_Rect());
	GUI_LCD_SetDrawMode(0);
	WObj::Init();
}
void GUI_SetDefault(void) {
	GUI.BkColor(GUI_DEFAULT_BKCOLOR);
	GUI.PenColor(GUI_DEFAULT_COLOR);
	GUI_SetTextAlign(0);
	GUI_SetTextMode(0);
	GUI_SetDrawMode(0);
	GUI.Font(GUI_DEFAULT_FONT);
	GUI.pUserClipRect = nullptr;
}

RGBC GUI_GetBitmapPixel(CBitmap *pBMP, unsigned x, unsigned y) {
	switch (pBMP->BitsPerPixel) {
	case 1:
		return ((const uint8_t *)pBMP->pData)[
			(x >> 3) + y * pBMP->BytesPerLine
		] >> (x & 0x7) & 0x1;
	case 2:
		return ((const uint8_t *)pBMP->pData)[
			(x >> 2) + y * pBMP->BytesPerLine
		] >> ((x << 1) & 0x6) & 0x3;
	case 4:
	{
		RGBC Value = ((const uint8_t *)pBMP->pData)[
			(x >> 1) + y * pBMP->BytesPerLine];
		return (x & 1) ? (Value & 0xF) : (Value >> 4);
	}
	case 8:
		return  ((const uint8_t *)pBMP->pData)[x + y * pBMP->BytesPerLine];
	case 16:
		return ((const uint8_t *)pBMP->pData)[(x << 1) + y * pBMP->BytesPerLine];
	}
	return 0;
}

#pragma region String
void GUI_DispString(const char *s) {
	if (!s)
		return;
	auto FontSizeY = GUI.pAFont->YDist;
	auto xOrg = GUI.dispPos.x;
	auto yAdjust = GUI_GetYAdjust();
	GUI.dispPos.y -= yAdjust;
	for (; *s; s++) {
		int LineNumChars = GUI__GetLineNumChars(s, 0x7fff);
		int xLineSize = GUI__GetLineDistX(s, LineNumChars);
		int xAdjust;
		switch (GUI.TextAlign & GUI_TA_HORIZONTAL) {
			case GUI_TA_HCENTER:
				xAdjust = xLineSize / 2;
				break;
			case GUI_TA_RIGHT:
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
			switch (GUI.TextAlign & GUI_TA_HORIZONTAL) {
			case GUI_TA_HCENTER:
			case GUI_TA_RIGHT:
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
	GUI.TextAlign &= ~GUI_TA_HORIZONTAL;
}
void GUI__DispStringInRect(const char *s, const SRect &rText, int TextAlign, int MaxNumChars) {
	int y = 0;
	const char *sOrg = s;
	int xLine = 0;
	auto FontYSize = GUI_GetFontSizeY();
	auto r = rText;
	/* handle vertical alignment */
	if ((TextAlign & GUI_TA_VCENTER) == GUI_TA_TOP)
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
		switch (TextAlign & GUI_TA_VCENTER) {
		case GUI_TA_BASELINE:
		case GUI_TA_BOTTOM:
			y = r.y1 - NumLines * FontYSize + 1;
			break;
		case GUI_TA_VCENTER:
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
		switch (TextAlign & GUI_TA_HORIZONTAL) {
		case GUI_TA_HCENTER:
			xLine = r.x0 + (r.x1 - r.x0 - xLineSize) / 2; break;
		case GUI_TA_LEFT:
			xLine = r.x0; break;
		case GUI_TA_RIGHT:
			xLine = r.x1 - xLineSize + 1;
		}
		SRect rLine;
		rLine.x0 = GUI.dispPos.x = xLine;
		rLine.x1 = rLine.x0 + xLineSize - 1;
		rLine.y0 = GUI.dispPos.y = y;
		rLine.y1 = y + FontYSize - 1;
		GUI__DispLine(s, LineLen, rLine);
		s += GUI_UC__NumChars2NumBytes(s, LineLen);
		y += GUI.pAFont->YDist;
		if (GUI__HandleEOLine(&s))
			break;
	}
}
void GUI_DispStringInRect(const char *s, const SRect &rText, int TextAlign, int MaxLen) {
	if (!s)
		return;
	auto pOldClipRect = WObj::SetUserClipRect(&rText);
	if (pOldClipRect) {
		auto &&r = rText & *pOldClipRect;
		WObj::SetUserClipRect(&r);
	}
	GUI__DispStringInRect(s, rText, TextAlign, MaxLen);
	WObj::SetUserClipRect(pOldClipRect);
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
							   { GUI_GetCharDistX(c),
								 GUI_GetFontSizeY() });
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

int GUI_GetYSizeOfFont(CFont *pFont) { return pFont->YSize; }
int GUI_GetYDistOfFont(CFont *pFont) { return pFont->YDist; }
int GUI_GetStringDistX(const char *s) { return GUI__GetLineDistX(s, GUI__strlen(s)); }
int GUI_GetTextAlign(void) { return GUI.TextAlign; }
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
			LineSizeX += GUI_GetCharDistX(Char);
	}
	if (LineSizeX > xMax)
		xMax = LineSizeX;
	if (!NumLines)
		NumLines = 1;
	pRect->x1 = pRect->x0 + xMax - 1;
	pRect->y1 = pRect->y0 + GUI_GetFontSizeY() * NumLines - 1;
}
int GUI_GetTextMode(void) { return GUI.TextMode; }
bool GUI__SetText(char **ppText, const char *s) {
	if (!s) {
		GUI_ALLOC_Free(*ppText);
		return true;
	}
	if (!GUI__strcmp(*ppText, s))
		return false;
	auto pString = (char *)GUI_ALLOC_Alloc(GUI__strlen(s) + 1);
	GUI__strcpy(pString, s);
	GUI_ALLOC_Free(*ppText);
	*ppText = pString;
	return true;
}
int GUI_GetFontSizeY(void) { return GUI.pAFont->YSize; }

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
	switch (TextAlign & GUI_TA_HORIZONTAL) {
	case GUI_TA_HCENTER:
		xPos = pTextRectIn->x0 + ((pTextRectIn->x1 - pTextRectIn->x0 + 1) - TextWidth) / 2;
		break;
	case GUI_TA_RIGHT:
		xPos = pTextRectIn->x1 - TextWidth + 1;
		break;
	default:
		xPos = pTextRectIn->x0;
	}
	int TextHeight = GUI.pAFont->YDist;
	switch (TextAlign & GUI_TA_VCENTER) {
	case GUI_TA_VCENTER:
		yPos = pTextRectIn->y0 + ((pTextRectIn->y1 - pTextRectIn->y0 + 1) - TextHeight) / 2;
		break;
	case GUI_TA_BOTTOM:
		yPos = pTextRectIn->y1 - TextHeight + 1;
		break;
	case GUI_TA_BASELINE:
	default:
		yPos = pTextRectIn->y0;
	}
	pTextRectOut->x0 = xPos;
	pTextRectOut->y0 = yPos;
	pTextRectOut->x1 = xPos + TextWidth - 1;
	pTextRectOut->y1 = yPos + TextHeight - 1;
}

int GUI_SetTextAlign(int Align) {
	int r = GUI.TextAlign;
	GUI.TextAlign = Align;
	return r;
}
int GUI_SetTextMode(int Mode) {
	int r = GUI.TextMode;
	GUI.TextMode = Mode;
	return r;
}
void GUI_LCD_SetClipRectEx(const SRect &rClip) {
	GUI.rClip = rClip & LCD_Rect();
}
static void GUI__cbDrawTextStyle(uint16_t Char) {
	int x1 = GUI.dispPos.x - 1,
		x0 = x1 - GUI.pAFont->XDist(Char) + 1;
	if (GUI.TextStyle & GUI_TS_UNDERLINE) {
		int yOff = GUI.pAFont->Baseline;
		if (yOff >= GUI.pAFont->YSize)
			yOff = GUI.pAFont->YSize - 1;
		GUI_LCD_FillRect({ GUI.dispPos.y + yOff, x0, x1, x0 });
	}
	if (GUI.TextStyle & GUI_TS_STRIKETHRU)
		GUI_LCD_FillRect({
			GUI.dispPos.y + GUI.pAFont->Baseline - (GUI.pAFont->CHeight + 1) / 2,
			x0, x1, x0 });
	if (GUI.TextStyle & GUI_TS_OVERLINE) {
		int yOff = GUI.pAFont->Baseline - GUI.pAFont->CHeight - 1;
		if (yOff < 0)
			yOff = 0;
		GUI_LCD_FillRect({ GUI.dispPos.y + yOff, x0, x1, x0 });
	}
}
char GUI_SetTextStyle(char Style) {
	char OldStyle = GUI.TextStyle;
	GUI_pfDispCharStyle = GUI__cbDrawTextStyle;    /* Init function pointer (function in this module) */
	GUI.TextStyle = Style;
	return OldStyle;
}

static PidState _State;
int GUI_PID_GetState(PidState *pState) {
	*pState = _State;
	return pState->Pressed != 0;
}
void GUI_PID_StoreState(const PidState *pState) {
	if (GUI__memcmp(&_State, pState, sizeof(_State)))
		_State = *pState;
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
		xDist += GUI_GetCharDistX(Char);
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
		xDist += GUI_GetCharDistX(Char);
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

void GUI_DRAW_BASE::Paint(SRect r) {
	r += GUI.off;
	WObj::IVR([&] {
		Draw(r);
	});
}

void GUI_DRAW_BMP::Draw(SRect r) { GUI.DrawBitmap(*pBitmap, r.left_top()); }
Point GUI_DRAW_BMP::Size() const { return pBitmap->Size; }
GUI_DRAW_BMP *GUI_DRAW_BMP::Create(CBitmap *pBitmap) { return new(GUI_ALLOC_AllocZero(sizeof(GUI_DRAW_BMP))) GUI_DRAW_BMP(pBitmap); }

void GUI_DRAW_SELF::Draw(SRect r) { pfDraw(r); }
GUI_DRAW_SELF *GUI_DRAW_SELF::Create(GUI_DRAW_CB *pfDraw) { return new(GUI_ALLOC_AllocZero(sizeof(GUI_DRAW_SELF))) GUI_DRAW_SELF(pfDraw); }

#pragma endregion
