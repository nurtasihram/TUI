
#include <string.h>
#include <stdlib.h>

#include "GUI_X_MEM.h"

#pragma region String Basics
void GUI__strcpy(char *s, const char *c) {
	strcpy(s, c);
}
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len) {
	memcpy(sDest, pSrc, Len);
}
#pragma endregion

#pragma region Memory Manager
void *GUI_MEM_Alloc(size_t Size) {
	return Size ? malloc(Size) : nullptr;
}
void *GUI_MEM_AllocZero(size_t Size) {
	if (auto ptr = GUI_MEM_Alloc(Size)) {
		memset(ptr, 0, Size);
		return ptr;
	}
	return nullptr;
}
void *GUI_MEM_Realloc(void *pOld, size_t NewSize) {
	return realloc(pOld, NewSize);
}
void GUI_MEM_Free(void *ptr) {
	if (ptr)
		free(ptr);
}
#pragma endregion
