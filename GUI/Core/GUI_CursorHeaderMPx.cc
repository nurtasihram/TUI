#include "GUI.h"

#include "chars_pic.inl"
static const unsigned char GUI_PixelsHeaderM[5 * 17] = {
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	____XX__,______XX,ooXX____,____XX__,________,
	__XXooXX,XXXXXXXX,ooXXXXXX,XXXXooXX,________,
	XXoooooo,oooooooo,oooooooo,oooooooo,XX______,
	__XXooXX,XXXXXXXX,ooXXXXXX,XXXXooXX,________,
	____XX__,______XX,ooXX____,____XX__,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
};
const Bitmap _BitmapHeaderM = {
	17, 17, 5, 2,
	GUI_PixelsHeaderM,
	&GUI_CursorPal
};
const Cursor GUI_CursorHeaderM = {
	&_BitmapHeaderM, 8, 8
};
const Bitmap _Bitmap = {
	17, 17, 5, 2,
	GUI_PixelsHeaderM,
	&GUI_CursorPalI
};
const Cursor GUI_CursorHeaderMI = {
	&_Bitmap, 8, 8
};
