#pragma once

#include "GUI_Types.h"

void LCD_SetBitmap(const BitmapRect &);
void LCD_GetBitmap(BitmapRect &);
void LCD_FillRect(const SRect &);

BitmapRect LCD_AllocBitmap(const SRect &);
void LCD_FreeBitmap(BitmapRect &);

SRect LCD_Rect();

void GUI_X_LCD_Init();
