#pragma once

enum TEXTSTYLES {
	TS_NORMAL = 0,
	TS_UNDERLINE,
	TS_STRIKETHRU,
	TS_OVERLINE
};

enum GUI_KEYS {
	GUI_KEY_BACKSPACE         = 8,
	GUI_KEY_TAB               = 9,
	GUI_KEY_ENTER             = 13,
	GUI_KEY_LEFT              = 16,
	GUI_KEY_UP                = 17,
	GUI_KEY_RIGHT             = 18,
	GUI_KEY_DOWN              = 19,
	GUI_KEY_HOME              = 23,
	GUI_KEY_END               = 24,
	GUI_KEY_SHIFT             = 25,
	GUI_KEY_CONTROL           = 26,
	GUI_KEY_ESCAPE            = 27,
	GUI_KEY_INSERT            = 29,
	GUI_KEY_DELETE            = 30,
	GUI_KEY_SPACE             = 32,
	GUI_KEY_F1                = 40,
	GUI_KEY_F2                = 41
};

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

enum BPP1_DAT : uint8_t {
	________________ = 0x00,
	XX______________ = 0x01,
	__XX____________ = 0x02,
	XXXX____________ = 0x03,
	____XX__________ = 0x04,
	XX__XX__________ = 0x05,
	__XXXX__________ = 0x06,
	XXXXXX__________ = 0x07,
	______XX________ = 0x08,
	XX____XX________ = 0x09,
	__XX__XX________ = 0x0A,
	XXXX__XX________ = 0x0B,
	____XXXX________ = 0x0C,
	XX__XXXX________ = 0x0D,
	__XXXXXX________ = 0x0E,
	XXXXXXXX________ = 0x0F,
	________XX______ = 0x10,
	XX______XX______ = 0x11,
	__XX____XX______ = 0x12,
	XXXX____XX______ = 0x13,
	____XX__XX______ = 0x14,
	XX__XX__XX______ = 0x15,
	__XXXX__XX______ = 0x16,
	XXXXXX__XX______ = 0x17,
	______XXXX______ = 0x18,
	XX____XXXX______ = 0x19,
	__XX__XXXX______ = 0x1A,
	XXXX__XXXX______ = 0x1B,
	____XXXXXX______ = 0x1C,
	XX__XXXXXX______ = 0x1D,
	__XXXXXXXX______ = 0x1E,
	XXXXXXXXXX______ = 0x1F,
	__________XX____ = 0x20,
	XX________XX____ = 0x21,
	__XX______XX____ = 0x22,
	XXXX______XX____ = 0x23,
	____XX____XX____ = 0x24,
	XX__XX____XX____ = 0x25,
	__XXXX____XX____ = 0x26,
	XXXXXX____XX____ = 0x27,
	______XX__XX____ = 0x28,
	XX____XX__XX____ = 0x29,
	__XX__XX__XX____ = 0x2A,
	XXXX__XX__XX____ = 0x2B,
	____XXXX__XX____ = 0x2C,
	XX__XXXX__XX____ = 0x2D,
	__XXXXXX__XX____ = 0x2E,
	XXXXXXXX__XX____ = 0x2F,
	________XXXX____ = 0x30,
	XX______XXXX____ = 0x31,
	__XX____XXXX____ = 0x32,
	XXXX____XXXX____ = 0x33,
	____XX__XXXX____ = 0x34,
	XX__XX__XXXX____ = 0x35,
	__XXXX__XXXX____ = 0x36,
	XXXXXX__XXXX____ = 0x37,
	______XXXXXX____ = 0x38,
	XX____XXXXXX____ = 0x39,
	__XX__XXXXXX____ = 0x3A,
	XXXX__XXXXXX____ = 0x3B,
	____XXXXXXXX____ = 0x3C,
	XX__XXXXXXXX____ = 0x3D,
	__XXXXXXXXXX____ = 0x3E,
	XXXXXXXXXXXX____ = 0x3F,
	____________XX__ = 0x40,
	XX__________XX__ = 0x41,
	__XX________XX__ = 0x42,
	XXXX________XX__ = 0x43,
	____XX______XX__ = 0x44,
	XX__XX______XX__ = 0x45,
	__XXXX______XX__ = 0x46,
	XXXXXX______XX__ = 0x47,
	______XX____XX__ = 0x48,
	XX____XX____XX__ = 0x49,
	__XX__XX____XX__ = 0x4A,
	XXXX__XX____XX__ = 0x4B,
	____XXXX____XX__ = 0x4C,
	XX__XXXX____XX__ = 0x4D,
	__XXXXXX____XX__ = 0x4E,
	XXXXXXXX____XX__ = 0x4F,
	________XX__XX__ = 0x50,
	XX______XX__XX__ = 0x51,
	__XX____XX__XX__ = 0x52,
	XXXX____XX__XX__ = 0x53,
	____XX__XX__XX__ = 0x54,
	XX__XX__XX__XX__ = 0x55,
	__XXXX__XX__XX__ = 0x56,
	XXXXXX__XX__XX__ = 0x57,
	______XXXX__XX__ = 0x58,
	XX____XXXX__XX__ = 0x59,
	__XX__XXXX__XX__ = 0x5A,
	XXXX__XXXX__XX__ = 0x5B,
	____XXXXXX__XX__ = 0x5C,
	XX__XXXXXX__XX__ = 0x5D,
	__XXXXXXXX__XX__ = 0x5E,
	XXXXXXXXXX__XX__ = 0x5F,
	__________XXXX__ = 0x60,
	XX________XXXX__ = 0x61,
	__XX______XXXX__ = 0x62,
	XXXX______XXXX__ = 0x63,
	____XX____XXXX__ = 0x64,
	XX__XX____XXXX__ = 0x65,
	__XXXX____XXXX__ = 0x66,
	XXXXXX____XXXX__ = 0x67,
	______XX__XXXX__ = 0x68,
	XX____XX__XXXX__ = 0x69,
	__XX__XX__XXXX__ = 0x6A,
	XXXX__XX__XXXX__ = 0x6B,
	____XXXX__XXXX__ = 0x6C,
	XX__XXXX__XXXX__ = 0x6D,
	__XXXXXX__XXXX__ = 0x6E,
	XXXXXXXX__XXXX__ = 0x6F,
	________XXXXXX__ = 0x70,
	XX______XXXXXX__ = 0x71,
	__XX____XXXXXX__ = 0x72,
	XXXX____XXXXXX__ = 0x73,
	____XX__XXXXXX__ = 0x74,
	XX__XX__XXXXXX__ = 0x75,
	__XXXX__XXXXXX__ = 0x76,
	XXXXXX__XXXXXX__ = 0x77,
	______XXXXXXXX__ = 0x78,
	XX____XXXXXXXX__ = 0x79,
	__XX__XXXXXXXX__ = 0x7A,
	XXXX__XXXXXXXX__ = 0x7B,
	____XXXXXXXXXX__ = 0x7C,
	XX__XXXXXXXXXX__ = 0x7D,
	__XXXXXXXXXXXX__ = 0x7E,
	XXXXXXXXXXXXXX__ = 0x7F,
	______________XX = 0x80,
	XX____________XX = 0x81,
	__XX__________XX = 0x82,
	XXXX__________XX = 0x83,
	____XX________XX = 0x84,
	XX__XX________XX = 0x85,
	__XXXX________XX = 0x86,
	XXXXXX________XX = 0x87,
	______XX______XX = 0x88,
	XX____XX______XX = 0x89,
	__XX__XX______XX = 0x8A,
	XXXX__XX______XX = 0x8B,
	____XXXX______XX = 0x8C,
	XX__XXXX______XX = 0x8D,
	__XXXXXX______XX = 0x8E,
	XXXXXXXX______XX = 0x8F,
	________XX____XX = 0x90,
	XX______XX____XX = 0x91,
	__XX____XX____XX = 0x92,
	XXXX____XX____XX = 0x93,
	____XX__XX____XX = 0x94,
	XX__XX__XX____XX = 0x95,
	__XXXX__XX____XX = 0x96,
	XXXXXX__XX____XX = 0x97,
	______XXXX____XX = 0x98,
	XX____XXXX____XX = 0x99,
	__XX__XXXX____XX = 0x9A,
	XXXX__XXXX____XX = 0x9B,
	____XXXXXX____XX = 0x9C,
	XX__XXXXXX____XX = 0x9D,
	__XXXXXXXX____XX = 0x9E,
	XXXXXXXXXX____XX = 0x9F,
	__________XX__XX = 0xA0,
	XX________XX__XX = 0xA1,
	__XX______XX__XX = 0xA2,
	XXXX______XX__XX = 0xA3,
	____XX____XX__XX = 0xA4,
	XX__XX____XX__XX = 0xA5,
	__XXXX____XX__XX = 0xA6,
	XXXXXX____XX__XX = 0xA7,
	______XX__XX__XX = 0xA8,
	XX____XX__XX__XX = 0xA9,
	__XX__XX__XX__XX = 0xAA,
	XXXX__XX__XX__XX = 0xAB,
	____XXXX__XX__XX = 0xAC,
	XX__XXXX__XX__XX = 0xAD,
	__XXXXXX__XX__XX = 0xAE,
	XXXXXXXX__XX__XX = 0xAF,
	________XXXX__XX = 0xB0,
	XX______XXXX__XX = 0xB1,
	__XX____XXXX__XX = 0xB2,
	XXXX____XXXX__XX = 0xB3,
	____XX__XXXX__XX = 0xB4,
	XX__XX__XXXX__XX = 0xB5,
	__XXXX__XXXX__XX = 0xB6,
	XXXXXX__XXXX__XX = 0xB7,
	______XXXXXX__XX = 0xB8,
	XX____XXXXXX__XX = 0xB9,
	__XX__XXXXXX__XX = 0xBA,
	XXXX__XXXXXX__XX = 0xBB,
	____XXXXXXXX__XX = 0xBC,
	XX__XXXXXXXX__XX = 0xBD,
	__XXXXXXXXXX__XX = 0xBE,
	XXXXXXXXXXXX__XX = 0xBF,
	____________XXXX = 0xC0,
	XX__________XXXX = 0xC1,
	__XX________XXXX = 0xC2,
	XXXX________XXXX = 0xC3,
	____XX______XXXX = 0xC4,
	XX__XX______XXXX = 0xC5,
	__XXXX______XXXX = 0xC6,
	XXXXXX______XXXX = 0xC7,
	______XX____XXXX = 0xC8,
	XX____XX____XXXX = 0xC9,
	__XX__XX____XXXX = 0xCA,
	XXXX__XX____XXXX = 0xCB,
	____XXXX____XXXX = 0xCC,
	XX__XXXX____XXXX = 0xCD,
	__XXXXXX____XXXX = 0xCE,
	XXXXXXXX____XXXX = 0xCF,
	________XX__XXXX = 0xD0,
	XX______XX__XXXX = 0xD1,
	__XX____XX__XXXX = 0xD2,
	XXXX____XX__XXXX = 0xD3,
	____XX__XX__XXXX = 0xD4,
	XX__XX__XX__XXXX = 0xD5,
	__XXXX__XX__XXXX = 0xD6,
	XXXXXX__XX__XXXX = 0xD7,
	______XXXX__XXXX = 0xD8,
	XX____XXXX__XXXX = 0xD9,
	__XX__XXXX__XXXX = 0xDA,
	XXXX__XXXX__XXXX = 0xDB,
	____XXXXXX__XXXX = 0xDC,
	XX__XXXXXX__XXXX = 0xDD,
	__XXXXXXXX__XXXX = 0xDE,
	XXXXXXXXXX__XXXX = 0xDF,
	__________XXXXXX = 0xE0,
	XX________XXXXXX = 0xE1,
	__XX______XXXXXX = 0xE2,
	XXXX______XXXXXX = 0xE3,
	____XX____XXXXXX = 0xE4,
	XX__XX____XXXXXX = 0xE5,
	__XXXX____XXXXXX = 0xE6,
	XXXXXX____XXXXXX = 0xE7,
	______XX__XXXXXX = 0xE8,
	XX____XX__XXXXXX = 0xE9,
	__XX__XX__XXXXXX = 0xEA,
	XXXX__XX__XXXXXX = 0xEB,
	____XXXX__XXXXXX = 0xEC,
	XX__XXXX__XXXXXX = 0xED,
	__XXXXXX__XXXXXX = 0xEE,
	XXXXXXXX__XXXXXX = 0xEF,
	________XXXXXXXX = 0xF0,
	XX______XXXXXXXX = 0xF1,
	__XX____XXXXXXXX = 0xF2,
	XXXX____XXXXXXXX = 0xF3,
	____XX__XXXXXXXX = 0xF4,
	XX__XX__XXXXXXXX = 0xF5,
	__XXXX__XXXXXXXX = 0xF6,
	XXXXXX__XXXXXXXX = 0xF7,
	______XXXXXXXXXX = 0xF8,
	XX____XXXXXXXXXX = 0xF9,
	__XX__XXXXXXXXXX = 0xFA,
	XXXX__XXXXXXXXXX = 0xFB,
	____XXXXXXXXXXXX = 0xFC,
	XX__XXXXXXXXXXXX = 0xFD,
	__XXXXXXXXXXXXXX = 0xFE,
	XXXXXXXXXXXXXXXX = 0xFF
};
enum BPP2_DAT : uint8_t {
	________ = 0x00,
	XX______ = 0x01,
	oo______ = 0x02,
	dd______ = 0x03,
	__XX____ = 0x04,
	XXXX____ = 0x05,
	ooXX____ = 0x06,
	ddXX____ = 0x07,
	__oo____ = 0x08,
	XXoo____ = 0x09,
	oooo____ = 0x0A,
	ddoo____ = 0x0B,
	__dd____ = 0x0C,
	XXdd____ = 0x0D,
	oodd____ = 0x0E,
	dddd____ = 0x0F,
	____XX__ = 0x10,
	XX__XX__ = 0x11,
	oo__XX__ = 0x12,
	dd__XX__ = 0x13,
	__XXXX__ = 0x14,
	XXXXXX__ = 0x15,
	ooXXXX__ = 0x16,
	ddXXXX__ = 0x17,
	__ooXX__ = 0x18,
	XXooXX__ = 0x19,
	ooooXX__ = 0x1A,
	ddooXX__ = 0x1B,
	__ddXX__ = 0x1C,
	XXddXX__ = 0x1D,
	ooddXX__ = 0x1E,
	ddddXX__ = 0x1F,
	____oo__ = 0x20,
	XX__oo__ = 0x21,
	oo__oo__ = 0x22,
	dd__oo__ = 0x23,
	__XXoo__ = 0x24,
	XXXXoo__ = 0x25,
	ooXXoo__ = 0x26,
	ddXXoo__ = 0x27,
	__oooo__ = 0x28,
	XXoooo__ = 0x29,
	oooooo__ = 0x2A,
	ddoooo__ = 0x2B,
	__ddoo__ = 0x2C,
	XXddoo__ = 0x2D,
	ooddoo__ = 0x2E,
	ddddoo__ = 0x2F,
	____dd__ = 0x30,
	XX__dd__ = 0x31,
	oo__dd__ = 0x32,
	dd__dd__ = 0x33,
	__XXdd__ = 0x34,
	XXXXdd__ = 0x35,
	ooXXdd__ = 0x36,
	ddXXdd__ = 0x37,
	__oodd__ = 0x38,
	XXoodd__ = 0x39,
	oooodd__ = 0x3A,
	ddoodd__ = 0x3B,
	__dddd__ = 0x3C,
	XXdddd__ = 0x3D,
	oodddd__ = 0x3E,
	dddddd__ = 0x3F,
	______XX = 0x40,
	XX____XX = 0x41,
	oo____XX = 0x42,
	dd____XX = 0x43,
	__XX__XX = 0x44,
	XXXX__XX = 0x45,
	ooXX__XX = 0x46,
	ddXX__XX = 0x47,
	__oo__XX = 0x48,
	XXoo__XX = 0x49,
	oooo__XX = 0x4A,
	ddoo__XX = 0x4B,
	__dd__XX = 0x4C,
	XXdd__XX = 0x4D,
	oodd__XX = 0x4E,
	dddd__XX = 0x4F,
	____XXXX = 0x50,
	XX__XXXX = 0x51,
	oo__XXXX = 0x52,
	dd__XXXX = 0x53,
	__XXXXXX = 0x54,
	XXXXXXXX = 0x55,
	ooXXXXXX = 0x56,
	ddXXXXXX = 0x57,
	__ooXXXX = 0x58,
	XXooXXXX = 0x59,
	ooooXXXX = 0x5A,
	ddooXXXX = 0x5B,
	__ddXXXX = 0x5C,
	XXddXXXX = 0x5D,
	ooddXXXX = 0x5E,
	ddddXXXX = 0x5F,
	____ooXX = 0x60,
	XX__ooXX = 0x61,
	oo__ooXX = 0x62,
	dd__ooXX = 0x63,
	__XXooXX = 0x64,
	XXXXooXX = 0x65,
	ooXXooXX = 0x66,
	ddXXooXX = 0x67,
	__ooooXX = 0x68,
	XXooooXX = 0x69,
	ooooooXX = 0x6A,
	ddooooXX = 0x6B,
	__ddooXX = 0x6C,
	XXddooXX = 0x6D,
	ooddooXX = 0x6E,
	ddddooXX = 0x6F,
	____ddXX = 0x70,
	XX__ddXX = 0x71,
	oo__ddXX = 0x72,
	dd__ddXX = 0x73,
	__XXddXX = 0x74,
	XXXXddXX = 0x75,
	ooXXddXX = 0x76,
	ddXXddXX = 0x77,
	__ooddXX = 0x78,
	XXooddXX = 0x79,
	ooooddXX = 0x7A,
	ddooddXX = 0x7B,
	__ddddXX = 0x7C,
	XXddddXX = 0x7D,
	ooddddXX = 0x7E,
	ddddddXX = 0x7F,
	______oo = 0x80,
	XX____oo = 0x81,
	oo____oo = 0x82,
	dd____oo = 0x83,
	__XX__oo = 0x84,
	XXXX__oo = 0x85,
	ooXX__oo = 0x86,
	ddXX__oo = 0x87,
	__oo__oo = 0x88,
	XXoo__oo = 0x89,
	oooo__oo = 0x8A,
	ddoo__oo = 0x8B,
	__dd__oo = 0x8C,
	XXdd__oo = 0x8D,
	oodd__oo = 0x8E,
	dddd__oo = 0x8F,
	____XXoo = 0x90,
	XX__XXoo = 0x91,
	oo__XXoo = 0x92,
	dd__XXoo = 0x93,
	__XXXXoo = 0x94,
	XXXXXXoo = 0x95,
	ooXXXXoo = 0x96,
	ddXXXXoo = 0x97,
	__ooXXoo = 0x98,
	XXooXXoo = 0x99,
	ooooXXoo = 0x9A,
	ddooXXoo = 0x9B,
	__ddXXoo = 0x9C,
	XXddXXoo = 0x9D,
	ooddXXoo = 0x9E,
	ddddXXoo = 0x9F,
	____oooo = 0xA0,
	XX__oooo = 0xA1,
	oo__oooo = 0xA2,
	dd__oooo = 0xA3,
	__XXoooo = 0xA4,
	XXXXoooo = 0xA5,
	ooXXoooo = 0xA6,
	ddXXoooo = 0xA7,
	__oooooo = 0xA8,
	XXoooooo = 0xA9,
	oooooooo = 0xAA,
	ddoooooo = 0xAB,
	__ddoooo = 0xAC,
	XXddoooo = 0xAD,
	ooddoooo = 0xAE,
	ddddoooo = 0xAF,
	____ddoo = 0xB0,
	XX__ddoo = 0xB1,
	oo__ddoo = 0xB2,
	dd__ddoo = 0xB3,
	__XXddoo = 0xB4,
	XXXXddoo = 0xB5,
	ooXXddoo = 0xB6,
	ddXXddoo = 0xB7,
	__ooddoo = 0xB8,
	XXooddoo = 0xB9,
	ooooddoo = 0xBA,
	ddooddoo = 0xBB,
	__ddddoo = 0xBC,
	XXddddoo = 0xBD,
	ooddddoo = 0xBE,
	ddddddoo = 0xBF,
	______dd = 0xC0,
	XX____dd = 0xC1,
	oo____dd = 0xC2,
	dd____dd = 0xC3,
	__XX__dd = 0xC4,
	XXXX__dd = 0xC5,
	ooXX__dd = 0xC6,
	ddXX__dd = 0xC7,
	__oo__dd = 0xC8,
	XXoo__dd = 0xC9,
	oooo__dd = 0xCA,
	ddoo__dd = 0xCB,
	__dd__dd = 0xCC,
	XXdd__dd = 0xCD,
	oodd__dd = 0xCE,
	dddd__dd = 0xCF,
	____XXdd = 0xD0,
	XX__XXdd = 0xD1,
	oo__XXdd = 0xD2,
	dd__XXdd = 0xD3,
	__XXXXdd = 0xD4,
	XXXXXXdd = 0xD5,
	ooXXXXdd = 0xD6,
	ddXXXXdd = 0xD7,
	__ooXXdd = 0xD8,
	XXooXXdd = 0xD9,
	ooooXXdd = 0xDA,
	ddooXXdd = 0xDB,
	__ddXXdd = 0xDC,
	XXddXXdd = 0xDD,
	ooddXXdd = 0xDE,
	ddddXXdd = 0xDF,
	____oodd = 0xE0,
	XX__oodd = 0xE1,
	oo__oodd = 0xE2,
	dd__oodd = 0xE3,
	__XXoodd = 0xE4,
	XXXXoodd = 0xE5,
	ooXXoodd = 0xE6,
	ddXXoodd = 0xE7,
	__oooodd = 0xE8,
	XXoooodd = 0xE9,
	oooooodd = 0xEA,
	ddoooodd = 0xEB,
	__ddoodd = 0xEC,
	XXddoodd = 0xED,
	ooddoodd = 0xEE,
	ddddoodd = 0xEF,
	____dddd = 0xF0,
	XX__dddd = 0xF1,
	oo__dddd = 0xF2,
	dd__dddd = 0xF3,
	__XXdddd = 0xF4,
	XXXXdddd = 0xF5,
	ooXXdddd = 0xF6,
	ddXXdddd = 0xF7,
	__oodddd = 0xF8,
	XXoodddd = 0xF9,
	oooodddd = 0xFA,
	ddoodddd = 0xFB,
	__dddddd = 0xFC,
	XXdddddd = 0xFD,
	oodddddd = 0xFE,
	dddddddd = 0xFF
};

