#include "GUI.h"

#include "chars_pic.inl"
static const unsigned char GUI_Pixels_CrossM[126] = {
	________,________,__XXXXXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	XXXXXXXX,XXXXXXXX,XXXXooXX,XXXXXXXX,XXXXXXXX,XX______,
	XXoooooo,oooooooo,ooooXXoo,oooooooo,oooooooo,XX______,
	XXXXXXXX,XXXXXXXX,XXXXooXX,XXXXXXXX,XXXXXXXX,XX______,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXooXX,________,________,________,
	________,________,__XXXXXX,________,________,________,
};
const Bitmap GUI_BitmapCrossM = {
	21, 21, 6, 2,
	GUI_Pixels_CrossM,
	&GUI_CursorPal
};
const Cursor GUI_CursorCrossM = {
  &GUI_BitmapCrossM, 10, 10
};
const Bitmap GUI_BitmapCrossMI = {
	21, 21, 6, 2, 
	GUI_Pixels_CrossM,
	&GUI_CursorPalI
};
const Cursor GUI_CursorCrossMI = {
	&GUI_BitmapCrossMI, 10, 10
};
