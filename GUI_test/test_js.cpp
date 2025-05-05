#include "WM.h"
#include "GUI_X_JS.h"

void MainTask() {
	for (;;) {
		GUI_X_JS_Exec();
		WObj::Exec();
	}
}
