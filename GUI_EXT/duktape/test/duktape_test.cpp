#include <iostream>

#include "wx_window.h"
#include "duk_config.h"
#include "../duktape.h"

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

#pragma region property
public: // property
	struct c_property {
		const char *name;
		duk_c_function setter, getter;
	};
	inline void property(const char *name,
						 duk_c_function setter,
						 duk_c_function getter) {
		duk_push_string(ctx, name);
		duk_push_c_function(ctx, getter, 0);
		duk_push_c_function(ctx, setter, 1);
		duk_def_prop(ctx, -4,
					 /* getter */ DUK_DEFPROP_HAVE_GETTER |
					 /* setter */ DUK_DEFPROP_HAVE_SETTER |
					 /* configurable : false */	DUK_DEFPROP_HAVE_CONFIGURABLE | 0
		);
	}
	inline void properties(std::initializer_list<c_property> props) {
		for (auto &p : props)
			property(p.name, p.setter, p.getter);
	}
public: // constant
	struct c_constant {
		const char *name;
		duk_uint_t value;
	};
	inline void constant(const char *name, duk_uint_t value) {
		duk_push_string(ctx, name);
		duk_push_uint(ctx, value);
		duk_def_prop(ctx, 0,
					 /* value */				DUK_DEFPROP_HAVE_VALUE |
					 /* enumerable : true */	DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_ENUMERABLE |
					 /* configurable : false */	DUK_DEFPROP_HAVE_CONFIGURABLE | 0
		);
	}
	inline void constants(std::initializer_list<c_constant> consts) {
		for (auto &c : consts)
			constant(c.name, c.value);
	}
public: // function
	struct c_function {
		const char *name;
		duk_c_function constructor;
	};
	inline void function(const char *name, duk_c_function constructor) {
		duk_push_string(ctx, name);
		duk_push_c_function(ctx, constructor, DUK_VARARGS);
		duk_put_prop(ctx, -3);
	}
	inline void functions(std::initializer_list<c_function> funcs) {
		for (auto &f : funcs)
			function(f.name, f.constructor);
	}
#pragma endregion

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
