#include "GUI.h"

static const BPP2_DAT GUI_Pixels_CrossS[]{
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
CCursor GUI_CursorCrossS{
	{
		/* Size */ { 11, 11 },
		/* BytesPerLine */ 3,
		/* Bits */ GUI_Pixels_CrossS,
		/* Palette */ &GUI_CursorPal,
		/* Transparent */ true
	}, { 5, 5 }
};
CCursor GUI_CursorCrossSI{
	{
		/* Size */ { 11, 11 },
		/* BytesPerLine */ 3,
		/* Bits */ GUI_Pixels_CrossS,
		/* Palette */ &GUI_CursorPalI,
		/* Transparent */ true
	}, { 5, 5 }
};
