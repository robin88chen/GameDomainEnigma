/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "ShadowMapTest.h"

ShadowMapTest* g_app = new ShadowMapTest("shadow map Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_ShadowMapTestApp_ShadowMapTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ShadowMapTestApp_ShadowMapTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
