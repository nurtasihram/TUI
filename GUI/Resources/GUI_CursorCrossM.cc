#include "GUI.h"

static const BPP2_DAT GUI_Pixels_CrossM[]{
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
CCursor GUI_CursorCrossM{
	{
		/* Size */ { 21, 21 },
		/* BytesPerLine */ 6,
		/* Bits */ GUI_Pixels_CrossM,
		/* Palette */ &GUI_CursorPal,
		/* Transparent */ true
	}, { 10, 10 }
};
CCursor GUI_CursorCrossMI{
	{
		/* Size */ { 21, 21 },
		/* BytesPerLine */ 6,
		/* Bits */ GUI_Pixels_CrossM,
		/* Palette */ &GUI_CursorPalI,
		/* Transparent */ true
	}, { 10, 10 }
};
