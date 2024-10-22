#include "GUI.h"
void GUI_DispNextLine(void) {
	GUI.DispPosY += GUI_GetFontDistY();
	GUI.DispPosX = GUI.LBorder;
}
void GL_DispChar(uint16_t c) {
	if (c == '\n')
		GUI_DispNextLine();
	else if (c != '\r') {
		GUI.pAFont->pfDispChar(c);
		if (GUI_pfDispCharStyle)
			GUI_pfDispCharStyle(c);
	}
}
int GUI_GetYAdjust(void) {
	switch (GUI.TextAlign & GUI_TA_VERTICAL) {
	case GUI_TA_BOTTOM:
		return GUI.pAFont->YSize - 1;
	case GUI_TA_VCENTER:
		return GUI.pAFont->YSize / 2;
	case GUI_TA_BASELINE:
		return GUI.pAFont->YSize / 2;
	}
	return 0;
}
int GUI_GetFontDistY(void) {
	return GUI.pAFont->YDist;
}
int GUI_GetCharDistX(uint16_t c) {
	return GUI.pAFont->pfGetCharDistX(c);
}
int GUI__GetLineNumChars(const char *s, int MaxNumChars) {
	if (!s)
		return 0;
	if (GUI.pAFont->pafEncode) 
		return GUI.pAFont->pafEncode->pfGetLineLen(s, MaxNumChars);
	int NumChars;
	for (NumChars = 0; NumChars < MaxNumChars; NumChars++) {
		uint16_t Data = GUI_UC__GetCharCodeInc(&s);
		if (Data == 0 || Data == '\n')
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
static void _DispLine(const char *s, int MaxNumChars, const SRect *pRect) {
	if (GUI.pClipRect_HL)
		if (!(*GUI.pClipRect_HL && *pRect))
			return;
	if (GUI.pAFont->pafEncode)
		GUI.pAFont->pafEncode->pfDispLine(s, MaxNumChars);
	else while (--MaxNumChars >= 0) {
		uint16_t Char = GUI_UC__GetCharCodeInc(&s);
		GUI.pAFont->pfDispChar(Char);
		if (GUI_pfDispCharStyle)
			GUI_pfDispCharStyle(Char);
	}
}

#include "WM.h"
void GUI__DispLine(const char *s, int MaxNumChars, const SRect *pr) {
	SRect r = *pr;
	WM_ADDORG(r.x0, r.y0);
	WM_ADDORG(r.x1, r.y1);
	WM_ITERATE_START(&r) {
		GUI.DispPosX = r.x0;
		GUI.DispPosY = r.y0;
		_DispLine(s, MaxNumChars, &r);
	} WM_ITERATE_END();
	WM_SUBORG(GUI.DispPosX, GUI.DispPosY);
}
