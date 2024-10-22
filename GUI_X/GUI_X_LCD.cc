#define DLL_IMPORTS 1
#define AYXANDAR
#include "SimDisp.h"

#include "GUI.h"

Ayxandar SimDisp::Ayx;

void LCD_DrawHLine(int x0, int y, int x1) {
	SimDisp::Ayx.Fill(GUI.PenColor(), {x0, y, x1, y});
}
void LCD_DrawVLine(int x, int y0, int y1) {
	SimDisp::Ayx.Fill(GUI.PenColor(), { x, y0, x, y1 });
}
void LCD_FillRect(int x0, int y0, int x1, int y1) {
	SimDisp::Ayx.Fill(GUI.PenColor(), { x0, y0, x1, y1 });
}

void LCD_SetPixel(int x, int y, RGBC Color) {
	SimDisp::Ayx.Dot({ x, y }, Color);
}
RGBC LCD_GetPixel(int x, int y) {
	return SimDisp::Ayx.Dot({ x, y });
}

static uint16_t xSizeDisp = 0, ySizeDisp = 0;
SRect LCD_Rect() {
	return { 0, 0, xSizeDisp, ySizeDisp };
}

void LCD_Init(void) {
	SimDisp::LoadDll(_T("SimClient.dll"));
	//SimDisp::LoadDll(_T("SimDisp.dll"));
	assert(SimDisp::Open(L"TUI By Nurtas Ihram", 800, 480));
	SimDisp::SetOnMouse([](int16_t xPos, int16_t yPos, int16_t zPos,
						   tSimDisp_MouseKey mk) {
		if (xPos < 0 || yPos < 0)
			return;
		static PidState _State;
		if (mk.Left) _State.Pressed = 1;
		else _State.Pressed = 0;
		_State.x = xPos;
		_State.y = yPos;
		GUI_PID_StoreState(&_State);
	});
	SimDisp::SetOnDestroy([] {
		ExitProcess(0);
	});
	SimDisp::SetOnResize([](uint16_t nSizeX, uint16_t nSizeY) -> BOOL {
		xSizeDisp = nSizeX;
		ySizeDisp = nSizeY;
		return TRUE;
	});
	SimDisp::GetSize(&xSizeDisp, &ySizeDisp);
	SimDisp::HideCursor(true);
	SimDisp::AutoFlush(true);
	SimDisp::Show(true);
	SimDisp::Ayx.Init();
}