#pragma region Colors
using RGBC = uint32_t;
constexpr RGBC RGBC_B(uint8_t b) { return b; }
constexpr RGBC RGBC_G(uint8_t g) { return g << 0x08; }
constexpr RGBC RGBC_R(uint8_t r) { return r << 0x10; }
constexpr RGBC RGBC_C(uint8_t r, uint8_t g, uint8_t b) { return RGBC_R(r) | RGBC_G(g) | RGBC_B(b); }
constexpr RGBC RGBC_GRAY(uint8_t l) { return RGBC_C(l, l, l); }
enum Color : RGBC {
	RGB_BLACK              = RGBC_GRAY(0x00),
	RGB_DARKGRAY           = RGBC_GRAY(0x40),
	RGB_GRAY               = RGBC_GRAY(0x80),
	RGB_LIGHTGRAY          = RGBC_GRAY(0xD3),
	RGB_WHITE              = RGBC_GRAY(0xFF),

	RGB_RED                = RGBC_R(0xFF),
	RGB_GREEN              = RGBC_G(0xFF),
	RGB_BLUE               = RGBC_B(0xFF),

	RGB_DARKRED            = RGBC_R(0x80),
	RGB_DARKGREEN          = RGBC_G(0x80),
	RGB_DARKBLUE           = RGBC_B(0x80),

