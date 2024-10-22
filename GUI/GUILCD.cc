#include "GUI.h"

RGBC GUI_LCD_GetPixel(Point p) {
	if (LCD_Rect() <= p)
		return LCD_GetPixel(p);
	return RGB_INVALID_COLOR;
}
void GUI_LCD_SetPixel(Point p) {
	if (GUI.rClip <= p)
		LCD_SetPixel(p, GUI.PenColor());
}
void GUI_LCD_FillRect(SRect r) {
	if (r &= GUI.rClip)
		LCD_FillRect(r);
}

BitmapRect::BitmapRect(CBitmap &bmp, Point Pos) :
	SRect(bmp.Rect(Pos)),
	pData(bmp.pData),
	BytesPerLine(bmp.BytesPerLine),
	BitsPerPixel(bmp.BitsPerPixel),
	HasTrans(bmp.HasTrans),
	pPalEntries(bmp.PalEntries()) {}
BitmapRect &BitmapRect::operator&=(const SRect &rClip) {
	auto rNew = *this & rClip;
	auto off = rNew.left_top() - left_top();
	auto off_bytes = 0;
	if (off.x > 0) {
		auto xoff_bits = off.x * BitsPerPixel;
		BitsXOff = xoff_bits & 7;
		off_bytes = xoff_bits >> 3;
	}
	if (off.y > 0)
		off_bytes += off.y * BytesPerLine;
	(const uint8_t *&)pData += off_bytes;
	SRect::operator=(rNew);
	return*this;
}

void GUI_LCD_DrawBitmap(BitmapRect bc) {
	bc &= GUI.rClip;
	LCD_DrawBitmap(bc);
}
void GUI_LCD_SetClipRectMax(void) {
	GUI.rClip = LCD_Rect();
}
DRAWMODE GUI_LCD_SetDrawMode(DRAWMODE dm) {
	DRAWMODE OldDM = GUI.DrawMode;
	if ((GUI.DrawMode ^ dm) & DRAWMODE_REV) {
		RGBC temp = GUI.BkColor();
		GUI.BkColor(GUI.PenColor());
		GUI.PenColor(temp);
	}
	GUI.DrawMode = dm;
	return OldDM;
}
