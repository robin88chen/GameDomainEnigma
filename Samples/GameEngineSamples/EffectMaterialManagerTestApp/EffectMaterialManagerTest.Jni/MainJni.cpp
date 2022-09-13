/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "EffectMaterialManagerTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

EffectMaterialManagerTest* g_app = new EffectMaterialManagerTest("Effect Material Manager Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_EffectMaterialManagerTestApp_EffectMaterialManagerTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_EffectMaterialManagerTestApp_EffectMaterialManagerTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
    if (Enigma::FileSystem::FileSystem::Instance())
    {
        Enigma::FileSystem::FileSystem::Instance()->AddMountPath(std::make_shared<Enigma::FileSystem::AndroidMountPath>("",
            EffectMaterialManagerTest::MediaPathName));
    }
}
