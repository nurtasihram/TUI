#define USE_AYXANDAR
#include "SimDisp.h"

#include "GUI.h"

Ayxandar SimDisp::Ayx;

void LCD_FillRect(const SRect &r) {
	SimDisp::Ayx.Fill(GUI.PenColor(), { r.x0, r.y0, r.x1, r.y1 });
}

void LCD_SetPixel(Point p, RGBC Color) {
	SimDisp::Ayx.Dot({ p.x, p.y }, Color);
}
RGBC LCD_GetPixel(Point p) {
	return SimDisp::Ayx.Dot({ p.x, p.y });
}
