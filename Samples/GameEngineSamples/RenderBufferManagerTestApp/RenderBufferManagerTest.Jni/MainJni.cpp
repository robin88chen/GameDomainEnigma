/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "RenderBufferManagerTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

RenderBufferManagerTest* g_app = new RenderBufferManagerTest("Render Buffer Manager Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_RenderBufferManagerTestApp_RenderBufferManagerTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_RenderBufferManagerTestApp_RenderBufferManagerTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
