#pragma once

enum WM_MESSSAGES : uint16_t {
	 WM_NULL = 0,

	 /// @brief �����Ыص�����T��
	 WM_CREATE,
	 /// @brief �����R��������T��
	 WM_DELETE,
	 /// @brief ø�s����e�T��
	 /// @param SRect rInvalid
	 WM_PAINT,

	 /// @brief �������ʵ�����T��
	 WM_MOVE,
	 /// @brief �������ŵ�����T��
	 WM_SIZE,

	 /// @brief ��L�b����W���D���T��
	 /// @param KEY_STATE ���䪬�A
	 WM_KEY,

	 /// @brief ����b����W���D���T��
	 /// @param MOUSE_STATE PID�۹窱�A
	 WM_MOUSE,
	 /// @brief ���Цb�l����W���D���T��
	 /// @param MOUSE_STATE PID�۹窱�A
	 /// @param pSrc �l����
	 WM_MOUSE_CHILD,
	 /// @brief ���Цb����W���D���T��
	 /// @param MOUSE_STATE PID�۹窱�A
	 WM_MOUSE_OVER,
	 /// @brief ���Цb����W
	 /// @param MOUSE_CHANGED_STATE PID�۹���ܪ��A
	 WM_MOUSE_CHANGED,

	 /// @brief ���鮷��������T��
	 WM_CAPTURE_RELEASED,

	 /// @brief ��������ϯબ�A���ܳq��
	 /// @param bool �O�_�ϯ�
	 WM_NOTIFY_ENABLE,
	 /// @brief ��������J�I���A���ܳq��
	 /// @param FOCUS_CHANGED_STATE �J�I���A
	 WM_NOTIFY_CHILD_HAS_FOCUS,
	 /// @brief �����Ȥᵡ����ܳq��
	 WM_NOTIFY_CLIENT_CHANGE,
	 /// @brief �����l����T���q��
	 /// @param uint16_t �q��ID
	 WM_NOTIFY_CHILD,
	 /// @brief �����l����T���q���Ϯg
	 /// @param uint16_t �q��ID
	 WM_NOTIFY_CHILD_REFLECT,

	 /// @brief ����������W
	 /// @return ���V�R�A�uŪ���������W�r��
	 WM_GET_CLASS,
	 /// @brief �������Ȥᵡ��
	 /// @return WObj���w
	 WM_GET_CLIENT_WINDOW,
	 /// @brief �������?���x��
	 /// @return SRect 8 bytes
	 WM_GET_INSIDE_RECT,
	 /// @brief �������Ȥ�x��
	 /// @return SRect 8 bytes
	 WM_GET_SERVE_RECT,
	 /// @brief �������I���C��
	 /// @return uint32_t R8G8B8A8�榡����m
	 WM_GET_BKCOLOR,
	 /// @brief �������ID
	 /// @return uint16_t ID
	 WM_GET_ID,
	 /// @brief �]�m����ID
	 /// @param uint16_t ID
	 WM_SET_ID,
	 /// @brief �����J
	 /// @param bool �]�m/���m�J�I
	 WM_FOCUS,
	 /// @brief �������O�_���J
	 /// @return bool �O�_���J
	 WM_FOCUSSABLE,

	 WM_MENU,

	 /// @brief ��l�ƹ�ܮ�
	 WM_INIT_DIALOG,
	 /// @brief ���V��ܮت��A
	 /// @param DIALOG_STATE* ���V�n�]�w����ܮت��A�A�Y��NULL�h��^��e���A
	 /// @return DIALOG_STATE* ���V��e��ܮت��A
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
