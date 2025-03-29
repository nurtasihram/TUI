#include "Static.h"
#include "Button.h"
#include "CheckBox.h"
#include "ProgBar.h"
#include "ScrollBar.h"
#include "Slider.h"
#include "Radio.h"
#include "ListBox.h"
#include "Frame.h"
#include "Header.h"
#include "ListView.h"
#include "MultiPage.h"

#include "GUI_X_JS.h"

void property(duk_context *ctx, const char *name, duk_c_function setter, duk_c_function getter) {
	duk_push_string(ctx, name);
	if (getter)
		duk_push_c_function(ctx, getter, 0);
	if (setter)
		duk_push_c_function(ctx, setter, 1);
	duk_def_prop(ctx, -(2 + (getter ? 1 : 0) + (setter ? 1 : 0)),
					/* getter */ (getter ? DUK_DEFPROP_HAVE_GETTER : 0) |
					/* setter */ (setter ? DUK_DEFPROP_HAVE_SETTER : 0) |
					/* configurable : false */	DUK_DEFPROP_CLEAR_CONFIGURABLE
	);
}
struct Property {
	const char *name;
	duk_c_function setter, getter;
};
void property(duk_context *ctx, const Property *c, uint32_t len) {
	for (; len--; ++c)
		property(ctx, c->name, c->setter, c->getter);
}
template<size_t len>
void property(duk_context *ctx, const Property(&c)[len]) {
	property(ctx, c, len);
}

void constant(duk_context *ctx, const char *name, duk_uint_t value) {
	duk_push_string(ctx, name);
	duk_push_uint(ctx, value);
	duk_def_prop(ctx, 0,
					/* value */				DUK_DEFPROP_HAVE_VALUE |
					/* enumerable : true */	DUK_DEFPROP_SET_ENUMERABLE |
					/* configurable : false */	DUK_DEFPROP_CLEAR_CONFIGURABLE
	);
}
struct Constant {
	const char *name;
	duk_uint_t value;
};
void constant(duk_context *ctx, const Constant *c, uint32_t len) {
	for (; len--; ++c)
		constant(ctx, c->name, c->value);
}
template<size_t len>
void constant(duk_context *ctx, const Constant(&c)[len]) {
	constant(ctx, c, len);
}

void function_global(duk_context *ctx, const char *name, duk_c_function constructor, duk_idx_t nargs = DUK_VARARGS) {
	duk_push_c_function(ctx, constructor, nargs);
	duk_put_global_string(ctx, name);
}

void this_extends(duk_context *ctx, const char *classname) {
	duk_push_this(ctx);
	duk_get_global_string(ctx, classname);
	duk_set_prototype(ctx, -2);
}
void class_extends(duk_context *ctx, const char *classname) {
	duk_push_object(ctx);
	duk_get_global_string(ctx, classname);
	duk_set_prototype(ctx, -2);
}

void object_global(duk_context *ctx, void(*defs)(duk_context *ctx)) {
	duk_push_global_object(ctx);
	defs(ctx);
	duk_pop(ctx);
}
void object_struct(duk_context *ctx, const char *name, void(*constuctor)(duk_context *ctx)) {
	duk_push_object(ctx);
	constuctor(ctx);
	duk_put_global_string(ctx, name);
}
void member_int(duk_context *ctx, const char *name, duk_int_t i) {
	duk_push_int(ctx, i);
	duk_put_prop_string(ctx, -2, name);
}
void member_function(duk_context *ctx, const char *name, duk_c_function func, duk_idx_t nargs = 0) {
	duk_push_c_function(ctx, func, nargs);
	duk_put_prop_string(ctx, -2, name);
}

WObj *JS_X_WObj(duk_context *ctx) {
	duk_push_this(ctx);
	duk_get_prop_string(ctx, -1, "__p");
	if (duk_get_type(ctx, -1) != DUK_TYPE_POINTER)
		return nullptr; // pointer lost
	return (::WObj *)duk_get_pointer(ctx, -1);
}
void JS_X_WObj(duk_context *ctx, WObj *p) {
	duk_push_this(ctx);
	duk_push_string(ctx, "__p");
	duk_push_pointer(ctx, p);
	duk_def_prop(ctx, -3,
					DUK_DEFPROP_HAVE_VALUE
					| DUK_DEFPROP_HAVE_WRITABLE | 0		// writable : false
					| DUK_DEFPROP_HAVE_CONFIGURABLE | 0	// configurable : false
	);
}

