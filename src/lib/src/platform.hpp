#ifndef PLATFORM_HPP
#define PLATFORM_HPP

// https://abseil.io/docs/cpp/platforms/macros

#define Q_PLATFORM_ANDROID 1
#define Q_PLATFORM_IOS 2
#define Q_PLATFORM_LINUX 3
#define Q_PLATFORM_MACOS 4
#define Q_PLATFORM_WINDOWS 5

#if defined __ANDROID__
#define Q_PLATFORM Q_PLATFORM_ANDROID
#elif defined __APPLE__
    #include <TargetConditionals.h>
    #if defined TARGET_OS_MAC
    #define Q_PLATFORM Q_PLATFORM_MACOS
    #elif
    #define Q_PLATFORM Q_PLATFORM_IOS
    #endif//#error "Err!"
#elif defined __linux__
#error "Err!"
#elif defined _WIN32
#define Q_PLATFORM Q_PLATFORM_WINDOWS
#endif

#ifndef Q_DEBUG
#ifndef NDEBUG
#define Q_DEBUG
#endif
#endif  // !Q_DEBUG

namespace platform {
void print(const char* str);
}

#endif  // PLATFORM_HPP