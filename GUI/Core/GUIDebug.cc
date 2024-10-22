#include "GUI_Protected.h"
#include "GUI_X.h"
#define MAXLEN 50
static void _CopyString(char* d, const char* s, int MaxLen) {
	while ((MaxLen > 0) && *s) {
		*d++ = *s++;
		MaxLen--;
	}
	*d = 0;
}
static void _sPrintHex32(int val, char *buf) {
	uint8_t i, c;
	for (i = 0; i < 8; ++i) {
		c = val & 0xf00000;
		*buf++ = c < 10 ? c + '0' : c + ('A' - 0xA);
		val <<= 4;
	}
	*buf = '\0';
}
void GUI_ErrorOut(const char* s) { GUI_X_ErrorOut(s); }
void GUI_ErrorOut1(const char* s, int p0) {
	char ac[MAXLEN + 10] = { 0 };
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	GUI_ErrorOut(ac);
}
void GUI_ErrorOut2(const char* s, int p0, int p1) {
	char ac[MAXLEN + 20] = { 0 };
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	_sPrintHex32(p1, sOut);
	GUI_ErrorOut(ac);
}
void GUI_ErrorOut3(const char* s, int p0, int p1, int p2) {
	char ac[MAXLEN + 30] = { 0 };
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	_sPrintHex32(p1, sOut);
	_sPrintHex32(p2, sOut);
	GUI_ErrorOut(ac);
}
void GUI_ErrorOut4(const char* s, int p0, int p1, int p2, int p3) {
	char ac[MAXLEN + 40] = { 0 };
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	_sPrintHex32(p1, sOut);
	_sPrintHex32(p2, sOut);
	_sPrintHex32(p3, sOut);
	GUI_ErrorOut(ac);
}
void GUI_Log(const char* s) { GUI_X_Log(s); }
void GUI_Log1(const char* s, int p0) {
	char ac[MAXLEN + 10] = { 0 };
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	GUI_Log(ac);
}
void GUI_Log2(const char* s, int p0, int p1) {
	char ac[MAXLEN + 20] = { 0 };
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	_sPrintHex32(p1, sOut);
	GUI_Log(ac);
}
void GUI_Log3(const char* s, int p0, int p1, int p2) {
	char ac[MAXLEN + 30] = { 0 };
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	_sPrintHex32(p1, sOut);
	_sPrintHex32(p2, sOut);
	GUI_Log(ac);
}
void GUI_Log4(const char* s, int p0, int p1, int p2, int p3) {
	char ac[MAXLEN + 40] = { 0 };
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	_sPrintHex32(p1, sOut);
	_sPrintHex32(p2, sOut);
	_sPrintHex32(p3, sOut);
	GUI_Log(ac);
}
void GUI_Warn(const char* s) { GUI_X_Warn(s); }
void GUI_Warn1(const char* s, int p0) {
	char ac[MAXLEN + 10];
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	GUI_Warn(ac);
}
void GUI_Warn2(const char* s, int p0, int p1) {
	char ac[MAXLEN + 20];
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	_sPrintHex32(p1, sOut);
	GUI_Warn(ac);
}
void GUI_Warn3(const char* s, int p0, int p1, int p2) {
	char ac[MAXLEN + 30];
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	_sPrintHex32(p1, sOut);
	_sPrintHex32(p2, sOut);
	GUI_Warn(ac);
}
void GUI_Warn4(const char* s, int p0, int p1, int p2, int p3) {
	char ac[MAXLEN + 40];
	char* sOut = ac;
	_CopyString(ac, s, MAXLEN);
	sOut += GUI__strlen(sOut);
	_sPrintHex32(p0, sOut);
	_sPrintHex32(p1, sOut);
	_sPrintHex32(p2, sOut);
	_sPrintHex32(p3, sOut);
	GUI_Warn(ac);
}
