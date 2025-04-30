#include "Button.h"
#include "Frame.h"
//#include "MultiEdit.h"

#pragma region OSK
#define ID_BUTTON (GUI_ID_USER + 0)
#define OSK_BTN_HOLD(btn, hold) btn->Pressed(hold)
enum KeyType {
	K_CHAR,
	K_ALFABET,
	K_FN_CASE,
	K_CHAR_FN,
	K_CAPS,
	K_BACKSPACE,
	K_TAB,
	K_ENTER,
	K_ESC,
	K_UI,
	K_LALT,
	K_RALT,
	K_LCTRL,
	K_RCTRL,
	K_LSHIFT,
	K_RSHIFT,
	K_MENU,
	K_UP = GUI_KEY_UP,
	K_DOWN = GUI_KEY_DOWN,
	K_LEFT = GUI_KEY_LEFT,
	K_RIGHT = GUI_KEY_RIGHT
};
struct ButtonInfo {
	const char *acLabel1;
	const char *acLabel2;
	uint8_t id1, id2;
	const KeyType type;
	float x0, y0;
	float cx, cy;
	Button *pButton;
};
static ButtonInfo _aButtonData[] = {
	{ "Esc", nullptr, 0x39, 0x00, K_ESC, 0.f, 0.f, 0.0625f, 0.2f },
	{ "1", "!\0F1", 0x1E, 0x3A, K_CHAR_FN, 0.125f, 0.f, 0.0625f, 0.2f },
	{ "2", "@\0F2", 0x1F, 0x3B, K_CHAR_FN, 0.1875f, 0.f, 0.0625f, 0.2f },
	{ "3", "#\0F3", 0x20, 0x3C, K_CHAR_FN, 0.25f, 0.f, 0.0625f, 0.2f },
	{ "4", "$\0F4", 0x21, 0x3D, K_CHAR_FN, 0.3125f, 0.f, 0.0625f, 0.2f },
	{ "5", "%\0F5", 0x22, 0x3E, K_CHAR_FN, 0.375f, 0.f, 0.0625f, 0.2f },
	{ "6", "^\0F6", 0x23, 0x3F, K_CHAR_FN, 0.4375f, 0.f, 0.0625f, 0.2f },
	{ "7", "&\0F7", 0x24, 0x40, K_CHAR_FN, 0.5f, 0.f, 0.0625f, 0.2f },
	{ "8", "*\0F8", 0x25, 0x41, K_CHAR_FN, 0.5625f, 0.f, 0.0625f, 0.2f },
	{ "9", "(\0F9", 0x26, 0x42, K_CHAR_FN, 0.625f, 0.f, 0.0625f, 0.2f },
	{ "0", ")\0F10", 0x27, 0x43, K_CHAR_FN, 0.6875f, 0.f, 0.0625f, 0.2f },
	{ "-", "_\0F11", 0x2D, 0x44, K_CHAR_FN, 0.75f, 0.f, 0.0625f, 0.2f },
	{ "=", "+\0F12", 0x2E, 0x45, K_CHAR_FN, 0.8125f, 0.f, 0.0625f, 0.2f },
	{ "Backspace", nullptr, 0xBB, 0x00, K_BACKSPACE, 0.875f, 0.f, 0.125f, 0.2f },
	{ "Tab", nullptr, 0x2B, 0x00, K_TAB, 0.f, 0.2f, 0.125f, 0.2f },
	{ "q", "Q", 0x14, 0x00, K_ALFABET, 0.125f, 0.2f, 0.0625f, 0.2f },
	{ "w", "W", 0x1A, 0x00, K_ALFABET, 0.1875f, 0.2f, 0.0625f, 0.2f },
	{ "e", "E", 0x08, 0x00, K_ALFABET, 0.25f, 0.2f, 0.0625f, 0.2f },
	{ "r", "R", 0x15, 0x00, K_ALFABET, 0.3125f, 0.2f, 0.0625f, 0.2f },
	{ "t", "T", 0x17, 0x00, K_ALFABET, 0.375f, 0.2f, 0.0625f, 0.2f },
	{ "y", "Y", 0x1C, 0x00, K_ALFABET, 0.4375f, 0.2f, 0.0625f, 0.2f },
	{ "u", "U", 0x18, 0x00, K_ALFABET, 0.5f, 0.2f, 0.0625f, 0.2f },
	{ "i", "I", 0x0C, 0x00, K_ALFABET, 0.5625f, 0.2f, 0.0625f, 0.2f },
	{ "o", "O", 0x12, 0x00, K_ALFABET, 0.625f, 0.2f, 0.0625f, 0.2f },
	{ "p", "P", 0x13, 0x00, K_ALFABET, 0.6875f, 0.2f, 0.0625f, 0.2f },
	{ "[", "{", 0x2F, 0x00, K_CHAR, 0.75f, 0.2f, 0.0625f, 0.2f },
	{ "]", "}", 0x30, 0x00, K_CHAR, 0.8125f, 0.2f, 0.0625f, 0.2f },
	{ "Caps", nullptr, 0x39, 0x00, K_CAPS, 0.f, 0.4f, 0.15f, 0.2f },
	{ "a", "A", 0x04, 0x00, K_ALFABET, 0.15f, 0.4f, 0.0625f, 0.2f },
	{ "s", "S", 0x16, 0x00, K_ALFABET, 0.2125f, 0.4f, 0.0625f, 0.2f },
	{ "d", "D", 0x07, 0x00, K_ALFABET, 0.275f, 0.4f, 0.0625f, 0.2f },
	{ "f", "F", 0x09, 0x00, K_ALFABET, 0.3375f, 0.4f, 0.0625f, 0.2f },
	{ "g", "G", 0x0A, 0x00, K_ALFABET, 0.4f, 0.4f, 0.0625f, 0.2f },
	{ "h", "H", 0x0B, 0x00, K_ALFABET, 0.4625f, 0.4f, 0.0625f, 0.2f },
	{ "j", "J", 0x0D, 0x00, K_ALFABET, 0.525f, 0.4f, 0.0625f, 0.2f },
	{ "k", "K", 0x0E, 0x00, K_ALFABET, 0.5875f, 0.4f, 0.0625f, 0.2f },
	{ "l", "L", 0x0F, 0x00, K_ALFABET, 0.65f, 0.4f, 0.0625f, 0.2f },
	{ ";", ":", 0x33, 0x00, K_CHAR, 0.7125f, 0.4f, 0.0625f, 0.2f },
	{ "'", "\"", 0x34, 0x00, K_CHAR, 0.775f, 0.4f, 0.0625f, 0.2f },
	{ "\\", "|", 0x31, 0x00, K_CHAR, 0.875f, 0.2f, 0.125f, 0.2f },
	{ "Enter", nullptr, 0x28, 0x00, K_ENTER, 0.8375f, 0.4f, 0.1625f, 0.2f },
	{ "Shift", nullptr, 0x00, 0x00, K_LSHIFT, 0.f, 0.6f, 0.1875f, 0.2f },
	{ "z", "Z", 0x1D, 0x00, K_ALFABET, 0.1875f, 0.6f, 0.0625f, 0.2f },
	{ "x", "X", 0x1B, 0x00, K_ALFABET, 0.25f, 0.6f, 0.0625f, 0.2f },
	{ "c", "C", 0x06, 0x00, K_ALFABET, 0.3125f, 0.6f, 0.0625f, 0.2f },
	{ "v", "V", 0x19, 0x00, K_ALFABET, 0.375f, 0.6f, 0.0625f, 0.2f },
	{ "b", "B", 0x05, 0x00, K_ALFABET, 0.4375f, 0.6f, 0.0625f, 0.2f },
	{ "n", "N", 0x11, 0x00, K_ALFABET, 0.5f, 0.6f, 0.0625f, 0.2f },
	{ "m", "M", 0x10, 0x00, K_ALFABET, 0.5625f, 0.6f, 0.0625f, 0.2f },
	{ ",", "<", 0x36, 0x00, K_CHAR, 0.625f, 0.6f, 0.0625f, 0.2f },
	{ ".", ">", 0x37, 0x00, K_CHAR, 0.6875f, 0.6f, 0.0625f, 0.2f },
	{ "/", "?", 0x38, 0x00, K_CHAR, 0.75f, 0.6f, 0.0625f, 0.2f },
	{ "Shift", nullptr, 0x00, 0x00, K_RSHIFT, 0.8125f, 0.6f, 0.1875f, 0.2f },
	{ "Ctrl", nullptr, 0x00, 0x00, K_LCTRL, 0.0625f, 0.8f, 0.0625f, 0.2f },
	{ "UI", nullptr, 0x00, 0x00, K_UI, 0.125f, 0.8f, 0.0625f, 0.2f },
	{ "Alt", nullptr, 0x00, 0x00, K_LALT, 0.1875f, 0.8f, 0.0625f, 0.2f },
	{ " ", " ", 0x2C, 0x00, K_CHAR, 0.25f, 0.8f, 0.375f, 0.2f },
	{ "Alt", nullptr, 0x00, 0x00, K_RALT, 0.625f, 0.8f, 0.0625f, 0.2f },
	{ "Menu", nullptr, 0x76, 0x00, K_MENU, 0.6875f, 0.8f, 0.0625f, 0.2f },
	{ "Fn", nullptr, 0x00, 0x00, K_FN_CASE, 0.f, 0.8f, 0.0625f, 0.2f },
	{ "Ctrl", nullptr, 0x00, 0x00, K_RCTRL, 0.75f, 0.8f, 0.0625f, 0.2f },
	{ "`", "~", 0x35, 0x00, K_CHAR, 0.0625f, 0.f, 0.0625f, 0.2f },
	{ "^", nullptr, 0x52, 0x61, K_UP, 0.875f, 0.8f, 0.0625f, 0.1f },
	{ "v", nullptr, 0x51, 0x5B, K_DOWN, 0.875f, 0.9f, 0.0625f, 0.1f },
	{ "<", nullptr, 0x50, 0x00, K_LEFT, 0.8125f, 0.8f, 0.0625f, 0.2f },
	{ ">", nullptr, 0x4F, 0x00, K_RIGHT, 0.9375f, 0.8f, 0.0625f, 0.2f }
};
struct FnCase {
	uint8_t LCtrl : 1;
	uint8_t LShift : 1;
	uint8_t LAlt : 1;
	uint8_t LUI : 1;
	uint8_t RCtrl : 1;
	uint8_t RShift : 1;
	uint8_t RAlt : 1;
	uint8_t RUI : 1;
};
static FnCase fn_keys;
static uint8_t Caps = 0, FnSel = 0;
static Button *pbCaps = nullptr, *pbFn = nullptr;
static Button *pbShiftL = nullptr, *pbShiftR = nullptr;
static Button *pbCtrlL = nullptr, *pbCtrlR = nullptr;
static Button *pbAltL = nullptr, *pbAltR = nullptr;

