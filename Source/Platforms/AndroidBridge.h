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
        static void setJavaVM(JavaVM* javaVM);
        static JavaVM* getJavaVM();
        static JNIEnv* getEnv();
        static jobject getActivity();

        static AAssetManager* getAAssetManager();
        static void setAAssetManager(AAssetManager* aasset_mgr);

        static const std::string& getDataFilePath();
        static void setDataFilePath(const std::string& file_path);

        static std::string stringFromJString(JNIEnv* env, jstring str);

        static bool getStaticMethodInfo(JniMethodInfo& methodinfo, const char* className,
            const char* methodName, const char* paramCode);

        template <typename... Ts>
        static void callStaticVoidMethod(const std::string& className,
            const std::string& methodName, Ts... xs)
        {
            JniMethodInfo t;
            std::string signature = "(" + std::string(getJNISignature(xs...)) + ")V";
            if (getStaticMethodInfo(t, className.c_str(), methodName.c_str(), signature.c_str()))
            {
                t.m_env->CallStaticVoidMethod(t.m_classID, t.m_methodID, Convert(t, xs)...);
                t.m_env->DeleteLocalRef(t.m_classID);
                deleteLocalRefs(t.m_env);
            }
            else
            {
                Debug::ErrorPrintf("Failed to find static java method. Class name: %s, method name: %s, signature: %s ",
                    className.c_str(), methodName.c_str(), signature.c_str());
            }
        }
        template <typename... Ts>
        static int callStaticIntMethod(const std::string& className,
            const std::string& methodName, Ts... xs)
        {
            jint ret = 0;
            JniMethodInfo t;
            std::string signature = "(" + std::string(getJNISignature(xs...)) + ")I";
            if (getStaticMethodInfo(t, className.c_str(), methodName.c_str(), signature.c_str()))
            {
                ret = t.m_env->CallStaticIntMethod(t.m_classID, t.m_methodID, Convert(t, xs)...);
                t.m_env->DeleteLocalRef(t.m_classID);
                deleteLocalRefs(t.m_env);
            }
            else
            {
                Debug::ErrorPrintf("Failed to find static java method. Class name: %s, method name: %s, signature: %s ",
                    className.c_str(), methodName.c_str(), signature.c_str());
            }
            return ret;
        }
    private:
        static JNIEnv* cacheEnv(JavaVM* jvm);
        static std::string getJNISignature() { return ""; }
        static std::string getJNISignature(bool) { return "Z"; }
        static std::string getJNISignature(char) { return "C"; }
        static std::string getJNISignature(short) { return "S"; }
        static std::string getJNISignature(int) { return "I"; }
        static std::string getJNISignature(long) { return "J"; }
        static std::string getJNISignature(float) { return "F"; }
        static  std::string getJNISignature(double) { return "D"; }
        static std::string getJNISignature(const char*) { return "Ljava/lang/String;"; }
        static std::string getJNISignature(const std::string&) { return "Ljava/lang/String;"; }
        template <typename T>
        static std::string getJNISignature(T x)
        {
            // This template should never be instantiated
            static_assert(sizeof(x) == 0, "Unsupported argument type");
            return "";
        }
        template <typename T, typename... Ts>
        static std::string getJNISignature(T x, Ts... xs)
        {
            return getJNISignature(x) + getJNISignature(xs...);
        }
        static jstring convert(JniMethodInfo& t, const char* x);
        static jstring convert(JniMethodInfo& t, const std::string& x);
        template <typename T>
        static T convert(JniMethodInfo&, T x) {
            return x;
        }
        static void deleteLocalRefs(JNIEnv* env);

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
