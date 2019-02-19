#pragma once

#include <functional>
#include <memory>
#include <string>

/*class Canvas {
  public:
    Canvas();
    ~Canvas();
  protected:
  private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

class Canvas::Impl {

};

Canvas::Canvas():_impl(nullptr) {
}

Canvas::~Canvas() = default;
*/

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

    Window(const Settings& settings);
    ~Window();

    int run(AnimationFrameCallback&& callback = nullptr);

  protected:
  private:
    std::unique_ptr<Context> m_context;
};