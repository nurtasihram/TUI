#include "GUI.h"
void GUIMONO_DispChar(uint16_t c) {
	int c0, c1;
	const unsigned char * pd;
	int x = GUI.DispPosX;
	int y = GUI.DispPosY;
	const GUI_FONT_MONO * pMono = GUI.pAFont->p.pMono;
	unsigned int FirstChar = pMono->FirstChar;
	if (c >= FirstChar && c <= pMono->LastChar) {
		pd = pMono->pData;
		c0 = ((int)c) - FirstChar;
		c1 = -1;
	}
	else {
		GUI_FONT_TRANSINFO const * pti = pMono->pTrans;
		pd = pMono->pTransData;
		if (pti) {
			FirstChar = pti->FirstChar;
			if (c >= FirstChar && c <= pti->LastChar) {
				GUI_FONT_TRANSLIST const * ptl;
				c -= pti->FirstChar;
				ptl = pti->pList;
				ptl += c;
				c0 = ptl->c0;
				c1 = ptl->c1;
			}
			else {
				c0 = c1 = -1;
			}
		}
		else {
			c0 = c1 = -1;
		}
	}
	if (c0 != -1) {
		int BytesPerChar = GUI.pAFont->YSize * pMono->BytesPerLine;
		int XSize = pMono->XSize;
		int YSize = GUI.pAFont->YSize;
		DRAWMODE DrawMode = GUI.TextMode;
		uint8_t OldMode = GUI_LCD_SetDrawMode(DrawMode);
		GUI_LCD_DrawBitmap(x, y,
					   XSize, YSize,
					   1, pMono->BytesPerLine,
					   pd + c0 * BytesPerChar, GUI_LCD_ACOLOR);
		if (c1 != -1) {
			GUI_LCD_SetDrawMode(DrawMode | DRAWMODE_TRANS);
			GUI_LCD_DrawBitmap(x, y,
						   XSize, YSize,
						   1, pMono->BytesPerLine,
						   pd + c1 * BytesPerChar, GUI_LCD_ACOLOR);
		}
		if (GUI.pAFont->YDist > GUI.pAFont->YSize) {
			if (DrawMode != DRAWMODE_TRANS) {
				GUI_LCD_SetDrawMode(DrawMode ^ DRAWMODE_REV);  /* Reverse so we can fill with BkColor */
				GUI_LCD_FillRect(x,
							 y + GUI.pAFont->YSize * GUI.pAFont->YDist,
							 x + XSize, y + GUI.pAFont->YDist);
			}
		}
		GUI_LCD_SetDrawMode(OldMode);
	}
	GUI.DispPosX += pMono->XDist;
}
int GUIMONO_GetCharDistX(uint16_t c) {
	return GUI.pAFont->p.pMono->XDist;
}
void GUIMONO_GetFontInfo(CFont* pFont, FontInfo* pfi) {
	pfi->Flags = GUI_FONTINFO_FLAG_MONO;
}
bool GUIMONO_IsInFont(CFont *pFont, uint16_t c) {
	const GUI_FONT_MONO *pMono = pFont->p.pMono;
	unsigned int FirstChar = pMono->FirstChar;
	if (c >= FirstChar && c <= pMono->LastChar)
		return 1;
	GUI_FONT_TRANSINFO const *pti = pMono->pTrans;
	if (!pti)
		return 0;
	return c >= pti->FirstChar && c <= pti->LastChar;
}
