#include "platform.hpp"

#if Q_PLATFORM == Q_PLATFORM_ANDROID

#include <android/log.h>

#elif Q_PLATFORM == Q_PLATFORM_WINDOWS

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif

/*
void runtime_assert(bool expression, const char* message) {
	if (!expression) {
		platform::print("assertion failed: ");
		platform::print(message);
		bx::debugBreak();
		throw std::logic_error(message);
	}
};
*/

namespace platform {

	void print(const char* str) {
#if Q_PLATFORM == Q_PLATFORM_ANDROID
		__android_log_print(ANDROID_LOG_INFO,"quak","%s",str);
#elif Q_PLATFORM == Q_PLATFORM_WINDOWS
		OutputDebugString(str);
#endif
	}
}
