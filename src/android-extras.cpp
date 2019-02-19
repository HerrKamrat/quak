#include "android-extras.hpp"

#include <jni.h>
#include <memory>
#include <string>

#include <log.hpp>
#include "lib.hpp"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <jni.h>

#include <physfs.h>
static const uint32_t aaa_ar_header =  '.' | 'A' << 8 | 'A' << 16 | 'A' << 24;
static const size_t aaa_ar_header_length = sizeof(aaa_ar_header);

static jobject android_java_asset_manager = nullptr;
static AAssetManager* android_native_asset_manager = nullptr;

/** archive */
PHYSFS_Io* aaa_ar_unsupportedFileOpenOp(void* opaque, const char* filename);
int aaa_ar_unsupportedOp(void* opaque, const char* filename);
PHYSFS_Io* aaa_ar_openRead(void *opaque, const char *filename);
void* aaa_ar_openArchive(PHYSFS_Io* io, const char* name, int forWrite, int* claimed);
PHYSFS_EnumerateCallbackResult aaa_ar_enumerate(void* opaque,
                                                const char* dirname,
                                                PHYSFS_EnumerateCallback cb,
                                                const char* origdir,
                                                void* callbackdata);
int aaa_ar_stat(void* opaque, const char* fn, PHYSFS_Stat* stat);
void aaa_ar_closeArchive(void* opaque);

/** io */
PHYSFS_sint64 aaa_io_read(struct PHYSFS_Io *io, void *buf, PHYSFS_uint64 len);
int aaa_io_seek(struct PHYSFS_Io *io, PHYSFS_uint64 offset);
PHYSFS_sint64 aaa_io_tell(struct PHYSFS_Io *io);
PHYSFS_sint64 aaa_io_length(struct PHYSFS_Io *io);
struct PHYSFS_Io* aaa_io_duplicate(struct PHYSFS_Io *io);
int aaa_io_flush(struct PHYSFS_Io *io);
void aaa_io_destroy(struct PHYSFS_Io *io);

PHYSFS_Io aaa_io_interface = {
        0,
        nullptr,
        aaa_io_read,
        //write
        nullptr,
        aaa_io_seek,
        aaa_io_tell,
        aaa_io_length,
        aaa_io_duplicate,
        aaa_io_flush,
        aaa_io_destroy
};

/** io -> opaque*/
struct AssetInfo {
    AAsset* asset;
    char* file;
};

/** Archive */
PHYSFS_Io* aaa_ar_unsupportedFileOpenOp(void* opaque, const char* filename) {
    PHYSFS_setErrorCode(PHYSFS_ERR_UNSUPPORTED);
    return nullptr;
}

int aaa_ar_unsupportedOp(void* opaque, const char* filename) {
    PHYSFS_setErrorCode(PHYSFS_ERR_UNSUPPORTED);
    return 0;
}

PHYSFS_Io* aaa_ar_openRead(void *opaque, const char *filename) {
    AAsset* asset = AAssetManager_open(android_native_asset_manager, filename, AASSET_MODE_BUFFER);
    if(asset){
        auto info = new AssetInfo;
        auto io = new PHYSFS_Io;

        mempcpy(io, &aaa_io_interface, sizeof(PHYSFS_Io));
        io->opaque = info;
        info->asset = asset;

        int l = strlen(filename);

        auto dup = new char[l];
        memcpy(dup, filename, l);
        info->file = dup;

        return io;
    }
    return nullptr;
}

void* aaa_ar_openArchive(PHYSFS_Io* io, const char* name, int forWrite, int* claimed) {
    if(io->length(io) == aaa_ar_header_length){
        uint32_t data = 0;
        io->read(io, &data, aaa_ar_header_length);
        if(data == aaa_ar_header){
            *claimed = true;
            if(forWrite){
                PHYSFS_setErrorCode(PHYSFS_ERR_NO_WRITE_DIR);
                return nullptr;
            }

            return reinterpret_cast<void *>(1);
        }
    }
    return nullptr;
}

PHYSFS_EnumerateCallbackResult aaa_ar_enumerate(void* opaque,
                                         const char* dirname,
                                         PHYSFS_EnumerateCallback cb,
                                         const char* origdir,
                                         void* callbackdata) {
    PHYSFS_setErrorCode(PHYSFS_ERR_UNSUPPORTED);
    return PHYSFS_ENUM_ERROR;
}

int aaa_ar_stat(void* opaque, const char* fn, PHYSFS_Stat* stat) {
    AAsset* asset = AAssetManager_open(android_native_asset_manager, fn, AASSET_MODE_UNKNOWN);
    if(asset){

        stat->filetype = PHYSFS_FILETYPE_REGULAR;
        stat->filesize = AAsset_getLength(asset);

        AAsset_close(asset);
        return 1;
    }
    AAssetDir* dir = AAssetManager_openDir(android_native_asset_manager, fn);
    if(dir){

        stat->filetype = PHYSFS_FILETYPE_DIRECTORY;
        stat->filesize = -1;

        AAssetDir_close(dir);
        return 1;
    }
    return 0;
}

void aaa_ar_closeArchive(void* opaque) {
}


