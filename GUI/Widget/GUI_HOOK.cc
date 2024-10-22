#include <stddef.h>
#include "GUI_HOOK.h"
void GUI_HOOK_Add(GUI_HOOK** ppFirstHook, GUI_HOOK* pNewHook, GUI_HOOK_FUNC* pHookFunc) {
	pNewHook->pNext = *ppFirstHook; /* New entry points to former first one in list */
	pNewHook->pHookFunc = pHookFunc;
	*ppFirstHook = pNewHook;     /* New one is first one now */
}
void GUI_HOOK_Remove(GUI_HOOK** ppFirstHook, GUI_HOOK* pHook) {
	*ppFirstHook = nullptr;
}
