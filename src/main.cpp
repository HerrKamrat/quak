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

#include <SDL.h>
#include <stb_image.h>

#include "cmd.hpp"
#include "data_view.hpp"
#include "platform.hpp"
#include "file.hpp"
#include "gfx.hpp"
#include "log.hpp"
#include "vm.hpp"

#include "image.hpp"

#if Q_PLATFORM == Q_PLATFORM_ANDROID
#include "android-extras.hpp"
#endif

void noop(Scripting::CallArgs& args) {
}

/** FILE SYSTEM */
void fs_resolve(Scripting::CallArgs& args) {
    auto fs = reinterpret_cast<FileSystem*>(args.callee());
    std::string_view arg0;
    args.arg(0, arg0);
    auto res = fs->normalize_path(arg0);
    args.rval(res);
};

void fs_read(Scripting::CallArgs& args) {
    auto fs = reinterpret_cast<FileSystem*>(args.callee());
    std::string_view arg0;
    args.arg(0, arg0);
    auto res = fs->read(arg0);
    args.rval(res->view());
};

void fs_write(Scripting::CallArgs& args) {
    auto fs = reinterpret_cast<FileSystem*>(args.callee());
    std::string_view arg0, arg1;
    args.arg(0, arg0);
    args.arg(0, arg1);
    auto res = fs->write(arg0, arg1);
    args.rval(res);
};

/** SYSTEM */
void sys_print(Scripting::CallArgs& args) {
    std::string_view s;
    args.arg(0, s);

    info("[JS] {}", s);
};

void sys_test(Scripting::CallArgs& args) {
    Uint32DataView s{};
    args.arg(0, s);

    const_cast<uint32_t*>(s.data())[0] = 123;
};


template<class T1, class T2>
constexpr DataView<T1> view_cast(DataView<T2> t) {
    auto ptr = reinterpret_cast<T1*>(t.data());
    auto len = (t.size() * t.byte_size) / DataView<T1>::byte_size;
    
	return {ptr, len};
} 

/** GFX / WINDOW */
void gfx_poll(Scripting::CallArgs& args) {
    auto gfx = reinterpret_cast<Window*>(args.callee());

    Uint8DataView data{};
    bool wait;
    args.arg(0, data);
    args.arg(1, wait);
    auto e = view_cast<Event>(data);

    auto res = gfx->poll(e, wait);
    args.rval(res);
    // Scripting::uint32_array_view s;
    // args.arg(0, s);

    // const_cast<uint32_t*>(s.data())[0] = 123;
};

void gfx_commit(Scripting::CallArgs& args) {
    auto gfx = reinterpret_cast<Window*>(args.callee());

    Uint8DataView data{};

	bool present;
    args.arg(0, data);
    args.arg(1, present);

    //auto c = reinterpret_cast<Window::DrawCommand*>(data.data());
    //auto s = (data.size() * data.byte_size) / sizeof(Window::DrawCommand);

	auto commands = view_cast<Window::DrawCommand>(data);

	for (int i = 0; i < data.size(); ++i) {
        info("[cpp] cmds {}", data[i]);
    }

	gfx->commit(commands, present);
};

const Scripting::Module module_sys = {"__sys", {{"print", sys_print, -1}, {"test", sys_test, -1}}};

const Scripting::Module module_fs = {
    "fs",
    {{"resolve", fs_resolve, 1}, {"read", fs_read, 1}, {"write", fs_write, 1}}};

const Scripting::Module module_gfx = {"__gfx", {{"poll", gfx_poll, 2}, {"commit", gfx_commit, 2}}};

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

   /* try {
        FileSystem fs(args.cwd());
        if (args.dir()) {
            fs.mount(args.dir());
        }

        Window window(Window::Settings::simple(320, 240, "MyWin"));

        Scripting::Context ctx;
        ctx.registerModule(module_sys);
        ctx.registerModule(module_fs, &fs);
        ctx.registerModule(module_gfx, &window);

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
*/
    // ScriptEngine vm;
    // vm.run();
    /*
     */

    {
        FileSystem fs(args.cwd());
#if Q_PLATFORM == Q_PLATFORM_ANDROID
        aaa_PHYSFS_mountAndroidAssets("/", true);
#endif
        if (args.dir()) {
            fs.mount(args.dir());
        }

        Window window(Window::Settings::simple(320, 240, "MyWin"));

        std::string p0 = "/tex/Blue/characterBlue (3).png";
        std::string p1 = "/tex/Green/characterGreen (3).png";

        auto d0 = fs.read(p0);
        auto d1 = fs.read(p1);

        auto i0 = ImageReader::read(*d0);
        auto i1 = ImageReader::read(*d1);

#ifdef ASAPI
        AssetManager mgr;
        mgr.fs = fs;
        mgr.gfx = window;

        auto id1 = mgr.loadTexture("path.png");
        mgr.reloadTexture(id1);
        if (mgr.isLoaded("other.png")) {
            auto id2 = mgr.get("other.png");
        }
#endif

        /*
        std::vector<Window::DrawCommand> cmds;

        auto drawRect = [&cmds](int x, int y, int w, int h) {
            Window::DrawCommand cmd;
            cmd.drawRect = {Window::DrawCommandType::DrawRect, {}, x, y, w, h};
            cmds.push_back({cmd});
        };

        auto setColor = [&cmds](int c) {
            Window::DrawCommand cmd;
            cmd.setColor = {Window::DrawCommandType::SetColor, {}, c};
            cmds.push_back({cmd});
        };
        auto setTexture = [&cmds](Texture t) {
            Window::DrawCommand cmd;
            cmd.setTexture = {Window::DrawCommandType::SetTexture, {}, t.key};
            cmds.push_back({cmd});
        };

		auto t0 = window.createTexture(i0);
        auto t1 = window.createTexture(i1);

        // window.createTexture();


		for (int i = 0; i < 10; i++) {
            drawRect(i * 10, i * 10, 10, 10);
        }

        setTexture(t0);
        drawRect(10, 0, 20, 20);
        setTexture(t1);
        drawRect(40, 40, 20, 20);

		//cmds.push_back({20, 20, 10, 10, (int)0xff00ffff});


        int flag = 0;
         Event events[4];
          window.commit(cmds);
         while ((flag = window.poll(events, 4))) {
            if (flag == 2)
                window.commit(cmds);
            if (events[0].type == Type::KeyboardInputEvent &&
                    events[0].keyboard.key == SDLK_ESCAPE) {
                break;
            }
         }
         */

		Scripting::Context ctx;
        ctx.registerModule(module_sys);
        ctx.registerModule(module_fs, &fs);
        ctx.registerModule(module_gfx, &window);

        Scripting::Context::error_code err;

        std::string file = "/js/internal.js";
        err = ctx.evalModule(file);
        if (err == Scripting::Context::error_code::no_error) {
            file = args.script("main");
            file = "/js/" + file;
            err = ctx.evalModule(file);
        }
        info("script '{}' has exited with the code {}", file, static_cast<int>(err));
    }

	/**
     */
    using namespace std::chrono_literals;

    return -1;
}
