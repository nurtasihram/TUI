#include "GUI.h"
#include "GUI_X_SIM.h"

void MainTask();

extern "C" void GUI_Main() {
	GUI_X_Init();
	GUI.Init();
	GUI.Cursor.Visible(true);
	MainTask();
}
