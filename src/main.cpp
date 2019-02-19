/*#include <iostream>

#include "lib.hpp"

int main(int argc, char *argv[])
{
    std::cout << lib::version() << std::endl;
    return 0;
}
*/

#include <chrono>
#include <iostream>
#include <thread>

#include "platform.hpp"

#include "file.hpp"
#include "gfx.hpp"
#include "log.hpp"
#include "main.h"
#include "vm.hpp"

void init() {
}

void draw(Canvas& canvas) {
}

void cleanup() {
}

static const char* CMD_FLAG_EVAL = "--eval";
static const char* CMD_FLAG_EVAL_SHORT = "-e";

static const char* CMD_FLAG_DIR = "--dir";
static const char* CMD_FLAG_DIR_SHORT = "-d";

class CmdArgs {
  public:
    CmdArgs(int argc, char** argv) : m_argc(argc), m_argv(argv) {
        char* b = m_argv[0];
        int l = strlen(b);
        char* e = b + l;
        char* c = e;
        while ((--c) > b && *c != '\\') {
        }

        m_cwd.append(b, c);
        m_exec.append(c + 1, e);
    }

    const char* exec() {
        return m_exec.data();
    }

    const char* cwd() {
        return m_cwd.data();
    }

    const char* dir(const char* defaultValue = nullptr) {
        const char* v = get(CMD_FLAG_DIR, CMD_FLAG_DIR_SHORT);
        if (!v && m_argc > 1) {
            v = value(m_argc - 1);
        }
        return v ? v : defaultValue;
    }

    const char* script(const char* defaultValue = nullptr) {
        const char* v = get(CMD_FLAG_EVAL, CMD_FLAG_EVAL_SHORT);
        if (!v && m_argc > 1) {
            v = value(m_argc - 1);
        }
        return v ? v : defaultValue;
    }

    bool has(const char* longFlag, const char* shortFlag) {
        return flagIndexOf(longFlag, shortFlag) > 0;
    }

    const char* get(const char* longFlag, const char* shortFlag) {
        int i = flagIndexOf(longFlag, shortFlag);
        return i <= 0 ? nullptr : value(i + 1);
    }

  protected:
  private:
    bool empty() {
        return m_argc <= 1;
    }
    const char* value(int i) {
        return i >= 0 && i < m_argc && m_argv[i][0] != '-' ? m_argv[i] : nullptr;
    }

    int flagIndexOf(const char* longFlag, const char* shortFlag) {
        int i0 = longFlag == nullptr ? -1 : indexOf(longFlag);
        int i1 = shortFlag == nullptr ? -1 : indexOf(shortFlag);
        return i0 > i1 ? i0 : i1;
    }

    int indexOf(const char* c) {
        for (int i = 1; i < m_argc; i++) {
            const char* a = m_argv[i];
            if (strcmp(a, c) == 0) {
                return i;
            }
        }
        return -1;
    }

    int m_argc;
    char** m_argv;
    std::string m_exec;
    std::string m_cwd;
};

#include <SDL.h>

void fs_resolve(Scripting::CallArgs& args){
    auto fs = reinterpret_cast<FileSystem*>(args.callee());
    std::string_view arg0;
	args.arg(0, arg0);
	auto res = fs->normalize_path(arg0);
	args.rval(res);
};

void fs_read(Scripting::CallArgs& args){
    auto fs = reinterpret_cast<FileSystem*>(args.callee());
    std::string_view arg0;
    args.arg(0, arg0);
    auto res = fs->read(arg0);
	args.rval(res->view());
};

void fs_write(Scripting::CallArgs& args){
    auto fs = reinterpret_cast<FileSystem*>(args.callee());
    std::string_view arg0, arg1;
    args.arg(0, arg0);
    args.arg(0, arg1);
    auto res = fs->write(arg0, arg1);
    args.rval(res);
};

void sys_print(Scripting::CallArgs& args){
	
    std::string_view s;
	args.arg(0, s);
	
	info("[JS] {}", s);
};

#include <duktape.h>

const Scripting::Module module_sys = {"__sys",
                                    nullptr,
                                    {{"print", sys_print, -1}}};
const Scripting::Module module_fs = {
    "fs",
                   nullptr,
                                     {{"resolve", fs_resolve, 1},
                                      {"read", fs_read, 1},
                                      {"write", fs_write, 1}}};

/**
    const duk_function_list_entry my_module_funcs[] = {
    {"tweak", do_tweak, 0},
    {"adjust", do_adjust, 3},
    {"frobnicate", do_frobnicate, DUK_VARARGS},
    {NULL, NULL, 0}};
*/

int main(int argc, char* argv[]) {
    Scripting::CallArgs ca(nullptr, nullptr, 3);

    CmdArgs args(argc, argv);
    info("================================================================================");
    if (args.exec())
        info("exec:   {}", args.exec());
    if (args.cwd())
        info("cwd:    {}", args.cwd());
    if (args.dir())
        info("dir:    {}", args.dir());
    if (args.script())
        info("script: {}", args.script());
    info("================================================================================");

#if Q_PLATFORM == Q_PLATFORM_ANDROID
#endif

	#ifdef Q_DEBUG
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
	#endif  // Q_DEBUG


    try {
        FileSystem fs(args.cwd());
        if (args.dir()) {
            fs.mount(args.dir());
        }

        Scripting::Context ctx;
        ctx.registerModule(module_sys);
        ctx.registerModule(module_fs);

        Scripting::Context::error_code err;

        std::string file = "/js/internal.js";
        err = ctx.evalModule(file);
        if (err == Scripting::Context::error_code::no_error) {
			file = args.script("main");
			file = "/js/" + file; 
			err = ctx.evalModule(file);
        }
        info("script '{}' has exited with the code {}", file, static_cast<int>(err));

        return static_cast<int>(err);

    } catch (const std::exception& ex) {
        error("Error: {}", ex.what());
    }

    return -1;
    // ScriptEngine vm;
    // vm.run();

    // Window window(Window::Settings::simple(320, 240, "MyWin"));
    // window.run(draw);

    using namespace std::chrono_literals;
}

/*extern "C" {
int SDL_main(int argc, char* argv[]){
    return main(argc, argv);
}
}*/
