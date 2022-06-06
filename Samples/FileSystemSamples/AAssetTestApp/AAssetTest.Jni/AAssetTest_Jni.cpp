#include "AAssetTest_Jni.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AAssetTest_Jni", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AAssetTest_Jni", __VA_ARGS__))

extern "C" {
	/*這個 trivial 函式會傳回您為其編譯此動態原生程式庫的平台 ABI。*/
	const char * AAssetTest_Jni::getPlatformABI()
	{
	#if defined(__arm__)
	#if defined(__ARM_ARCH_7A__)
	#if defined(__ARM_NEON__)
		#define ABI "armeabi-v7a/NEON"
	#else
		#define ABI "armeabi-v7a"
	#endif
	#else
		#define ABI "armeabi"
	#endif
	#elif defined(__i386__)
		#define ABI "x86"
	#else
		#define ABI "unknown"
	#endif
		LOGI("This dynamic shared library is compiled with ABI: %s", ABI);
		return "This native library is compiled with ABI: %s" ABI ".";
	}

	void AAssetTest_Jni()
	{
	}

	AAssetTest_Jni::AAssetTest_Jni()
	{
	}

	AAssetTest_Jni::~AAssetTest_Jni()
	{
	}
}
