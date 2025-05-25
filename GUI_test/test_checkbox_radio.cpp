#include "Widgets/Frame.h"
#include "Widgets/CheckBox.h"
#include "Widgets/Radio.h"

void MainTask() {
	auto pFrame = new Frame(
		{ 0, 0, 250, 250 },
		nullptr, 0,
		0, 0,
		"CheckBox & Radio");
	pFrame->AddCloseButton();
	//pFrame->AddMaxButton();
	//pFrame->AddMinButton();

	auto pCheck0 = new CheckBox(
		SRect::left_top({ 50, 50 }, { 145, 16 }),
		pFrame->Client(), 0,
		WC_VISIBLE, 0,
		"CheckBox 0");
	pCheck0->NumStates(3);

	new CheckBox(
		SRect::left_top({ 50, 50 + 16 }, { 145, 16 }),
		pFrame->Client(), 0,
		WC_VISIBLE, 0,
		"CheckBox 1");

	new CheckBox(
		SRect::left_top({ 50, 50 + 16 * 2 }, { 145, 16 }),
		pFrame->Client(), 0,
		WC_VISIBLE, 0,
		"CheckBox 2");

	new Radio(
		SRect::left_top({ 50, 50 + 16 * 3 }, { 145, 16 * 3 }),
		pFrame->Client(), 0,
		WC_VISIBLE, 0,
		"Radio 0\0"
		"Radio 1\0"
		"Radio 2\0", 16);

	new Button(
		SRect::left_top({ 35, 170 }, { 40, 30 }),
		pFrame->Client(), 0,
		WC_VISIBLE,
		0,
		"Click");

	pFrame->Visible(true);

	for (;;)
		WObj::Exec();
}
