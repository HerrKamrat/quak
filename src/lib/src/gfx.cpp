#include "gfx.hpp"

#include <iostream>

#include <SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "file.hpp"
#include "log.hpp"

struct Id {
    uint16_t index;
    uint16_t check;
};

struct Color {
    uint8_t r, g, b, a;
};

struct Texture {
    Id key;
    int width;
    int height;
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
    };

    std::vector<TextureObject> m_textures;

  public:
    Context(const Window::Settings settings) : m_window(nullptr), m_renderer(nullptr) {
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

    Texture createTexture(int w, int h, const Buffer& buffer) {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        auto texture =
            SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, w, h);
        if (texture == NULL) {
            fprintf(stderr, "could not create texture: %s\n", SDL_GetError());
            return {{0, 0}, 0, 0};
        }
        SDL_Rect rect = {0, 0, w, h};
        SDL_UpdateTexture(texture, &rect, buffer.data(), rect.w * 4);

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        Id key = {0, 0};
        key.index = m_textures.size();

        TextureObject obj = {key, texture, rect};
        m_textures.push_back(obj);
        Texture tex = {key, rect.w, rect.h};
        return tex;
    }

    void clear() {
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
        SDL_RenderClear(m_renderer);
    }

    // protected:
    // private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Rect m_bounds;

    uint64_t m_frameCounter;
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

FileSystem fs;

std::unique_ptr<Buffer> loadImage(FileSystem& fs, const std::string& path, int& w, int& h) {
    auto file = fs.read(path);
    info("file: {} -> {}", path, file->size());

    int c;
    auto src = reinterpret_cast<const unsigned char*>(file->data());
    auto dst = stbi_load_from_memory(src, file->size(), &w, &h, &c, 4);
    std::unique_ptr<Buffer> b = std::make_unique<Buffer>();

    b->reset((char*)dst, w * h * 4, w * h * 4, [](Buffer::pointer data) { stbi_image_free(data); });
    return std::move(b);
}

void test() {
    fs.mount("E:/Projects/quack/assets/kenney_animalpackredux.zip");
    fs.mount("E:/Projects/quack/assets/pixeluipack.zip");

    Context gfx(Window::Settings::simple(240, 240, ":D"));

    SDL_Rect rect = {26, 26, 0, 0};
    SDL_Rect rect2 = {0, 0, 480, 640};

    auto file = fs.read("/PNG/Square without details (outline)/duck.png");
    info("file: {}", file->size());

    /*int c;
    auto src = reinterpret_cast<const unsigned char*>(file->data());
    auto dst = stbi_load_from_memory(src, file->size(), &rect.w, &rect.h, &c, 4);
    info("image: {} x {} ({})", rect.w, rect.h, c);
    delete[] dst;
        */
    Texture texture;
    {
        auto data = loadImage(fs, "/PNG/Square without details (outline)/duck.png", rect.w, rect.h);
        texture = gfx.createTexture(rect.w, rect.h, *data);
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
