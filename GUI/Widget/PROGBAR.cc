
#include "Widget.h"
#include "PROGBAR.h"
#define PROGBAR_DEFAULT_FONT       GUI_DEFAULT_FONT
#define PROGBAR_DEFAULT_BARCOLOR0  0x555555
#define PROGBAR_DEFAULT_BARCOLOR1  0xAAAAAA
#define PROGBAR_DEFAULT_TEXTCOLOR0 0xFFFFFF
#define PROGBAR_DEFAULT_TEXTCOLOR1 0x000000
static void _FreeText(ProgBar_Obj *pObj) {
	GUI_ALLOC_Free(pObj->pText);
	pObj->pText = nullptr;
}
static int _Value2X(const ProgBar_Obj* pObj, int v) {
	int EffectSize = pObj->pEffect->EffectSize;
	int xSize = pObj->rect.x1 - pObj->rect.x0 + 1;
	int Min = pObj->Min;
	int Max = pObj->Max;
	if (v < Min)
		v = Min;
	if (v > Max)
		v = Max;
	return EffectSize + ((xSize - 2 * EffectSize) * (int32_t)(v - Min)) / (Max - Min);
}
static void _DrawPart(const ProgBar_Obj* pObj, int Index,
	int xText, int yText, const char* pText) {
	GUI.BkColor(pObj->BarColor[Index]);
	GUI.PenColor(pObj->TextColor[Index]);
	GUI_Clear();
	GUI_GotoXY(xText, yText);
	GUI_DispString(pText);
}
static const char *_GetText(const ProgBar_Obj *pObj, char *pBuffer) {
	if (pObj->pText)
		return pObj->pText;
	char prg = (char)(100.0f * ((pObj->v - pObj->Min) / (pObj->Max - pObj->Min)));
	char c1 = prg / 10, c0 = prg - prg * 10;
	const char *pText = pBuffer;
	*pBuffer++ = (char)c0 + '0';
	if (c1)
		*pBuffer++ = (char)c1 + '0';
	*pBuffer++ = '%';
	*pBuffer = '\0';
	return pText;
}
static void _GetTextRect(const ProgBar_Obj* pObj, SRect* pRect, const char* pText) {
	int xSize = pObj->rect.x1 - pObj->rect.x0 + 1,
		ySize = pObj->rect.y1 - pObj->rect.y0 + 1;
	int TextWidth = GUI_GetStringDistX(pText),
		TextHeight = GUI_GetFontSizeY(),
		EffectSize = pObj->pEffect->EffectSize;
	switch (pObj->TextAlign & GUI_TA_HORIZONTAL) {
	case GUI_TA_CENTER:
		pRect->x0 = (xSize - TextWidth) / 2;
		break;
	case GUI_TA_RIGHT:
		pRect->x0 = xSize - TextWidth - 1 - EffectSize;
		break;
	default:
		pRect->x0 = EffectSize;
	}
	pRect->y0 = (ySize - TextHeight) / 2;
	pRect->x0 += pObj->XOff;
	pRect->y0 += pObj->YOff;
	pRect->x1 = pRect->x0 + TextWidth - 1;
	pRect->y1 = pRect->y0 + TextHeight - 1;
}
static void _Paint(ProgBar_Obj *pObj) {
	SRect rInside, rClient, rText;
	char ac[5];
	WM_GetClientRect(&rClient);
	rInside = rClient / pObj->pEffect->EffectSize;
	int xPos = _Value2X(pObj, pObj->v);
	const char *pText = _GetText(pObj, ac);
	GUI.Font(pObj->pFont);
	_GetTextRect(pObj, &rText, pText);
	int tm = GUI_SetTextMode(DRAWMODE_TRANS);
	SRect r = rInside;
	r.x1 = xPos - 1;
	WObj::SetUserClipRect(&r);
	_DrawPart(pObj, 0, rText.x0, rText.y0, pText);
	r = rInside;
	r.x0 = xPos;
	WObj::SetUserClipRect(&r);
	_DrawPart(pObj, 1, rText.x0, rText.y0, pText);
	WObj::SetUserClipRect(nullptr);
	
	GUI_SetTextMode(tm);
	WIDGET__EFFECT_DrawDownRect(pObj, &rClient);
}
static void _Delete(ProgBar_Obj *pObj) {
	_FreeText(pObj);
}
static void _PROGBAR_Callback(WM_MESSAGE* pMsg) {
	ProgBar_Obj *pObj = (ProgBar_Obj *)pMsg->pWin;
	if (WIDGET_HandleActive(pObj, pMsg) == 0)
		return;
	switch (pMsg->MsgId) {
	case WM_PAINT:
		_Paint(pObj);
		return;
	case WM_DELETE:
		_Delete(pObj);
		break;
	}
	WM_DefaultProc(pMsg);
}
ProgBar_Obj * PROGBAR_CreateEx(int x0, int y0, int xsize, int ysize, WObj *pParent,
								int WinFlags, int ExFlags, int Id) {
	auto pObj = (ProgBar_Obj *)WObj::Create(
		x0, y0, xsize, ysize, pParent, WinFlags, _PROGBAR_Callback,
		sizeof(ProgBar_Obj) - sizeof(WObj));
	if (!pObj)
		return 0;
	WIDGET__Init(pObj, Id, 0);
	WIDGET_SetEffect(pObj, &Widget::Effect::None); /* Standard effect for progbar: None */
	pObj->pFont = GUI_DEFAULT_FONT;
	pObj->BarColor[0] = PROGBAR_DEFAULT_BARCOLOR0;
	pObj->BarColor[1] = PROGBAR_DEFAULT_BARCOLOR1;
	pObj->TextColor[0] = PROGBAR_DEFAULT_TEXTCOLOR0;
	pObj->TextColor[1] = PROGBAR_DEFAULT_TEXTCOLOR1;
	pObj->TextAlign = GUI_TA_CENTER;
	pObj->Max = 100;
	pObj->Min = 0;
	return pObj;
}
ProgBar_Obj * PROGBAR_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb) {
	return PROGBAR_CreateEx(
		pCreateInfo->x0 + x0, pCreateInfo->y0 + y0, pCreateInfo->xSize, pCreateInfo->ySize,
		pParent, 0, pCreateInfo->Flags, pCreateInfo->Id);
}
void PROGBAR_SetValue(ProgBar_Obj *pObj, int v) {
	if (!pObj)
		return;
	if (v < pObj->Min)
		v = pObj->Min;
	if (v > pObj->Max)
		v = pObj->Max;
	if (pObj->v == v)
		return;
	SRect r;
	if (v < pObj->v) {
		r.x0 = _Value2X(pObj, v);
		r.x1 = _Value2X(pObj, pObj->v);
	}
	else {
		r.x0 = _Value2X(pObj, pObj->v);
		r.x1 = _Value2X(pObj, v);
	}
	r.y0 = 0;
	r.y1 = 4095;
	if (pObj->pText) {
		pObj->v = v;
		return;
	}
	char acBuffer[5];
	auto pOldFont = GUI.Font();
	GUI.Font(pObj->pFont);
	SRect rText;
	_GetTextRect(pObj, &rText, _GetText(pObj, acBuffer));
	r |= rText;
	pObj->v = v;
	_GetTextRect(pObj, &rText, _GetText(pObj, acBuffer));
	r |= rText;
	GUI.Font(pOldFont);
	pObj->Invalidate(&r);
}
void PROGBAR_SetFont(ProgBar_Obj *pObj, CFont *pfont) {
	if (!pObj)
		return;
	pObj->pFont = pfont;
	pObj->Invalidate();
}
void PROGBAR_SetBarColor(ProgBar_Obj *pObj, unsigned int Index, RGBC color) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->BarColor))
		return;
	pObj->BarColor[Index] = color;
	pObj->Invalidate();
}
void PROGBAR_SetTextColor(ProgBar_Obj *pObj, unsigned int Index, RGBC color) {
	if (!pObj)
		return;
	if (Index >= GUI_COUNTOF(pObj->TextColor))
		return;
	pObj->TextColor[Index] = color;
	pObj->Invalidate();
}
void PROGBAR_SetText(ProgBar_Obj *pObj, const char *s) {
	if (!pObj)
		return;
	SRect r1;
	char acBuffer[5];
	auto pOldFont = GUI.Font();
	GUI.Font(pObj->pFont);
	_GetTextRect(pObj, &r1, _GetText(pObj, acBuffer));
	if (GUI__SetText(&pObj->pText, s)) {
		SRect r2;
		_GetTextRect(pObj, &r2, _GetText(pObj, acBuffer));
		r1 |= r2;
		pObj->Invalidate(&r1);
	}
	GUI.Font(pOldFont);
}
void PROGBAR_SetTextAlign(ProgBar_Obj *pObj, int Align) {
	if (!pObj)
		return;
	pObj->TextAlign = Align;
	pObj->Invalidate();
}
void PROGBAR_SetTextPos(ProgBar_Obj *pObj, int XOff, int YOff) {
	if (!pObj)
		return;
	pObj->XOff = XOff;
	pObj->YOff = YOff;
	pObj->Invalidate();
}
void PROGBAR_SetMinMax(ProgBar_Obj *pObj, int Min, int Max) {
	if (!pObj)
		return;
	if (Max <= Min)
		return;
	if ((Max != pObj->Max) || (Min != pObj->Min))
		return;
	pObj->Min = Min;
	pObj->Max = Max;
	pObj->Invalidate();
}
