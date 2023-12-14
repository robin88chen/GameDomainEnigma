#include "AndroidBridge.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <android/log.h>
#include <pthread.h>

using namespace Enigma::Platforms;

static pthread_key_t g_vm_thread_key;

void _detachCurrentThread(void* a)
{
    AndroidBridge::getJavaVM()->DetachCurrentThread();
}

AAssetManager* AndroidBridge::m_assetManager = nullptr;
std::string AndroidBridge::m_dataFilePath;
JavaVM* AndroidBridge::m_javaVM = nullptr;
jobject AndroidBridge::m_activity = nullptr;
std::unordered_map<JNIEnv*, std::vector<jobject>> AndroidBridge::m_localRefs;

JavaVM* AndroidBridge::getJavaVM()
{
    pthread_t thisthread = pthread_self();
    Debug::Printf("GetJavaVM(), pthread_self() = %ld", thisthread);
    return m_javaVM;
}

void AndroidBridge::setJavaVM(JavaVM* javaVM)
{
    pthread_t thisthread = pthread_self();
    Debug::Printf("SetJavaVM(%p), pthread_self() = %ld", javaVM, thisthread);
    m_javaVM = javaVM;

    pthread_key_create(&g_vm_thread_key, _detachCurrentThread);
}

JNIEnv* AndroidBridge::cacheEnv(JavaVM* jvm)
{
    JNIEnv* _env = nullptr;
    // get jni environment
    jint ret = jvm->GetEnv((void**)&_env, JNI_VERSION_1_4);

    switch (ret)
    {
    case JNI_OK:
        // Success!
        pthread_setspecific(g_vm_thread_key, _env);
        return _env;

    case JNI_EDETACHED:
        // Thread not attached
        if (jvm->AttachCurrentThread(&_env, nullptr) < 0)
        {
            Debug::ErrorPrintf("Failed to get the environment using AttachCurrentThread()");
            return nullptr;
        }
        else
        {
            // Success : Attached and obtained JNIEnv!
            pthread_setspecific(g_vm_thread_key, _env);
            return _env;
        }

    case JNI_EVERSION:
        // Cannot recover from this error
        Debug::ErrorPrintf("JNI interface version 1.4 not supported");
    default:
        Debug::ErrorPrintf("Failed to get the environment using GetEnv()");
        return nullptr;
    }
}

JNIEnv* AndroidBridge::getEnv()
{
    JNIEnv* _env = (JNIEnv*)pthread_getspecific(g_vm_thread_key);
    if (_env == nullptr)
        _env = AndroidBridge::cacheEnv(m_javaVM);
    return _env;
}

jobject AndroidBridge::getActivity()
{
    return m_activity;
}

AAssetManager* AndroidBridge::getAAssetManager()
{
    return m_assetManager;
}

void AndroidBridge::setAAssetManager(AAssetManager* aasset_mgr)
{
    m_assetManager = aasset_mgr;
}

const std::string& AndroidBridge::getDataFilePath()
{
    return m_dataFilePath;
}

void AndroidBridge::setDataFilePath(const std::string& file_path)
{
    m_dataFilePath = file_path;
}

std::string AndroidBridge::stringFromJString(JNIEnv* env, jstring str)
{
    jboolean isCopy;
    const char* native_str = env->GetStringUTFChars(str, &isCopy);
    if (native_str == 0)
    {
        return "String from jstring null";
    }
    std::string ret_str(native_str);
    env->ReleaseStringUTFChars(str, native_str);
    return ret_str;
}

bool AndroidBridge::getStaticMethodInfo(JniMethodInfo& methodinfo,
    const char* className, const char* methodName, const char* paramCode)
{
    if ((nullptr == className) || (nullptr == methodName) ||
        (nullptr == paramCode))
    {
        return false;
    }

    JNIEnv* env = getEnv();
    if (!env)
    {
        Debug::ErrorPrintf("Failed to get JNIEnv");
        return false;
    }

    jclass classID = env->FindClass(className);
    if (!classID) {
        Debug::ErrorPrintf("Failed to find class %s", className);
        env->ExceptionClear();
        return false;
    }

    jmethodID methodID = env->GetStaticMethodID(classID, methodName, paramCode);
    if (!methodID) {
        Debug::ErrorPrintf("Failed to find static method id of %s", methodName);
        env->ExceptionClear();
        return false;
    }

    methodinfo.m_classID = classID;
    methodinfo.m_env = env;
    methodinfo.m_methodID = methodID;
    return true;
}

jstring AndroidBridge::convert(JniMethodInfo& t, const char* x)
{
    jstring ret = t.m_env->NewStringUTF(x ? x : "");
    m_localRefs[t.m_env].push_back(ret);
    return ret;
}

jstring AndroidBridge::convert(JniMethodInfo& t, const std::string& x)
{
    return convert(t, x.c_str());
}

void AndroidBridge::deleteLocalRefs(JNIEnv* env)
{
    if (!env)
    {
        return;
    }

    for (const auto& ref : m_localRefs[env])
    {
        env->DeleteLocalRef(ref);
    }
    m_localRefs[env].clear();
}

#include "AndroidBridgeJni.inl"

#endif