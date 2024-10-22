#include "WM.h"

GUI_CONTEXT     GUI;
void (*GUI_pfDispCharStyle)(uint16_t Char) = nullptr;
static void _InitContext(GUI_CONTEXT *pContext) {
	pContext->pClipRect_HL = &GUI.ClipRect;
	pContext->pAFont = GUI_DEFAULT_FONT;
	pContext->pClipRect_HL = &GUI.ClipRect;
	pContext->pWndActive = WM_GetDesktopWindow();
	pContext->pUC_API = &GUI__API_TableNone;
	pContext->ClipRect = LCD_Rect();
	GUI.BkColor(GUI_DEFAULT_BKCOLOR);
	GUI.PenColor(GUI_DEFAULT_COLOR);
}
void GUI_ClearRect(int x0, int y0, int x1, int y1) {
	DRAWMODE PrevDraw = GUI_LCD_SetDrawMode(DRAWMODE_REV);
	SRect r;
	WM_ADDORG(x0, y0);
	WM_ADDORG(x1, y1);
	r.x0 = x0;
	r.x1 = x1;
	r.y0 = y0;
	r.y1 = y1;
	WM_ITERATE_START(&r) {
		GUI_LCD_FillRect(x0, y0, x1, y1);
	} WM_ITERATE_END();
	GUI_LCD_SetDrawMode(PrevDraw);
}
void GUI_Clear(void) {
	GUI_GotoXY(0, 0);
	GUI_ClearRect(GUI_XMIN, GUI_YMIN, GUI_XMAX, GUI_YMAX);
}
void GUI_X_LCD_Init();
void GUI_Init(void) {
	_InitContext(&GUI);
	GUI_X_LCD_Init();
	WObj::Init();
}
void GUI_SetDefault(void) {
	GUI.BkColor(GUI_DEFAULT_BKCOLOR);
	GUI.PenColor(GUI_DEFAULT_COLOR);
	GUI_SetTextAlign(0);
	GUI_SetTextMode(0);
	GUI_SetDrawMode(0);
	GUI.Font(GUI_DEFAULT_FONT);
}
RGBC GUI_GetBitmapPixel(const Bitmap *pBMP, unsigned x, unsigned y) {
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
void GUI_DrawBitmap(const Bitmap *pBitmap, int x0, int y0) {
	SRect r;
	WM_ADDORG(x0, y0);
	r.x1 = (r.x0 = x0) + pBitmap->XSize - 1;
	r.y1 = (r.y0 = y0) + pBitmap->YSize - 1;
	WM_ITERATE_START(&r) {
		DRAWMODE PrevDraw = GUI_SetDrawMode(0);
		const LogPalette *pPal = pBitmap->pPal;
		GUI_SetDrawMode((pPal && pPal->HasTrans) ? (PrevDraw | DRAWMODE_TRANS) : PrevDraw & (~DRAWMODE_TRANS));
		GUI_LCD_DrawBitmap(x0, y0, pBitmap->XSize, pBitmap->YSize,
						   pBitmap->BitsPerPixel, pBitmap->BytesPerLine,
						   pBitmap->pData, pPal ? pPal->pPalEntries : nullptr);
		GUI_SetDrawMode(PrevDraw);
	} WM_ITERATE_END();
}
void GUI_DrawPixel(int x, int y) {
	SRect r;
	WM_ADDORG(x, y);
	r.x0 = r.x1 = x;
	r.y0 = r.y1 = y;
	WM_ITERATE_START(&r);
	{
		GUI_LCD_SetPixel(x, y);
	} WM_ITERATE_END();
}
void GUI_DispString(const char *s) {
	if (!s)
		return;
	int FontSizeY = GUI_GetFontDistY();
	int xOrg = GUI.DispPosX;
	int xAdjust, yAdjust = GUI_GetYAdjust();
	GUI.DispPosY -= yAdjust;
	for (; *s; s++) {
		int LineNumChars = GUI__GetLineNumChars(s, 0x7fff);
		int xLineSize = GUI__GetLineDistX(s, LineNumChars);
		SRect r;
		switch (GUI.TextAlign & GUI_TA_HORIZONTAL) {
		case GUI_TA_CENTER: xAdjust = xLineSize / 2; break;
		case GUI_TA_RIGHT:  xAdjust = xLineSize; break;
		default:            xAdjust = 0;
		}
		r.x0 = GUI.DispPosX -= xAdjust;
		r.x1 = r.x0 + xLineSize - 1;
		r.y0 = GUI.DispPosY;
		r.y1 = r.y0 + FontSizeY - 1;
		GUI__DispLine(s, LineNumChars, &r);
		GUI.DispPosY = r.y0;
		s += GUI_UC__NumChars2NumBytes(s, LineNumChars);
		if (*s == '\n' || *s == '\r') {
			switch (GUI.TextAlign & GUI_TA_HORIZONTAL) {
			case GUI_TA_CENTER:
			case GUI_TA_RIGHT:
				GUI.DispPosX = xOrg;
				break;
			default:
				GUI.DispPosX = GUI.LBorder;
			}
			if (*s == '\n')
				GUI.DispPosY += FontSizeY;
		}
		else
			GUI.DispPosX = r.x0 + xLineSize;
		if (*s == 0)
			break;
	}
	GUI.DispPosY += yAdjust;
	GUI.TextAlign &= ~GUI_TA_HORIZONTAL;
}
void GUI__DispStringInRect(const char *s, SRect *pRect, int TextAlign, int MaxNumChars) {
	SRect r;
	SRect rLine;
	int y = 0;
	const char *sOrg = s;
	int FontYSize;
	int xLine = 0;
	int LineLen;
	int NumCharsRem;           /* Number of remaining characters */
	FontYSize = GUI_GetFontSizeY();
	if (pRect)
		r = *pRect;
	else
		WM_GetClientRect(&r);
	/* handle vertical alignment */
	if ((TextAlign & GUI_TA_VERTICAL) == GUI_TA_TOP)
		y = r.y0;
	else {
		int NumLines;
		/* Count the number of lines */
		for (NumCharsRem = MaxNumChars, NumLines = 1; NumCharsRem; NumLines++) {
			LineLen = GUI__GetLineNumChars(s, NumCharsRem);
			NumCharsRem -= LineLen;
			s += GUI_UC__NumChars2NumBytes(s, LineLen);
			if (GUI__HandleEOLine(&s))
				break;
		}
		/* Do the vertical alignment */
		switch (TextAlign & GUI_TA_VERTICAL) {
		case GUI_TA_BASELINE:
		case GUI_TA_BOTTOM:
			y = r.y1 - NumLines * FontYSize + 1;
			break;
		case GUI_TA_VCENTER:
			y = r.y0 + (r.y1 - r.y0 + 1 - NumLines * FontYSize) / 2;
			break;
		}
	}
	/* Output string */
	for (NumCharsRem = MaxNumChars, s = sOrg; NumCharsRem;) {
		int xLineSize;
		LineLen = GUI__GetLineNumChars(s, NumCharsRem);
		NumCharsRem -= LineLen;
		xLineSize = GUI__GetLineDistX(s, LineLen);
		switch (TextAlign & GUI_TA_HORIZONTAL) {
		case GUI_TA_HCENTER:
			xLine = r.x0 + (r.x1 - r.x0 - xLineSize) / 2; break;
		case GUI_TA_LEFT:
			xLine = r.x0; break;
		case GUI_TA_RIGHT:
			xLine = r.x1 - xLineSize + 1;
		}
		rLine.x0 = GUI.DispPosX = xLine;
		rLine.x1 = rLine.x0 + xLineSize - 1;
		rLine.y0 = GUI.DispPosY = y;
		rLine.y1 = y + FontYSize - 1;
		GUI__DispLine(s, LineLen, &rLine);
		s += GUI_UC__NumChars2NumBytes(s, LineLen);
		y += GUI_GetFontDistY();
		if (GUI__HandleEOLine(&s))
			break;
	}
}
void GUI_DispStringInRectMax(const char *s, SRect *pRect, int TextAlign, int MaxLen) {
	if (!s)
		return;
	const SRect *pOldClipRect = nullptr;
	if (pRect) {
		pOldClipRect = WObj::SetUserClipRect(pRect);
		if (pOldClipRect) {
			auto &&r = *pRect & *pOldClipRect;
			WObj::SetUserClipRect(&r);
		}
	}
	GUI__DispStringInRect(s, pRect, TextAlign, MaxLen);
	WObj::SetUserClipRect(pOldClipRect);
}
void GUI_DispStringInRect(const char *s, SRect *pRect, int TextAlign) {
	GUI_DispStringInRectMax(s, pRect, TextAlign, 0x7fff);
}
void GUI_DispStringHCenterAt(const char *s, int x, int y) {
	int Align = GUI_SetTextAlign((GUI.TextAlign & ~GUI_TA_LEFT) | GUI_TA_CENTER);
	GUI_DispStringAt(s, x, y);
	GUI_SetTextAlign(Align);
}
void GUI_DispStringAt(const char *s, int x, int y) {
	GUI.DispPosX = x;
	GUI.DispPosY = y;
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
	SRect r;
	WM_ADDORG(GUI.DispPosX, GUI.DispPosY);
	r.x1 = (r.x0 = GUI.DispPosX) + GUI_GetCharDistX(c) - 1;
	r.y1 = (r.y0 = GUI.DispPosY) + GUI_GetFontSizeY() - 1;
	WM_ITERATE_START(&r) {
		GL_DispChar(c);
	} WM_ITERATE_END();
	if (c != '\n')
		GUI.DispPosX = r.x1 + 1;
	WM_SUBORG(GUI.DispPosX, GUI.DispPosY);
}
void GUI_DispCharAt(uint16_t c, int x, int y) {
	GUI.DispPosX = x;
	GUI.DispPosY = y;
	GUI_DispChar(c);
}
void GUI_GetFontInfo(CFont *pFont, FontInfo *pFontInfo) {
	if (pFont == nullptr)
		pFont = GUI.pAFont;
	pFontInfo->Baseline = pFont->Baseline;
	pFontInfo->CHeight = pFont->CHeight;
	pFontInfo->LHeight = pFont->LHeight;
	pFont->pfGetFontInfo(pFont, pFontInfo);
}
char GUI_IsInFont(CFont *pFont, uint16_t c) {
	if (pFont == nullptr)
		pFont = GUI.pAFont;
	return pFont->pfIsInFont(pFont, c);
}
int GUI_GetYSizeOfFont(CFont *pFont) { return pFont->YSize; }
int GUI_GetYDistOfFont(CFont *pFont) { return pFont->YDist; }
int GUI_GetStringDistX(const char *s) { return GUI__GetLineDistX(s, GUI__strlen(s)); }
int GUI_GetTextAlign(void) { return GUI.TextAlign; }
void GUI_GetTextExtend(SRect *pRect, const char *s, int MaxNumChars) {
	int xMax = 0, NumLines = 0, LineSizeX = 0;
	pRect->x0 = GUI.DispPosX;
	pRect->y0 = GUI.DispPosY;
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
bool GUI__SetText(char **phText, const char *s) {
	if (!GUI__strcmp(*phText, s))
		return false;
	auto hMem = (char *)GUI_ALLOC_AllocNoInit(GUI__strlen(s) + 1);
	if (!hMem)
		return false;
	char *pMem = (char *)(hMem);
	GUI__strcpy(pMem, s);
	GUI_ALLOC_Free(*phText);
	*phText = hMem;
	return true;
}
void GUI_ClearRectEx(const SRect *pRect) {
	GUI_ClearRect(pRect->x0, pRect->y0, pRect->x1, pRect->y1);
}
int GUI_GetFontSizeY(void) { return GUI.pAFont->YSize; }
static void _DrawFocusRect(const SRect *pr) {
	int i;
	for (i = pr->x0; i <= pr->x1; i += 2) {
		GUI_LCD_SetPixel(i, pr->y0);
		GUI_LCD_SetPixel(i, pr->y1);
	}
	for (i = pr->y0; i <= pr->y1; i += 2) {
		GUI_LCD_SetPixel(pr->x0, i);
		GUI_LCD_SetPixel(pr->x1, i);
	}
}
void GUI_DrawFocusRect(const SRect *pRect, int Dist) {
	SRect r = *pRect / Dist;
	WM_ADDORG(r.x0, r.y0);
	WM_ADDORG(r.x1, r.y1);
	WM_ITERATE_START(&r);
	{
		_DrawFocusRect(&r);
	} WM_ITERATE_END();
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
	int TextHeight = GUI_GetFontDistY();
	switch (TextAlign & GUI_TA_VERTICAL) {
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
void GUI_DrawRect(int x0, int y0, int x1, int y1) {
	SRect r;
	WM_ADDORG(x0, y0);
	WM_ADDORG(x1, y1);
	r.x0 = x0;
	r.x1 = x1;
	r.y0 = y0;
	r.y1 = y1;
	WM_ITERATE_START(&r);
	{
		GUI_LCD_DrawHLine(x0, y0, x1);
		GUI_LCD_DrawHLine(x0, y1, x1);
		GUI_LCD_DrawVLine(x0, y0 + 1, y1 - 1);
		GUI_LCD_DrawVLine(x1, y0 + 1, y1 - 1);
	} WM_ITERATE_END();
}
void GUI_DrawHLine(int y0, int x0, int x1) {
	SRect r;
	WM_ADDORG(x0, y0);
	WM_ADDORGX(x1);
	r.x0 = x0;
	r.x1 = x1;
	r.y1 = r.y0 = y0;
	WM_ITERATE_START(&r) {
		GUI_LCD_DrawHLine(x0, y0, x1);
	} WM_ITERATE_END();
}
void GUI_DrawVLine(int x0, int y0, int y1) {
	SRect r;
	WM_ADDORG(x0, y0);
	WM_ADDORGY(y1);
	r.x1 = r.x0 = x0;
	r.y0 = y0;
	r.y1 = y1;
	WM_ITERATE_START(&r);
	{
		GUI_LCD_DrawVLine(x0, y0, y1);
	} WM_ITERATE_END();
}
void GUI_FillRect(int x0, int y0, int x1, int y1) {
	SRect r;
	WM_ADDORG(x0, y0);
	WM_ADDORG(x1, y1);
	r.x0 = x0; r.x1 = x1;
	r.y0 = y0; r.y1 = y1;
	WM_ITERATE_START(&r);
	{
		GUI_LCD_FillRect(x0, y0, x1, y1);
	} WM_ITERATE_END();
}
void GUI_FillRectEx(const SRect *pRect) {
	GUI_FillRect(pRect->x0, pRect->y0, pRect->x1, pRect->y1);
}
int GUI_SetLBorder(int x) {
	int r = GUI.LBorder;
	GUI.LBorder = x;
	return r;
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
	GUI.ClipRect = rClip & LCD_Rect();
}
static void GUI__cbDrawTextStyle(uint16_t Char) {
	int x1 = GUI.DispPosX - 1,
		x0 = x1 - GUI.pAFont->pfGetCharDistX(Char) + 1;
	if (GUI.TextStyle & GUI_TS_UNDERLINE) {
		int yOff = GUI.pAFont->Baseline;
		if (yOff >= GUI.pAFont->YSize)
			yOff = GUI.pAFont->YSize - 1;
		GUI_LCD_DrawHLine(x0, GUI.DispPosY + yOff, x1);
	}
	if (GUI.TextStyle & GUI_TS_STRIKETHRU)
		GUI_LCD_DrawHLine(
			x0,
			GUI.DispPosY
			+ GUI.pAFont->Baseline
			- (GUI.pAFont->CHeight + 1) / 2,
			x1);
	if (GUI.TextStyle & GUI_TS_OVERLINE) {
		int yOff = GUI.pAFont->Baseline - GUI.pAFont->CHeight - 1;
		if (yOff < 0)
			yOff = 0;
		GUI_LCD_DrawHLine(x0, GUI.DispPosY + yOff, x1);
	}
}
char GUI_SetTextStyle(char Style) {
	char OldStyle = GUI.TextStyle;
	GUI_pfDispCharStyle = GUI__cbDrawTextStyle;    /* Init function pointer (function in this module) */
	GUI.TextStyle = Style;
	return OldStyle;
}
void GUI_GotoXY(int x, int y) {
	GUI.DispPosX = x;
	GUI.DispPosY = y;
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
