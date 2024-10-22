#include "Widget.h"
#include "GUI.h"

const Widget::Effect Widget::Effect::None;

#pragma region Simple
class Effect_Simple : public Widget::Effect {
public:
	Effect_Simple() : Widget::Effect(1) {}
	void DrawUp() const override {
		SRect r;
		WM_GetClientRect(&r);
		DrawUpRect(r);
	}
	void DrawDown() const override {
		SRect r;
		WM_GetClientRect(&r);
		DrawDownRect(r);
	}
	void DrawUpRect(const SRect &r) const override {
		GUI.PenColor(GUI_BLACK);
		GUI_DrawRect(r.x0, r.y0, r.x1, r.y1); /* Draw rectangle around it */
	}
	void DrawDownRect(const SRect &r) const override {
		GUI.PenColor(GUI_BLACK);
		GUI_DrawRect(r.x0, r.y0, r.x1, r.y1); /* Draw rectangle around it */
	}
};
const Widget::Effect &&Widget::Effect::Simple = Effect_Simple();
#pragma endregion

#pragma region D3
class Effect_D3 : public Widget::Effect {
public:
	Effect_D3() : Widget::Effect(2) {}
	void DrawUp() const override {
		SRect r;
		WM_GetClientRect(&r);
		DrawUpRect(r);
	}
	void DrawDown() const override {
		SRect r;
		WM_GetClientRect(&r);
		DrawDownRect(r);
	}
	void DrawUpRect(const SRect &r) const override {
		GUI.PenColor(0x000000);
		GUI_DrawRect(r.x0, r.y0, r.x1, r.y1);
		GUI.PenColor(0xffffff);
		GUI_DrawHLine(r.y0 + 1, r.x0 + 1, r.x1 - 2);
		GUI_DrawVLine(r.x0 + 1, r.y0 + 1, r.y1 - 2);
		GUI.PenColor(0x555555);
		GUI_DrawHLine(r.y1 - 1, r.x0 + 1, r.x1 - 1);
		GUI_DrawVLine(r.x1 - 1, r.y0 + 1, r.y1 - 2);
	}
	void DrawDownRect(const SRect &r) const override {
		GUI.PenColor(0x808080);
		GUI_DrawHLine(r.y0, r.x0, r.x1);
		GUI_DrawVLine(r.x0, r.y0 + 1, r.y1);
		GUI.PenColor(0x0);
		GUI_DrawHLine(r.y0 + 1, r.x0 + 1, r.x1 - 1);
		GUI_DrawVLine(r.x0 + 1, r.y0 + 2, r.y1 - 1);
		GUI.PenColor(0xffffff);
		GUI_DrawHLine(r.y1, r.x0 + 1, r.x1);
		GUI_DrawVLine(r.x1, r.y0 + 1, r.y1);
		GUI.PenColor(0xc0c0c0);
		GUI_DrawHLine(r.y1 - 1, r.x0 + 2, r.x1 - 1);
		GUI_DrawVLine(r.x1 - 1, r.y0 + 2, r.y1 - 1);
	}
};
const Widget::Effect &&Widget::Effect::D3 = Effect_D3();
#pragma endregion

#pragma region D3L1
class Effect_D3L1 : public Widget::Effect {
public:
	Effect_D3L1() : Widget::Effect(1) {}
	void DrawUp() const override {
		SRect r;
		WM_GetClientRect(&r);
		DrawUpRect(r);
	}
	void DrawDown() const override {
		SRect r;
		WM_GetClientRect(&r);
		DrawDownRect(r);
	}
	void DrawUpRect(const SRect &r) const override {
		GUI.PenColor(0xE7E7E7);
		GUI_DrawHLine(r.y0, r.x0, r.x1 - 1);
		GUI_DrawVLine(r.x0, r.y0 + 1, r.y1 - 1);
		GUI.PenColor(0x606060);
		GUI_DrawHLine(r.y1, r.x0, r.x1);
		GUI_DrawVLine(r.x1, r.y0, r.y1 - 1);
	}
	void DrawDownRect(const SRect &r) const override {
		GUI.PenColor(0x606060);
		GUI_DrawHLine(r.y0, r.x0, r.x1 - 1);
		GUI_DrawVLine(r.x0, r.y0 + 1, r.y1 - 1);
		GUI.PenColor(0xE7E7E7);
		GUI_DrawHLine(r.y1, r.x0, r.x1);
		GUI_DrawVLine(r.x1, r.y0, r.y1 - 1);
	}
};
const Widget::Effect &&Widget::Effect::D3L1 = Effect_D3L1();
#pragma endregion

#pragma region D3L2
class Effect_D3L2 : public Widget::Effect {
public:
	Effect_D3L2() : Widget::Effect(2) {}
	void DrawUp() const override {
		SRect r;
		WM_GetClientRect(&r);
		DrawUpRect(r);
	}
	void DrawDown() const override {
		SRect r;
		WM_GetClientRect(&r);
		DrawDownRect(r);
	}
	void DrawUpRect(const SRect &r) const override {
		GUI.PenColor(0xD0D0D0);
		GUI_DrawHLine(r.y0, r.x0, r.x1 - 1);
		GUI_DrawVLine(r.x0, r.y0 + 1, r.y1 - 1);
		GUI.PenColor(0xE7E7E7);
		GUI_DrawHLine(r.y0 + 1, r.x0 + 1, r.x1 - 2);
		GUI_DrawVLine(r.x0 + 1, r.y0 + 2, r.y1 - 2);
		GUI.PenColor(0x606060);
		GUI_DrawHLine(r.y1, r.x0, r.x1);
		GUI_DrawVLine(r.x1, r.y0, r.y1 - 1);
		GUI.PenColor(0x9A9A9A);
		GUI_DrawHLine(r.y1 - 1, r.x0 + 1, r.x1 - 1);
		GUI_DrawVLine(r.x1 - 1, r.y0 + 1, r.y1 - 2);
	}
	void DrawDownRect(const SRect &r) const override {
		GUI.PenColor(0x9A9A9A);
		GUI_DrawHLine(r.y0, r.x0, r.x1 - 1);
		GUI_DrawVLine(r.x0, r.y0 + 1, r.y1 - 1);
		GUI.PenColor(0x606060);
		GUI_DrawHLine(r.y0 + 1, r.x0 + 1, r.x1 - 2);
		GUI_DrawVLine(r.x0 + 1, r.y0 + 2, r.y1 - 2);
		GUI.PenColor(0xE7E7E7);
		GUI_DrawHLine(r.y1, r.x0, r.x1);
		GUI_DrawVLine(r.x1, r.y0, r.y1 - 1);
		GUI.PenColor(0xD0D0D0);
		GUI_DrawHLine(r.y1 - 1, r.x0 + 1, r.x1 - 1);
		GUI_DrawVLine(r.x1 - 1, r.y0 + 1, r.y1 - 2);
	}
};
const Widget::Effect &&Widget::Effect::D3L2 = Effect_D3L2();
#pragma endregion

const Widget::Effect *Widget::pEffectDefault = &Widget::Effect::D3;
