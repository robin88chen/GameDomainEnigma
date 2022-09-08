/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "EffectMaterialTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

EffectMaterialTest* g_app = new EffectMaterialTest("Effect Material Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_EffectMaterialTestApp_EffectMaterialTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_EffectMaterialTestApp_EffectMaterialTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
    if (Enigma::FileSystem::FileSystem::Instance())
    {
        Enigma::FileSystem::FileSystem::Instance()->AddMountPath(std::make_shared<Enigma::FileSystem::AndroidMountPath>("",
            EffectMaterialTest::MediaPathName));
    }
}
