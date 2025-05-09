#pragma once

typedef void(*GUI_X_SIM_pfnOnSize)(uint16_t cx, uint16_t cy);
typedef void(*GUI_X_SIM_pfnOnMouse)(int16_t x, int16_t y, int8_t key);
typedef void(*GUI_X_SIM_pfnOnKey)(UINT vk, BOOL bPressed);

#define DLL_INL_LIST "GUI_X_SIM.inl"
#define MOD_NAME GUI_X_SIM
#include "wx_dll.inl"
