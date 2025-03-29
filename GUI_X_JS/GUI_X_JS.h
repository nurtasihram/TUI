#pragma once

void GUI_X_JS_Init();
void GUI_X_JS_Exec();

#include "duktape.h"

struct js_exception {
	duk_errcode_t code;
	const char *MsgId;
};
extern duk_context *ctx;

#ifdef _WIN32
void GUI_X_JS_CMDL();
#endif
