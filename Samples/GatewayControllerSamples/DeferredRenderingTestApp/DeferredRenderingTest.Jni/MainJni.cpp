/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "DeferredRenderingTest.h"

DeferredRenderingTest* g_app = new DeferredRenderingTest("deferred rendering Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_DeferredRenderingTestApp_DeferredRenderingTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_DeferredRenderingTestApp_DeferredRenderingTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
