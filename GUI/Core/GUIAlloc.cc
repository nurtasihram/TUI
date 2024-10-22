#include <stdlib.h>

#include "GUI.h"


void * GUI_ALLOC_AllocNoInit(size_t Size) {
	return malloc(Size);
}
void GUI_ALLOC_Free(void * hMem) {
	if (hMem == nullptr)
		return;
	free(hMem);
}
void GUI_ALLOC_Init(void) {
}
////////////
void * GUI_ALLOC_Realloc(void * hOld, int NewSize) {
	return realloc(hOld, NewSize);
}
void * GUI_ALLOC_AllocInit(const void *pInitData, size_t Size) {
	if (Size == 0)
		return nullptr;
	void * hMem = GUI_ALLOC_AllocNoInit(Size);
	if (!hMem)
		return nullptr;
	if (pInitData)
		GUI__memcpy(hMem, pInitData, Size);
	else
		GUI__memset(hMem, 0, Size);
	return hMem;
}
void * GUI_ALLOC_AllocZero(size_t Size) {
	void * hMem = GUI_ALLOC_AllocNoInit(Size);
	if (!hMem)
		return nullptr;
	GUI__memset(hMem, 0, Size);
	return hMem;
}
