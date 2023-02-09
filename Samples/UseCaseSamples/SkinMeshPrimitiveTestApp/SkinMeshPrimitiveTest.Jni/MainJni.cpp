/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "SkinMeshPrimitiveTest.h"

SkinMeshPrimitiveTest* g_app = new SkinMeshPrimitiveTest("skin mesh Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_SkinMeshPrimitiveTestApp_SkinMeshPrimitiveTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_SkinMeshPrimitiveTestApp_SkinMeshPrimitiveTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
