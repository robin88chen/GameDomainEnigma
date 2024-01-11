/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "ModelPrimitiveTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

ModelPrimitiveTest* g_app = new ModelPrimitiveTest("model primitive Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_ModelPrimitiveTestApp_ModelPrimitiveTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->initBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ModelPrimitiveTestApp_ModelPrimitiveTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
