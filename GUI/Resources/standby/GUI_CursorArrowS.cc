#include "GUI.h"

static const BPP2_DAT GUI_Pixels_ArrowS[]{
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
CCursor GUI_CursorArrowS{
	{
		/* Size */ { 9, 15 },
		/* BytesPerLine */ 3,
		/* Bits */ GUI_Pixels_ArrowS,
		/* Palette */ &GUI_CursorPal,
		/* Transparent */ true
	}
};
CCursor GUI_CursorArrowSI{
	{
		/* Size */ { 9, 15 },
		/* BytesPerLine */ 3,
		/* Bits */ GUI_Pixels_ArrowS,
		/* Palette */ &GUI_CursorPalI,
		/* Transparent */ true
	}
};