	RGB_LIGHTBLUE          = RGBC_C(0x80, 0x80, 0xFF),
	RGB_LIGHTGREEN         = RGBC_C(0x80, 0xFF, 0x80),
	RGB_LIGHTRED           = RGBC_C(0xFF, 0x80, 0x80),

	RGB_BROWN              = RGBC_C(0xA5, 0x2A, 0x2A),

	RGB_YELLOW             = RGBC_C(0xFF, 0xFF, 0x00),
	RGB_LIGHTYELLOW        = RGBC_C(0xFF, 0xFF, 0x80),
	RGB_DARKYELLOW         = RGBC_C(0x80, 0x80, 0x00),

	RGB_MAGENTA            = RGBC_C(0xFF, 0x00, 0xFF),
	RGB_LIGHTMAGENTA       = RGBC_C(0xFF, 0x80, 0xFF),
	RGB_DARKMAGENTA        = RGBC_C(0x80, 0x00, 0x80),

	RGB_CYAN               = RGBC_C(0x00, 0xFF, 0xFF),
	RGB_LIGHTCYAN          = RGBC_C(0x80, 0xFF, 0xFF),
	RGB_DARKCYAN           = RGBC_C(0x00, 0x80, 0x80),

	RGB_INVALID_COLOR      = 0xFF000000,
};
#pragma endregion

using	  DRAWMODE = uint8_t;
constexpr DRAWMODE
		  DRAWMODE_NORMAL (0 << 1),
		  DRAWMODE_TRANS  (1 << 1);

using		ALIGN = uint8_t; /* take only 4-btis */
constexpr   ALIGN
			ALIGN_LEFT		(1 << 0),
			ALIGN_RIGHT		(2 << 0),
			ALIGN_HCENTER	(3 << 0),
			ALIGN_TOP		(1 << 2),
			ALIGN_BOTTOM	(2 << 2),
			ALIGN_VCENTER	(3 << 2);

using	  TEXTALIGN = uint8_t;
constexpr TEXTALIGN TEXTALIGN_LEFT		(ALIGN_LEFT),
					TEXTALIGN_RIGHT		(ALIGN_RIGHT),
					TEXTALIGN_HCENTER	(ALIGN_HCENTER),
					TEXTALIGN_TOP		(ALIGN_TOP),
					TEXTALIGN_BOTTOM	(ALIGN_BOTTOM),
					TEXTALIGN_VCENTER	(ALIGN_VCENTER);
constexpr TEXTALIGN TEXTALIGN_CENTER	(TEXTALIGN_HCENTER | TEXTALIGN_VCENTER);
