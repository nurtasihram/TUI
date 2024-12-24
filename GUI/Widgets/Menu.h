#pragma once
#include "GUI_Array.h"
#include "WM.h"

#define MENU_CF_HORIZONTAL              WC_USER(0)
#define MENU_CF_VERTICAL                WC_USER(1)
#define MENU_CF_OPEN_ON_POINTEROVER     WC_USER(2)
#define MENU_CF_CLOSE_ON_SECOND_CLICK   WC_USER(3)
#define MENU_CF_HIDE_DISABLED_SEL       WC_USER(4)  /* Hides the selection when a disabled item is selected */

#define MENU_IF_DISABLED          (1<<0)
#define MENU_IF_SEPARATOR         (1<<1)

#define MENU_CF_ACTIVE            (1<<6)  /* Internal flag only */
#define MENU_CF_POPUP             (1<<7)  /* Internal flag only */

enum MENU_CI {
	 MENU_CI_ENABLED = 0,
	 MENU_CI_SELECTED,
	 MENU_CI_DISABLED,
	 MENU_CI_DISABLED_SEL,
	 MENU_CI_ACTIVE_SUBMENU
};
enum MENU_BI {
	 MENU_BI_LEFT = 0,
	 MENU_BI_RIGHT,
	 MENU_BI_TOP,
	 MENU_BI_BOTTOM
};

enum MENU_MSGID : uint16_t {
	MENU_ON_ITEMSELECT = 0, /* Send to owner when selecting a menu item */
	MENU_ON_INITMENU,		/* Send to owner when for the first time selecting a submenu */
	MENU_ON_INITSUBMENU,	/* Send to owner when selecting a submenu */

	MENU_ON_OPEN,			/* Internal message of menu widget (send to submenus) */
	MENU_ON_CLOSE,			/* Internal message of menu widget (send to submenus) */
	MENU_IS_MENU			/* Internal message of menu widget. Owner must call */
							/* DefCallback() when not handle the message. */
};

struct MENU_MSG_DATA {
	MENU_MSGID MsgType;
	uint16_t ItemId;
};
class Menu : public Widget {
public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aTextColor[5]{
			RGB_BLACK,
			RGB_WHITE,
			RGBC_GRAY(0x7C),
			RGB_LIGHTGRAY,
			RGB_WHITE
		};
		RGBC aBkColor[5]{
			RGB_LIGHTGRAY,
			RGBC_B(0x98),
			RGB_LIGHTGRAY,
			RGBC_B(0x98),
			RGBC_GRAY(0x7C),
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
		uint16_t Id = 0, Flags = 0;
		uint16_t TextWidth = 0;
	};
private:
	Property Props;
	GUI_Array<Item> ItemArray;
	WObj *pOwner = nullptr;
	uint16_t Width = 0, Height = 0;
	int16_t Sel = -1;
	uint16_t Flags = 0;
	bool bSubmenuActive = false;

private:
	void _RecalcTextWidthOfItems();
	void _ResizeMenu();
	void _SetItem(unsigned Index, const Menu::Item &NewItem);
	void _SetItemFlags(unsigned Index, uint16_t Mask, uint16_t Flags);
	void _InvalidateItem(unsigned Index);
	int  _FindItem(uint16_t ItemId, Menu **ppMenu);
	size_t _SendMenuMessage(WObj *pDestWin, MENU_MSGID MsgType, uint16_t ItemId = 0);

	bool _IsTopLevelMenu();
	bool _HasEffect();
	int _GetEffectSize();
	int _CalcTextWidth(const char *sText);
	int _GetItemWidth(unsigned Index);
	int _GetItemHeight(unsigned Index);
	int _CalcMenuSizeX();
	int _CalcMenuSizeY();
	int _CalcWindowSizeX();
	int _CalcWindowSizeY();
	int _GetItemFromPos(Point Pos);
	Point _GetItemPos(unsigned Index);
	void _SetCapture();
	void _ReleaseCapture();
	void _CloseSubmenu();
	void _OpenSubmenu(unsigned Index);
	void _ClosePopup();
	void _SetSelection(int Index);
	void _SelectItem(unsigned Index);
	void _DeselectItem();
	void _ActivateItem(unsigned Index);
	void _ActivateMenu(unsigned Index);
	void _DeactivateMenu();
	bool _ForwardMouseOverMsg(Point Pos);
	bool _HandlePID(const PID_STATE &pid);
	void _ForwardPIDMsgToOwner(int MsgId, PID_STATE *pState);
	WM_RESULT _OnMenu(const MENU_MSG_DATA *pMsgData);
	bool _OnTouch(const PID_STATE *pState);
	bool _OnMouseOver(const PID_STATE *pState);
	void _OnPaint();
	void _SetPaintColors(const Menu::Item &Item, int ItemIndex) const;

	static WM_RESULT _Callback(WObj *pWin, int MsgId, WM_PARAM Param, WObj *pSrc);

public:
	Menu(int x0, int y0, int xsize, int ysize,
		 WObj *pParent, uint16_t Id,
		 WM_CF Flags, uint16_t ExFlags);
	Menu(uint16_t ExFlags = 0, uint16_t Id = 0, WM_CF Flags = 0) :
		Menu(0, 0, 0, 0,
			 WM_UNATTACHED, Id,
			 Flags, ExFlags) {}
private:
	~Menu() { ItemArray.Delete(); }

public:
	void Attach(WObj *pDestWin, Point Pos, Point Size);
	void Popup(Menu *pParentMenu, Point Pos, Point Size, uint16_t Flags);
	void AddItem(const Menu::Item &Item);
	void DeleteItem(uint16_t ItemId);
	void InsertItem(uint16_t ItemId, const Menu::Item &Item);
	void SetItem(uint16_t ItemId, const Menu::Item &Item);

	void DisableItem(uint16_t ItemId);
	void EnableItem(uint16_t ItemId);

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		if (Props.pFont != pFont) {
			Props.pFont = pFont;
			_RecalcTextWidthOfItems();
			_ResizeMenu();
		}
	}
public: // Property - TextColor
	/* R */ inline auto TextColor(MENU_CI Index) const { return Props.aTextColor[Index]; }
	/* W */ inline void TextColor(MENU_CI Index, RGBC Color) {
		if (Props.aTextColor[Index] != Color) {
			Props.aTextColor[Index] = Color;
			Invalidate();
		}
	}
public: // Property - BkColor
	/* R */ inline auto BkColor(MENU_CI Index) const { return Props.aBkColor[Index]; }
	/* W */ inline void BkColor(MENU_CI Index, RGBC Color) {
		if (Props.aBkColor[Index] != Color) {
			Props.aBkColor[Index] = Color;
			Invalidate();
		}
	}
public: // Property - BorderSize
	/* R */ inline auto BorderSize(MENU_BI Index) const { return Props.aBorder[Index]; }
	/* W */ inline void BorderSize(MENU_BI Index, uint8_t BorderSize) {
		if (Props.aBorder[Index] != BorderSize) {
			Props.aBorder[Index] = BorderSize;
			_ResizeMenu();
		}
	}
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return ItemArray.NumItems(); }
public: // Property - Owner
	/* R */ inline auto Owner() const { return pOwner; }
	/* W */ inline void Owner(WObj *pOwner) { this->pOwner = pOwner; }
#pragma endregion

};
