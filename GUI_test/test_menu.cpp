#include "Frame.h"

#define ID_MENU             (GUI_ID_USER +  0)
#define ID_MENU_FILE_NEW    (GUI_ID_USER +  1)
#define ID_MENU_FILE_OPEN   (GUI_ID_USER +  2)
#define ID_MENU_FILE_CLOSE  (GUI_ID_USER +  3)
#define ID_MENU_FILE_EXIT   (GUI_ID_USER +  4)
#define ID_MENU_FILE_RECENT (GUI_ID_USER +  5)
#define ID_MENU_RECENT_0    (GUI_ID_USER +  6)
#define ID_MENU_RECENT_1    (GUI_ID_USER +  7)
#define ID_MENU_RECENT_2    (GUI_ID_USER +  8)
#define ID_MENU_RECENT_3    (GUI_ID_USER +  9)
#define ID_MENU_EDIT_UNDO   (GUI_ID_USER + 10)
#define ID_MENU_EDIT_REDO   (GUI_ID_USER + 11)
#define ID_MENU_EDIT_COPY   (GUI_ID_USER + 12)
#define ID_MENU_EDIT_PASTE  (GUI_ID_USER + 13)
#define ID_MENU_EDIT_DELETE (GUI_ID_USER + 14)
#define ID_MENU_HELP_ABOUT  (GUI_ID_USER + 15)

static void _AddMenuItem(
	Menu *pMenu, Menu *pSubmenu,
	const char *pText,
	uint16_t Id, uint16_t Flags = 0) {
	Menu::Item Item;
	Item.pText = const_cast<char *>(pText);
	Item.pSubmenu = pSubmenu;
	Item.Flags = Flags;
	Item.Id = Id;
	pMenu->AddItem(Item);
}

static Menu *_CreateMenu(Frame *pParent) {
	auto pMenu = new Menu(MENU_CF_HORIZONTAL);
	pParent->Menu(pMenu);

	auto pMenuRecent = new Menu(MENU_CF_VERTICAL);
	_AddMenuItem(pMenuRecent, 0, "File 1", ID_MENU_RECENT_0);
	_AddMenuItem(pMenuRecent, 0, "File 2", ID_MENU_RECENT_1);
	_AddMenuItem(pMenuRecent, 0, "File 3", ID_MENU_RECENT_2);
	_AddMenuItem(pMenuRecent, 0, "File 4", ID_MENU_RECENT_3);

	auto pMenuFile = new Menu(MENU_CF_VERTICAL);
	_AddMenuItem(pMenuFile, 0, "New", ID_MENU_FILE_NEW);
	_AddMenuItem(pMenuFile, 0, "Open", ID_MENU_FILE_OPEN);
	_AddMenuItem(pMenuFile, 0, "Close", ID_MENU_FILE_CLOSE, MENU_IF_DISABLED);
	_AddMenuItem(pMenuFile, 0, 0, 0, MENU_IF_SEPARATOR);
	_AddMenuItem(pMenuFile, pMenuRecent, "Files...", ID_MENU_FILE_RECENT);
	_AddMenuItem(pMenuFile, 0, 0, 0, MENU_IF_SEPARATOR);
	_AddMenuItem(pMenuFile, 0, "Exit", ID_MENU_FILE_EXIT);

	auto pMenuEdit = new Menu(MENU_CF_VERTICAL);
	_AddMenuItem(pMenuEdit, 0, "Undo", ID_MENU_EDIT_UNDO);
	_AddMenuItem(pMenuEdit, 0, "Redo", ID_MENU_EDIT_REDO);
	_AddMenuItem(pMenuEdit, 0, 0, 0, MENU_IF_SEPARATOR);
	_AddMenuItem(pMenuEdit, 0, "Copy", ID_MENU_EDIT_COPY);
	_AddMenuItem(pMenuEdit, 0, "Paste", ID_MENU_EDIT_PASTE);
	_AddMenuItem(pMenuEdit, 0, "Delete", ID_MENU_EDIT_DELETE);

	auto pMenuHelp = new Menu(MENU_CF_VERTICAL);
	_AddMenuItem(pMenuHelp, 0, "About", ID_MENU_HELP_ABOUT);

	_AddMenuItem(pMenu, pMenuFile, "File", 0);
	_AddMenuItem(pMenu, pMenuEdit, "Edit", 0);
	_AddMenuItem(pMenu, pMenuHelp, "Help", 0);
	return pMenu;
}

void MainTask() {
	auto pFrm1 = new Frame(
		10, 10, 300, 220,
		WObj::Desktop(), 0,
		WC_VISIBLE, FRAME_CF_MOVEABLE,
		"Frame 1");
	pFrm1->AddMaxButton();
	pFrm1->AddMinButton();

	auto pMenu = new Menu(MENU_CF_HORIZONTAL);
	pFrm1->Menu(pMenu);

	auto pMenuRecent = new Menu(MENU_CF_VERTICAL);
	_AddMenuItem(pMenuRecent, 0, "File 1", ID_MENU_RECENT_0);
	_AddMenuItem(pMenuRecent, 0, "File 2", ID_MENU_RECENT_1);
	_AddMenuItem(pMenuRecent, 0, "File 3", ID_MENU_RECENT_2);
	_AddMenuItem(pMenuRecent, 0, "File 4", ID_MENU_RECENT_3);

	auto pMenuFile = new Menu(MENU_CF_VERTICAL);
	_AddMenuItem(pMenuFile, 0, "New", ID_MENU_FILE_NEW);
	_AddMenuItem(pMenuFile, 0, "Open", ID_MENU_FILE_OPEN);
	_AddMenuItem(pMenuFile, 0, "Close", ID_MENU_FILE_CLOSE, MENU_IF_DISABLED);
	_AddMenuItem(pMenuFile, 0, 0, 0, MENU_IF_SEPARATOR);
	_AddMenuItem(pMenuFile, pMenuRecent, "Files...", ID_MENU_FILE_RECENT);
	_AddMenuItem(pMenuFile, 0, 0, 0, MENU_IF_SEPARATOR);
	_AddMenuItem(pMenuFile, 0, "Exit", ID_MENU_FILE_EXIT);

	auto pMenuEdit = new Menu(MENU_CF_VERTICAL);
	_AddMenuItem(pMenuEdit, 0, "Undo", ID_MENU_EDIT_UNDO);
	_AddMenuItem(pMenuEdit, 0, "Redo", ID_MENU_EDIT_REDO);
	_AddMenuItem(pMenuEdit, 0, 0, 0, MENU_IF_SEPARATOR);
	_AddMenuItem(pMenuEdit, 0, "Copy", ID_MENU_EDIT_COPY);
	_AddMenuItem(pMenuEdit, 0, "Paste", ID_MENU_EDIT_PASTE);
	_AddMenuItem(pMenuEdit, 0, "Delete", ID_MENU_EDIT_DELETE);

	auto pMenuHelp = new Menu(MENU_CF_VERTICAL);
	_AddMenuItem(pMenuHelp, 0, "About", ID_MENU_HELP_ABOUT);

	_AddMenuItem(pMenu, pMenuFile, "File", 0);
	_AddMenuItem(pMenu, pMenuEdit, "Edit", 0);
	_AddMenuItem(pMenu, pMenuHelp, "Help", 0);

	for (;;)
		WObj::Exec1();
}
