#include "GUI.h"

void GUI_DispNextLine() {
	GUI.dispPos.y += GUI.pAFont->YDist;
	GUI.dispPos.x = 0;
}
void GL_DispChar(uint16_t c) {
	if (c == '\n')
		GUI_DispNextLine();
	else if (c != '\r') {
		GUI.pAFont->DispChar(c);
		if (GUI_pfDispCharStyle)
			GUI_pfDispCharStyle(c);
	}
}
int GUI_GetYAdjust() {
	switch (GUI.TextAlign & GUI_TA_VCENTER) {
	case GUI_TA_BOTTOM:
		return GUI.pAFont->YSize - 1;
	case GUI_TA_VCENTER:
		return GUI.pAFont->YSize / 2;
	case GUI_TA_BASELINE:
		return GUI.pAFont->YSize / 2;
	}
	return 0;
}
int GUI_GetCharDistX(uint16_t c) {
	return GUI.pAFont->XDist(c);
}
int GUI__GetLineNumChars(const char *s, int MaxNumChars) {
	if (!s)
		return 0;
	if (GUI.pAFont->pafEncode) 
		return GUI.pAFont->pafEncode->pfGetLineLen(s, MaxNumChars);
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
	if (GUI.pAFont->pafEncode)
		return GUI.pAFont->pafEncode->pfGetLineDistX(s, MaxNumChars);
	int Dist = 0;
	while (--MaxNumChars >= 0)
		Dist += GUI_GetCharDistX(GUI_UC__GetCharCodeInc(&s));
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
		if (GUI.pAFont->pafEncode)
			GUI.pAFont->pafEncode->pfDispLine(s, MaxNumChars);
		else while (--MaxNumChars >= 0) {
			auto Char = GUI_UC__GetCharCodeInc(&s);
			GUI.pAFont->DispChar(Char);
			if (GUI_pfDispCharStyle)
				GUI_pfDispCharStyle(Char);
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
	auto pCharInfo = pProp->paCharInfo + (c - pProp->First);
	auto BytesPerLine = pCharInfo->BytesPerLine;
	CPalette pal{ GUI_LCD_ACOLOR };
	GUI_LCD_DrawBitmap({ {
			/* Size */ { pCharInfo->XSize, GUI.pAFont->YSize },
			/* BytesPerLine */ BytesPerLine,
			/* BitsPerPixel */ 1,
			/* Bits */ pCharInfo->pData,
			/* Palette */ &pal,
			/* Transparent */ (GUI.TextMode & DRAWMODE_TRANS) == DRAWMODE_TRANS
		}, GUI.dispPos });
	if (GUI.pAFont->YDist > GUI.pAFont->YSize) {
		int YDist = GUI.pAFont->YDist;
		int YSize = GUI.pAFont->YSize;
		if (GUI.TextMode != DRAWMODE_TRANS) {
			RGBC OldColor = GUI.PenColor();
			GUI.PenColor(GUI.BkColor());
			GUI_LCD_FillRect({ GUI.dispPos.x,
							   GUI.dispPos.y + YSize,
							   GUI.dispPos.x + pCharInfo->XSize,
							   GUI.dispPos.y + YDist });
			GUI.PenColor(OldColor);
		}
	}
	GUI.dispPos.x += pCharInfo->XDist;
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
		CPalette pal{ GUI_LCD_ACOLOR };
		GUI_LCD_DrawBitmap({ {
			/* Size */ { xSize, YSize },
			/* BytesPerLine */ BytesPerLine,
			/* BitsPerPixel */ 1,
			/* Bits */ pd + c0 * BytesPerChar,
			/* Palette */ &pal,
			/* Transparent */ (GUI.TextMode & DRAWMODE_TRANS) == DRAWMODE_TRANS
		}, { x, y } });
		if (c1 != -1) {
			GUI_LCD_DrawBitmap({ {
				/* Size */ { xSize, YSize },
				/* BytesPerLine */ BytesPerLine,
				/* BitsPerPixel */ 1,
				/* Bits */ pd + c1 * BytesPerChar,
				/* Palette */ &pal,
				/* Transparent */ true
			}, { x, y } });
		}
		if (YDist > YSize) {
			if (GUI.DrawMode != DRAWMODE_TRANS) {
				auto OldMode = GUI_LCD_SetDrawMode(GUI.DrawMode ^ DRAWMODE_REV);  /* Reverse so we can fill with BkColor */
				GUI_LCD_FillRect({ x,
								   y + GUI.pAFont->YSize * GUI.pAFont->YDist,
								   x + xSize,
								   y + GUI.pAFont->YDist });
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
