/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "SceneGraphJsonGatewayTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

SceneGraphJsonGatewayTest* g_app = new SceneGraphJsonGatewayTest("Scene graph json gateway Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_SceneGraphJsonGatewayTestApp_SceneGraphJsonGatewayTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_SceneGraphJsonGatewayTestApp_SceneGraphJsonGatewayTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
