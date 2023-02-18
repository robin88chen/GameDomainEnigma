/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "SceneGraphPawnTest.h"

SceneGraphPawnTest* g_app = new SceneGraphPawnTest("scene graph pawn Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_SceneGraphPawnTestApp_SceneGraphPawnTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_SceneGraphPawnTestApp_SceneGraphPawnTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
