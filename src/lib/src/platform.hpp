#ifndef PLATFORM_HPP
#define PLATFORM_HPP

// https://abseil.io/docs/cpp/platforms/macros

#define Q_PLATFORM_ANDROID 1
#define Q_PLATFORM_APPLE 2
#define Q_PLATFORM_LINUX 3
#define Q_PLATFORM_WINDOWS 4

#if defined __ANDROID__
#define Q_PLATFORM Q_PLATFORM_ANDROID
#elif defined __APPLE__
#error "Err!"
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