static void _OSK_updateShiftCaps() {
	uint8_t Shift = fn_keys.LShift | fn_keys.RShift;
	pbCaps->Pressed(Caps);
	pbFn->Pressed(FnSel);
	pbShiftL->Pressed(Shift);
	pbShiftR->Pressed(Shift);
	pbCtrlL->Pressed(fn_keys.LCtrl);
	pbCtrlR->Pressed(fn_keys.LCtrl);
	pbAltL->Pressed(fn_keys.LAlt);
	pbAltR->Pressed(fn_keys.LAlt);
	auto Cap = Shift ^ Caps;
	for (int i = 0; i < sizeof(_aButtonData) / sizeof(ButtonInfo); ++i) {
		uint8_t m;
		auto &inf = _aButtonData[i];
		if (inf.type == K_CHAR)
			m = Shift;
		else if (inf.type == K_ALFABET)
			m = Cap;
		else if (inf.type == K_CHAR_FN) {
			if (FnSel) {
				inf.pButton->Text(inf.acLabel2 + 2);
				continue;
			}
			m = Shift;
		}
		else
			continue;
		inf.pButton->Text(m ? inf.acLabel2 : inf.acLabel1);
	}
}
static int _OSK_ceil(float f) {
	int i = (int)f;
	return f - i > 0.f ? i + 1 : i;
}

