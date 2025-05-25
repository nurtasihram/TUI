#pragma once

/// @brief 控件樣本ID枚擧
enum GUI_ID : int {
	GUI_ID_OK                = 1,
	GUI_ID_CANCEL            = 2,
	GUI_ID_YES               = 3,
	GUI_ID_NO                = 4,
	GUI_ID_CLOSE             = 5,
	GUI_ID_HELP              = 6,
	GUI_ID_MAXIMIZE          = 7,
	GUI_ID_MINIMIZE          = 8,
	GUI_ID_VSCROLL  = 0xFE,
	GUI_ID_HSCROLL  = 0xFF,
	GUI_ID_EDIT0    = 0x100,
	GUI_ID_EDIT1    = 0x101,
	GUI_ID_EDIT2    = 0x102,
	GUI_ID_EDIT3    = 0x103,
	GUI_ID_EDIT4    = 0x104,
	GUI_ID_EDIT5    = 0x105,
	GUI_ID_EDIT6    = 0x106,
	GUI_ID_EDIT7    = 0x107,
	GUI_ID_EDIT8    = 0x108,
	GUI_ID_EDIT9    = 0x109,
	GUI_ID_LISTBOX0 = 0x110,
	GUI_ID_LISTBOX1 = 0x111,
	GUI_ID_LISTBOX2 = 0x112,
	GUI_ID_LISTBOX3 = 0x113,
	GUI_ID_LISTBOX4 = 0x114,
	GUI_ID_LISTBOX5 = 0x115,
	GUI_ID_LISTBOX6 = 0x116,
	GUI_ID_LISTBOX7 = 0x117,
	GUI_ID_LISTBOX8 = 0x118,
	GUI_ID_LISTBOX9 = 0x119,
	GUI_ID_CHECK0   = 0x120,
	GUI_ID_CHECK1   = 0x121,
	GUI_ID_CHECK2   = 0x122,
	GUI_ID_CHECK3   = 0x123,
	GUI_ID_CHECK4   = 0x124,
	GUI_ID_CHECK5   = 0x125,
	GUI_ID_CHECK6   = 0x126,
	GUI_ID_CHECK7   = 0x127,
	GUI_ID_CHECK8   = 0x128,
	GUI_ID_SLIDER0  = 0x130,
	GUI_ID_SLIDER1  = 0x131,
	GUI_ID_SLIDER2  = 0x132,
	GUI_ID_SLIDER3  = 0x133,
	GUI_ID_SLIDER4  = 0x134,
	GUI_ID_SLIDER5  = 0x135,
	GUI_ID_SLIDER6  = 0x136,
	GUI_ID_SLIDER7  = 0x137,
	GUI_ID_SLIDER8  = 0x138,
	GUI_ID_SLIDER9  = 0x139,
	GUI_ID_SCROLLBAR0 = 0x140,
	GUI_ID_SCROLLBAR1 = 0x141,
	GUI_ID_SCROLLBAR2 = 0x142,
	GUI_ID_SCROLLBAR3 = 0x142,
	GUI_ID_RADIO0 = 0x150,
	GUI_ID_RADIO1 = 0x151,
	GUI_ID_RADIO2 = 0x152,
	GUI_ID_RADIO3 = 0x153,
	GUI_ID_RADIO4 = 0x154,
	GUI_ID_RADIO5 = 0x155,
	GUI_ID_RADIO6 = 0x156,
	GUI_ID_RADIO7 = 0x157,
	GUI_ID_TEXT0  = 0x160,
	GUI_ID_TEXT1  = 0x161,
	GUI_ID_TEXT2  = 0x162,
	GUI_ID_TEXT3  = 0x163,
	GUI_ID_TEXT4  = 0x164,
	GUI_ID_TEXT5  = 0x165,
	GUI_ID_TEXT6  = 0x166,
	GUI_ID_TEXT7  = 0x167,
	GUI_ID_TEXT8  = 0x168,
	GUI_ID_TEXT9  = 0x169,
	GUI_ID_BUTTON0 = 0x170,
	GUI_ID_BUTTON1 = 0x171,
	GUI_ID_BUTTON2 = 0x172,
	GUI_ID_BUTTON3 = 0x173,
	GUI_ID_BUTTON4 = 0x174,
	GUI_ID_BUTTON5 = 0x175,
	GUI_ID_BUTTON6 = 0x176,
	GUI_ID_BUTTON7 = 0x177,
	GUI_ID_BUTTON8 = 0x178,
	GUI_ID_BUTTON9 = 0x179,
	GUI_ID_DROPDOWN0  = 0x180,
	GUI_ID_DROPDOWN1  = 0x181,
	GUI_ID_DROPDOWN2  = 0x182,
	GUI_ID_DROPDOWN3  = 0x183,
	GUI_ID_MULTIEDIT0 = 0x190,
	GUI_ID_MULTIEDIT1 = 0x191,
	GUI_ID_MULTIEDIT2 = 0x192,
	GUI_ID_MULTIEDIT3 = 0x193,
	GUI_ID_LISTVIEW0  = 0x200,
	GUI_ID_LISTVIEW1  = 0x201,
	GUI_ID_LISTVIEW2  = 0x202,
	GUI_ID_LISTVIEW3  = 0x203,
	GUI_ID_PROGBAR0   = 0x210,
	GUI_ID_PROGBAR1   = 0x211,
	GUI_ID_PROGBAR2   = 0x212,
	GUI_ID_PROGBAR3   = 0x213,
	GUI_ID_USER     = 0x800
};

