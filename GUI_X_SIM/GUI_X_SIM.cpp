#define DLL_IMPORTS
#define USE_AYXANDAR
#include "SimDisp.h"

#define DLL_EXPORTS
#include "GUI_X_SIM.h"

#include "wx_window.h"
#include "wx_realtime.h"

#pragma region Function Exportation 
GUI_X_SIM_pfnOnSize pfnIntResize;
GUI_X_SIM_pfnOnMouse pfnIntMouse;
GUI_X_SIM_pfnOnKey pfnIntKey;

REG_FUNC(void, IntResize, GUI_X_SIM_pfnOnSize pfnOnSize) {
	pfnIntResize = pfnOnSize;
}
REG_FUNC(void, IntMouse, GUI_X_SIM_pfnOnMouse pfnOnMouse) {
	pfnIntMouse = pfnOnMouse;
}
REG_FUNC(void, IntKey, GUI_X_SIM_pfnOnKey pfnOnKey) {
	pfnIntKey = pfnOnKey;
}

REG_FUNC(void, CreateDisplay, const wchar_t *lpTitle, uint16_t xSize, uint16_t ySize) {
	using namespace SimDisp;
	LoadDll(_T("SimClient.dll"));
	assertl(Open(L"TUI - User Interface", xSize, ySize));
	SetOnResize([](uint16_t nSizeX, uint16_t nSizeY) -> uint8_t {
		pfnIntResize(nSizeX, nSizeY);
		return TRUE;
	});
	SetOnMouse([](int16_t xPos, int16_t yPos, int16_t zPos,
				  tSimDisp_MouseKey mk) {
		if (xPos < 0 || yPos < 0)
			return;
		pfnIntMouse(xPos, yPos, mk.Left);
	});
	SetOnKey([](uint16_t vk, uint8_t bPressed) {
		pfnIntKey(vk, bPressed);
	});
	SetOnClose([] {
	});
	HideCursor(true);
	Show(true);
	Ayx.Init();
	pfnIntResize(xSize, ySize);
}
REG_FUNC(void, UserClose, void) {
	SimDisp::Ayx.Fill(RGB(0, 0, 0));
	ExitProcess(0);
}

REG_FUNC(void, Dot, uint16_t x, uint16_t y, uint32_t rgb) {
	SimDisp::Ayx.Dot({ x, y }, rgb);
}
REG_FUNC(void, Fill, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t rgb) {
	SimDisp::Ayx.Fill(rgb, { x0, y0, x1, y1 });
}
REG_FUNC(void, SetBitmap, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const uint32_t *lpBits, uint16_t BytesPerLine) {
	SimDisp::Ayx.SetRect({ x0, y0, x1, y1 }, lpBits, BytesPerLine);
}
REG_FUNC(void, GetBitmap, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t *lpBits, uint16_t BytesPerLine) {
	SimDisp::Ayx.GetRect({ x0, y0, x1, y1 }, lpBits, BytesPerLine);
}

using namespace WX;
REG_FUNC(void, OpenConsole, void) {
	Console.Attach(SimDisp::ConsoleEnableShow(true));
	Console.Title(S("TUI - Console"));
	Console.Select();
	Console.Reopen();
	SimDisp::ConsoleEnableShow(true);
}
REG_FUNC(void, CloseConsole, void) {
	Console.Free();
}

#pragma endregion
