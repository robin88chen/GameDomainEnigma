/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "VisibilityManagedTest.h"

VisibilityManagedTest* g_app = new VisibilityManagedTest("visibility managed Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_VisibilityManagedTestApp_VisibilityManagedTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_VisibilityManagedTestApp_VisibilityManagedTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
