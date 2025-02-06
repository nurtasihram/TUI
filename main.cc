#include "GUI.h"
#include "GUI_X_JS.h"

#define DLL_IMPORTS
#define USE_AYXANDAR
#include "SimDisp.h"

#include "./wx/window.h"
#include "./wx/realtime.h"

using namespace WX;

static uint16_t xSizeDisp = 0, ySizeDisp = 0;
SRect LCD_Rect() {
	return { 0, 0, xSizeDisp, ySizeDisp };
}

void SimDisp_Start(uint16_t xSize, uint16_t ySize) {
	SimDisp::LoadDll(_T("SimClient.dll"));
	assertl(SimDisp::Open(L"TUI - User Interface", xSize, ySize));
	SimDisp::SetOnMouse([](int16_t xPos, int16_t yPos, int16_t zPos,
						   tSimDisp_MouseKey mk) {
		if (xPos < 0 || yPos < 0)
			return;
		GUI.PID_STATE.Pressed = mk.Left;
		GUI.PID_STATE.x = xPos;
		GUI.PID_STATE.y = yPos;
	});
	SimDisp::SetOnClose([] {
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

struct BaseOf_Thread(JSCBox) {
	inline void OnStart() {
		auto wSimDisp = force_cast<Window>(SimDisp::GetHWND());
		LSize szScreen{ GetSystemMetrics(SM_CXSCREEN) + 1, GetSystemMetrics(SM_CYSCREEN) + 1 };
		auto &&rsSimDisp = wSimDisp.Rect();
		wSimDisp.Position({ (szScreen.cx - rsSimDisp.xsize()) / 2, 0 } );
		ConsoleCtl con;
		con.Attach(SimDisp::ConsoleEnableOpen(true));
		con.Select();
		con.std_reopen();
		//con.Title(S("TUI - JavaScript Commandline"));
		//((Window)con).Position(rsSimDisp.left_bottom());
		//((Window)con).Size({ rsSimDisp.xsize(), szScreen.cy - rsSimDisp.ysize() });
		GUI_X_JSCBox();
	}
} box;

int main() {
	SimDisp_Start(800, 480);
	GUI.Init();
	GUI.Cursor.Visible(true);
	assertl(box.Create());
	MainTask();
	return 0;
}

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd) {
	return main();
}
