/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "MeshPrimitiveTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

MeshPrimitiveTest* g_app = new MeshPrimitiveTest("mesh primitive Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_MeshPrimitiveTestApp_MeshPrimitiveTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_MeshPrimitiveTestApp_MeshPrimitiveTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
