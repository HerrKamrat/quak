#include "gfx.hpp"

#include <iostream>

#include <SDL.h>

#include "file.hpp"
#include "log.hpp"

struct Color {
    uint8_t r, g, b, a;
};

using Rect = SDL_Rect;

inline std::runtime_error sdl_error(const char* msg) {
    auto err = SDL_GetError();
    auto full = fmt::format("{}. Error: {}", msg, err);
    return std::runtime_error(msg);
}

class Context {
    class TextureObject {
      public:
        Id key;
        SDL_Texture* ptr;
        SDL_Rect bounds;

        operator bool() const {
            return ptr != nullptr;
        }
    };

    std::vector<TextureObject> m_textures;

  public:
    Context(const Window::Settings settings)
        : m_window(nullptr), m_renderer(nullptr), m_currentColor({255}), m_currentTexture({0}) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
            throw sdl_error("Failed to initialize sdl");
        }

        const char* title = settings.title.data();
        m_window = SDL_CreateWindow(
            title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, settings.width,
            settings.height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

        if (!m_window) {
            throw sdl_error("Failed to create window");
        }

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
        if (!m_renderer) {
            throw sdl_error("Failed to create renderer");
        }

        SDL_GetRendererOutputSize(m_renderer, &m_bounds.w, &m_bounds.h);
    }

    ~Context() {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    void present() {
        int s = 5;
        SDL_Rect rects[4] = {
            {0, 0, m_bounds.w, s},
            {0, m_bounds.h - s, m_bounds.w, s},

            {0, s, s, m_bounds.h - 2 * s},
            {m_bounds.w - s, s, s, m_bounds.h - 2 * s},

        };

        if (m_frameCounter % 2 == 0) {
            SDL_SetRenderDrawColor(m_renderer, 0xff, 0, 0, 0);
            SDL_RenderFillRects(m_renderer, rects, 4);
            SDL_SetRenderDrawColor(m_renderer, m_currentColor.r, m_currentColor.g, m_currentColor.b,
                                   m_currentColor.a);
        }
        SDL_RenderPresent(m_renderer);

        m_frameCounter++;
    }

    void resize(int w, int h) {
        info("resize: {}, {}", w, h);
        if (SDL_GetRendererOutputSize(m_renderer, &m_bounds.w, &m_bounds.h) != 0) {
            throw sdl_error("Failed to get output size");
        }
        // m_bounds.w = w;
    }

    void draw(const Texture& texture,
              const Rect& rect,
              uint8_t r,
              uint8_t g,
              uint8_t b,
              uint8_t a) {
        auto obj = m_textures[texture.key.index];
        SDL_SetTextureColorMod(obj.ptr, r, g, b);
        SDL_SetTextureAlphaMod(obj.ptr, a);
        SDL_RenderCopyEx(m_renderer, obj.ptr, &obj.bounds, &rect, 0, nullptr, SDL_FLIP_NONE);
        const int i = SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL;
    }

    void draw(const Rect& rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
        SDL_RenderFillRects(m_renderer, &rect, 1);
        /*SDL_SetTextureColorMod(obj.ptr, r, g, b);
        SDL_SetTextureAlphaMod(obj.ptr, a);
        SDL_RenderCopyEx(m_renderer, obj.ptr, &obj.bounds, &rect, 0, nullptr, SDL_FLIP_NONE);
        const int i = SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL;*/
    }

    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h) {
        SDL_Rect rect{x, y, w, h};
        if (m_currentTexture.ptr) {
            SDL_RenderCopyEx(m_renderer, m_currentTexture.ptr, &m_currentTexture.bounds, &rect, 0,
                             nullptr, SDL_FLIP_NONE);
        } else {
            SDL_RenderFillRects(m_renderer, &rect, 1);
        }
    }

    void setColor(int32_t color) {
        m_currentColor.r = 0xff & (color >> 8);
        m_currentColor.g = 0xff & (color >> 16);
        m_currentColor.b = 0xff & (color >> 24);
        m_currentColor.a = 0xff & (color >> 0);

        SDL_SetRenderDrawColor(m_renderer, m_currentColor.r, m_currentColor.g, m_currentColor.b,
                               m_currentColor.a);

        if (m_currentTexture) {
            SDL_SetTextureColorMod(m_currentTexture.ptr, m_currentColor.r, m_currentColor.g,
                                   m_currentColor.b);
            SDL_SetTextureAlphaMod(m_currentTexture.ptr, m_currentColor.a);
        }
    }

    void setTexture(Id texture) {
        auto obj = m_textures.at(texture.index);
        if (obj.key.check != texture.check) {
            warn("Wrong check sum for texture!");
        }
        m_currentTexture = obj;

        if (m_currentTexture) {
            SDL_SetTextureColorMod(m_currentTexture.ptr, m_currentColor.r, m_currentColor.g,
                                   m_currentColor.b);
            SDL_SetTextureAlphaMod(m_currentTexture.ptr, m_currentColor.a);
        }
    }

    Texture createTexture(const Image& image) {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        auto texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA32,
                                         SDL_TEXTUREACCESS_STATIC, image.width(), image.height());
        if (texture == NULL) {
            error("Context::createTexture: {}", SDL_GetError());
            return {{0, 0}, 0, 0};
        }

        SDL_Rect rect = {0, 0, image.width(), image.height()};
        SDL_UpdateTexture(texture, &rect, image.data(), image.stride());

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        Id key = {0, 0};
        key.index = m_textures.size();

        TextureObject obj = {key, texture, rect};
        m_textures.push_back(obj);

        Texture tex = {key, rect.w, rect.h};
        info("Context::createTexture {}", key);
        return tex;
    }

    void clear() {
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
        SDL_RenderClear(m_renderer);

        m_currentColor = {255, 255, 255, 255};
        SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 25);
        m_currentTexture = {0};
    }

    // protected:
    // private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Rect m_bounds;

    uint64_t m_frameCounter;

    SDL_Color m_currentColor;
    TextureObject m_currentTexture;
};

