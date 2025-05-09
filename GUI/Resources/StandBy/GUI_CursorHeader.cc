#include "GUI.h"

static const BPP2_DAT _acCursorHeader[]{
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
CCursor GUI_CursorHeader{
	{
		/* Size */ { 17, 17 },
		/* BytesPerLine */ 5,
		/* Bits */ _acCursorHeader,
		/* Palette */ &GUI_PalCursor,
		/* Transparent */ true
	}, { 8, 8 }
};
