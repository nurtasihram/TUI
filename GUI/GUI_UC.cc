#include "GUI.h"
int GUI_UC_GetCharSize(const char *s) {
	return GUI.pUC_API->pfGetCharSize(s);
}
uint16_t GUI_UC_GetCharCode(const char *s) {
	return GUI.pUC_API->pfGetCharCode(s);
}
int GUI_UC_Encode(char *s, uint16_t Char) {
	return GUI.pUC_API->pfEncode(s, Char);
}
int GUI_UC__CalcSizeOfChar(uint16_t Char) {
	return GUI.pUC_API->pfCalcSizeOfChar(Char);
}
uint16_t GUI_UC__GetCharCodeInc(const char **ps) {
	const char *s = *ps;
	uint16_t r = GUI_UC__GetCharCode(s);
	s += GUI_UC__GetCharSize(s);
	*ps = s;

	return r;
}
int GUI_UC__NumChars2NumBytes(const char *s, int NumChars) {
	int NumBytes = 0;
	while (NumChars--) {
		int CharSize = GUI_UC__GetCharSize(s);
		s += CharSize;
		NumBytes += CharSize;
	}
	return NumBytes;
}
int GUI_UC__NumBytes2NumChars(const char *s, intptr_t NumBytes) {
	int Chars = 0, Bytes = 0;
	while (NumBytes > Bytes) {
		int CharSize = GUI_UC__GetCharSize(s + Bytes);
		Bytes += CharSize;
		Chars++;
	}
	return Chars;
}
/*********************************************************************
*
*       GUI_UC2DB
*
* Purpose:
*   Convert Convert UNICODE 16-bit value into double byte version
*   - Byte0: First byte, msb
*   - Byte1: Last  byte, lsb
*/
void GUI_UC2DB(uint16_t Code, uint8_t *pOut) {
	// move regular ASCII to (unused) 0xE000-0xE0ff area
	if (Code < 0x100) {
		if (Code != 0)  // end marker ?
			Code += 0xe000;
	}
	// If Low byte 0: Move to (unused) 0xE100-0xE1ff area
	else if ((Code & 0xff) == 0)
		Code = (Code >> 8) + 0xe100;
	// Convert into double byte, putting msb first
	*pOut++ = Code >> 8; // Save msb first
	*pOut = Code & 0xff;
}
/*********************************************************************
*
*       GUI_DB2UC
*
* Purpose:
*   Convert Unicode double byte into UNICODE.
*   - Byte0: First byte, msb
*   - Byte1: Last  byte, lsb
*/
uint16_t GUI_DB2UC(uint8_t Byte0, uint8_t Byte1) {
	if (Byte0 == 0)
		return 0;
	if ((Byte0 & 0xfe) == 0xe0)
		return Byte0 == 0xe0 ?
		Byte1 : // ASCII
		((uint16_t)Byte1) << 8; // low byte was zero
	return Byte1 | (((uint16_t)Byte0) << 8);
}