static void _OSK_createButtons(PWObj pWin, Point size) {
	for (int i = 0; i < sizeof(_aButtonData) / sizeof(ButtonInfo); ++i) {
		auto inf = _aButtonData + i;
		auto pButton = inf->pButton = new Button(
			SRect::left_top(
				{ _OSK_ceil(inf->x0 * size.x), _OSK_ceil(inf->y0 * size.y) },
				{ _OSK_ceil(inf->cx * size.x), _OSK_ceil(inf->cy * size.y) }),
			pWin, ID_BUTTON + i,
			WC_VISIBLE, 0,
			inf->acLabel1);
		pButton->Focussable(false);
		switch (inf->type) {
		case K_CAPS: pbCaps = pButton; break;
		case K_FN_CASE: pbFn = pButton; break;
		case K_LSHIFT: pbShiftL = pButton; break;
		case K_RSHIFT: pbShiftR = pButton; break;
		case K_LCTRL: pbCtrlL = pButton; break;
		case K_RCTRL: pbCtrlR = pButton; break;
		case K_LALT: pbAltL = pButton; break;
		case K_RALT: pbAltR = pButton; break;
		default: break;
		}
	}
}
static void _OSK_updateButtons(PWObj pWin, Point size) {
	auto &&r = pWin->Rect();
	for (int i = 0; i < sizeof(_aButtonData) / sizeof(ButtonInfo); ++i) {
		auto inf = _aButtonData + i;
		inf->pButton->Position(
			{ _OSK_ceil(inf->x0 * size.x),
			  _OSK_ceil(inf->y0 * size.y) });
		inf->pButton->Size(
			{ _OSK_ceil(inf->cx * size.x),
			  _OSK_ceil(inf->cy * size.y) });
	}
}
static void _OSK_onKey(uint16_t Id, bool bPressed) {
	auto inf = _aButtonData + Id - ID_BUTTON;
	uint8_t ab = 0;
	int c = 0;
	switch (inf->type) {
	case K_CHAR_FN:
		if (FnSel) {
			c = inf->acLabel1[0];
			switch (c) {
			case '1':
				c = GUI_KEY_F1;
				break;
			case '2':
				c = GUI_KEY_F2;
				break;
			default:
				;
			}
			FnSel = 0;
			_OSK_updateShiftCaps();
			break;
		}
		goto _K_CHAR;
	case K_ALFABET:
		ab = Caps;
	case K_CHAR:
	_K_CHAR:
		c = ab ^ fn_keys.LShift ? inf->acLabel2[0] : inf->acLabel1[0];
		if (fn_keys.LShift && !fn_keys.RShift) {
			fn_keys.LShift = 0;
			_OSK_updateShiftCaps();
		}
		break;
	case K_BACKSPACE: c = GUI_KEY_BACKSPACE; break;
	case K_TAB:       c = GUI_KEY_TAB;       break;
	case K_ENTER:     c = GUI_KEY_ENTER;     break;
	case K_ESC:       c = GUI_KEY_ESCAPE;    break;
	case K_FN_CASE:
		FnSel = !FnSel;
		_OSK_updateShiftCaps();
		return;
	case K_CAPS:
		Caps = !Caps;
		_OSK_updateShiftCaps();
		return;
	case K_LCTRL:
		fn_keys.RCtrl = 0;
		fn_keys.LCtrl = !fn_keys.LCtrl;
		_OSK_updateShiftCaps();
		c = GUI_KEY_CONTROL;
		break;
	case K_RCTRL:
		fn_keys.RCtrl = fn_keys.LCtrl = ~fn_keys.LCtrl;
		_OSK_updateShiftCaps();
		c = GUI_KEY_CONTROL;
		break;
	case K_LALT:
		fn_keys.RAlt = 0;
		fn_keys.LAlt = ~fn_keys.LAlt;
		_OSK_updateShiftCaps();
//		c = GUI_KEY_CONTROL;
		break;
	case K_RALT:
		fn_keys.RAlt = fn_keys.LAlt = ~fn_keys.LAlt;
		_OSK_updateShiftCaps();
//		c = GUI_KEY_CONTROL;
		break;
	case K_LSHIFT:
		fn_keys.RShift = 0;
		fn_keys.LShift = ~fn_keys.LShift;
		_OSK_updateShiftCaps();
		c = GUI_KEY_SHIFT;
		return;
	case K_RSHIFT:
		fn_keys.RShift = fn_keys.LShift = ~fn_keys.LShift;
		_OSK_updateShiftCaps();
		c = GUI_KEY_SHIFT;
		return;
	case K_UP:
	case K_DOWN:
	case K_LEFT:
	case K_RIGHT:
		c = inf->type;
		break;
	default:
		;
	}
	GUI_StoreKeyMsg(c, bPressed);
	GUI_PollKeyMsg();
}

