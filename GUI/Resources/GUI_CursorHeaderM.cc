#include "GUI.h"

static const BPP2_DAT GUI_PixelsHeaderM[]{
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	____XX__,______XX,ooXX____,____XX__,________,
	__XXooXX,XXXXXXXX,ooXXXXXX,XXXXooXX,________,
	XXoooooo,oooooooo,oooooooo,oooooooo,XX______,
	__XXooXX,XXXXXXXX,ooXXXXXX,XXXXooXX,________,
	____XX__,______XX,ooXX____,____XX__,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
	________,______XX,ooXX____,________,________,
};
CCursor GUI_CursorHeaderM{
	{
		/* Size */ { 17, 17 },
		/* BytesPerLine */ 5,
		/* Bits */ GUI_PixelsHeaderM,
		/* Palette */ &GUI_CursorPal,
		/* Transparent */ true
	}, { 8, 8 }
};
CCursor GUI_CursorHeaderMI{
	{
		/* Size */ { 17, 17 },
		/* BytesPerLine */ 5,
		/* Bits */ GUI_PixelsHeaderM,
		/* Palette */ &GUI_CursorPalI,
		/* Transparent */ true
	}, { 8, 8 }
};
