#include "WM.h"

#include "GUI_X_JS.h"

#include "./wx/window.h"
#include "./wx/realtime.h"

using namespace WX;

class GUI_X_JS : public duktape {

public:
	inline void Point(::Point p) {
		duk_push_object(ctx);
		duk_push_int(ctx, p.x);
		duk_put_prop_string(ctx, -2, "x");
		duk_push_int(ctx, p.y);
		duk_put_prop_string(ctx, -2, "y");
	}
	inline ::Point Point() const {
		::Point p;
		if (duk_is_array(ctx, 0)) {
			if (!duk_get_prop_index(ctx, 0, 0))
				return false;
			p.x = duk_get_int(ctx, -1);
			if (!duk_get_prop_index(ctx, 0, 1))
				return false;
			p.y = duk_get_int(ctx, -1);
			return true;
		}
		if (duk_is_object(ctx, 0)) {
			duk_dup(ctx, 0);
			if (!duk_get_prop_string(ctx, 0, "x"))
				return false;
			p.x = duk_to_int(ctx, -1);
			if (!duk_get_prop_string(ctx, 0, "y"))
				return false;
			p.y = duk_to_int(ctx, -1);
			return true;
		}
		return false;
	}

};

void GUI_X_JSCBox() {
	char buff[1024];
	duktape ctx;
	for (;;) {
		std::cout << "JavaScript > ";
		std::cin.getline(buff, sizeof(buff));
		try {
			ctx(buff);
		} catch (duktape::exception err) {
			std::cout << "FATAL: " << err.MsgId << std::endl;
			std::cout << "CODE:  " << err.code << std::endl;
		}
	}
}
