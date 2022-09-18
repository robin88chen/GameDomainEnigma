/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "EffectPassTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

EffectPassTest* g_app = new EffectPassTest("Effect Pass Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_EffectPassTestApp_EffectPassTestApp_particleprimitivetestNative(JNIEnv * env, jobject thiz)
{
    // Enter code here
}

extern "C"
JNIEXPORT void JNICALL
Java_com_EffectPassTestApp_EffectPassTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_EffectPassTestApp_EffectPassTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
    if (Enigma::FileSystem::FileSystem::Instance())
    {
        Enigma::FileSystem::FileSystem::Instance()->AddMountPath(std::make_shared<Enigma::FileSystem::AndroidMountPath>("",
            EffectPassTest::MediaPathName));
    }
}
