#pragma once

#include <functional>
#include <memory>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "image.hpp"
#include "event.hpp"

struct Id {
    uint16_t index;
    uint16_t check;
};
inline std::ostream& operator<<(std::ostream& os, const Id& id) {
    return os << fmt::format("Id={}x{}", id.index, id.check);
}

struct Texture {
    Id key;
    int width;
    int height;
};

class Canvas {
  public:
    Canvas& rect();

  protected:
  private:
};

class Context;

class Window {
  public:
    using AnimationFrameCallback = std::function<void(Canvas&)>;
    enum class Flags {
        Fullscreen = 1 << 0,
        Borderless = 1 << 1,
        Default = 0,
    };

    struct Settings {
        int x;
        int y;
        int width;
        int height;
        Flags flags;
        std::string title;

        static inline Settings simple(int w, int h, const std::string& title) {
            return {-1, -1, w, h, Flags::Default, title};
        }
    };

    enum class DrawCommandType : uint8_t { 
		Noop = 0,
		Clear,
		DrawRect, 
		SetColor, 
		SetTexture 
	};

	struct CommonCommand {
        DrawCommandType type;
        uint8_t _[3 + 4 * 4];
    };

    struct DrawRectCommand {
        DrawCommandType type;
        uint8_t _[3];

        int32_t x;
        int32_t y;
        int32_t w;
        int32_t h;
    };

    struct SetColorCommand {
        DrawCommandType type;
        uint8_t _[3 + 4 * 3];

		int32_t color;
    };

    struct SetTextureCommand {
        DrawCommandType type;
        uint8_t _[3 + 4 * 3];

		Id texture;
    };

    union DrawCommand {
        DrawCommandType type;
        CommonCommand clear;
        DrawRectCommand drawRect;
        SetColorCommand setColor;
        SetTextureCommand setTexture;
        uint8_t _[4 * 5];
    };

	static_assert(sizeof(DrawCommand) == sizeof(int32_t) * 5);

    static_assert(sizeof(DrawCommand) == sizeof(CommonCommand));
        static_assert(sizeof(DrawCommand) == sizeof(DrawRectCommand));
    static_assert(sizeof(DrawCommand) == sizeof(SetColorCommand));
        static_assert(sizeof(DrawCommand) == sizeof(SetTextureCommand));

	static_assert(offsetof(DrawRectCommand, type) == 0);
    static_assert(offsetof(DrawRectCommand, x) == 4);
    static_assert(offsetof(DrawRectCommand, y) == 8);


		

    Window(const Settings& settings);
    ~Window();

    Texture createTexture(const Image& img);

    int poll(DataView<Event> events, bool wait = true) {
        return poll(events.data(), events.size(), wait);
	};
    int poll(Event* events, size_t length = 1, bool wait = true);

    void commit(DataView<DrawCommand> commands, bool present = true);
    void commit(const std::vector<DrawCommand>& commands, bool present = true);
    void commit(const DrawCommand* commands, int length, bool present = true);

    int run(AnimationFrameCallback&& callback = nullptr);

  protected:
  private:
    std::unique_ptr<Context> m_context;
};