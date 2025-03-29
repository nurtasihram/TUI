#pragma once

#include "GUI.h"

void LCD_SetBitmap(const BitmapRect &br);
void LCD_GetBitmap(BitmapRect &br);
void LCD_FillRect(const SRect &r);

BitmapRect LCD_AllocBitmap(const SRect &r);
void LCD_FreeBitmap(BitmapRect &br);

void GUI_X_LCD_Init();
