#include <windows.h>
#include <stdio.h>

#include "GUI_X.h"

void GUI_X_Log(const char *s) {
	printf("LOG: %s", s);
}

void GUI_X_Warn(const char *s) {
	printf("WARN: %s", s);
}

void GUI_X_ErrorOut(const char *s) {
	printf("ERR: %s", s);
}