/** IO */
PHYSFS_sint64 aaa_io_read(struct PHYSFS_Io *io, void *buf, PHYSFS_uint64 len){
    auto* info = static_cast<AssetInfo *>(io->opaque);
    auto r = AAsset_read(info->asset, buf, len);
    return r;
}

int aaa_io_seek(struct PHYSFS_Io *io, PHYSFS_uint64 offset) {
    auto* info = static_cast<AssetInfo *>(io->opaque);
    auto r = AAsset_seek(info->asset, offset, SEEK_SET);
    return r;
}

PHYSFS_sint64 aaa_io_tell(struct PHYSFS_Io *io){
    auto* info = static_cast<AssetInfo *>(io->opaque);
    auto r0 = AAsset_getLength(info->asset);
    auto r1 = AAsset_getRemainingLength(info->asset);
    auto r = r0 - r1;
    return r;
}

PHYSFS_sint64 aaa_io_length(struct PHYSFS_Io *io){
    auto* info = static_cast<AssetInfo *>(io->opaque);
    auto r = AAsset_getLength(info->asset);
    return r;
}

struct PHYSFS_Io* aaa_io_duplicate(struct PHYSFS_Io *io){
    auto* info = static_cast<AssetInfo *>(io->opaque);
    PHYSFS_Io* r = aaa_ar_openRead(reinterpret_cast<void *>(1), info->file);
    return r;
}

int aaa_io_flush(struct PHYSFS_Io *io){
    return 0;
}

void aaa_io_destroy(struct PHYSFS_Io *io){
    auto* info = static_cast<AssetInfo *>(io->opaque);
    AAsset_close(info->asset);

    delete[] info->file;
    delete info;
}

PHYSFS_Archiver aaa_archiver = {
    0,
    {"AAA", "Faux archiver for androids internal asset folder (read only, non-enumerable)",
     "<TODO-author>", "<TODO-url>", 0},
    aaa_ar_openArchive,
    aaa_ar_enumerate,
    aaa_ar_openRead,
    // openWrite
    aaa_ar_unsupportedFileOpenOp,
    // openAppend
    aaa_ar_unsupportedFileOpenOp,
    // remove
    aaa_ar_unsupportedOp,
    // mkdir,
    aaa_ar_unsupportedOp,
    aaa_ar_stat,
    aaa_ar_closeArchive
};

int aaa_PHYSFS_mountAndroidAssets(const char* mountPoint, int appendToPath){
    PHYSFS_registerArchiver(&aaa_archiver);
    PHYSFS_mountMemory(&aaa_ar_header, sizeof(aaa_ar_header), [](void*) {}, ".AAA", mountPoint, appendToPath);
    return 1;
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_gabrielsulka_quak_MainActivity_stringFromJNI(JNIEnv* env, jobject /* this */) {
    std::string hello = "Hello from C++";
    hello = lib::version();

    info("Java_com_gabrielsulka_quak_MainActivity_stringFromJNI{}", hello);

    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_gabrielsulka_quak_MainActivity_setNativeAssetManger(JNIEnv* env,
                                                             jobject thiz,
                                                             jobject assetManager) {
    info("Java_com_gabrielsulka_quak_MainActivity_setNativeAssetManger{}", "Begin!");
    if (env->IsSameObject(assetManager, android_java_asset_manager)) {
        info("Java_com_gabrielsulka_quak_MainActivity_setNativeAssetManger{}", "END: No change!");
        return;
    }
    if (android_java_asset_manager) {
        info("Java_com_gabrielsulka_quak_MainActivity_setNativeAssetManger{}",
             "Free previous aam.");
        env->DeleteGlobalRef(android_java_asset_manager);
        android_java_asset_manager = nullptr;
        android_native_asset_manager = nullptr;
    }
    if (!assetManager) {
        info("Java_com_gabrielsulka_quak_MainActivity_setNativeAssetManger{}", "END: No new aam!");
        return;
    }
    info("Java_com_gabrielsulka_quak_MainActivity_setNativeAssetManger{}", "END: New aam!");
    android_java_asset_manager = env->NewGlobalRef(assetManager);
    android_native_asset_manager = AAssetManager_fromJava(env, android_java_asset_manager);

    // PHYSFS_Archiver ar = {0};

    // PHYSFS_Io io = {0};
    // PHYSFS_mountIo()

    PHYSFS_init(nullptr);
    aaa_PHYSFS_mountAndroidAssets("/assets", true);
    auto io = aaa_ar_openRead(nullptr, "/assets/compressed.zip");
    int i = PHYSFS_mountIo(io, "/assets/compressed.zip", "/z", 1);

    //auto file = PHYSFS_openRead("/assets/raw/R0.txt");
    auto file = PHYSFS_openRead("/z/z.txt");
    if(!file)
    file = PHYSFS_openRead("/assets/compressed.zip");

    auto len = PHYSFS_fileLength(file);
    char* content = new char[len + 1];
    PHYSFS_readBytes(file, content, len);
    content[len] = 0;
    info("Java_com_gabrielsulka_quak_MainActivity_setNativeAssetManger READ: <{}>", content);

}
