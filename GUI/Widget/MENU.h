#pragma once
#include "GUI_ARRAY.h"
#include "Widget.h"
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
									  /* WM_DefaultProc() when not handle the message.      */
typedef struct {
	uint16_t MsgType;
	uint16_t ItemId;
} MENU_MSG_DATA;
typedef struct {
	RGBC    aTextColor[5];
	RGBC    aBkColor[5];
	uint8_t aBorder[4];
	CFont  *pFont;
} MENU_PROPS;
struct Menu : public Widget {
	MENU_PROPS  Props;
	GUI_ARRAY   ItemArray;
	WObj     *pOwner;
	uint16_t    Flags;
	char        IsSubmenuActive;
	int         Width;
	int         Height;
	int         Sel;
};
typedef struct {
	char     *pText;
	uint16_t  Id;
	uint16_t  Flags;
	Menu *pSubmenu;
} MENU_ITEM_DATA;
typedef struct {
	Menu   *pSubmenu;
	uint16_t    Id;
	uint16_t    Flags;
	int         TextWidth;
	char        acText[1];
} MENU_ITEM;
Menu *MENU_CreateEx      (int x0, int y0, int xSize, int ySize, WObj *pParent, int WinFlags, int ExFlags, int Id);
Menu *MENU_CreateIndirect(const GUI_WIDGET_CREATE_INFO *pCreateInfo, WObj *pParent, int x0, int y0, WM_CALLBACK *cb);
void      MENU_Attach       (Menu *pObj, WObj *pDestWin, Point Pos, Point Size, int Flags);
void      MENU_Popup        (Menu *pObj, WObj *pDestWin, Point Pos, Point Size, int Flags);
void      MENU_SetOwner     (Menu *pObj, WObj *pOwner);
void      MENU_AddItem      (Menu *pObj, const MENU_ITEM_DATA *pItemData);
void      MENU_DeleteItem   (Menu *pObj, uint16_t ItemId);
void      MENU_InsertItem   (Menu *pObj, uint16_t ItemId, const MENU_ITEM_DATA *pItemData);
void      MENU_SetItem      (Menu *pObj, uint16_t ItemId, const MENU_ITEM_DATA *pItemData);
void      MENU_GetItem      (Menu *pObj, uint16_t ItemId, MENU_ITEM_DATA *pItemData);
void      MENU_GetItemText  (Menu *pObj, uint16_t ItemId, char *pBuffer, unsigned BufferSize);
unsigned  MENU_GetNumItems  (Menu *pObj);
void      MENU_DisableItem  (Menu *pObj, uint16_t ItemId);
void      MENU_EnableItem   (Menu *pObj, uint16_t ItemId);
void      MENU_SetTextColor (Menu *pObj, unsigned ColorIndex, RGBC Color);
void      MENU_SetBkColor   (Menu *pObj, unsigned ColorIndex, RGBC Color);
void      MENU_SetBorderSize(Menu *pObj, unsigned BorderIndex, uint8_t BorderSize);
void      MENU_SetFont      (Menu *pObj, CFont *pFont);
RGBC                 MENU_GetDefaultTextColor (unsigned ColorIndex);
RGBC                 MENU_GetDefaultBkColor   (unsigned ColorIndex);
uint8_t              MENU_GetDefaultBorderSize(unsigned BorderIndex);
const Widget::Effect *MENU_GetDefaultEffect    (void);
CFont               *MENU_GetDefaultFont      (void);
void                 MENU_SetDefaultTextColor (unsigned ColorIndex, RGBC Color);
void                 MENU_SetDefaultBkColor   (unsigned ColorIndex, RGBC Color);
void                 MENU_SetDefaultBorderSize(unsigned BorderIndex, uint8_t BorderSize);
void                 MENU_SetDefaultEffect    (const Widget::Effect *pEffect);
void                 MENU_SetDefaultFont      (CFont *pFont);
#define MENU_Delete(pObj)         WM_DeleteWindow(pObj)
#define MENU_Paint(pObj)          WM_Paint(pObj)
#define MENU_Invalidate(pObj)     pObj->Invalidate()
// private
void      MENU__RecalcTextWidthOfItems(Menu* pObj);
void      MENU__ResizeMenu            (Menu *pObj);
unsigned  MENU__GetNumItems           (Menu* pObj);
char      MENU__SetItem               (Menu *pObj, unsigned Index, const MENU_ITEM_DATA* pItemData);
void      MENU__SetItemFlags          (Menu* pObj, unsigned Index, uint16_t Mask, uint16_t Flags);
void      MENU__InvalidateItem        (Menu *pObj, unsigned Index);
int       MENU__FindItem              (Menu *pObj, uint16_t ItemId, Menu **ppMenu);
size_t    MENU__SendMenuMessage       (Menu *pObj, WObj *pDestWin, uint16_t MsgType, uint16_t ItemId);
