#pragma once
#include "GUI_Array.h"
#include "WM.h"

using		MENU_CF = uint16_t;
constexpr	MENU_CF
			MENU_CF_HORIZONTAL            = WC_EX_USER(0),
			MENU_CF_VERTICAL              = WC_EX_USER(1),
			MENU_CF_POPUP                 = WC_EX_USER(1),
			MENU_CF_OPEN_ON_POINTEROVER   = WC_EX_USER(2),
			MENU_CF_CLOSE_ON_SECOND_CLICK = WC_EX_USER(3),
			MENU_CF_HIDE_DISABLED_SEL     = WC_EX_USER(4), /* Hides the selection when a disabled item is selected */
			MENU_CF__ACTIVE               = WC_EX_USER(5),
			MENU_CF__POPUP                = WC_EX_USER(6);
using		MENU_IF = uint8_t;
constexpr	MENU_IF
			MENU_IF_DISABLED  = 1 << 0,
			MENU_IF_SEPARATOR = 1 << 1;
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

class Menu : public Widget {

public:
	struct Property {
		CFont *pFont{ &GUI_Font13_1 };
		RGBC aTextColor[5]{
			/* Enabled */ RGB_BLACK,
			/* Selected */ RGB_WHITE,
			/* Disabled */ RGBC_GRAY(0x7C),
			/* Disabled selected */ RGB_LIGHTGRAY,
			/* Active */ RGB_WHITE
		};
		RGBC aBkColor[5]{
			/* Enabled */ RGB_LIGHTGRAY,
			/* Selected */ RGBC_B(0x98),
			/* Disabled */ RGB_LIGHTGRAY,
			/* Disabled selected */ RGBC_B(0x98),
			/* Active */ RGBC_GRAY(0x7C),
		};
		uint8_t aBorder[4]{
			4,
			4,
			2,
			2,
		};
	} static DefaultProps;

public:
	struct Item {
		Menu *pSubmenu = nullptr;
		GUI_STRING Text;
		uint16_t Id = 0;
		MENU_IF Flags = 0;
		Point Size;
		Item(MENU_IF Flags = 0) : Flags(Flags) {}
		Item(GUI_PCSTR pText, Menu *pSubmenu = nullptr, MENU_IF Flags = 0) :
			pSubmenu(pSubmenu), Text(pText), Flags(Flags) {}
		Item(GUI_PCSTR pText, uint16_t Id, MENU_IF Flags) :
			Text(pText), Id(Id), Flags(Flags) {}
		Item(GUI_PCSTR pText, uint16_t Id, Menu *pSubmenu = nullptr, MENU_IF Flags = 0) :
			pSubmenu(pSubmenu), Text(pText), Id(Id), Flags(Flags) {}
	};
	struct MSG_DAT {
		MENU_MSGID MsgType;
		uint16_t ItemId = 0;
	};
	Property Props = DefaultProps;
	GUI_Array<Item> ItemArray;
	uint16_t Width = 0, Height = 0;
	int16_t Sel = -1;
	bool bSubmenuActive = false;

private:
	int _SendMenuMessage(WObj *pDest, MENU_MSGID MsgType, uint16_t ItemId);

	bool _IsTopLevelMenu();
	bool _HasEffect();
	int _GetEffectSize();
	int _GetItemWidth(uint16_t Index);
	int _GetItemHeight(uint16_t Index);
	int _CalcMenuSizeX();
	int _CalcMenuSizeY();
	int _CalcWindowSizeX();
	int _CalcWindowSizeY();
	int _GetItemFromPos(Point Pos);
	Point _GetItemPos(uint16_t Index);
	void _SetCapture();
	void _ReleaseCapture();
	void _CloseSubmenu();
	void _OpenSubmenu(uint16_t Index);
	void _ClosePopup();
	void _SetSelection(int16_t Index);
	void _SelectItem(uint16_t Index);
	void _DeselectItem();
	void _ActivateItem(uint16_t Index);
	void _ActivateMenu(uint16_t Index);
	void _DeactivateMenu();
	bool _ForwardMouseOverMsg(Point);
	bool _HandleMouse(MOUSE_STATE);
	void _ForwardMouseMsgToOwner(uint16_t MsgId, MOUSE_STATE State);
	WM_RESULT _OnMenu(MSG_DAT);
	bool _OnMouse(MOUSE_STATE);
	void _OnPaint();
	void _SetPaintColors(const Menu::Item &Item, int ItemIndex) const;
	void _InvalidateItem(int16_t Index);
	void _RecalcTextWidthOfItems();
	void _ResizeMenu();
	void _SetItem(uint16_t Index, Item &);

	static WM_RESULT _Callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc);

private:
	Menu(const SRect &rc = {},
		 PWObj pParent = nullptr, uint16_t Id = 0,
		 WM_CF Flags = WC_HIDE, MENU_CF FlagsEx = 0);
public:
	Menu(MENU_CF FlagsEx = 0, uint16_t Id = 0) :
		Menu({},
			 nullptr, Id,
			 WC_HIDE, FlagsEx) {}
private:
	~Menu() = default;

public:
	void Attach(PWObj pOwner, Point Pos, Point Size);
	void Popup(Menu *pOwnerMenu, Point Pos);

	void Add(Item);

	void Delete(uint16_t ItemId);
	void Insert(uint16_t ItemId, Item &Item);
	void Set(uint16_t ItemId, Item &Item);

//	void Enable(uint16_t ItemId, bool bEnable);

#pragma region Properties
public: // Property - Font
	/* R */ inline auto Font() const { return Props.pFont; }
	/* W */ inline void Font(CFont *pFont) {
		Props.pFont = pFont;
		_RecalcTextWidthOfItems();
		_ResizeMenu();
	}
public: // Property - TextColor
	/* R */ inline auto TextColor(MENU_CI Index) const { return Props.aTextColor[Index]; }
	/* W */ inline void TextColor(MENU_CI Index, RGBC Color) {
		Props.aTextColor[Index] = Color;
		Invalidate();
	}
public: // Property - BkColor
	/* R */ inline auto BkColor(MENU_CI Index) const { return Props.aBkColor[Index]; }
	/* W */ inline void BkColor(MENU_CI Index, RGBC Color) {
		Props.aBkColor[Index] = Color;
		Invalidate();
	}
public: // Property - BorderSize
	/* R */ inline auto BorderSize(MENU_BI Index) const { return Props.aBorder[Index]; }
	/* W */ inline void BorderSize(MENU_BI Index, uint8_t BorderSize) {
		Props.aBorder[Index] = BorderSize;
		_ResizeMenu();
	}
public: // Property - NumItems
	/* R */ inline auto NumItems() const { return ItemArray.NumItems(); }
#pragma endregion

};
