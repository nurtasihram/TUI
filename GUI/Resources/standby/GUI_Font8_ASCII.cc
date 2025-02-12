#include "GUI.h"

static const BPP1_DAT acFont8ASCII[][8] {
  {
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________}

/*  !  */
 ,{
   XX______________,
   XX______________,
   XX______________,
   XX______________,
   XX______________,
   ________________,
   XX______________,
   ________________}


/*  "  */
 ,{
   XX__XX__________,
   XX__XX__________,
   XX__XX__________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________}

/* #  */
 ,{
   __XX__XX________,
   __XX__XX________,
   XXXXXXXXXX______,
   __XX__XX________,
   XXXXXXXXXX______,
   __XX__XX________,
   __XX__XX________,
   ________________}

/*  $  */
 ,{
   ____XX__________,
   __XXXXXXXX______,
   XX__XX__________,
   __XXXXXX________,
   ____XX__XX______,
   XXXXXXXX________,
   ____XX__________}

/*  %  */
 ,{
   XXXX____________,
   XXXX____XX______,
   ______XX________,
   ____XX__________,
   __XX____________,
   XX____XXXX______,
   ______XXXX______,
   ________________}

/*  &  */
 ,{
   __XXXX__________,
   XX____XX________,
   XX__XX__________,
   __XX____________,
   XX__XX__XX______,
   XX____XX________,
   __XXXX__XX______,
   ________________}

/*  '  */
 ,{
   XXXX____________,
   __XX____________,
   XX______________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________}

/*  (  */
 ,{
   ____XX__________,
   __XX____________,
   XX______________,
   XX______________,
   XX______________,
   __XX____________,
   ____XX__________,
   ________________}

/*  )  */
 ,{
   XX______________,
   __XX____________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   __XX____________,
   XX______________,
   ________________}

/*  *  */
 ,{
   ________________,
   __XX__XX________,
   ____XX__________,
   XXXXXXXXXX______,
   ____XX__________,
   __XX__XX________,
   ________________,
   ________________}

 ,{
   ________________,
   ____XX__________,
   ____XX__________,
   XXXXXXXXXX______,
   ____XX__________,
   ____XX__________,
   ________________,
   ________________}

 ,{
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   XXXX____________,
   __XX____________,
   XX______________}

 ,{
   ________________,
   ________________,
   ________________,
   XXXXXXXXXX______,
   ________________,
   ________________,
   ________________,
   ________________}

 ,{
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   XXXX____________,
   XXXX____________,
   ________________}

 ,{
   ________________,
   ________XX______,
   ______XX________,
   ____XX__________,
   __XX____________,
   XX______________,
   ________________,
   ________________}

/* 0 */
 ,{
   __XXXXXX________,
   XX______XX______,
   XX____XXXX______,
   XX__XX__XX______,
   XXXX____XX______,
   XX______XX______,
   __XXXXXX________,
   ________________}

/* 1 */
 ,{
   ____XX__________,
   __XXXX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   __XXXXXX________,
   ________________}

/* 2 */
 ,{
   __XXXXXX________,
   XX______XX______,
   ________XX______,
   ____XXXX________,
   __XX____________,
   XX______________,
   XXXXXXXXXX______,
   ________________}

/* 3 */
 ,{
   __XXXXXX________,
   XX______XX______,
   ________XX______,
   ____XXXX________,
   ________XX______,
   XX______XX______,
   __XXXXXX________,
   ________________}

/* 4 */
 ,{
   ______XX________,
   ____XXXX________,
   __XX__XX________,
   XX____XX________,
   XXXXXXXXXX______,
   ______XX________,
   ______XX________,
   ________________}

/* 5 */
 ,{
   XXXXXXXXXX______,
   XX______________,
   XXXXXXXX________,
   ________XX______,
   ________XX______,
   XX______XX______,
   __XXXXXX________,
   ________________}

/* 6 */
 ,{
   ____XXXX________,
   __XX____________,
   XX______________,
   XXXXXXXX________,
   XX______XX______,
   XX______XX______,
   __XXXXXX________,
   ________________}

/* 7 */
 ,{
   XXXXXXXXXX______,
   ________XX______,
   ______XX________,
   ____XX__________,
   __XX____________,
   __XX____________,
   __XX____________,
   ________________}

/* 8 */
 ,{
   __XXXXXX________,
   XX______XX______,
   XX______XX______,
   __XXXXXX________,
   XX______XX______,
   XX______XX______,
   __XXXXXX________,
   ________________}

/* 9 */
 ,{
   __XXXXXX________,
   XX______XX______,
   XX______XX______,
   __XXXXXXXX______,
   ________XX______,
   ______XX________,
   __XXXX__________,
   ________________}

/* ':' 3a */
 ,{
   ________________,
   XXXX____________,
   XXXX____________,
   ________________,
   XXXX____________,
   XXXX____________,
   ________________,
   ________________}

/* ';' 3b */
 ,{
   ________________,
   ________________,
   XXXX____________,
   XXXX____________,
   ________________,
   XXXX____________,
   __XX____________,
   XX______________}


/* '<' 3c */
 ,{
   ______XX________,
   ____XX__________,
   __XX____________,
   XX______________,
   __XX____________,
   ____XX__________,
   ______XX________,
   ________________}

/* '=' 3d */
 ,{
   ________________,
   ________________,
   XXXXXXXXXX______,
   ________________,
   XXXXXXXXXX______,
   ________________,
   ________________,
   ________________}

/* '>' */
 ,{
   XX______________,
   __XX____________,
   ____XX__________,
   ______XX________,
   ____XX__________,
   __XX____________,
   XX______________,
   ________________}

/* '?' */
 ,{
   __XXXXXX________,
   XX______XX______,
   ________XX______,
   ______XX________,
   ____XX__________,
   ________________,
   ____XX__________,
   ________________}

/* @ */
 ,{
   __XXXXXX________,
   XX______XX______,
   ________XX______,
   __XXXX__XX______,
   XX__XX__XX______,
   XX__XX__XX______,
   __XXXXXX________,
   ________________}

/* A */
 ,{
   __XXXXXX________,
   XX______XX______,
   XX______XX______,
   XXXXXXXXXX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   ________________}

/* B */
 ,{
   XXXXXXXX________,
   XX______XX______,
   XX______XX______,
   XXXXXXXX________,
   XX______XX______,
   XX______XX______,
   XXXXXXXX________,
   ________________}

/* C */
 ,{
   __XXXXXX________,
   XX______XX______,
   XX______________,
   XX______________,
   XX______________,
   XX______XX______,
   __XXXXXX________,
   ________________}

/* D */
 ,{
   XXXXXX__________,
   XX____XX________,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XX____XX________,
   XXXXXX__________,
   ________________}

/* E */
 ,{
   XXXXXXXXXX______,
   XX______________,
   XX______________,
   XXXXXXXX________,
   XX______________,
   XX______________,
   XXXXXXXXXX______,
   ________________}

/* 1 */
 ,{
   XXXXXXXXXX______,
   XX______________,
   XX______________,
   XXXXXXXX________,
   XX______________,
   XX______________,
   XX______________,
   ________________}

/* 1 */
 ,{
   __XXXXXX________,
   XX______XX______,
   XX______________,
   XX______________,
   XX____XXXX______,
   XX______XX______,
   __XXXXXXXX______,
   ________________}

/* 1 */
 ,{
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XXXXXXXXXX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   ________________}

/* I */
 ,{
   XXXXXX__________,
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   XXXXXX__________,
   ________________}

/* J */
 ,{
   ____XXXXXX______,
   ______XX________,
   ______XX________,
   ______XX________,
   ______XX________,
   XX____XX________,
   __XXXX__________,
   ________________}

/* K */
 ,{
   XX______XX______,
   XX____XX________,
   XX__XX__________,
   XXXX____________,
   XX__XX__________,
   XX____XX________,
   XX______XX______,
   ________________}

/* L */
 ,{
   XX______________,
   XX______________,
   XX______________,
   XX______________,
   XX______________,
   XX______________,
   XXXXXXXXXX______,
   ________________}

/* M */
 ,{
   XX______XX______,
   XXXX__XXXX______,
   XX__XX__XX______,
   XX__XX__XX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   ________________}

/* N */
 ,{
   XX______XX______,
   XX______XX______,
   XXXX____XX______,
   XX__XX__XX______,
   XX____XXXX______,
   XX______XX______,
   XX______XX______,
   ________________}

/* O */
 ,{
   __XXXXXX________,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   __XXXXXX________,
   ________________}

/* P */
 ,{
   XXXXXXXX________,
   XX______XX______,
   XX______XX______,
   XXXXXXXX________,
   XX______________,
   XX______________,
   XX______________,
   ________________}

/* Q */
 ,{
   __XXXXXX________,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XX__XX__XX______,
   XX____XX________,
   __XXXX__XX______,
   ________________}

/* R */
 ,{
   XXXXXXXX________,
   XX______XX______,
   XX______XX______,
   XXXXXXXX________,
   XX__XX__________,
   XX____XX________,
   XX______XX______,
   ________________}

/* S */
 ,{
   __XXXXXX________,
   XX______XX______,
   XX______________,
   __XXXXXX________,
   ________XX______,
   XX______XX______,
   __XXXXXX________,
   ________________}

/* T */
 ,{
   XXXXXXXXXX______,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ________________}

/* U */
 ,{
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   __XXXXXX________,
   ________________}

/* V */
 ,{
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   __XX__XX________,
   ____XX__________,
   ________________}

/* W */
 ,{
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XX__XX__XX______,
   XX__XX__XX______,
   XX__XX__XX______,
   __XX__XX________,
   ________________}

/* X */
 ,{
   XX______XX______,
   XX______XX______,
   __XX__XX________,
   ____XX__________,
   __XX__XX________,
   XX______XX______,
   XX______XX______,
   ________________}

/* Y */
 ,{
   XX______XX______,
   XX______XX______,
   __XX__XX________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ________________}

/* Z */
 ,{
   XXXXXXXXXX______,
   ________XX______,
   ______XX________,
   ____XX__________,
   __XX____________,
   XX______________,
   XXXXXXXXXX______,
   ________________}

/* 5b */
 ,{
   XXXXXX__________,
   XX______________,
   XX______________,
   XX______________,
   XX______________,
   XX______________,
   XXXXXX__________,
   ________________}

/* 5c */
 ,{
   ________________,
   XX______________,
   __XX____________,
   ____XX__________,
   ______XX________,
   ________XX______,
   ________________,
   ________________}

/* 5d */
 ,{
   XXXXXX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   XXXXXX__________,
   ________________}

/* 5e */
 ,{
   ____XX__________,
   __XX__XX________,
   XX______XX______,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________}

/* 5f */
 ,{
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   XXXXXXXXXX______}

/* 60 */
 ,{
   XX______________,
   __XX____________,
   ____XX__________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________}

/* a */
 ,{
   ________________,
   ________________,
   __XXXXXX________,
   ________XX______,
   __XXXXXXXX______,
   XX______XX______,
   __XXXXXXXX______,
   ________________}

/* b */
 ,{
   XX______________,
   XX______________,
   XX__XXXX________,
   XXXX____XX______,
   XX______XX______,
   XX______XX______,
   XXXXXXXX________,
   ________________}

/* c */
 ,{
   ________________,
   ________________,
   __XXXX__________,
   XX____XX________,
   XX______________,
   XX____XX________,
   __XXXX__________,
   ________________}

/* d */
 ,{
   ________XX______,
   ________XX______,
   __XXXX__XX______,
   XX____XXXX______,
   XX______XX______,
   XX______XX______,
   __XXXXXXXX______,
   ________________}

/* e */
 ,{
   ________________,
   ________________,
   __XXXXXX________,
   XX______XX______,
   XXXXXXXXXX______,
   XX______________,
   __XXXXXX________,
   ________________}

/* f */
 ,{
   ____XX__________,
   __XX__XX________,
   __XX____________,
   XXXXXX__________,
   __XX____________,
   __XX____________,
   __XX____________,
   ________________}

/* g */
 ,{
   ________________,
   ________________,
   __XXXXXXXX______,
   XX______XX______,
   XX______XX______,
   __XXXXXXXX______,
   ________XX______,
   __XXXXXX________}

/* h */
 ,{
   XX______________,
   XX______________,
   XX__XXXX________,
   XXXX____XX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   ________________}

/* i */
 ,{
   XX______________,
   ________________,
   XX______________,
   XX______________,
   XX______________,
   XX______________,
   XX______________,
   ________________}

/* j */
 ,{
   ____XX__________,
   ________________,
   __XXXX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   XXXX____________}

/* k */
 ,{
   XX______________,
   XX______________,
   XX____XX________,
   XX__XX__________,
   XXXX____________,
   XX__XX__________,
   XX____XX________,
   ________________}

/* l */
 ,{
   XXXX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   XXXXXX__________,
   ________________}

/* m */
 ,{
   ________________,
   ________________,
   XXXX__XX________,
   XX__XX__XX______,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   ________________}

/* n */
 ,{
   ________________,
   ________________,
   XX__XXXX________,
   XXXX__XX________,
   XX____XX________,
   XX____XX________,
   XX____XX________,
   ________________}

/* o */
 ,{
   ________________,
   ________________,
   __XXXX__________,
   XX____XX________,
   XX____XX________,
   XX____XX________,
   __XXXX__________,
   ________________}

/* p */
 ,{
   ________________,
   ________________,
   XXXXXX__________,
   XX____XX________,
   XX____XX________,
   XXXXXX__________,
   XX______________,
   XX______________}

/* q */
 ,{
   ________________,
   ________________,
   __XXXXXX________,
   XX____XX________,
   XX____XX________,
   __XXXXXX________,
   ______XX________,
   ______XX________}

/* r */
 ,{
   ________________,
   ________________,
   XX__XX__________,
   XXXX____________,
   XX______________,
   XX______________,
   XX______________,
   ________________}

/* s */
 ,{
   ________________,
   ________________,
   __XXXXXX________,
   XX______________,
   __XXXX__________,
   ______XX________,
   XXXXXX__________,
   ________________}

/* t */
 ,{
   __XX____________,
   __XX____________,
   XXXXXX__________,
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   ________________}

/* u */
 ,{
   ________________,
   ________________,
   XX____XX________,
   XX____XX________,
   XX____XX________,
   XX____XX________,
   __XXXXXX________,
   ________________}

/* v */
 ,{
   ________________,
   ________________,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   __XX__XX________,
   ____XX__________,
   ________________}

/* w */
 ,{
   ________________,
   ________________,
   XX______XX______,
   XX______XX______,
   XX__XX__XX______,
   XX__XX__XX______,
   __XX__XX________,
   ________________}

/* XX */
 ,{
   ________________,
   ________________,
   XX______XX______,
   __XX__XX________,
   ____XX__________,
   __XX__XX________,
   XX______XX______,
   ________________}

/* y */
 ,{
   ________________,
   ________________,
   XX____XX________,
   XX____XX________,
   XX____XX________,
   __XXXXXX________,
   ______XX________,
   __XXXX__________}

/* z */
 ,{
   ________________,
   ________________,
   XXXXXXXXXX______,
   ______XX________,
   ____XX__________,
   __XX____________,
   XXXXXXXXXX______,
   ________________}

/* 0x7b */
 ,{
   ____XX__________,
   __XX____________,
   __XX____________,
   XX______________,
   __XX____________,
   __XX____________,
   ____XX__________,
   ________________}

/* 0x7c */
 ,{
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   ________________}

/* 0x7d */
 ,{
   XX______________,
   __XX____________,
   __XX____________,
   ____XX__________,
   __XX____________,
   __XX____________,
   XX______________,
   ________________}

/* 0x7e */
 ,{
   __XXXX__XX______,
   XX____XX________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________}
};

