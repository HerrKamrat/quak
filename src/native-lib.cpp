#include <jni.h>
#include <string>

#include "lib.hpp"

extern "C" JNIEXPORT jstring JNICALL
Java_com_gabrielsulka_quak_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    std::string hello = "Hello from C++";
    hello = lib::version();

    return env->NewStringUTF(hello.c_str());
}
