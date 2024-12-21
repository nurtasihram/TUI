#include "Frame.h"
#include "CheckBox.h"
#include "Radio.h"

void MainTask() {
	auto pFrame = new Frame(
		0, 0, 250, 250,
		WObj::Desktop(), WC_VISIBLE,
		0, 0, "CheckBox & Radio");
	pFrame->Moveable(true);
	pFrame->AddCloseButton();
	pFrame->AddMaxButton();
	pFrame->AddMinButton();

	auto pCheck0 = new CheckBox(50, 50, 145, 15, pFrame->Client(), 0, 0, "CheckBox 0");
	pCheck0->NumStates(3);
	pCheck0->Visible(true);

	auto pCheck1 = new CheckBox(50, 50 + 15, 145, 15, pFrame->Client(), 0, 0, "CheckBox 1");
	pCheck1->Visible(true);

	auto pCheck2 = new CheckBox(50, 50 + 15 * 2, 145, 15, pFrame->Client(), 0, 0, "CheckBox 2");
	pCheck2->Visible(true);

	auto pRadios = new Radio(50, 50 + 15 * 3, 145, 15 * 3, pFrame->Client(), 0, 0, 0, 3, 15);
	pRadios->Text(0, "Radio 0");
	pRadios->Text(1, "Radio 1");
	pRadios->Text(2, "Radio 2");
	pRadios->Visible(true);

	auto pBtn = new Button(35, 170, 40, 30, pFrame->Client(), 0, 0, "Click");
	pBtn->Visible(true);

	pFrame->Resizeable(true);
	pFrame->Visible(true);

	for (;;)
		WObj::Exec();
}
