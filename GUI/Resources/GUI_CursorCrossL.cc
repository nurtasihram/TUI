
#include "GUI.h"

static const BPP2_DAT GUI_Pixels_CrossL[]{
	________,________,________,____XXXX,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	XXXXXXXX,XXXXXXXX,XXXXXXXX,XXXXXXoo,XXXXXXXX,XXXXXXXX,XXXXXXXX,XXXXXX__,
	XXoooooo,oooooooo,oooooooo,ooooooXX,oooooooo,oooooooo,oooooooo,ooooXX__,
	XXXXXXXX,XXXXXXXX,XXXXXXXX,XXXXXXoo,XXXXXXXX,XXXXXXXX,XXXXXXXX,XXXXXX__,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXoo,XX______,________,________,________,
	________,________,________,____XXXX,XX______,________,________,________
};
CCursor GUI_CursorCrossL{
	{
		/* Size */ { 31, 31 },
		/* BytesPerLine */ 8,
		/* Bits */ GUI_Pixels_CrossL,
		/* Palette */ &GUI_CursorPal,
		/* Transparent */ true
	}, { 15, 15 }
};
CCursor GUI_CursorCrossLI{
	{
		/* Size */ { 31, 31 },
		/* BytesPerLine */ 8,
		/* Bits */ GUI_Pixels_CrossL,
		/* Palette */ &GUI_CursorPalI,
		/* Transparent */ true
	}, { 15, 15 }
};