enum WM_MESSSAGES : uint16_t {
	 WM_NULL = 0,

	 /// @brief 完成創建窗體後響應
	 WM_CREATE,
	 /// @brief 完成刪除窗體後響應
	 WM_DELETE,
	 /// @brief 繪製窗體前響應
	 /// @param SRect rInvalid
	 WM_PAINT,

	 /// @brief 完成移動窗體後響應
	 WM_MOVED,
	 /// @brief 完成裁剪窗體後響應
	 WM_SIZED,

	 /// @brief 鍵盤在窗體上活躍時響應
	 /// @param KEY_STATE 按鍵狀態
	 WM_KEY,

	 /// @brief 鼠鍵在窗體上活躍時響應
	 /// @param MOUSE_STATE PID相對狀態
	 WM_MOUSE,
	 /// @brief 鼠標在子窗體上活躍時響應
	 /// @param MOUSE_STATE PID相對狀態
	 /// @param pSrc 子窗體
	 WM_MOUSE_CHILD,
	 /// @brief 鼠標在窗體上活躍時響應
	 /// @param MOUSE_STATE PID相對狀態
	 WM_MOUSE_OVER,
	 /// @brief 鼠標在窗體上
	 /// @param MOUSE_CHANGED_STATE PID相對改變狀態
	 WM_MOUSE_CHANGED,

	 /// @brief 窗體捕獲釋放後響應
	 WM_CAPTURE_RELEASED,

	 /// @brief 收取窗體使能狀態改變通知
	 /// @param bool 是否使能
	 WM_NOTIFY_ENABLE,
	 /// @brief 收取窗體焦點狀態改變通知
	 /// @param FOCUS_CHANGED_STATE 焦點狀態
	 WM_NOTIFY_CHILD_HAS_FOCUS,
	 /// @brief 收取客戶窗體改變通知
	 WM_NOTIFY_CLIENT_CHANGE,
	 /// @brief 收取子窗體訊息通知
	 /// @param uint16_t 通知ID
	 WM_NOTIFY_CHILD,
	 /// @brief 收取子窗體訊息通知反射
	 /// @param uint16_t 通知ID
	 WM_NOTIFY_CHILD_REFLECT,

