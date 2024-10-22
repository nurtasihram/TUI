#include "GUI.h"

#include "chars_pic.inl"
const unsigned char GUI_Pixels_CrossS[33] = {
	________,XXXXXX__,________,
	________,XXooXX__,________,
	________,XXooXX__,________,
	________,XXooXX__,________,
	XXXXXXXX,XXooXXXX,XXXXXX__,
	XXoooooo,ooXXoooo,ooooXX__,
	XXXXXXXX,XXooXXXX,XXXXXX__,
	________,XXooXX__,________,
	________,XXooXX__,________,
	________,XXooXX__,________,
	________,XXXXXX__,________,
};
const Bitmap GUI_BitmapCrossS = {
	11, 11, 3, 2,
	GUI_Pixels_CrossS,
	&GUI_CursorPal
};
const Cursor GUI_CursorCrossS = {
	&GUI_BitmapCrossS, 5, 5
};
const Bitmap GUI_BitmapCrossSI = {
	11, 11, 3, 2, 
	GUI_Pixels_CrossS,
	&GUI_CursorPalI
};
const Cursor GUI_CursorCrossSI = {
  &GUI_BitmapCrossSI, 5, 5
};