static const FontProp::CharInfo GUI_Font8_ASCII_CharInfo[95] {
   { 3,3,1, acFont8ASCII[0]}  /* char.code 32: ' ' */
  ,{ 2,2,1, acFont8ASCII[1]}  /* char.code 33: '!' */
  ,{ 4,4,1, acFont8ASCII[2]}  /* char.code 34: '"' */
  ,{ 6,6,1, acFont8ASCII[3]}  /* char.code 35: '#' */
  ,{ 6,6,1, acFont8ASCII[4]}  /* char.code 36: '$' */
  ,{ 6,6,1, acFont8ASCII[5]}  /* char.code 37: '%' */
  ,{ 6,6,1, acFont8ASCII[6]}  /* char.code 38: '&' */
  ,{ 3,3,1, acFont8ASCII[7]}  /* char.code 39: ''' */
  ,{ 4,4,1, acFont8ASCII[8]}  /* char.code 40: '(' */
  ,{ 4,4,1, acFont8ASCII[9]}  /* char.code 41: ')' */
  ,{ 6,6,1, acFont8ASCII[10]} /* char.code 42: '*' */
  ,{ 6,6,1, acFont8ASCII[11]} /* char.code 43: '+' */
  ,{ 3,3,1, acFont8ASCII[12]} /* char.code 44: ',' */
  ,{ 6,6,1, acFont8ASCII[13]} /* char.code 45: '-' */
  ,{ 3,3,1, acFont8ASCII[14]} /* char.code 46: '.' point */
  ,{ 6,6,1, acFont8ASCII[15]} /* char.code 47: '/' */
  ,{ 6,6,1, acFont8ASCII[16]} /* char.code 48: '0' */
  ,{ 6,6,1, acFont8ASCII[17]} /* char.code 49: '1' */
  ,{ 6,6,1, acFont8ASCII[18]} /* char.code 50: '2' */
  ,{ 6,6,1, acFont8ASCII[19]} /* char.code 51: '3' */
  ,{ 6,6,1, acFont8ASCII[20]} /* char.code 52: '4' */
  ,{ 6,6,1, acFont8ASCII[21]} /* char.code 53: '5' */
  ,{ 6,6,1, acFont8ASCII[22]} /* char.code 54: '6' */
  ,{ 6,6,1, acFont8ASCII[23]} /* char.code 55: '7' */
  ,{ 6,6,1, acFont8ASCII[24]} /* char.code 56: '8' */
  ,{ 6,6,1, acFont8ASCII[25]} /* char.code 57: '9' */
  ,{ 3,3,1, acFont8ASCII[26]} /* char.code 58: ':' */
  ,{ 3,3,1, acFont8ASCII[27]} /* char.code 59: ';' */
  ,{ 5,5,1, acFont8ASCII[28]} /* char.code 60: '<' */
  ,{ 6,6,1, acFont8ASCII[29]} /* char.code 61: '=' */
  ,{ 5,5,1, acFont8ASCII[30]} /* char.code 62: '>' */
  ,{ 6,6,1, acFont8ASCII[31]} /* char.code 63: '?' */
  ,{ 6,6,1, acFont8ASCII[32]} /* char.code 64: '@' */
  ,{ 6,6,1, acFont8ASCII[33]} /* char.code 65: 'A' */
  ,{ 6,6,1, acFont8ASCII[34]} /* char.code 66: 'B' */
  ,{ 6,6,1, acFont8ASCII[35]} /* char.code 67: 'C' */
  ,{ 6,6,1, acFont8ASCII[36]} /* char.code 68: 'D' */
  ,{ 6,6,1, acFont8ASCII[37]} /* char.code 69: 'E' */
  ,{ 6,6,1, acFont8ASCII[38]} /* char.code 70: 'F' */
  ,{ 6,6,1, acFont8ASCII[39]} /* char.code 71: 'G' */
  ,{ 6,6,1, acFont8ASCII[40]} /* char.code 72: 'H' */
  ,{ 4,4,1, acFont8ASCII[41]} /* char.code 73: 'I' */
  ,{ 6,6,1, acFont8ASCII[42]} /* char.code 74: 'J' */
  ,{ 6,6,1, acFont8ASCII[43]} /* char.code 75: 'K' */
  ,{ 6,6,1, acFont8ASCII[44]} /* char.code 76: 'L' */
  ,{ 6,6,1, acFont8ASCII[45]} /* char.code 77: 'M' */
  ,{ 6,6,1, acFont8ASCII[46]} /* char.code 78: 'N' */
  ,{ 6,6,1, acFont8ASCII[47]} /* char.code 79: 'O' */
  ,{ 6,6,1, acFont8ASCII[48]} /* char.code 80: 'P' */
  ,{ 6,6,1, acFont8ASCII[49]} /* char.code 81: 'Q' */
  ,{ 6,6,1, acFont8ASCII[50]} /* char.code 82: 'R' */
  ,{ 6,6,1, acFont8ASCII[51]} /* char.code 83: 'S' */
  ,{ 6,6,1, acFont8ASCII[52]} /* char.code 84: 'T' */
  ,{ 6,6,1, acFont8ASCII[53]} /* char.code 85: 'U' */
  ,{ 6,6,1, acFont8ASCII[54]} /* char.code 86: 'V' */
  ,{ 6,6,1, acFont8ASCII[55]} /* char.code 87: 'W' */
  ,{ 6,6,1, acFont8ASCII[56]} /* char.code 88: 'X' */
  ,{ 6,6,1, acFont8ASCII[57]} /* char.code 89: 'Y' */
  ,{ 6,6,1, acFont8ASCII[58]} /* char.code 90: 'Z' */
  ,{ 4,4,1, acFont8ASCII[59]} /* char.code 91: '[' */
  ,{ 6,6,1, acFont8ASCII[60]} /* char.code 92: '\' */
  ,{ 4,4,1, acFont8ASCII[61]} /* char.code 93: ']' */
  ,{ 6,6,1, acFont8ASCII[62]} /* char.code 94: '^' */
  ,{ 6,6,1, acFont8ASCII[63]} /* char.code 95: '_' */
  ,{ 6,6,1, acFont8ASCII[64]} /* char.code 96: '`' */
  ,{ 6,6,1, acFont8ASCII[65]} /* char.code 97: 'a' */
  ,{ 6,6,1, acFont8ASCII[66]} /* char.code 98: 'b' */
  ,{ 5,5,1, acFont8ASCII[67]} /* char.code 99: 'c' */
  ,{ 6,6,1, acFont8ASCII[68]} /* char.code 100: 'd' */
  ,{ 6,6,1, acFont8ASCII[69]} /* char.code 101: 'e' */
  ,{ 5,5,1, acFont8ASCII[70]} /* char.code 102: 'f' */
  ,{ 6,6,1, acFont8ASCII[71]} /* char.code 103: 'g' */
  ,{ 6,6,1, acFont8ASCII[72]} /* char.code 104: 'h' */
  ,{ 2,2,1, acFont8ASCII[73]} /* char.code 105: 'i' */
  ,{ 4,4,1, acFont8ASCII[74]} /* char.code 106: 'j' */
  ,{ 5,5,1, acFont8ASCII[75]} /* char.code 107: 'k' */
  ,{ 4,4,1, acFont8ASCII[76]} /* char.code 108: 'l' */
  ,{ 6,6,1, acFont8ASCII[77]} /* char.code 109: 'm' */
  ,{ 5,5,1, acFont8ASCII[78]} /* char.code 110: 'n' */
  ,{ 5,5,1, acFont8ASCII[79]} /* char.code 111: 'o' */
  ,{ 5,5,1, acFont8ASCII[80]} /* char.code 112: 'p' */
  ,{ 5,5,1, acFont8ASCII[81]} /* char.code 113: 'q' */
  ,{ 4,4,1, acFont8ASCII[82]} /* char.code 114: 'r' */
  ,{ 5,5,1, acFont8ASCII[83]} /* char.code 115: 's' */
  ,{ 4,4,1, acFont8ASCII[84]} /* char.code 116: 't' */
  ,{ 5,5,1, acFont8ASCII[85]} /* char.code 117: 'u' */
  ,{ 6,6,1, acFont8ASCII[86]} /* char.code 118: 'v' */
  ,{ 6,6,1, acFont8ASCII[87]} /* char.code 119: 'w' */
  ,{ 6,6,1, acFont8ASCII[88]} /* char.code 120: 'x' */
  ,{ 5,5,1, acFont8ASCII[89]} /* char.code 121: 'y' */
  ,{ 6,6,1, acFont8ASCII[90]} /* char.code 122: 'z' */
  ,{ 4,4,1, acFont8ASCII[91]} /* char.code 123: '{' */
  ,{ 3,3,1, acFont8ASCII[92]} /* char.code 124: '|' */
  ,{ 4,4,1, acFont8ASCII[93]} /* char.code 125: '}' */
  ,{ 6,6,1, acFont8ASCII[94]} /* char.code 126: '~' */
};

const FontProp GUI_Font8_ASCII {
	8, /* height of font  */
	8, /* space of font y */ 
	7,
	5,
	7,
	32, /* first character */
	126, /* last character */
	GUI_Font8_ASCII_CharInfo /* address of first character */
};
