#include "Widgets/Frame.h"

enum {
	ID_MENU = GUI_ID_USER,
	ID_MENU_FILE_NEW,
	ID_MENU_FILE_OPEN,
	ID_MENU_FILE_CLOSE,
	ID_MENU_FILE_EXIT,
	ID_MENU_FILE_RECENT,
	ID_MENU_RECENT_0,
	ID_MENU_RECENT_1,
	ID_MENU_RECENT_2,
	ID_MENU_RECENT_3,
	ID_MENU_EDIT_UNDO,
	ID_MENU_EDIT_REDO,
	ID_MENU_EDIT_COPY,
	ID_MENU_EDIT_PASTE,
	ID_MENU_EDIT_DELETE,
	ID_MENU_HELP_ABOUT
};

void MainTask() {
	auto pFrm1 = new Frame(
		SRect::left_top({ 10, 10 }, { 300, 220 }),
		WObj::Desktop(), 0,
		WC_VISIBLE, 0,
		"Frame 1");
	pFrm1->AddMaxButton();
	pFrm1->AddMinButton();

	auto pMenu = new Menu(MENU_CF_HORIZONTAL);
	pFrm1->Menu(pMenu);

	auto pMenuRecent = new Menu(MENU_CF_POPUP);
	pMenuRecent->Add({ "File 1", ID_MENU_RECENT_0 });
	pMenuRecent->Add({ "File 1", ID_MENU_RECENT_0 });
	pMenuRecent->Add({ "File 1", ID_MENU_RECENT_0 });
	pMenuRecent->Add({ "File 1", ID_MENU_RECENT_0 });

	auto pMenuFile = new Menu(MENU_CF_POPUP);
	pMenuFile->Add({ "New", ID_MENU_FILE_NEW });
	pMenuFile->Add({ "Open", ID_MENU_FILE_OPEN });
	pMenuFile->Add({ "Close", ID_MENU_FILE_CLOSE, MENU_IF_DISABLED });
	pMenuFile->Add({ MENU_IF_SEPARATOR });
	pMenuFile->Add({ "Files...", ID_MENU_FILE_RECENT, pMenuRecent });
	pMenuFile->Add({ MENU_IF_SEPARATOR });
	pMenuFile->Add({ "Exit", ID_MENU_FILE_EXIT });

	auto pMenuEdit = new Menu(MENU_CF_POPUP);
	pMenuEdit->Add({ "Undo", ID_MENU_EDIT_UNDO });
	pMenuEdit->Add({ "Redo", ID_MENU_EDIT_REDO });
	pMenuEdit->Add({ MENU_IF_SEPARATOR });
	pMenuEdit->Add({ "Copy", ID_MENU_EDIT_COPY });
	pMenuEdit->Add({ "Paste", ID_MENU_EDIT_PASTE });
	pMenuEdit->Add({ "Delete", ID_MENU_EDIT_DELETE });

	auto pMenuHelp = new Menu(MENU_CF_POPUP);
	pMenuHelp->Add({ "About", ID_MENU_HELP_ABOUT });

	pMenu->Add({ "File", pMenuFile });
	pMenu->Add({ "Edit", pMenuEdit });
	pMenu->Add({ "Help", pMenuHelp });

	for (;;)
		WObj::Exec();
}
