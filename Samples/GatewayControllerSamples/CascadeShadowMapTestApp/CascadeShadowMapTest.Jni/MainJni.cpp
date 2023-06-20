/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "CascadeShadowMapTest.h"

CascadeShadowMapTest* g_app = new CascadeShadowMapTest("cascade shadow map Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_CascadeShadowMapTestApp_CascadeShadowMapTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_CascadeShadowMapTestApp_CascadeShadowMapTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
