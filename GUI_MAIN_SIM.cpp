#include "GUI.h"
#include "GUI_X_JS.h"
#include "GUI_X_LCD.h"
#include "GUI_X_SIM.h"

void MainTask();

extern "C" void GUI_Main() {
	GUI_X_LCD_Init();
	GUI_X_SIM::OpenConsole();
	GUI_X_SIM::BindKey([](UINT vk, BOOL bPressed) {
		GUI.StoreKeyMsg(vk, bPressed);
	});
	GUI.Init();
	GUI.Cursor.Visible(true);
	MainTask();
}
