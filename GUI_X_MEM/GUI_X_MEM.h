#pragma once

#include <stdint.h>

void GUI__strcpy(char *s, const char *c);
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len);

void *GUI_MEM_Alloc(size_t Size);
void *GUI_MEM_AllocZero(size_t Size);
void *GUI_MEM_Realloc(void *pOld, size_t NewSize);
void  GUI_MEM_Free(void *ptr);
