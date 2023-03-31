/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "GameCommonTest.h"

GameCommonTest* g_app = new GameCommonTest("game common Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_GameCommonTestApp_GameCommonTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_GameCommonTestApp_GameCommonTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
