#include "ProgBar.h"

int ProgBar::_Value2X(int v) const {
	auto EffectSize = this->EffectSize();
	auto xSize = rect.xsize();
	auto Min = this->Min, Max = this->Max;
	if (v < Min) v = Min;
	if (v > Max) v = Max;
	return EffectSize + ((xSize - 2 * EffectSize) * (int32_t)(v - Min)) / (Max - Min);
}
void ProgBar::_DrawPart(int Index, Point ptText, const char *pText) const {
	GUI.BkColor(Props.aBarColor[Index]);
	GUI.PenColor(Props.aTextColor[Index]);
	GUI.Clear();
	GUI_DispStringAt(pText, ptText);
}
const char *ProgBar::_GetText(char *pBuffer) const {
	if (text) return text;
	char prg = (char)(100.0f * ((v - Min) / (Max - Min)));
	char c1 = prg / 10, c0 = prg - prg * 10;
	const char *pText = pBuffer;
	*pBuffer++ = (char)c0 + '0';
	if (c1)
		*pBuffer++ = (char)c1 + '0';
	*pBuffer++ = '%';
	*pBuffer = '\0';
	return pText;
}
SRect ProgBar::_GetTextRect(const char *pText) const {
	auto &&size = rect.size();
	int TextWidth = GUI_GetStringDistX(pText),
		TextHeight = GUI_GetFontSizeY(),
		EffectSize = this->EffectSize();
	Point p0{ Off.x, (size.y - TextHeight) / 2 };
	switch (Props.Align & GUI_TA_HORIZONTAL) {
		case GUI_TA_HCENTER:
			p0.x += (size.x - TextWidth) / 2;
			break;
		case GUI_TA_RIGHT:
			p0.x += size.x - TextWidth - EffectSize - 1;
			break;
		default:
			p0.x += EffectSize;
	}
	return SRect::left_top(p0, { TextWidth - 1, TextHeight - 1 });
}
void ProgBar::_OnPaint() const {
	auto &&rClient = ClientRect();
	auto xPos = _Value2X(v);
	char ac[5];
	auto pText = _GetText(ac);
	GUI.Font(Props.pFont);
	auto &&rText = _GetTextRect(pText);
	auto rInside = rClient / EffectSize();
	auto r = rInside;
	r.x1 = xPos - 1;
	WObj::SetUserClipRect(&r);
	_DrawPart(0, rText.left_top(), pText);
	r = rInside;
	r.x0 = xPos;
	WObj::SetUserClipRect(&r);
	_DrawPart(1, rText.left_top(), pText);
	WObj::SetUserClipRect(nullptr);
	DrawDown(rClient);
}
void ProgBar::_Callback(WM_MSG *pMsg) {
	auto pObj = (ProgBar *)pMsg->pWin;
	if (!pObj->HandleActive(pMsg))
		return;
	switch (pMsg->msgid) {
		case WM_PAINT:
			pObj->_OnPaint();
			return;
		case WM_DELETE:
			pObj->~ProgBar();
			break;
	}
	DefCallback(pMsg);
}

ProgBar::ProgBar(
	int x0, int y0, int xsize, int ysize,
	WObj *pParent, uint16_t Id, uint16_t Flags, uint16_t ExFlags,
	int16_t Min, int16_t Max, int16_t v, const char *s) :
	Widget(x0, y0, xsize, ysize,
		   _Callback, pParent, Id, Flags),
	Min(Min), Max(Max), v(v), text(s)
{ Effect(&Widget::EffectItf::None); }

void ProgBar::Value(int16_t v) {
	if (v < Min) v = Min;
	if (v > Max) v = Max;
	if (v == this->v) return;
	SRect r;
	if (v < this->v) {
		r.x0 = _Value2X(v);
		r.x1 = _Value2X(v);
	}
	else {
		r.x0 = _Value2X(v);
		r.x1 = _Value2X(v);
	}
	r.y0 = 0;
	r.y1 = 4095;
	if (!text) {
		this->v = v;
		Invalidate();
		return;
	}
	char acBuffer[5];
	auto pOldFont = GUI.Font();
	GUI.Font(Props.pFont);
	r |= _GetTextRect(_GetText(acBuffer));
	this->v = v;
	r |= _GetTextRect(_GetText(acBuffer));
	GUI.Font(pOldFont);
	Invalidate(r);
}
void ProgBar::Text(const char *s) {
	char acBuffer[5];
	auto pOldFont = GUI.Font();
	GUI.Font(Props.pFont);
	auto &&r1 = _GetTextRect(_GetText(acBuffer));
	if (GUI__SetText(&text, s)) {
		auto &&r2 = _GetTextRect(_GetText(acBuffer));
		Invalidate(r1 | r2);
	}
	GUI.Font(pOldFont);
}
void ProgBar::Range(int Min, int Max) {
	if (Max <= Min)
		return;
	if (this->Max != Max || this->Min != Min) {
		this->Min = Min;
		this->Max = Max;
		Invalidate();
	}
}
