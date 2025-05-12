#include "Widgets/Button.h"
#include "Widgets/Frame.h"
//#include "MultiEdit.h"
#include "ext_osk.h"

void Show_Keypad() {
	auto pFrame = new Frame(
		{ 0, 0, 320, 240 },
		WObj::Desktop(), 0,
		WC_VISIBLE, FRAME_CF_RESIZEABLE,
		"Notepad");
	pFrame->AddCloseButton(FRAME_BUTTON_RIGHT, 0);
	pFrame->AddMaxButton(FRAME_BUTTON_RIGHT, 0);
	pFrame->AddMinButton(FRAME_BUTTON_RIGHT, 0);
	new Button(
		SRect::left_top({ 0, 0 }, { 40, 25 }),
		pFrame->Client(), 0,
		WC_VISIBLE, 0,
		"Btn1");
	auto a = new Button(
		SRect::left_top({ 0, 30 }, { 40, 25 }),
		pFrame->Client(), 0,
		WC_VISIBLE, BUTTON_CF_SWITCH,
		"Btn2");
}

void MainTask() {
	Show_Keypad();
	ShowOsk(true);
	for (;;)
		WObj::Exec();
}
