
#include "WM.h"
static struct {
	int Key;
	int PressedCnt;
} _KeyMsg;
static int _KeyMsgCnt;
static int _Key;
GUI_KEY_MSG_HOOK *GUI_pfKeyMsgHook;
int GUI_GetKey(void) {
	int r = _Key;
	_Key = 0;
	return r;
}
void GUI_StoreKey(int Key) {
	if (!_Key)
		_Key = Key;
}
void GUI_ClearKeyBuffer(void) {
	while (GUI_GetKey()) {}
}
void GUI_StoreKeyMsg(int Key, int PressedCnt) {
	_KeyMsg.Key = Key;
	_KeyMsg.PressedCnt = PressedCnt;
	_KeyMsgCnt = 1;
}
bool GUI_PollKeyMsg(void) {
	if (!_KeyMsgCnt)
		return false;
	int Key = _KeyMsg.Key;
	_KeyMsgCnt--;
	WObj::OnKey(Key, _KeyMsg.PressedCnt);
	if (_KeyMsg.PressedCnt == 1)
		GUI_StoreKey(Key);
	return true;
}
void GUI_SendKeyMsg(int Key, int PressedCnt) {
	if (!WObj::OnKey(Key, PressedCnt))
		GUI_StoreKeyMsg(Key, PressedCnt);
}
GUI_KEY_MSG_HOOK *GUI_SetKeyMsgHook(GUI_KEY_MSG_HOOK *pHook) {
	GUI_KEY_MSG_HOOK *r = GUI_pfKeyMsgHook;
	GUI_pfKeyMsgHook = pHook;
	return r;
}
