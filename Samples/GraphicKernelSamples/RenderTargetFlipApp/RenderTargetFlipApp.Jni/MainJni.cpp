/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "RenderTargetFlipApp_Jni.h"

RenderTargetFlipApp* g_app = new RenderTargetFlipApp("Render Target Flip App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_RenderTargetFlipApp_RenderTargetFlipApp_particleprimitivetestNative(JNIEnv * env, jobject thiz)
{
    // Enter code here
}

extern "C"
JNIEXPORT void JNICALL
Java_com_RenderTargetFlipApp_RenderTargetFlipApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_RenderTargetFlipApp_RenderTargetFlipApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
    /*if (Enigma::FileSystem::Instance())
    {
        Enigma::FileSystem::Instance()->AddMountPath(new Enigma::AndroidMountPath("entities/", Enigma::GeometryData::GetDataFilePathID()));
    }*/
}
