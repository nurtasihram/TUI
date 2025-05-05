#include "Frame.h"
#include "CheckBox.h"
#include "Radio.h"

void MainTask() {
	auto pFrame = new Frame(
		{ 0, 0, 250, 250 },
		WObj::Desktop(), 0,
		0,
		FRAME_CF_MOVEABLE,
		"CheckBox & Radio");
	pFrame->Moveable(true);	
	pFrame->AddCloseButton();
	pFrame->AddMaxButton();
	pFrame->AddMinButton();

	auto pCheck0 = new CheckBox(
		SRect::left_top({ 50, 50 }, { 145, 16 }),
		pFrame->Client(), 0,
		0,
		0,
		"CheckBox 0");
	pCheck0->NumStates(3);
	pCheck0->Visible(true);

	auto pCheck1 = new CheckBox(
		SRect::left_top({ 50, 50 + 16 }, { 145, 16 }),
		pFrame->Client(), 0,
		0,
		0,
		"CheckBox 1");
	pCheck1->Visible(true);

	auto pCheck2 = new CheckBox(
		SRect::left_top({ 50, 50 + 16 * 2 }, { 145, 16 }),
		pFrame->Client(), 0,
		0,
		0,
		"CheckBox 2");
	pCheck2->Visible(true);

	auto pRadios = new Radio(
		SRect::left_top({ 50, 50 + 16 * 3 }, { 145, 16 * 3 }),
		pFrame->Client(),
		0,
		0,
		0, 3, 16);
	pRadios->Text(0, "Radio 0");
	pRadios->Text(1, "Radio 1");
	pRadios->Text(2, "Radio 2");
	pRadios->Visible(true);

	auto pBtn = new Button(
		SRect::left_top({ 35, 170 }, { 40, 30 }),
		pFrame->Client(), 0,
		0,
		0,
		"Click");
	pBtn->Visible(true);

	pFrame->Visible(true);

	for (;;)
		WObj::Exec();
}
