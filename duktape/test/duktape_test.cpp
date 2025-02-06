#include <initializer_list>
#include <iostream>

#include "./wx/window.h"
#include "duktape.h"

using namespace WX;

class duktape {
	duk_context *ctx = O;
public:
	static void *duk_alloc(void *, duk_size_t size) {
		return malloc(size);
	}
	static void *duk_realloc(void *, void *ptr, duk_size_t size) {
		return realloc(ptr, size);
	}
	static void duk_free(void *, void *ptr) {
		free(ptr);
	}
	struct exception {
		duk_errcode_t code;
		const char *MsgId;
	};
	static void duk_fatal(duk_context *ctx, duk_errcode_t code, const char *MsgId) {
		throw exception{ code, MsgId };
	}
public:
	duktape(Null) {}
	duktape() : ctx(duk_create_heap(
		duk_alloc, duk_realloc, duk_free, O, duk_fatal)) {}
	~duktape() {
		if (ctx) {
			duk_destroy_heap(ctx);
			ctx = nullptr;
		}
	}
public:
	struct property {
		const char *name;
		duk_c_function setter, getter;
	};
	inline void add_properties(std::initializer_list<property> props) {
		for (auto &p : props) {
			duk_push_string(ctx, p.name);
			duk_push_c_function(ctx, p.getter, 0);
			duk_push_c_function(ctx, p.setter, 1);
			duk_def_prop(ctx, -4,
						 /* getter */				DUK_DEFPROP_HAVE_GETTER |
						 /* setter */				DUK_DEFPROP_HAVE_SETTER |
						 /* configurable : false */	DUK_DEFPROP_HAVE_CONFIGURABLE | 0
			);
		}
	}
	struct constant {
		const char *name;
		duk_uint_t value;
	};
	inline void add_constants(std::initializer_list<constant> consts) {
		for (auto &c : consts) {
			duk_push_string(ctx, c.name);
			duk_push_uint(ctx, c.value);
			duk_def_prop(ctx, 0,
						 /* value */				DUK_DEFPROP_HAVE_VALUE |
						 /* enumerable : true */	DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
						 /* configurable : false */	DUK_DEFPROP_HAVE_CONFIGURABLE | 0
			);
		}
	}
	struct function {
		const char *name;
		duk_c_function constructor;
	};
	inline void add_functions(std::initializer_list<function> funcs) {
		for (auto &f : funcs) {
			duk_push_string(ctx, f.name);
			duk_push_c_function(ctx, f.constructor, DUK_VARARGS);
			duk_put_prop(ctx, -3);
		}
	}
public:
	inline auto operator()(const char *str) {
		return duk_eval_raw(
			ctx, str, 0,
			1 /*args*/ |
			DUK_COMPILE_EVAL |
			DUK_COMPILE_NOSOURCE |
			DUK_COMPILE_STRLEN |
			DUK_COMPILE_NOFILENAME);
	}
};

int main() {
	char buff[1024];
	duktape ctx;
	for (; ;) {
		std::cout << "JavaScript > ";
		std::cin.getline(buff, sizeof(buff));
		try {
			ctx(buff);
		} catch (duktape::exception err) {
			std::cout << "FATAL: " << err.MsgId << std::endl;
			std::cout << "CODE:  " << err.code << std::endl;
		}
	}
	system("pause");
	return 0;
}
