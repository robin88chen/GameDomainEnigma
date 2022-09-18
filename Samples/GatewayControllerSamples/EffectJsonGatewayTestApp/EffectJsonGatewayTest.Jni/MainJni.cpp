/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "EffectJsonGatewayTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

EffectJsonGatewayTest* g_app = new EffectJsonGatewayTest("Effect Json Gateway Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_EffectJsonGatewayTestApp_EffectJsonGatewayTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_EffectJsonGatewayTestApp_EffectJsonGatewayTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
