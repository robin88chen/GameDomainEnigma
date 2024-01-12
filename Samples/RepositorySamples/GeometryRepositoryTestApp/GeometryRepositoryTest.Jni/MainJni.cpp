/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "GeometryRepositoryTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

GeometryRepositoryTest* g_app = new GeometryRepositoryTest("geometry repository Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_GeometryRepositoryTestApp_GeometryRepositoryTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_GeometryRepositoryTestApp_GeometryRepositoryTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