struct DrawCommand {
    // SIMPLE SORTING FIRST!
    /* union {
         uint64_t key;
         struct {*/
    uint16_t z;
    uint16_t texture;
    uint32_t color;
    /*     };
     };*/

    Rect geometry;
};

// FileSystem fs;

std::unique_ptr<Buffer> loadImage(FileSystem& fs, const std::string& path, int& w, int& h) {
    /* auto file = fs.read(path);
     info("file: {} -> {}", path, file->size());

     int c;
     auto src = reinterpret_cast<const unsigned char*>(file->data());
     auto dst = stbi_load_from_memory(src, file->size(), &w, &h, &c, 4);
     std::unique_ptr<Buffer> b = std::make_unique<Buffer>();

     b->reset((char*)dst, w * h * 4, w * h * 4, [](Buffer::pointer data) { stbi_image_free(data);
     }); return std::move(b);*/
    return {};
}

void test() {
    // fs.mount("E:/Projects/quack/assets/kenney_animalpackredux.zip");
    // fs.mount("E:/Projects/quack/assets/pixeluipack.zip");

    Context gfx(Window::Settings::simple(240, 240, ":D"));

    SDL_Rect rect = {26, 26, 0, 0};
    SDL_Rect rect2 = {0, 0, 480, 640};

    // auto file = fs.read("/PNG/Square without details (outline)/duck.png");
    // info("file: {}", file->size());

    /*int c;
    auto src = reinterpret_cast<const unsigned char*>(file->data());
    auto dst = stbi_load_from_memory(src, file->size(), &rect.w, &rect.h, &c, 4);
    info("image: {} x {} ({})", rect.w, rect.h, c);
    delete[] dst;
        */
    Texture texture;
    {
        // auto data = loadImage(fs, "/PNG/Square without details (outline)/duck.png", rect.w,
        // rect.h);
        // texture = gfx.createTexture(rect.w, rect.h, *data);
    }

    bool quit = false;
    SDL_Event e;
    double r = 0;
    SDL_Point p = {16, 16};
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            info("event type: {}", e.type);
            if (e.type == SDL_QUIT || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)) {
                quit = true;
            }

            else if (e.type == SDL_WINDOWEVENT) {
                info("window event type: {}", e.type);
                if (e.window.event == SDL_WINDOWEVENT_RESIZED ||
                    e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    gfx.resize(e.window.data1, e.window.data2);
                }
            }
            // SDL_FillRect(screenSurface, NULL,
            // SDL_MapRGB(screenSurface->format, 0xFF, 0x80, 0x00));
            // SDL_UpdateWindowSurface(window);
            // SDL_Delay(16);
            SDL_SetRenderDrawColor(gfx.m_renderer, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderClear(gfx.m_renderer);

            gfx.draw(texture, rect2, 0xFF, 0x80, 0xFF, 0xff);
            gfx.draw(texture, rect, 0x80, 0x80, 0x80, 0x80);

            gfx.present();
            /*
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x80, 0xFF, 0xFF);
            SDL_RenderClear(renderer);

            SDL_Rect dst = {32, 32, rect.w, rect.h};
            // SDL_RenderCopy(renderer, texture, &rect, &dst);

            r += 0.5;
            SDL_RenderCopyEx(renderer, texture, &rect, &dst, r, nullptr, SDL_FLIP_NONE);
            SDL_RenderPresent(renderer);
                        */
        }
    }
}

