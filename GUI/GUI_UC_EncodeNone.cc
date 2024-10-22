#include "GUI.h"
static uint16_t _GetCharCode(const char *s) {
	return *(const uint8_t *)s;
}
static int _GetCharSize(const char *s) {
	return 1;
}
static int _CalcSizeOfChar(uint16_t Char) {
	return 1;
}
static int _Encode(char *s, uint16_t Char) {
	*s = (uint8_t)(Char);
	return 1;
}
const GUI_UC_ENC_APILIST GUI__API_TableNone = {
	_GetCharCode,     /*  return character code as uint16_t */
	_GetCharSize,     /*  return size of character: 1 */
	_CalcSizeOfChar,  /*  return size of character: 1 */
	_Encode           /*  Encode character */
};
void GUI_UC_SetEncodeNone(void) {
	GUI.pUC_API = &GUI__API_TableNone;
}
