#pragma once

#include "GUI_Types.h"

#pragma region GUI Portable Store Info
bool GUI_MOUSE_Store(MOUSE_STATE);
bool GUI_KEY_Store(KEY_STATE);
bool GUI_RECT_Store(SRect);
#pragma endregion

#pragma region Memory
void GUI__strcpy(char *s, const char *c);
void GUI__memcpy(void *sDest, const void *pSrc, size_t Len);

void *GUI_MEM_Alloc(size_t Size);
void *GUI_MEM_AllocZero(size_t Size);
void *GUI_MEM_Realloc(void *pOld, size_t NewSize);
void  GUI_MEM_Free(void *ptr);
#pragma endregion

#pragma region LCD
void GUI_X_LCD_SetBitmap(const BitmapRect &br);
void GUI_X_LCD_GetBitmap(BitmapRect &br);
void GUI_X_LCD_FillRect(const SRect &r, RGBC c);

BitmapRect GUI_X_LCD_AllocBitmap(const SRect &r);
void GUI_X_LCD_FreeBitmap(BitmapRect &br);
#pragma endregion

void GUI_X_Init();
bool GUI_X_Run();
