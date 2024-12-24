#include "GUI.h"

#define DLL_IMPORTS
#define USE_AYXANDAR
#include "SimDisp.h"

static uint16_t xSizeDisp = 0, ySizeDisp = 0;
SRect LCD_Rect() {
	return { 0, 0, xSizeDisp, ySizeDisp };
}

void SimDisp_Start(uint16_t xSize, uint16_t ySize) {
	SimDisp::LoadDll(_T("SimClient.dll"));
	//SimDisp::LoadDll(_T("SimDisp.dll"));
	assert(SimDisp::Open(L"TUI By Nurtas Ihram", xSize, ySize));
	SimDisp::SetOnMouse([](int16_t xPos, int16_t yPos, int16_t zPos,
						   tSimDisp_MouseKey mk) {
		if (xPos < 0 || yPos < 0)
			return;
		GUI.PID_STATE.Pressed = mk.Left;
		GUI.PID_STATE.x = xPos;
		GUI.PID_STATE.y = yPos;
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

void MainTask();

int main() {
	SimDisp_Start(800, 480);
	GUI.Init();
	GUI.Cursor.Visible(true);
	MainTask();
	return 0;
}
