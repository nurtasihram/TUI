#pragma once

#include "GUI_Types.h"

#pragma region Input
void GUI_X_MOUSE_SetState(MOUSE_STATE);
void GUI_X_KEY_SetState(KEY_STATE);
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

SRect GUI_X_LCD_Rect();

void GUI_X_LCD_Init();
#pragma endregion

void GUI_X_Init();
