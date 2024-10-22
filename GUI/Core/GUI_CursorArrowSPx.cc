#include "GUI.h"

#include "chars_pic.inl"
static const unsigned char GUI_Pixels_ArrowS[45] = {
	XX______,________,________,
	XXXX____,________,________,
	XXooXX__,________,________,
	XXooooXX,________,________,
	XXoooooo,XX______,________,
	XXoooooo,ooXX____,________,
	XXoooooo,ooooXX__,________,
	XXoooooo,ooooooXX,________,
	XXoooooo,ooXXXXXX,XX______,
	XXooXXoo,ooXX____,________,
	XXXX__XX,ooooXX__,________,
	XX____XX,ooooXX__,________,
	________,XXooooXX,________,
	________,XXooooXX,________,
	________,__XXXX__,________,
};
const Bitmap GUI_BitmapArrowS = {
	9, 15, 3, 2,
	GUI_Pixels_ArrowS,
	&GUI_CursorPal
};
const Cursor GUI_CursorArrowS = {
	&GUI_BitmapArrowS, 0, 0
};
const Bitmap GUI_BitmapArrowSI = {
	9, 15, 3, 2,
	GUI_Pixels_ArrowS,
	&GUI_CursorPalI
};
const Cursor GUI_CursorArrowSI = {
	&GUI_BitmapArrowSI, 0, 0
};
