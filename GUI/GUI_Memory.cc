
#include <string.h>
#include <stdlib.h>

#include "GUI.h"

#pragma region String Basics
int GUI__strlen(const char *s) {
	if (!s) return 0;
	int len = 0;
	while (*s++ != '\0')
		++len;
	return len;
}
void GUI__strcpy(char *s, const char *c) {
	strcpy(s, c);
}
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len) {
	memcpy(sDest, pSrc, Len);
}
uint16_t GUI__countStrings(const char *pStrings) {
	uint16_t cnt = 0;
	if (pStrings)
		for (; *pStrings; ++cnt)
			while (*pStrings++);
	return cnt;
}
const char *GUI__nextString(const char *pStrings) {
	while (*pStrings++);
	return pStrings;
}
#pragma endregion

#pragma region Memory Manager
void *GUI_MEM_Alloc(size_t Size) {
	return Size ? malloc(Size) : nullptr;
}
void *GUI_MEM_Realloc(void *pOld, int NewSize) {
	return realloc(pOld, NewSize);
}
void *GUI_MEM_AllocZero(size_t Size) {
	if (!Size) return nullptr;
	void *ptr = GUI_MEM_Alloc(Size);
	if (!ptr)
		return nullptr;
	memset(ptr, 0, Size);
	return ptr;
}
void GUI_MEM_Free(void *ptr) {
	if (ptr)
		free(ptr);
}
#pragma endregion
