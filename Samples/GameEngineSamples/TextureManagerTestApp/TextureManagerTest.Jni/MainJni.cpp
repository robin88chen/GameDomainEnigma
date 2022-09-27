/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "TextureManagerTest.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

TextureManagerTest* g_app = new TextureManagerTest("Texture Manager Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_TextureManagerTestApp_TextureManagerTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_TextureManagerTestApp_TextureManagerTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
