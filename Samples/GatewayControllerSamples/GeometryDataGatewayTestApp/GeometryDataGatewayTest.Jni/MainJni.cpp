/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "GeometryDataGatewayTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

GeometryDataGatewayTest* g_app = new GeometryDataGatewayTest("geometry data json gateway Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_GeometryDataGatewayTestApp_GeometryDataGatewayTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_GeometryDataGatewayTestApp_GeometryDataGatewayTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
