/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "CreateDeviceApp_Jni.h"

CreateDeviceApp* g_app = new CreateDeviceApp("Create Device App");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_CreateDeviceApp_CreateDeviceApp_particleprimitivetestNative(JNIEnv * env, jobject thiz)
{
	// Enter code here
}

extern "C"
JNIEXPORT void JNICALL
Java_com_CreateDeviceApp_CreateDeviceApp_initBridgeCallback(JNIEnv * env, jobject thiz)
{
	g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_CreateDeviceApp_CreateDeviceApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
	/*if (Enigma::FileSystem::Instance())
	{
		Enigma::FileSystem::Instance()->AddMountPath(new Enigma::AndroidMountPath("entities/", Enigma::GeometryData::GetDataFilePathID()));
	}*/
}
