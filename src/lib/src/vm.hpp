#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "buffer.hpp"
#include "data_view.hpp"

namespace Scripting {

class script_error : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};


    using index_t = int;

    template <class T>
    struct value_converter {
        static T get(void* ctx, index_t i);
        static void put(void* ctx, const T& t);
    };

    using string_value_converter = value_converter<std::string_view>;
    using int_value_converter = value_converter<int>;
    using number_value_converter = value_converter<double>;
    using boolean_value_converter = value_converter<bool>;

	
	using uint8_array_view_value_converter = value_converter<Uint8DataView>;
    using uint32_array_view_value_converter = value_converter<Uint32DataView>;


    template <class T>
    struct make_convertable {
        using type = T;
    };

    template <>
    struct make_convertable<std::string> {
        using type = std::string_view;
    };

    template <class T>
    using make_convertable_t = typename make_convertable<T>::type;

    class CallArgs {
    public:
        CallArgs(void* ctx, void* callee, int nargs);

        void* callee();

        bool has(index_t idx);

        bool hasReturnValue();

        template <class T>
        void arg(index_t idx, T& t);

        template <class T>
        void rval(const T& t);

    protected:
    private:
        void* m_ctx;
        void* m_callee;
        int m_nargs;
        bool m_rval;
    };
using ModuleFunction = void (*)(CallArgs& callArgs);

struct ModuleFunctionEntry {
    const char* key;
    ModuleFunction func;
    int nargs;
};

struct Module {
    const char* key;
    std::vector<ModuleFunctionEntry> value;
};

class Context {
  public:
    enum class error_code { no_error = 0, error = 1 };

    Context();

    void registerModule(const Module&, void* obj = nullptr);
    error_code eval(const std::string_view& src, const std::string_view& name = {});
    error_code evalModule(const std::string_view& srcPath);
    std::unique_ptr<void, void (*)(void*)> m_ctx;
};

template <class T>
void CallArgs::arg(index_t idx, T& t) {
    if (!has(idx)) {
        throw script_error("Argument index out of bounds");
    }
    t = value_converter<T>::get(m_ctx, idx);
};

template <class T>
void CallArgs::rval(const T& t) {
    if (m_rval) {
        throw script_error("Return value already set");
    }
    using C = make_convertable_t<T>;
    value_converter<C>::put(m_ctx, t);
    m_rval = true;
};

};  // namespace Scripting
