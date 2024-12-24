#include "GUI.h"

static const BPP2_DAT GUI_Pixels_ArrowM[]{
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
CCursor GUI_CursorArrow{
	{
		/* Size */ { 12, 20 },
		/* BytesPerLine */ 3,
		/* Bits */ GUI_Pixels_ArrowM,
		/* Palette */ &GUI_CursorPal,
		/* Transparent */ true
	}
};
CCursor GUI_CursorArrowI{
	{
		/* Size */ { 12, 20 },
		/* BytesPerLine */ 3,
		/* Bits */ GUI_Pixels_ArrowM,
		/* Palette */ &GUI_CursorPalI,
		/* Transparent */ true
	}
};
