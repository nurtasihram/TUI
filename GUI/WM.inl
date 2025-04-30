#pragma once

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
	 WM_MOVE,
	 /// @brief 完成裁剪窗體後響應
	 WM_SIZE,

	 /// @brief 鍵盤在窗體内活躍時相應
	 /// @param KEY_STATE 按鍵狀態
	 WM_KEY,

	 /// @brief 鼠鍵在窗體内活躍時相應
	 /// @param PID_STATE PID相對狀態
	 WM_MOUSE_KEY,
	 /// @brief 鼠標在子窗體内活躍時相應
	 /// @param PID_STATE PID相對狀態
	 /// @param pSrc 子窗體
	 WM_MOUSE_CHILD,
	 /// @brief 鼠標在窗體内活躍時相應
	 /// @param PID_STATE PID相對狀態
	 WM_MOUSE_OVER,
	 /// @brief 鼠標在窗體内
	 /// @param PID_CHANGED_STATE PID相對改變狀態
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
	 /// @brief 獲取窗體内部矩形
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
			WC_DISABLED          = 1 << 2,
			WC_ACTIVATE          = 1 << 3,
			WC_NOACTIVATE        = 1 << 4,
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

using	  WC_EX = uint16_t;
constexpr WC_EX WC_EX_VERTICAL = 1 << 0;
constexpr WC_EX WC_EX_USER(uint8_t x) { return 1 << (1 + x); }
