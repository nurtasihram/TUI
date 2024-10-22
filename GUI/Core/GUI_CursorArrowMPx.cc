#include "GUI.h"

#include "chars_pic.inl"
static const unsigned char GUI_Pixels_ArrowM[60] = {
	XX______,________,________,
	XXXX____,________,________,
	XXooXX__,________,________,
	XXooooXX,________,________,
	XXoooooo,XX______,________,
	XXoooooo,ooXX____,________,
	XXoooooo,ooooXX__,________,
	XXoooooo,ooooooXX,________,
	XXoooooo,oooooooo,XX______,
	XXoooooo,oooooooo,ooXX____,
	XXoooooo,oooooooo,ooooXX__,
	XXoooooo,ooooooXX,XXXXXXXX,
	XXooooXX,ooooooXX,________,
	XXooXX__,XXoooooo,XX______,
	XXXX____,XXoooooo,XX______,
	XX______,__XXoooo,ooXX____,
	________,__XXoooo,ooXX____,
	________,____XXoo,ooooXX__,
	________,____XXoo,ooooXX__,
	________,______XX,XXXX____
};
const Bitmap GUI_BitmapArrowM = {
	12, 20, 3, 2,
	GUI_Pixels_ArrowM,
	&GUI_CursorPal
};
const Cursor GUI_CursorArrowM = {
	&GUI_BitmapArrowM, 0, 0
};
const Bitmap GUI_BitmapArrowMI = {
	12, 20, 3, 2,
	GUI_Pixels_ArrowM,
	&GUI_CursorPalI};
const Cursor GUI_CursorArrowMI = {
	&GUI_BitmapArrowMI, 0, 0
};