#define _VPROPS_(name, cclass, type) \
	#name, \
	[](duk_context *ctx) -> duk_ret_t { \
		if (auto pObj = (cclass *)JS_X_WObj(ctx)) \
			pObj->name(duk_to_##type(ctx, 0)); \
		return 0; \
	}, \
	[](duk_context *ctx) -> duk_ret_t { \
		if (auto pObj = (cclass *)JS_X_WObj(ctx)) \
			duk_push_##type(ctx, pObj->name()); \
		else \
			duk_push_##type(ctx, false); \
		return 1; \
	}
#define _VPROPS_TEXT_(type) \
	"Text", \
	[](duk_context *ctx) -> duk_ret_t { \
		if (auto pObj = (type *)JS_X_WObj(ctx)) \
			pObj->Text(duk_to_string(ctx, 0)); \
		return 0; \
	}, \
	[](duk_context *ctx) -> duk_ret_t { \
		if (auto pObj = (type *)JS_X_WObj(ctx)) \
			duk_push_string(ctx, pObj->Text()); \
		else \
			duk_push_string(ctx, ""); \
		return 1; \
	}
#define _ENUM_(name) { #name, name }
static void LoadSymbols(duk_context *ctx) {

	static Constant global_flags[]{
		/* WindowCreate Flags */
		_ENUM_(WC_HIDE),
		_ENUM_(WC_VISIBLE),
		_ENUM_(WC_STAYONTOP),
		_ENUM_(WC_DISABLED),
		_ENUM_(WC_ACTIVATE),
		_ENUM_(WC_NOACTIVATE),
		_ENUM_(WC_FOCUSSABLE),
		_ENUM_(WC_ANCHOR_MASK),
		_ENUM_(WC_ANCHOR_HORIZONTAL),
		_ENUM_(WC_ANCHOR_VERTICAL),
		_ENUM_(WC_ANCHOR_LEFT),
		_ENUM_(WC_ANCHOR_RIGHT),
		_ENUM_(WC_ANCHOR_TOP),
		_ENUM_(WC_ANCHOR_BOTTOM),
		_ENUM_(WC_AUTOSCROLL),
		_ENUM_(WC_AUTOSCROLL_H),
		_ENUM_(WC_AUTOSCROLL_V),
		/* TextAlign Flags*/
		_ENUM_(TEXTALIGN_LEFT),
		_ENUM_(TEXTALIGN_RIGHT),
		_ENUM_(TEXTALIGN_HCENTER),
		_ENUM_(TEXTALIGN_TOP),
		_ENUM_(TEXTALIGN_BOTTOM),
		_ENUM_(TEXTALIGN_VCENTER),
		_ENUM_(TEXTALIGN_CENTER),
		/* Button Flags */
		_ENUM_(BUTTON_BI_UNPRESSED),
		_ENUM_(BUTTON_BI_PRESSED),
		_ENUM_(BUTTON_BI_DISABLED),
		_ENUM_(BUTTON_CI_UNPRESSED),
		_ENUM_(BUTTON_CI_PRESSED),
		_ENUM_(BUTTON_CI_DISABLED),
		/* CheckBox Flags */
		_ENUM_(CHECKBOX_BI_INACTIV),
		_ENUM_(CHECKBOX_BI_ACTIV),
		_ENUM_(CHECKBOX_BI_INACTIV_3STATE),
		_ENUM_(CHECKBOX_BI_ACTIV_3STATE),
		_ENUM_(CHECKBOX_CI_INACTIV),
		_ENUM_(CHECKBOX_CI_ACTIV),
		/* DropDown Flags */
		/* Frame Flags */
		_ENUM_(FRAME_CF_ACTIVE),
		_ENUM_(FRAME_CF_MOVEABLE),
		_ENUM_(FRAME_CF_RESIZEABLE),
		_ENUM_(FRAME_CF_TITLEVIS),
		_ENUM_(FRAME_CF_MINIMIZED),
		_ENUM_(FRAME_CF_MAXIMIZED),
		/* Header Flags */
		/* ListBox Flags */
		_ENUM_(LISTBOX_CF_AUTOSCROLLBAR_H),
		_ENUM_(LISTBOX_CF_AUTOSCROLLBAR_V),
		_ENUM_(LISTBOX_CF_MULTISEL),
		_ENUM_(LISTBOX_CI_UNSEL),
		_ENUM_(LISTBOX_CI_SEL),
		_ENUM_(LISTBOX_CI_SELFOCUS),
		/* ListView Flags */
		_ENUM_(LISTVIEW_CF_HEADER_DRAG),
		_ENUM_(LISTVIEW_CF_GRIDLINE),
		_ENUM_(LISTVIEW_CI_UNSEL),
		_ENUM_(LISTVIEW_CI_SEL),
		_ENUM_(LISTVIEW_CI_SELFOCUS),
		/* Menu Flags */
		_ENUM_(MULTIPAGE_CI_DISABLED),
		_ENUM_(MULTIPAGE_CI_ENABLED),
		/* MultiPage Flags */
		_ENUM_(MULTIPAGE_CI_DISABLED),
		_ENUM_(MULTIPAGE_CI_ENABLED),
		/* ProgBar Flags */
		_ENUM_(PROGBAR_CI_INACTIV),
		_ENUM_(PROGBAR_CI_ACTIV),
		/* Radio Flags */
		_ENUM_(RADIO_BI_INACTIV),
		_ENUM_(RADIO_BI_ACTIV),
		_ENUM_(RADIO_BI_CHECK),
		/* ScrollBar Flags */
		_ENUM_(SCROLLBAR_CF_VERTICAL),
		_ENUM_(SCROLLBAR_CF_FOCUSSABLE),
		/* Slider Flags */
		_ENUM_(SLIDER_CF_VERTICAL),
		/* Static Flags */
		_ENUM_(STATIC_CF_LEFT),
		_ENUM_(STATIC_CF_HCENTER),
		_ENUM_(STATIC_CF_RIGHT),
		_ENUM_(STATIC_CF_TOP),
		_ENUM_(STATIC_CF_VCENTER),
		_ENUM_(STATIC_CF_BOTTOM),
		_ENUM_(STATIC_CF_CENTER)
	};
	object_global(ctx, [](duk_context *ctx) {
		constant(ctx, global_flags);
	});

	/* Point */
	//duk_eval_raw(
	//	ctx,
	//	"PointObj={x:0,y:0,toString:function(){return'{x:'+this.x+',y:'+this.y+'}'}};"
	//	"function Point(x,y){this.x=x;this.y=y;}", 0,
	//	1 /*args*/ |
	//	DUK_COMPILE_EVAL |
	//	DUK_COMPILE_NOSOURCE |
	//	DUK_COMPILE_STRLEN |
	//	DUK_COMPILE_NOFILENAME);
	object_struct(ctx, "PointObj", [](duk_context *ctx) {
		member_int(ctx, "x", 0);
		member_int(ctx, "y", 0);
		member_function(ctx, "toString", [](duk_context *ctx) -> duk_ret_t {
			duk_push_this(ctx);
			duk_get_prop_string(ctx, -1, "x");
			duk_get_prop_string(ctx, -2, "y");
			duk_push_sprintf(ctx, "{x:%d,y:%d}",
							 duk_get_int(ctx, -2),
							 duk_get_int(ctx, -1));
			return 1;
		});
	});
	function_global(ctx, "Point", [](duk_context *ctx) {
		if (!duk_is_constructor_call(ctx))
			return 0;
		int x = 0, y = 0;
		switch (duk_get_top(ctx)) {
			case 0:
				break;
			case 1:
				if (duk_is_object(ctx, 0)) {
					duk_get_global_string(ctx, "Point");
					if (duk_instanceof(ctx, 0, -1))
					return 0;
				}
				x = y = duk_to_int(ctx, 0);
				break;
			case 2:
				x = duk_to_int(ctx, 0);
				y = duk_to_int(ctx, 1);
				break;
			default:
				return DUK_RET_SYNTAX_ERROR;
		}
		this_extends(ctx, "PointObj");
		duk_push_this(ctx);
		duk_push_int(ctx, x);
		duk_put_prop_string(ctx, -2, "x");
		duk_push_int(ctx, y);
		duk_put_prop_string(ctx, -2, "y");
		return 0;
	});

	/* Rect */
	static auto __SRect = [](duk_context *ctx) -> SRect {
		duk_push_this(ctx);
		duk_get_prop_string(ctx, -1, "x0");
		duk_get_prop_string(ctx, -2, "y0");
		duk_get_prop_string(ctx, -3, "x1");
		duk_get_prop_string(ctx, -4, "y1");
		return{
			duk_get_int(ctx, -4),
			duk_get_int(ctx, -3),
			duk_get_int(ctx, -2),
			duk_get_int(ctx, -1)
		};
	};
	object_struct(ctx, "RectObj", [](duk_context *ctx) {
		member_int(ctx, "x0", 0);
		member_int(ctx, "y0", 0);
		member_int(ctx, "x1", 0);
		member_int(ctx, "y1", 0);
		property(ctx, "SizeX", nullptr, [](duk_context *ctx) -> duk_ret_t {
			duk_push_this(ctx);
			duk_get_prop_string(ctx, -1, "x0");
			duk_get_prop_string(ctx, -2, "x1");
			auto x0 = duk_get_int(ctx, -2);
			auto x1 = duk_get_int(ctx, -1);
			duk_push_int(ctx, x1 - x0 + 1);
			return 1;
		});
		property(ctx, "SizeY", nullptr, [](duk_context *ctx) -> duk_ret_t {
			duk_push_this(ctx);
			duk_get_prop_string(ctx, -1, "y0");
			duk_get_prop_string(ctx, -2, "y1");
			auto y0 = duk_get_int(ctx, -2);
			auto y1 = duk_get_int(ctx, -1);
			duk_push_int(ctx, y1 - y0 + 1);
			return 1;
		});
		property(ctx, "Size", nullptr, [](duk_context *ctx) -> duk_ret_t {
			auto &&r = __SRect(ctx);
			duk_push_global_object(ctx);
			duk_get_prop_string(ctx, -1, "Point");
			duk_push_int(ctx, r.xsize());
			duk_push_int(ctx, r.ysize());
			duk_new(ctx, 2);
			return 1;
		});
		property(ctx, "LeftTop",
			[](duk_context *ctx) -> duk_ret_t {
				duk_get_prop_string(ctx, 0, "x");
				duk_get_prop_string(ctx, 0, "y");
				auto x0 = duk_get_int(ctx, -2),
						y0 = duk_get_int(ctx, -1);
				duk_push_this(ctx);
				member_int(ctx, "x0", x0);
				member_int(ctx, "y0", y0);
				return 0;
			},
			[](duk_context *ctx) -> duk_ret_t {
				duk_push_this(ctx);
				duk_get_prop_string(ctx, -1, "x0");
				duk_get_prop_string(ctx, -2, "y0");
				auto x0 = duk_get_int(ctx, -2),
					 y0 = duk_get_int(ctx, -1);
				duk_push_global_object(ctx);
				duk_get_prop_string(ctx, -1, "Point");
				duk_push_int(ctx, x0);
				duk_push_int(ctx, y0);
				duk_new(ctx, 2);
				return 1;
			}
		);
		member_function(ctx, "toString", [](duk_context *ctx) -> duk_ret_t {
			auto &&r = __SRect(ctx);
			duk_push_sprintf(
				ctx, "{ x0: %d, y0: %d, x1: %d, y1: %d }",
				r.x0, r.y0, r.x1, r.y1);
			return 1;
		});
	});
	function_global(ctx, "Rect", [](duk_context *ctx) {
		if (!duk_is_constructor_call(ctx))
			return 0;
		int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
		switch (duk_get_top(ctx)) {
			case 0:
				break;
			case 1:
				x0 = y0 = x1 = y1 = duk_to_int(ctx, 0);
				break;
			case 2:
				break;
			case 4:
				x0 = duk_to_int(ctx, 0);
				y0 = duk_to_int(ctx, 1);
				x1 = duk_to_int(ctx, 2);
				y1 = duk_to_int(ctx, 3);
				break;
			default:
				return 0;
		}
		this_extends(ctx, "RectObj");
		duk_push_this(ctx);
		member_int(ctx, "x0", x0);
		member_int(ctx, "y0", y0);
		member_int(ctx, "x1", x1);
		member_int(ctx, "y1", y1);
		return 1;
	});

	/* Constructor */
	static auto _Constructor = [](duk_context *ctx) -> WM_CREATESTRUCT {
		WM_CREATESTRUCT cs;
//		int nargs = duk_get
		if (duk_get_type(ctx, 0)) {}
	};

	/* Window */
	object_struct(ctx, "WObj", [](duk_context *ctx) {
		property(ctx, _VPROPS_(Enable, WObj, boolean));
		property(ctx, _VPROPS_(Visible, WObj, boolean));
		property(ctx, _VPROPS_(Focussable, WObj, boolean));
		property(ctx, _VPROPS_(StayOnTop, WObj, boolean));
		property(ctx, _VPROPS_(ID, WObj, boolean));
		property(ctx, "ClassName", nullptr, [](duk_context *ctx) -> duk_ret_t {
			if (auto pObj = JS_X_WObj(ctx))
				duk_push_string(ctx, pObj->ClassName());
			return 1;
		});
	});
	function_global(ctx, "Window", [](duk_context *ctx) -> duk_ret_t {
		class_extends(ctx, "WObj");
		return 1;
	});

	/* Widget */
	object_struct(ctx, "Widget", [](duk_context *ctx) {
		class_extends(ctx, "WObj");
	});

	/* Button */
	object_struct(ctx, "ButtonObj", [](duk_context *ctx) {
		class_extends(ctx, "Widget");
		property(ctx, _VPROPS_TEXT_(Button));
		property(ctx, _VPROPS_(Pressed, Button, boolean));
		property(ctx, _VPROPS_(TextAlign, Button, uint));
	});
	function_global(ctx, "Button", [](duk_context *ctx) -> duk_ret_t {
		if (!duk_is_constructor_call(ctx))
			return 0;
		//if (duk_get_type(ctx, -1) != DUK_TYPE_POINTER)
		auto pObj = new Button({ 0, 0, 100, 25 }, nullptr, 0, WC_VISIBLE, 0, "Value");
		JS_X_WObj(ctx, pObj);
		this_extends(ctx, "ButtonObj");
		return 1;
	}, DUK_VARARGS);

	/* CheckBox */
	object_struct(ctx, "CheckBoxObj", [](duk_context *ctx) {
		class_extends(ctx, "Widget");
		property(ctx, _VPROPS_TEXT_(CheckBox));
		property(ctx, _VPROPS_(TextColor, CheckBox, uint));
		property(ctx, _VPROPS_(BkColor, CheckBox, uint));
		property(ctx, _VPROPS_(Spacing, CheckBox, uint));
	});
	function_global(ctx, "CheckBox", [](duk_context *ctx) -> duk_ret_t {
		if (!duk_is_constructor_call(ctx))
			return 0;
		//if (duk_get_type(ctx, -1) != DUK_TYPE_POINTER)
		auto pObj = new Button({ 0, 0, 100, 25 }, nullptr, 0, WC_VISIBLE, 0, "Value");
		JS_X_WObj(ctx, pObj);
		this_extends(ctx, "CheckBoxObj");
		return 1;
	}, DUK_VARARGS);

	/*  */

}

#pragma region duktape stack
void *js_alloc(void *, duk_size_t size) {
	return malloc(size);
}
void *js_realloc(void *, void *ptr, duk_size_t size) {
	return realloc(ptr, size);
}
void js_free(void *, void *ptr) {
	free(ptr);
}
static void js_fatal(duk_context *ctx, duk_errcode_t code, const char *MsgId) {
	throw js_exception{ code, MsgId };
}
#pragma endregion

duk_context *ctx = nullptr;
void GUI_X_JS_Init() {
	if (!ctx)
		ctx = duk_create_heap(js_alloc, js_realloc, js_free, nullptr, js_fatal);
	LoadSymbols(ctx);
}