	 /// @brief 獲取窗體類名
	 /// @return 指向靜態只讀的窗體類名字串
	 WM_GET_CLASS,
	 /// @brief 獲取窗體客戶窗體
	 /// @return WObj指針
	 WM_GET_CLIENT_WINDOW,
	 /// @brief 獲取窗體?部矩形
	 /// @return SRect 8 bytes
	 WM_GET_INSIDE_RECT,
	 /// @brief 獲取窗體客戶矩形
	 /// @return SRect 8 bytes
	 WM_GET_SERVE_RECT,
	 /// @brief 獲取窗體背景顏色
	 /// @return uint32_t R8G8B8A8格式的色彩
	 WM_GET_BKCOLOR,
	 /// @brief 獲取窗體ID
	 /// @return uint16_t ID
	 WM_GET_ID,
	 /// @brief 設置窗體ID
	 /// @param uint16_t ID
	 WM_SET_ID,
	 /// @brief 窗體對焦
	 /// @param bool 設置/重置焦點
	 WM_FOCUS,
	 /// @brief 獲取窗體是否能對焦
	 /// @return bool 是否能對焦
	 WM_FOCUSSABLE,

	 WM_MENU,

	 /// @brief 初始化對話框
	 WM_INIT_DIALOG,
	 /// @brief 指向對話框狀態
	 /// @param DIALOG_STATE* 指向要設定的對話框狀態，若為NULL則返回當前狀態
	 /// @return DIALOG_STATE* 指向當前對話框狀態
	 WM_HANDLE_DIALOG_STATUS,

	_WM_WIDGET_BEGIN = 0x0300,
	WM_WIDGET_SET_EFFECT,
	_WM_WIDGET_END,

	_WM_USER = 0x0400
};
constexpr WM_MESSSAGES WM_USER(uint16_t idx) { return (WM_MESSSAGES)(_WM_USER + idx); }
constexpr WM_MESSSAGES WM_WIDGET(uint16_t idx) { return (WM_MESSSAGES)(_WM_WIDGET_END + idx); }

enum WM_NOTIFICATIONS : int {
	 WN_NULL,
	 WN_CLICKED,
	 WN_RELEASED,
	 WN_MOVED_OUT,
	 WN_SEL_CHANGED,
	 WN_VALUE_CHANGED,
	 WN_SCROLLBAR_ADDED,
	 WN_CHILD_DELETED,
	 WN_GOT_FOCUS,
	 WN_LOST_FOCUS,
	 WN_SCROLL_CHANGED,
	 WN_WIDGET,
	 WN_USER = 16
};

using		WM_CF = uint16_t;
constexpr	WM_CF
			WC_HIDE              = 0,
			WC_VISIBLE           = 1 << 0,
			WC_STAYONTOP         = 1 << 1,
			WC_POPUP             = 1 << 2,
			WC_DISABLED          = 1 << 3,
			WC_ACTIVATE          = 1 << 4,
			WC_FOCUSSABLE        = 1 << 5,
			WC_ANCHOR_LEFT       = 1 << 6,
			WC_ANCHOR_RIGHT      = 1 << 7,
			WC_ANCHOR_TOP        = 1 << 8,
			WC_ANCHOR_BOTTOM     = 1 << 9,
			WC_AUTOSCROLL_H      = 1 << 10,
			WC_AUTOSCROLL_V      = 1 << 11;
constexpr	WM_CF
			WC_AUTOSCROLL		 = WC_AUTOSCROLL_H | WC_AUTOSCROLL_V;
constexpr	WM_CF
			WC_ANCHOR_HORIZONTAL = WC_ANCHOR_LEFT | WC_ANCHOR_RIGHT;
constexpr	WM_CF
			WC_ANCHOR_VERTICAL	 = WC_ANCHOR_TOP | WC_ANCHOR_BOTTOM;
constexpr	WM_CF
			WC_ANCHOR_MASK		 = WC_ANCHOR_HORIZONTAL | WC_ANCHOR_VERTICAL;

using		WC_EX = uint16_t;
constexpr	WC_EX
			WC_EX_VERTICAL = 1 << 0;
constexpr	WC_EX WC_EX_USER(uint8_t x) { return WC_EX_VERTICAL << (1 + x); }
