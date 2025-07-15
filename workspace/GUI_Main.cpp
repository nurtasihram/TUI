#include "WM.h"
#include "GUI_X_SIM.h"

extern void MainTask();

extern "C" void GUI_Main() {
	GUI_X_Init();
	GUI_X_SIM::OpenConsole();
	GUI.Init();
	GUI.Cursor.Visible(true);
	WObj::Init();
	MainTask();
	WObj::Deinit();
	GUI_X_SIM::UserClose();
}
