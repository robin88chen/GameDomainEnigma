/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "DrawPrimitiveTestApp_Jni.h"

DrawPrimitiveTestApp* g_app = new DrawPrimitiveTestApp("Draw Primitive Test App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_DrawPrimitiveTestApp_DrawPrimitiveTestApp_particleprimitivetestNative(JNIEnv * env, jobject thiz)
{
    // Enter code here
}

extern "C"
JNIEXPORT void JNICALL
Java_com_DrawPrimitiveTestApp_DrawPrimitiveTestApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_DrawPrimitiveTestApp_DrawPrimitiveTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
    /*if (Enigma::FileSystem::Instance())
    {
        Enigma::FileSystem::Instance()->AddMountPath(new Enigma::AndroidMountPath("entities/", Enigma::GeometryData::GetDataFilePathID()));
    }*/
}