Canvas& Canvas::rect() {
    return *this;
};

Window::~Window() = default;

Window::Window(const Settings& settings) {
    m_context.reset(new Context(settings));
};

Texture Window::createTexture(const Image& img) {
    return m_context->createTexture(img);
};

void Window::commit(DataView<DrawCommand> cmds, bool present) {
    commit(cmds.data(), cmds.size(), present);
};

void Window::commit(const std::vector<DrawCommand>& cmds, bool present) {
    commit(cmds.data(), cmds.size(), present);
}

void Window::commit(const DrawCommand* commands, int length, bool present){
    SDL_Rect rect;
    uint8_t r, g, b, a;


    for (int i = 0; i < length; i++) {
        const auto& cmd = commands[i];
        switch (cmd.type) {
            case DrawCommandType::Clear:
                m_context->clear();
                break;
            case DrawCommandType::DrawRect:
                m_context->drawRect(cmd.drawRect.x, cmd.drawRect.y, cmd.drawRect.w, cmd.drawRect.h);
                continue;
            case DrawCommandType::SetColor:
                m_context->setColor(cmd.setColor.color);
                continue;
            case DrawCommandType::SetTexture:
                m_context->setTexture(cmd.setTexture.texture);
                continue;
        }
        /* rect = {cmd.x, cmd.y, cmd.w, cmd.h};
         r = 0xff & (cmd.c >> 8);
         g = 0xff & (cmd.c >> 16);
         b = 0xff & (cmd.c >> 24);
         a = 0xff & (cmd.c >> 0);
         m_context->draw(rect, r, g, b, a);*/
    }

    if (present) {
	    m_context->present();
    }
};

int FLAG_EXIT = 1;
int FLAG_DIRTY = 2;
int MAX = FLAG_EXIT | FLAG_DIRTY;


int Window::poll(Event* events, size_t length, bool wait) {
    int ret = 0;

    //Event* out = reinterpret_cast<Event*>(events.data());
    //size_t outLength = events.size() / sizeof(Event);

    Event ev;
    SDL_Event e;

	if (wait) {
	    SDL_WaitEvent(nullptr);
    }

	int i = 0;
    while (SDL_PollEvent(&e) != 0) {
        // info("event #{} type: {}", c, e.type);
        // TODO: REMOVE THIS
        if (e.type == SDL_QUIT || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)) {
            ret = 1;
            continue;
        } else if (e.type == SDL_WINDOWEVENT) {
            // info("window event type: {}", e.type);
            if (  // e.window.event == SDL_WINDOWEVENT_RESIZED ||
                e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                m_context->resize(e.window.data1, e.window.data2);

                ev.window = createWindowEventResize(e.window.data1, e.window.data2);

                ret = 2;
            } else {
                continue;
			}
        } else if (e.type == SDL_MOUSEBUTTONUP || e.type == SDL_MOUSEBUTTONUP) {
            ev.mouse = createMouseInputEvent(e.button.button, e.button.state, e.button.clicks,
                                             e.button.x, e.button.y);
        } else if (e.type == SDL_MOUSEMOTION) {
            ev.motion =
                createMouseMotionEvent(e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
        } else if (e.type == SDL_MOUSEWHEEL) {
            ev.mouse = e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED
                           ? createMouseWheelInputEvent(-e.wheel.x, -e.wheel.y)
                           : createMouseWheelInputEvent(e.wheel.x, e.wheel.y);
        } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            ev.keyboard = createKeyboardInputEvent(e.key.keysym.sym, e.key.state, e.key.keysym.mod,
                                                   e.key.repeat);
        } else {
            continue;
		}

        if (i < length) {
            events[i] = ev;
            ++i;
        } else {
            break;
        }
    }
    return ret;
};

int Window::run(AnimationFrameCallback&& callback) {
    if (!callback) {
        callback = [](Canvas&) {};
    }

    Canvas canvas;

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            info("event type: {}", e.type);
            if (e.type == SDL_QUIT || (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)) {
                quit = true;
            }

            else if (e.type == SDL_WINDOWEVENT) {
                info("window event type: {}", e.type);
                if (e.window.event == SDL_WINDOWEVENT_RESIZED ||
                    e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    m_context->resize(e.window.data1, e.window.data2);
                }
            }
            m_context->clear();
            callback(canvas);
            m_context->present();
        }
    }

    return 0;
};
