#include "vm.hpp"

// https://duktape.org/api.html
#include <duk_module_node.h>
#include <duktape.h>

#include "log.hpp"

using namespace Scripting;

using Context = Scripting::Context;
using CallArgs = Scripting::CallArgs;
using Module = Scripting::Module;

using error_code = Context::error_code;

static const char* PROP_KEY_CONTEXT = DUK_HIDDEN_SYMBOL("ctx");
static const char* PROP_KEY_CTOR = DUK_HIDDEN_SYMBOL("ctor");
static const char* PROP_KEY_DTOR = DUK_HIDDEN_SYMBOL("dtor");
static const char* PROP_KEY_ID = DUK_HIDDEN_SYMBOL("id");

static const char* PROP_KEY_GLOBAL_SYS_OBJECT = "__sys";

void delete_context(void* ptr) {
    duk_context* ctx = reinterpret_cast<duk_context*>(ptr);
    duk_destroy_heap(ctx);
}


Context::Context() : m_ctx(nullptr, delete_context) {
    duk_context* const ctx = duk_create_heap_default();
    m_ctx.reset((void*)ctx);

    duk_push_pointer(ctx, this);
    duk_put_global_string(ctx, PROP_KEY_CONTEXT);

	duk_eval_string(ctx,
R"(
    (function () {
        return {
            resolve: function (moduleId, parentId) {
                if (!moduleId.endsWith(".js")) {
                    moduleId += ".js"
                }
                var parts = parentId.split("/");
                parts.splice(-1, 1, moduleId);
                var path = parts.join("/");

                var resolvedId = fs.resolve(path);
                //__sys.print("resolve, moduleId: " + moduleId + ", parentId: " + parentId + ", resolvedId: " + resolvedId + ", path: " + path);
                return resolvedId;
            },
            load: function (moduleId, b, c) {
                //__sys.print("load, moduleId: " + moduleId + ", b: " + Object.keys(b) + ", c: " + Object.keys(c));
                var moduleSource = fs.read(moduleId);
                return moduleSource;
            }
        }
    })();
)");

    duk_module_node_init(ctx);

    duk_push_global_object(ctx);
    duk_put_global_string(ctx, "global");

    duk_push_global_stash(ctx);
    duk_eval_string(ctx,
                    "(function (E) {"
                    "return function format(v){"
                    "try{"
                    "if(v instanceof Error)"
                    "return '' + v;"
                    "return E('jx',v);"
                    "}catch(e){"
                    "return ''+v;"
                    "}"
                    "};"
                    "})(Duktape.enc)");
    duk_put_prop_string(ctx, -2, "dukFormat");
    duk_pop(ctx);
}

struct ExternalFunctionEntry {
    void* cookie;
    Scripting::ModuleFunction function;
    int nargs;
};

std::vector<ExternalFunctionEntry> fncs;

duk_ret_t module_callback(duk_context* ctx) {
    auto nargs = duk_get_top(ctx);
    auto index = duk_get_current_magic(ctx);
    //debug("module_callback, index: {}, nargs: {}", index, nargs);

    auto entry = fncs.at(index);

    CallArgs args(ctx, entry.cookie, nargs);
    auto func = entry.function;
    try {
        func(args);
    } catch (std::exception& e) {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "NativeError: %s", e.what());
        return duk_throw(ctx);
    }

    auto top = duk_get_top(ctx);
    //debug("module_callback, index: {}, nargs: {}, xyz: {}", index, nargs, xyz);
    return top - nargs;
}

duk_context* toContext(void* ptr) {
    return reinterpret_cast<duk_context*>(ptr);
};

Context* fromContext(duk_context* ctx) {
    duk_get_global_string(ctx, PROP_KEY_CONTEXT);
    void* ptr = duk_get_pointer(ctx, -1);
    Context* thiz = reinterpret_cast<Context*>(ptr);
    return thiz;
}

void Context::registerModule(const Module& module, void* obj) {
    auto ctx = toContext(m_ctx.get());

    //debug("register module: '{}'", module.key);
    const auto& values = module.value;

    duk_push_object(ctx);

    for (const auto& entry : values) {
        //debug("- register func: '{}'", entry.key);
        duk_push_c_function(ctx, module_callback, entry.nargs);
        duk_set_magic(ctx, -1, fncs.size());
        duk_put_prop_string(ctx, -2, entry.key);

        fncs.push_back({obj, entry.func, entry.nargs});
    }

    duk_put_global_string(ctx, module.key);
};

error_code Context::eval(const std::string_view& src, const std::string_view& name) {
    duk_context* const ctx = reinterpret_cast<duk_context*>(m_ctx.get());
    duk_ret_t res = -1;
    try {
        duk_push_lstring(ctx, src.data(), src.length());
        res = duk_module_node_peval_main(ctx, "/js/main.js");
        //        res = duk_peval_lstring(ctx, buf, len);
        if (res != 0) {
            duk_push_global_stash(ctx);
            duk_get_prop_string(ctx, -1, "dukFormat");
            duk_dup(ctx, -3);
            duk_call(ctx, 1); /* -> [ ... res stash formatted ] */
            auto error = duk_to_string(ctx, -1);
            warn(error);
        }

    } catch (std::exception& e) {
        warn(e.what());
    }
    return res == 0 ? error_code::no_error : error_code::error;
};



