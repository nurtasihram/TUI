#include "GUI_MAIN.h"

int main() {
	GUI_Main();
	return 0;
}

#include <Windows.h>

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd) {
	return main();
}
