#include "GUI.h"
#include "GUI_X_SIM.h"

void MainTask();

extern "C" void GUI_Main() {
	GUI_X_Init();
	GUI_X_SIM::OpenConsole();
	GUI.Init();
	GUI.Cursor.Visible(true);
	MainTask();
	GUI_X_SIM::UserClose();
}
