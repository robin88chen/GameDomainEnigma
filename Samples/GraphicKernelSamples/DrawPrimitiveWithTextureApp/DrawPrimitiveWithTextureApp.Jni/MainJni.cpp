/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "DrawPrimitiveWithTextureApp_Jni.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/AndroidMountPath.h"

DrawPrimitiveWithTextureApp* g_app = new DrawPrimitiveWithTextureApp("Draw Primitive with texture App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_DrawPrimitiveWithTextureApp_DrawPrimitiveWithTextureApp_particleprimitivetestNative(JNIEnv * env, jobject thiz)
{
    // Enter code here
}

extern "C"
JNIEXPORT void JNICALL
Java_com_DrawPrimitiveWithTextureApp_DrawPrimitiveWithTextureApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_DrawPrimitiveWithTextureApp_DrawPrimitiveWithTextureApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
    if (Enigma::FileSystem::FileSystem::Instance())
    {
        Enigma::FileSystem::FileSystem::Instance()->AddMountPath(Enigma::FileSystem::IMountPathPtr{ new Enigma::FileSystem::AndroidMountPath("",
            DrawPrimitiveWithTextureApp::AndroidAssetPathName)});
    }
}
