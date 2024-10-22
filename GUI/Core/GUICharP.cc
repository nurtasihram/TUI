#include "GUI.h"
static const GUI_FONT_PROP *GUIPROP_FindChar(const GUI_FONT_PROP *pProp, uint16_t c) {
	for (; pProp; pProp = pProp->pNext)
		if (c >= pProp->First && c <= pProp->Last)
			break;
	return pProp;
}
void GUIPROP_DispChar(uint16_t c) {
	
	DRAWMODE DrawMode = GUI.TextMode;
	const GUI_FONT_PROP *pProp = GUIPROP_FindChar(GUI.pAFont->p.pProp, c);
	
	if (!pProp)
		return;
	const GUI_CHARINFO *pCharInfo = pProp->paCharInfo + (c - pProp->First);
	int BytesPerLine = pCharInfo->BytesPerLine;
	DRAWMODE OldDrawMode = GUI_LCD_SetDrawMode(DrawMode);
	GUI_LCD_DrawBitmap(
		GUI.DispPosX, GUI.DispPosY,
		pCharInfo->XSize, GUI.pAFont->YSize,
		1, BytesPerLine,
		pCharInfo->pData, GUI_LCD_ACOLOR);
	if (GUI.pAFont->YDist > GUI.pAFont->YSize) {
		int YDist = GUI.pAFont->YDist;
		int YSize = GUI.pAFont->YSize;
		if (DrawMode != DRAWMODE_TRANS) {
			RGBC OldColor = GUI.PenColor();
			GUI.PenColor(GUI.BkColor());
			GUI_LCD_FillRect(GUI.DispPosX,
						 GUI.DispPosY + YSize,
						 GUI.DispPosX + pCharInfo->XSize,
						 GUI.DispPosY + YDist);
			GUI.PenColor(OldColor);
		}
	}
	GUI_LCD_SetDrawMode(OldDrawMode);
	GUI.DispPosX += pCharInfo->XDist;
}
int GUIPROP_GetCharDistX(uint16_t c) {
	const GUI_FONT_PROP *pProp = GUIPROP_FindChar(GUI.pAFont->p.pProp, c);
	return pProp ? pProp->paCharInfo[c - pProp->First].XSize : 0;
}
void GUIPROP_GetFontInfo(CFont *pFont, FontInfo *pfi) {
	pfi->Flags = GUI_FONTINFO_FLAG_PROP;
}
bool GUIPROP_IsInFont(CFont *pFont, uint16_t c) {
	const GUI_FONT_PROP *pProp = GUIPROP_FindChar(pFont->p.pProp, c);
	return pProp != nullptr;
}
