#pragma once

#include <SDL.h>

enum class Type : uint8_t { WindowEvent = 1, MouseInputEvent, MouseMotionEvent, KeyboardInputEvent };

struct CommonEvent {
    Type type;
    uint8_t subtype;
};

struct WindowEvent {
    enum class Event : uint8_t {
        Reset = 1,
        Resize,
        MouseFocus,
        KeyboardFocus,

    };

    Type type;
    Event event;

    /*uint8_t _0;
uint8_t _1;

    uint8_t _2;
uint8_t _3;*/
    uint8_t _[4];

    int32_t x;
    int32_t y;
};

inline WindowEvent createWindowEventResize(int x, int y) {
    return {Type::WindowEvent, WindowEvent::Event::Resize, {0}, x, y};
}

struct MouseInputEvent {
    enum class Event : uint8_t { ButtonDown = 1, ButtonUp, Wheel };

    Type type;
    Event event;

    uint8_t button;
    uint8_t state;

    uint8_t clicks;
    uint8_t _[3];

    int32_t x;
    int32_t y;
};

inline MouseInputEvent createMouseInputEvent(uint8_t button,
                                                     uint8_t state,
                                                     uint8_t clicks,
                                                     int32_t x,
                                                     int32_t y) {
    MouseInputEvent e = {
        Type::MouseInputEvent,
        state ? MouseInputEvent::Event::ButtonDown : MouseInputEvent::Event::ButtonUp,
                         button,
                         state,
                         clicks,
                         {0},
                         x,
                         y};

    return e;
}

inline MouseInputEvent createMouseWheelInputEvent(int32_t x, int32_t y) {
    MouseInputEvent e = {
        Type::MouseInputEvent, MouseInputEvent::Event::Wheel, 0, 0, 0, {0}, x, y};

    return e;
};


struct MouseMotionEvent {
    enum class Event : uint8_t { Motion = 1 };

    Type type;
    Event event;

    int32_t x;
    int32_t y;
 
	int16_t dx;
    int16_t dy;

    uint8_t _[0];
};

inline MouseMotionEvent createMouseMotionEvent(int32_t x, int32_t y, int16_t dx, int16_t dy) {
    MouseMotionEvent e = {
        Type::MouseMotionEvent,
        MouseMotionEvent::Event::Motion,
        x,y,dx,dy,
        {}
	};

    return e;
}



struct KeyboardInputEvent {
    enum class Event : uint8_t { KeyDown = 1, KeyUp, KeyRepeat };

    Type type;
    Event event;

    uint16_t key;
    uint16_t mod;

    uint8_t repeat;
    uint8_t _[9];
};

inline KeyboardInputEvent createKeyboardInputEvent(int32_t key,
                                                   uint8_t state,
                                                   uint16_t mod,
                                                   uint8_t repeat) {
    KeyboardInputEvent e = {
        Type::KeyboardInputEvent,
        repeat ? KeyboardInputEvent::Event::KeyRepeat
               : state ? KeyboardInputEvent::Event::KeyDown : KeyboardInputEvent::Event::KeyUp,
        mod,
        repeat,
        {0}};
    return e;
}

// static_assert(sizeof(WindowEvent) == 12);
static_assert(sizeof(WindowEvent) == sizeof(int32_t) * 4);

static_assert(sizeof(WindowEvent) == sizeof(MouseInputEvent));
static_assert(sizeof(WindowEvent) == sizeof(MouseMotionEvent));
static_assert(sizeof(WindowEvent) == sizeof(KeyboardInputEvent));

union Event {
    //{
    Type type;
    WindowEvent window;
    MouseInputEvent mouse;
    MouseMotionEvent motion;
    KeyboardInputEvent keyboard;
    //}
};
