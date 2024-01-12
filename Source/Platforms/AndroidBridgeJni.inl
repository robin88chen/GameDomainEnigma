#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include "AndroidBridge.h"

extern "C"
{
    JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
    {
        AndroidBridge::setJavaVM(vm);

        return JNI_VERSION_1_4;
    }

    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeActivity_initAssetManager(JNIEnv* env, jobject thiz, jobject assetManager)
    {
        AAssetManager* assetMgr = AAssetManager_fromJava(env, assetManager);
        if (assetMgr == 0)
        {
            __android_log_print(ANDROID_LOG_ERROR, "EnigmaAndroidBridge", "error get asset manager from java");
        }
        else
        {
            __android_log_print(ANDROID_LOG_VERBOSE, "EnigmaAndroidBridge", "get asset  manager from java");
        }
        AndroidBridge::setAAssetManager(assetMgr);
    }

    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeActivity_initDataFilePath(JNIEnv* env, jobject thiz, jstring files_dir)
    {
        jboolean isCopy;
        const char* native_files_dir = env->GetStringUTFChars(files_dir, &isCopy);
        if (native_files_dir == 0)
        {
            __android_log_print(ANDROID_LOG_ERROR, "EnigmaAndroidBridge", "native files dir null");
        }
        std::string path_dir(native_files_dir);
        env->ReleaseStringUTFChars(files_dir, native_files_dir);
        AndroidBridge::setDataFilePath(path_dir);
    }
}

#endif
