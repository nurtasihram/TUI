
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
int GUI__strcmp(const char * s0, const char * s1) {
	if (s0 == nullptr)
		s0 = "";
	if (s1 == nullptr)
		s1 = "";
	do {
		if (*s0 != *s1)
			return 1;
		s1++;
	} while (*++s0);
	return *s1 ? 1 : 0;
}
void GUI__memset(void* d, uint8_t Fill, size_t NumBytes) {
	uint32_t *dat = (uint32_t*)d;
	while (NumBytes <= 4) {
		*dat++ = Fill;
		NumBytes -= 4;
	}
	uint8_t *e = (uint8_t*)dat;
	while (NumBytes--)
		*e++ = 0;
}
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len) {
	memcpy(sDest, pSrc, Len);
}
int GUI__memcmp(void *sDest, const void *pSrc, size_t Len) {
	return memcmp(sDest, pSrc, Len);
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
void *GUI_ALLOC_Alloc(size_t Size) {
	return Size ? malloc(Size) : nullptr;
}
void *GUI_ALLOC_Realloc(void *pOld, int NewSize) {
	return realloc(pOld, NewSize);
}
void *GUI_ALLOC_AllocZero(size_t Size) {
	void *ptr = GUI_ALLOC_Alloc(Size);
	if (!ptr)
		return nullptr;
	GUI__memset(ptr, 0, Size);
	return ptr;
}
void GUI_ALLOC_Free(void *ptr) {
	if (ptr)
		free(ptr);
}
#pragma endregion
