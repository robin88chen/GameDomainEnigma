/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "RenderManagerTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

RenderManagerTest* g_app = new RenderManagerTest("Render Manager Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_RenderManagerTestApp_RenderManagerTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_RenderManagerTestApp_RenderManagerTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
