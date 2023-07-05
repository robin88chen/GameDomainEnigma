/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "TerrainPrimitiveTest.h"

TerrainPrimitiveTest* g_app = new TerrainPrimitiveTest("terrain primitive Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_TerrainPrimitiveTestApp_TerrainPrimitiveTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_TerrainPrimitiveTestApp_TerrainPrimitiveTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
