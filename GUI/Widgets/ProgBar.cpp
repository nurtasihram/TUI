#include "ProgBar.h"

int ProgBar::_Value2X(int v) const {
	auto EffectSize = this->EffectSize();
	auto xSize = SizeX();
	auto Min = this->Min, Max = this->Max;
	if (v < Min) v = Min;
	if (v > Max) v = Max;
	return EffectSize + ((xSize - 2 * EffectSize) * (int32_t)(v - Min)) / (Max - Min);
}
void ProgBar::_DrawPart(int Index, Point ptText, const char *pText) const {
	GUI.BkColor(Props.aBkColor[Index]);
	GUI.PenColor(Props.aTextColor[Index]);
	GUI.Clear();
	GUI.DrawStringAt(pText, ptText);
}
const char *ProgBar::_GetText(char *pBuffer) const {
	if (text) return text;
	const char *pText = pBuffer;
	if (v >= Max) {
		*pBuffer++ = '1';
		*pBuffer++ = '0';
		*pBuffer++ = '0';
		*pBuffer++ = '%';
		*pBuffer = '\0';
		return pText;
	}
	auto prg = (uint8_t)((100 * (v - Min)) / (Max - Min));
	auto c1 = prg / 10, c0 = prg - c1 * 10;
	if (c1)
		*pBuffer++ = (char)c1 + '0';
	*pBuffer++ = (char)c0 + '0';
	*pBuffer++ = '%';
	*pBuffer = '\0';
	return pText;
}
SRect ProgBar::_GetTextRect(const char *pText) const {
	auto &&size = Size();
	int TextWidth = GUI.XDist(pText),
		TextHeight = GUI.Font()->YSize,
		EffectSize = this->EffectSize();
	Point p0{ Off.x, (size.y - TextHeight) / 2 };
	switch (Props.Align & TEXTALIGN_HCENTER) {
		case TEXTALIGN_HCENTER:
			p0.x += (size.x - TextWidth) / 2;
			break;
		case TEXTALIGN_RIGHT:
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
	WObj::UserClip(&r);
	_DrawPart(0, rText.left_top(), pText);
	r = rInside;
	r.x0 = xPos;
	WObj::UserClip(&r);
	_DrawPart(1, rText.left_top(), pText);
	WObj::UserClip(nullptr);
	DrawDown(rClient);
}
WM_RESULT ProgBar::_Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	auto pObj = (ProgBar *)pWin;
	if (!pObj->HandleActive(MsgId, Param))
		return Param;
	switch (MsgId) {
	case WM_PAINT:
		pObj->_OnPaint();
		return 0;
	case WM_DELETE:
		pObj->~ProgBar();
		return 0;
	case WM_GET_CLASS:
		return ClassNames[WCLS_PROGBAR];
	}
	return DefCallback(pObj, MsgId, Param, pSrc);
}

ProgBar::ProgBar(const SRect &rc,
				 PWObj pParent, uint16_t Id,
				 WM_CF Flags, WC_EX FlagsEx,
				 int16_t Min, int16_t Max, int16_t v,
				 const char *s) :
	Widget(rc,
		   _Callback,
		   pParent, Id,
		   Flags, FlagsEx),
	text(s),
	Min(Min), Max(Max), v(v) {}

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
	text = s;
	auto &&r2 = _GetTextRect(_GetText(acBuffer));
	Invalidate(r1 | r2);
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
