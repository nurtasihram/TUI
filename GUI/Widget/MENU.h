#pragma once
#include "GUI_Array.h"
#include "WM.h"

#define MENU_CF_HORIZONTAL              (0<<0)
#define MENU_CF_VERTICAL                (1<<0)
#define MENU_CF_OPEN_ON_POINTEROVER     (1<<1)
#define MENU_CF_CLOSE_ON_SECOND_CLICK   (1<<2)
#define MENU_CF_HIDE_DISABLED_SEL       (1<<3)  /* Hides the selection when a disabled item is selected */
#define MENU_IF_DISABLED          (1<<0)
#define MENU_IF_SEPARATOR         (1<<1)
#define MENU_CI_ENABLED           0
#define MENU_CI_SELECTED          1
#define MENU_CI_DISABLED          2
#define MENU_CI_DISABLED_SEL      3
#define MENU_CI_ACTIVE_SUBMENU    4
#define MENU_BI_LEFT              0
#define MENU_BI_RIGHT             1
#define MENU_BI_TOP               2
#define MENU_BI_BOTTOM            3
#define MENU_ON_ITEMSELECT        0   /* Send to owner when selecting a menu item */
#define MENU_ON_INITMENU          1   /* Send to owner when for the first time selecting a submenu */
#define MENU_ON_INITSUBMENU       2   /* Send to owner when selecting a submenu */
#define MENU_ON_OPEN              3   /* Internal message of menu widget (send to submenus) */
#define MENU_ON_CLOSE             4   /* Internal message of menu widget (send to submenus) */
#define MENU_IS_MENU              5   /* Internal message of menu widget. Owner must call   */
									  /* DefCallback() when not handle the message.      */

struct MENU_MSG_DATA {
	uint16_t MsgType;
	uint16_t ItemId;
};
struct Menu : public Widget {
public:		
	struct Property {
		CFont *pFont = &GUI_Font13_1;
		RGBC aTextColor[5]{
			RGB_BLACK,
			RGB_WHITE,
			0x7C7C7C,
			RGB_LIGHTGRAY,
			RGB_WHITE
		};
		RGBC aBkColor[5]{
			RGB_LIGHTGRAY,
			0x980000,
			RGB_LIGHTGRAY,
			0x980000,
			0x7C7C7C
		};
		uint8_t aBorder[4]{
			4,
			4,
			2,
			2,
		};
	} static DefaultProps;
	struct Item {
		Menu *pSubmenu = nullptr;
		char *pText = nullptr;
		uint16_t Id, Flags;
		uint16_t TextWidth;
	};
// private:
	Property  Props;
	GUI_Array<Item> ItemArray;
	WObj *pOwner;
	uint16_t Flags;
	uint16_t Width;
	uint16_t Height;
	uint16_t Sel;
	bool bSubmenuActive;
public:
};

Menu *MENU_CreateEx      (int x0, int y0, int xSize, int ySize, WObj *pParent, uint16_t Flags, uint16_t ExFlags, uint16_t Id);
void      MENU_Attach       (Menu *pObj, WObj *pDestWin, Point Pos, Point Size, uint16_t Flags);
void      MENU_Popup        (Menu *pObj, WObj *pDestWin, Point Pos, Point Size, uint16_t Flags);
void      MENU_SetOwner     (Menu *pObj, WObj *pOwner);
void      MENU_AddItem      (Menu *pObj, const Menu::Item *pItem);
void      MENU_DeleteItem   (Menu *pObj, uint16_t ItemId);
void      MENU_InsertItem   (Menu *pObj, uint16_t ItemId, const Menu::Item *pItem);
void      MENU_SetItem      (Menu *pObj, uint16_t ItemId, const Menu::Item *pItem);
void      MENU_GetItem      (Menu *pObj, uint16_t ItemId, Menu::Item *pItem);
void      MENU_GetItemText  (Menu *pObj, uint16_t ItemId, char *pBuffer, unsigned BufferSize);
unsigned  MENU_GetNumItems  (Menu *pObj);
void      MENU_DisableItem  (Menu *pObj, uint16_t ItemId);
void      MENU_EnableItem   (Menu *pObj, uint16_t ItemId);
void      MENU_SetTextColor (Menu *pObj, unsigned ColorIndex, RGBC Color);
void      MENU_SetBkColor   (Menu *pObj, unsigned ColorIndex, RGBC Color);
void      MENU_SetBorderSize(Menu *pObj, unsigned BorderIndex, uint8_t BorderSize);
void      MENU_SetFont      (Menu *pObj, CFont *pFont);
// private
void      MENU__RecalcTextWidthOfItems(Menu* pObj);
void      MENU__ResizeMenu            (Menu *pObj);
void      MENU__SetItem               (Menu *pObj, unsigned Index, const Menu::Item *pItemData);
void      MENU__SetItemFlags          (Menu* pObj, unsigned Index, uint16_t Mask, uint16_t Flags);
void      MENU__InvalidateItem        (Menu *pObj, unsigned Index);
int       MENU__FindItem              (Menu *pObj, uint16_t ItemId, Menu **ppMenu);
size_t    MENU__SendMenuMessage       (Menu *pObj, WObj *pDestWin, uint16_t MsgType, uint16_t ItemId);
