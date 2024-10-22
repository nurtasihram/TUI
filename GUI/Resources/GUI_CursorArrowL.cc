#include "GUI.h"

static const BPP2_DAT GUI_Pixels_ArrowL[]{
	XX______,________,________,________,________,
	XXXX____,________,________,________,________,
	XXooXX__,________,________,________,________,
	XXooooXX,________,________,________,________,
	XXoooooo,XX______,________,________,________,
	XXoooooo,ooXX____,________,________,________,
	XXoooooo,ooooXX__,________,________,________,
	XXoooooo,ooooooXX,________,________,________,
	XXoooooo,oooooooo,XX______,________,________,
	XXoooooo,oooooooo,ooXX____,________,________,
	XXoooooo,oooooooo,ooooXX__,________,________,
	XXoooooo,oooooooo,ooooooXX,________,________,
	XXoooooo,oooooooo,oooooooo,XX______,________,
	XXoooooo,oooooooo,oooooooo,ooXX____,________,
	XXoooooo,oooooooo,oooooooo,ooooXX__,________,
	XXoooooo,oooooooo,oooooooo,ooooooXX,________,
	XXoooooo,oooooooo,oooooooo,oooooooo,XX______,
	XXoooooo,oooooooo,ooXXXXXX,XXXXXXXX,XXXX____,
	XXoooooo,ooXXoooo,ooXX____,________,________,
	XXoooooo,XX__XXoo,ooooXX__,________,________,
	XXooooXX,____XXoo,ooooXX__,________,________,
	XXooXX__,______XX,ooooooXX,________,________,
	XXXX____,______XX,ooooooXX,________,________,
	XX______,________,XXoooooo,XX______,________,
	________,________,XXoooooo,XX______,________,
	________,________,__XXoooo,ooXX____,________,
	________,________,__XXoooo,ooXX____,________,
	________,________,____XXoo,ooooXX__,________,
	________,________,____XXoo,ooooXX__,________,
	________,________,______XX,XXXX____,________
};
CCursor GUI_CursorArrowL{
	{
		/* Size */ { 18, 30 },
		/* BytesPerLine */ 5,
		/* Bits */ GUI_Pixels_ArrowL,
		/* Palette */ &GUI_CursorPal,
		/* Transparent */ true
	}
};
CCursor GUI_CursorArrowLI{
	{
		/* Size */ { 18, 30 },
		/* BytesPerLine */ 5,
		/* Bits */ GUI_Pixels_ArrowL,
		/* Palette */ &GUI_CursorPalI,
		/* Transparent */ true
	}
};
