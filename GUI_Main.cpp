#include "GUI.h"
#include "GUI_X_JS.h"
#include "GUI_X_LCD.h"

void MainTask();

extern "C" void GUI_Main() {
	GUI_X_LCD_Init();
	GUI_X_JS_Init();
	GUI_X_JS_CMDL();
	GUI.Init();
	GUI.Cursor.Visible(true);
	MainTask();
}
