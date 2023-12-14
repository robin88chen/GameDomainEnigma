#include "Platforms/PlatformConfig.h"

#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <jni.h>
#include "ApplicationBridge.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "Platforms/AndroidBridge.h"

extern "C"
{
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_ApplicationBridgeActivity_onBridgeCreate(JNIEnv* env, jobject thiz)
    {
        Enigma::Platforms::Debug::Printf("activity onBridgeCreate");
        // bridge function
        Enigma::Application::ApplicationBridge::onBridgeCreate();
    }

    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_ApplicationBridgeActivity_onBridgeDestroy(JNIEnv* env, jobject thiz)
    {
        Enigma::Platforms::Debug::Printf("activity onBridgeDestroy");
        // bridge function
        Enigma::Application::ApplicationBridge::onBridgeDestroy();
    }

    JNIEXPORT jintArray JNICALL
        Java_lib_bridge_enigma_ApplicationBridgeActivity_getGLContextAttrs(JNIEnv* env, jobject thiz)
    {
        Enigma::Platforms::Debug::Printf("activity getGLContextAttrs");
        const Enigma::Graphics::DeviceRequiredBits& rqb = Enigma::Graphics::IGraphicAPI::instance()->GetDeviceRequiredBits();
        int tmp[6] = { (int)rqb.m_minColorChannelBits, (int)rqb.m_minColorChannelBits, (int)rqb.m_minColorChannelBits,
            (int)rqb.m_minAlphaChannelBits, (int)rqb.m_minDepthBits, (int)rqb.m_minStencilBits };
        // bridge function
        jintArray glContextAttrsJava = env->NewIntArray(6);
        env->SetIntArrayRegion(glContextAttrsJava, 0, 6, tmp);

        return glContextAttrsJava;
    }

    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_ApplicationBridgeRenderer_onBridgeSurfaceCreate(JNIEnv* env, jobject thiz,
            jint jwidth, jint jheight, jintArray jattribs)
    {
        Enigma::Platforms::Debug::Printf("renderer onBridgeSurfaceCreate");
        // bridge function
        jint attribs[6];
        env->GetIntArrayRegion(jattribs, 0, 6, attribs);
        Enigma::Application::ApplicationBridge::onRendererSurfaceCreate(jwidth, jheight, attribs);
    }

    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_ApplicationBridgeRenderer_onBridgeSurfaceChange(JNIEnv* env, jobject thiz,
            jint jwidth, jint jheight)
    {
        Enigma::Platforms::Debug::Printf("renderer onBridgeSurfaceChange");
        // bridge function
        Enigma::Application::ApplicationBridge::onRendererSurfaceSizeChanged(jwidth, jheight);
    }

    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_ApplicationBridgeRenderer_onRendererDrawFrame(JNIEnv* env, jobject thiz)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onRendererDrawFrame();
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureLongPress(JNIEnv* env, jobject thiz, jfloat x, jfloat y)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureLongPress(x, y);
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureShowPress(JNIEnv* env, jobject thiz, jfloat x, jfloat y)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureShowPress(x, y);
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureDoubleTap(JNIEnv* env, jobject thiz, jfloat x, jfloat y)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureDoubleTap(x, y);
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureSingleTap(JNIEnv* env, jobject thiz, jfloat x, jfloat y)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureSingleTap(x, y);
    }

    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureScroll(JNIEnv* env, jobject thiz,
            jfloat startX, jfloat startY, jfloat currentX, jfloat currentY,
            jfloat deltaX, jfloat deltaY)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureScroll(startX, startY, currentX, currentY, deltaX, deltaY);
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureFling(JNIEnv* env, jobject thiz,
            jfloat startX, jfloat startY, jfloat currentX, jfloat currentY,
            jfloat velocityX, jfloat velocityY)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureFling(startX, startY, currentX, currentY, velocityX, velocityY);
    }

    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureScale(JNIEnv* env, jobject thiz,
            jfloat factor, jfloat focusX, jfloat focusY)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureScale(factor, focusX, focusY);
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureScaleBegin(JNIEnv* env, jobject thiz,
            jfloat spanX, jfloat spanY, jfloat focusX, jfloat focusY)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureScaleBegin(spanX, spanY, focusX, focusY);
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureScaleEnd(JNIEnv* env, jobject thiz,
            jfloat spanX, jfloat spanY, jfloat focusX, jfloat focusY)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureScaleEnd(spanX, spanY, focusX, focusY);
    }

    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureShove(JNIEnv* env, jobject thiz,
            jfloat focusX, jfloat focusY, jfloat deltaX, jfloat deltaY)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureShove(focusX, focusY, deltaX, deltaY);
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureShoveBegin(JNIEnv* env, jobject thiz,
            jfloat focusX, jfloat focusY)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureShoveBegin(focusX, focusY);
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeGLSurfaceView_nativeGestureShoveEnd(JNIEnv* env, jobject thiz,
            jfloat focusX, jfloat focusY)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onGestureShoveEnd(focusX, focusY);
    }

    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeEditBoxHelper_editBoxEditingDidBegin(JNIEnv* env, jclass,
            jint index)
    {
        // bridge function
        Enigma::Application::ApplicationBridge::onTextEditingBegin(index);
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeEditBoxHelper_editBoxEditingChanged(JNIEnv* env, jclass,
            jint index, jstring jtext)
    {
        std::string text = Enigma::Platforms::AndroidBridge::stringFromJString(env, jtext);
        // bridge function
        Enigma::Application::ApplicationBridge::onTextEditingChanged(index, text);
    }
    JNIEXPORT void JNICALL
        Java_lib_bridge_enigma_BridgeEditBoxHelper_editBoxEditingDidEnd(JNIEnv* env, jclass,
            jint index, jstring jtext)
    {
        std::string text = Enigma::Platforms::AndroidBridge::stringFromJString(env, jtext);
        // bridge function
        Enigma::Application::ApplicationBridge::onTextEditingEnd(index, text);
    }
}

#endif