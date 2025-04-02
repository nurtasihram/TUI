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

static const BPP2_DAT _acCursorMove[]{
	________,________,XX______,________,________,
	________,______XX,ooXX____,________,________,
	________,____XXoo,ooooXX__,________,________,
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
	________,____XXoo,ooooXX__,________,________,
	________,______XX,ooXX____,________,________,
	________,________,XX______,________,________,
};
CCursor GUI_CursorMove{
	{
		/* Size */ { 17, 17 },
		/* BytesPerLine */ 5,
		/* Bits */ _acCursorMove,
		/* Palette */ &GUI_PalCursor,
		/* Transparent */ true
	}, { 8, 8 }
};

static const BPP2_DAT _acCursorResizeH[]{
	________,XX______,________,XX______,________,
	______XX,XX______,________,XXXX____,________,
	____XXoo,XX______,________,XXooXX__,________,
	__XXoooo,XXXXXXXX,XXXXXXXX,XXooooXX,________,
	XXoooooo,oooooooo,oooooooo,oooooooo,XX______,
	__XXoooo,XXXXXXXX,XXXXXXXX,XXooooXX,________,
	____XXoo,XX______,________,XXooXX__,________,
	______XX,XX______,________,XXXX____,________,
	________,XX______,________,XX______,________,
};
CCursor GUI_CursorResizeH{
	{
		/* Size */ { 17, 9 },
		/* BytesPerLine */ 5,
		/* Bits */ _acCursorResizeH,
		/* Palette */ &GUI_PalCursor,
		/* Transparent */ true
	}, { 8, 4 }
};

static const BPP2_DAT _acCursorResizeV[]{
	________,XX______,________,
	______XX,ooXX____,________,
	____XXoo,ooooXX__,________,
	__XXoooo,ooooooXX,________,
	XXXXXXXX,ooXXXXXX,XX______,
	______XX,ooXX____,________,
	______XX,ooXX____,________,
	______XX,ooXX____,________,
	______XX,ooXX____,________,
	______XX,ooXX____,________,
	______XX,ooXX____,________,
	______XX,ooXX____,________,
	XXXXXXXX,ooXXXXXX,XX______,
	__XXoooo,ooooooXX,________,
	____XXoo,ooooXX__,________,
	______XX,ooXX____,________,
	________,XX______,________,
};
CCursor GUI_CursorResizeV{
	{
		/* Size */ { 8, 17 },
		/* BytesPerLine */ 3,
		/* Bits */ _acCursorResizeV,
		/* Palette */ &GUI_PalCursor,
		/* Transparent */ true
	}, { 4, 8 }
};

static const BPP2_DAT _acCursorResizeDD[]{
	XXXXXXXX,XXXX____,________,________,
	XXoooooo,XX______,________,________,
	XXooooXX,________,________,________,
	XXooXXoo,XX______,________,________,
	XXXX__XX,ooXX____,________,________,
	XX______,XXooXX__,________,________,
	________,__XXooXX,________,________,
	________,____XXoo,XX______,XX______,
	________,______XX,ooXX__XX,XX______,
	________,________,XXooXXoo,XX______,
	________,________,__XXoooo,XX______,
	________,________,XXoooooo,XX______,
	________,______XX,XXXXXXXX,XX______,
};
CCursor GUI_CursorResizeDD{ {
		/* Size */ { 13, 13 },
		/* BytesPerLine */ 4,
		/* Bits */ _acCursorResizeDD,
		/* Palette */ &GUI_PalCursor,
		/* Transparent */ true
	}, { 6, 6 }
};

static const BPP2_DAT _acCursorResizeDU[]{
	________,______XX,XXXXXXXX,XX______,
	________,________,XXoooooo,XX______,
	________,________,__XXoooo,XX______,
	________,________,XXooXXoo,XX______,
	________,______XX,ooXX__XX,XX______,
	________,____XXoo,XX______,XX______,
	________,__XXooXX,________,________,
	XX______,XXooXX__,________,________,
	XXXX__XX,ooXX____,________,________,
	XXooXXoo,XX______,________,________,
	XXooooXX,________,________,________,
	XXoooooo,XX______,________,________,
	XXXXXXXX,XXXX____,________,________,
};
CCursor GUI_CursorResizeDU{
	{
		/* Size */ { 13, 13 },
		/* BytesPerLine */ 4,
		/* Bits */ _acCursorResizeDU,
		/* Palette */ &GUI_PalCursor,
		/* Transparent */ true
	}, { 6, 6 }
};
