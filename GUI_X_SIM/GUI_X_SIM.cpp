#define DLL_IMPORTS
#define USE_AYXANDAR
#include "SimDisp.h"

#define DLL_EXPORTS
#include "GUI_X_SIM.h"

#include "./wx/window.h"
#include "./wx/realtime.h"

Ayxandar SimDisp::Ayx;

int16_t *pMouseX = nullptr, *pMouseY = nullptr;
int8_t *pMouseKey = 0;
uint16_t *pSizeX = nullptr, *pSizeY = nullptr;

#pragma region Function Exportation 
REG_FUNC(void, BindSize, uint16_t *xSize, uint16_t *ySize) {
	pSizeX = xSize;
	pSizeY = ySize;
}
REG_FUNC(void, BindMouse, int16_t *xMouse, int16_t *yMouse, int8_t *keyMouse) {
	pMouseX = xMouse;
	pMouseY = yMouse;
	pMouseKey = keyMouse;
}
REG_FUNC(void, CreateDisplay, const wchar_t *lpTitle, uint16_t xSize, uint16_t ySize) {
	using namespace SimDisp;
	LoadDll(_T("SimClient.dll"));
	assertl(Open(L"TUI - User Interface", xSize, ySize));
	SetOnMouse([](int16_t xPos, int16_t yPos, int16_t zPos,
						   tSimDisp_MouseKey mk) {
		if (xPos < 0 || yPos < 0)
			return;
		*pMouseX = xPos;
		*pMouseY = yPos;
		*pMouseKey = mk.Left;
	});
	SetOnClose([] {
		ExitProcess(0);
	});
	SetOnResize([](uint16_t nSizeX, uint16_t nSizeY) -> BOOL {
		*pSizeX = nSizeX;
		*pSizeY = nSizeY;
		return TRUE;
	});
	GetSize(pSizeX, pSizeY);
	HideCursor(true);
	Show(true);
	Ayx.Init();
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

#include <iostream>
#include "GUI_X_JS.h"

using namespace WX;
char acJscode[1024];
Event evtExec;
volatile bool bExec = false;
struct BaseOf_Thread(JSCBox) {
	inline void OnStart() {
		auto wSimDisp = force_cast<Window>(SimDisp::GetHWND());
		LSize szScreen{ GetSystemMetrics(SM_CXSCREEN) + 1, GetSystemMetrics(SM_CYSCREEN) + 1 };
		auto &&rsSimDisp = wSimDisp.Rect();
		wSimDisp.Position({ (szScreen.cx - rsSimDisp.xsize()) / 2, 0 } );
		ConsoleCtl con;
		con.Attach(SimDisp::ConsoleEnableShow(true));
		con.Title(S("TUI - JS-CL"));
		con.Select();
		con.Reopen();
		Window wCon = con;
		wCon.Size({ wCon.Size().cx, szScreen.cy - rsSimDisp.ysize() });
		SimDisp::ConsoleEnableShow(true);
		evtExec = Event::Create().Preset();
		for (;;) {
			evtExec.WaitForSignal();
			std::cout << "JavaScript > ";
			std::cin.getline(acJscode, sizeof(acJscode));
			bExec = true;
		}
	}
	void Run() { assertl(Create()); }
} box;

REG_FUNC(void, JS_Init, void) {
	box.Run();
}
REG_FUNC(const char *, JS_NewCommand, void) {
	if (!bExec) return O;
	evtExec.Reset();
	return acJscode;
}
REG_FUNC(void, JS_NextCommand, void) {
	evtExec.Set();
	bExec = false;
}
REG_FUNC(void, JS_ErrorCommand, js_exception err) {
	std::cout << "FATAL: " << err.MsgId << std::endl;
	std::cout << "CODE:  " << err.code << std::endl;
}

#pragma endregion

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			break;
		case DLL_PROCESS_DETACH:
			if (!lpvReserved) break;
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}
