#pragma once

#include "duktape.h"

struct js_exception {
	duk_errcode_t code;
	const char *MsgId;
};
extern duk_context *ctx;

void GUI_X_JS_Init();
void GUI_X_JS_Exec(const char *lpCmdl);
