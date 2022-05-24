#ifndef _ANDROID_BRIDGE_H
#define _ANDROID_BRIDGE_H

#include "PlatformLayer.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID

#include <android/asset_manager.h>
#include <string>
#include <jni.h>
#include <unordered_map>
#include <vector>

namespace Enigma::Platforms
{
    class AndroidBridge
    {
    public:
        struct JniMethodInfo
        {
            JNIEnv* m_env;
            jclass      m_classID;
            jmethodID   m_methodID;
        };
    public:
        static void SetJavaVM(JavaVM* javaVM);
        static JavaVM* GetJavaVM();
        static JNIEnv* GetEnv();
        static jobject GetActivity();

        static AAssetManager* GetAAssetManager();
        static void SetAAssetManager(AAssetManager* aasset_mgr);

        static const std::string& GetDataFilePath();
        static void SetDataFilePath(const std::string& file_path);

        static std::string StringFromJString(JNIEnv* env, jstring str);

        static bool GetStaticMethodInfo(JniMethodInfo& methodinfo, const char* className,
            const char* methodName, const char* paramCode);

        template <typename... Ts>
        static void CallStaticVoidMethod(const std::string& className,
            const std::string& methodName, Ts... xs)
        {
            JniMethodInfo t;
            std::string signature = "(" + std::string(GetJNISignature(xs...)) + ")V";
            if (GetStaticMethodInfo(t, className.c_str(), methodName.c_str(), signature.c_str()))
            {
                t.m_env->CallStaticVoidMethod(t.m_classID, t.m_methodID, Convert(t, xs)...);
                t.m_env->DeleteLocalRef(t.m_classID);
                DeleteLocalRefs(t.m_env);
            }
            else
            {
                Debug::ErrorPrintf("Failed to find static java method. Class name: %s, method name: %s, signature: %s ",
                    className.c_str(), methodName.c_str(), signature.c_str());
            }
        }
        template <typename... Ts>
        static int CallStaticIntMethod(const std::string& className,
            const std::string& methodName, Ts... xs)
        {
            jint ret = 0;
            JniMethodInfo t;
            std::string signature = "(" + std::string(GetJNISignature(xs...)) + ")I";
            if (GetStaticMethodInfo(t, className.c_str(), methodName.c_str(), signature.c_str()))
            {
                ret = t.m_env->CallStaticIntMethod(t.m_classID, t.m_methodID, Convert(t, xs)...);
                t.m_env->DeleteLocalRef(t.m_classID);
                DeleteLocalRefs(t.m_env);
            }
            else
            {
                Debug::ErrorPrintf("Failed to find static java method. Class name: %s, method name: %s, signature: %s ",
                    className.c_str(), methodName.c_str(), signature.c_str());
            }
            return ret;
        }
    private:
        static JNIEnv* CacheEnv(JavaVM* jvm);
        static std::string GetJNISignature() { return ""; }
        static std::string GetJNISignature(bool) { return "Z"; }
        static std::string GetJNISignature(char) { return "C"; }
        static std::string GetJNISignature(short) { return "S"; }
        static std::string GetJNISignature(int) { return "I"; }
        static std::string GetJNISignature(long) { return "J"; }
        static std::string GetJNISignature(float) { return "F"; }
        static  std::string GetJNISignature(double) { return "D"; }
        static std::string GetJNISignature(const char*) { return "Ljava/lang/String;"; }
        static std::string GetJNISignature(const std::string&) { return "Ljava/lang/String;"; }
        template <typename T>
        static std::string GetJNISignature(T x)
        {
            // This template should never be instantiated
            static_assert(sizeof(x) == 0, "Unsupported argument type");
            return "";
        }
        template <typename T, typename... Ts>
        static std::string GetJNISignature(T x, Ts... xs)
        {
            return GetJNISignature(x) + GetJNISignature(xs...);
        }
        static jstring Convert(JniMethodInfo& t, const char* x);
        static jstring Convert(JniMethodInfo& t, const std::string& x);
        template <typename T>
        static T Convert(JniMethodInfo&, T x) {
            return x;
        }
        static void DeleteLocalRefs(JNIEnv* env);

    private:
        static AAssetManager* m_assetManager;
        static std::string m_dataFilePath;
        static JavaVM* m_javaVM;
        static jobject m_activity;
        static std::unordered_map<JNIEnv*, std::vector<jobject>> m_localRefs;
    };
};
#endif

#endif // !_ANDROID_BEIDGE_H
