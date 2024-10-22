#pragma once
#include "WM_Intern.h"
typedef int GUI_HOOK_FUNC(WM_MESSAGE *pMsg);
typedef struct GUI_HOOK {
	struct GUI_HOOK *pNext;
	GUI_HOOK_FUNC *pHookFunc;
} GUI_HOOK;
void GUI_HOOK_Add   (GUI_HOOK **ppFirstHook, GUI_HOOK *pNewHook, GUI_HOOK_FUNC *pHookFunc);
void GUI_HOOK_Remove(GUI_HOOK **ppFirstHook, GUI_HOOK *pHook);