WM_RESULT _OSK_callback(PWObj pWin, int MsgId, WM_PARAM Param, PWObj pSrc) {
	switch (MsgId) {
	case WM_CREATE:
		GUI.PenColor(RGB_GRAY);
		break;
	case WM_PAINT:
		GUI.PenColor(RGB_GRAY);
		GUI.Fill(pWin->ClientRect());
		break;
	case WM_NOTIFY_CHILD:
		switch ((int)Param) {
		case WN_CLICKED:
			_OSK_onKey((int)pSrc->ID(), true);
			break;
		case WN_RELEASED:
			_OSK_onKey((int)pSrc->ID(), false);
			break;
		}
		break;
	case WM_SIZE:
		_OSK_updateButtons(pWin->Client(), pWin->Client()->Size());
		break;
	}
	return 0;
}

Frame *pKeypad = nullptr;
void OSK_Show() {
	pKeypad = new Frame(
		{ 0, 0, 320, 180 },
		WObj::Desktop(), 0,
		WC_VISIBLE | WC_STAYONTOP | WC_NOACTIVATE, 0,
		"Keyboard",
		_OSK_callback);
	_OSK_createButtons(pKeypad->Client(), pKeypad->Client()->Size());
	pKeypad->Focussable(false);
	pKeypad->Resizeable(true);
	pKeypad->Moveable(true);
	pKeypad->Visible(true);
}
#pragma endregion

void Show_Keypad() {
	auto pFrame = new Frame(
		{ 0, 0, 320, 240 },
		WObj::Desktop(), 0,
		WC_VISIBLE, FRAME_CF_MOVEABLE | FRAME_CF_RESIZEABLE,
		"Notepad");
	pFrame->AddCloseButton(FRAME_BUTTON_RIGHT, 0);
	pFrame->AddMaxButton(FRAME_BUTTON_RIGHT, 0);
	pFrame->AddMinButton(FRAME_BUTTON_RIGHT, 0);
	new Button(
		SRect::left_top({ 0, 0 }, { 40, 25 }),
		pFrame->Client(), 0,
		WC_VISIBLE, 0,
		"Btn1");
	auto a = new Button(
		SRect::left_top({ 0, 30 }, { 40, 25 }),
		pFrame->Client(), 0,
		WC_VISIBLE, BUTTON_CF_SWITCH,
		"Btn2");
}

void MainTask() {
	Show_Keypad();
	OSK_Show();
	for (;;)
		WObj::Exec();
}
