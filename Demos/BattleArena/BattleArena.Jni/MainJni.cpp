/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "../BattleArenaMain.h"

BattleArena::BattleArenaMain* g_app = new BattleArena::BattleArenaMain("battle arena");

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_RobinGames_BattleArena_BattleArenaMain_initBridgeCallback(JNIEnv * env, jobject thiz)
{
    g_app->InitBridgeCallback();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_RobinGames_BattleArena_BattleArenaMain_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
}