error_code Context::evalModule(const std::string_view& srcPath) {
    duk_context* const ctx = reinterpret_cast<duk_context*>(m_ctx.get());
    std::string src = fmt::format("require('{}')", srcPath);
    auto res = duk_peval_lstring(ctx, src.data(), src.length());

	if (res != 0) {
        duk_push_global_stash(ctx);
        duk_get_prop_string(ctx, -1, "dukFormat");
        duk_dup(ctx, -3);
        duk_call(ctx, 1); /* -> [ ... res stash formatted ] */
        auto error = duk_to_string(ctx, -1);
        warn(error);
    }

    return res == 0 ? error_code::no_error : error_code::error;
};

// template<typename T>
// using ValueType = Scripting::ValueType;

duk_context* as_duk_context(void* ptr) {
    return reinterpret_cast<duk_context*>(ptr);
}

/** CallArgs */
CallArgs::CallArgs(void* ctx, void* callee, int nargs)
    : m_ctx(ctx), m_callee(callee), m_nargs(nargs), m_rval(false) {
}

void* CallArgs::callee() {
    return m_callee;
};

bool CallArgs::has(index_t idx) {
    return idx < m_nargs;
}

bool CallArgs::hasReturnValue() {
    return m_rval;
}

/** VALUE CONVERTIONS */
template<>
std::string_view string_value_converter::get(void* context, index_t idx) {
    auto ctx = as_duk_context(context);
    duk_size_t len;
    const char* ptr = duk_get_lstring(ctx, idx, &len);
    std::string_view value(ptr, len);
    //debug("get std::string_view {} => {}", idx, value);
    return value;
};

template<>
void string_value_converter::put(void* context, const std::string_view& value) {
    auto ctx = as_duk_context(context);
    duk_push_lstring(ctx, value.data(), value.size());

    //debug("put std::string_view <size:{}>", value.size());
};

template<>
int int_value_converter::get(void* context, index_t idx) {
    auto ctx = as_duk_context(context);
    duk_int_t value = duk_get_int(ctx, idx);
    //debug("get int {} => {}", idx, value);
    return value;
};

template<>
void int_value_converter::put(void* context, const int& value) {
    auto ctx = as_duk_context(context);
    duk_push_int(ctx, value);
    //debug("put int {}", value);
};

template<>
double number_value_converter::get(void* context, index_t idx) {
    auto ctx = as_duk_context(context);
    duk_double_t value = duk_get_number(ctx, idx);
    //debug("get int {} => {}", idx, value);
    return value;
};

template<>
void number_value_converter::put(void* context, const double& value) {
    auto ctx = as_duk_context(context);
    duk_push_number(ctx, value);
    //debug("put double {}", value);
    return;
};


template<>
bool boolean_value_converter::get(void* context, index_t idx) {
    auto ctx = as_duk_context(context);
    auto value = duk_get_boolean(ctx, idx);
    //debug("get bool {} => {}", idx, value);
    return value;
};

template<>
void boolean_value_converter::put(void* context, const bool& value) {
    auto ctx = as_duk_context(context);
    duk_push_boolean(ctx, value);
    //debug("put bool {}", value);
};



template <>
Uint32DataView uint32_array_view_value_converter::get(void* context, index_t idx) {
    auto ctx = as_duk_context(context);
	duk_size_t len;
    uint32_t* ptr =
        reinterpret_cast<uint32_t*>(duk_get_buffer_data(ctx, idx, &len));
        len = len / sizeof(uint32_t);
    Uint32DataView value(ptr, len);
    // debug("get std::string_view {} => {}", idx, value);
    return value;
};

template <>
void uint32_array_view_value_converter::put(void* context, const Uint32DataView& value){
    //auto ctx = as_duk_context(context);
    //duk_push_boolean(ctx, value);
    // debug("put bool {}", value);
};

template <>
Uint8DataView uint8_array_view_value_converter::get(void* context, index_t idx) {
    using value_type = Uint8DataView::value_type;
    using pointer = Uint8DataView::pointer;
    
	auto ctx = as_duk_context(context);
    duk_size_t len;
        pointer ptr = reinterpret_cast<pointer>(duk_get_buffer_data(ctx, idx, &len));
    len = len / Uint8DataView::byte_size;
    Uint8DataView value(ptr, len);
    // debug("get std::string_view {} => {}", idx, value);
    return value;
};

template <>
void uint8_array_view_value_converter::put(void* context, const Uint8DataView& value){
    // auto ctx = as_duk_context(context);
    // duk_push_boolean(ctx, value);
    // debug("put bool {}", value);
};