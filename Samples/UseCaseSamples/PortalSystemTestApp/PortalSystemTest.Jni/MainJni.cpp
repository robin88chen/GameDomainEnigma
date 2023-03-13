/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "PortalSystemTest.h"

PortalSystemTest* g_app = new PortalSystemTest("portal system Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_PortalSystemTestApp_PortalSystemTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_PortalSystemTestApp_PortalSystemTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
