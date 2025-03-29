#include "GUI_X_JS.h"
#include "GUI_X_SIM.h"

#ifdef _WIN32
void GUI_X_JS_CMDL() {
	GUI_X_SIM::JS_Init();
}

void GUI_X_JS_Exec() {
	if (auto lpCmdl = GUI_X_SIM::JS_NewCommand()) {
		try {
			duk_eval_raw(
				ctx,
				lpCmdl, 0,
				1 /*args*/ |
				DUK_COMPILE_EVAL |
				DUK_COMPILE_NOSOURCE |
				DUK_COMPILE_STRLEN |
				DUK_COMPILE_NOFILENAME);
		} catch (js_exception err) {
			GUI_X_SIM::JS_ErrorCommand(err);
		}
		GUI_X_SIM::JS_NextCommand();
	}
}
#endif
