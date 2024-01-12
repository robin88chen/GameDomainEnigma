/*********************************************************************
 * \file   ApplicationBridge.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef _APPLICATION_BRIDGE_H
#define _APPLICATION_BRIDGE_H

#include "Platforms/PlatformConfig.h"
#include "InputHandlers/InputHandlerService.h"
#include <memory>

#if TARGET_PLATFORM == PLATFORM_ANDROID
#include <functional>

namespace  Enigma::Application
{
    class ApplicationBridge
    {
    public:
        static void initInputHandler(const std::shared_ptr<InputHandlers::InputHandlerService>& input_handler);
        /// on Bridge Activity Create Callback, 先呼叫 m_onBridgeCreate, 再 m_initializeGraphicDevice, 再 m_afterDeviceCreated
        static void onBridgeCreate();
        /// on Bridge Activity Destroy Callback, 先呼叫 m_beforeDeviceCleaning, 再 m_finalizeGraphicDevice, 再 m_onBridgeDestroy
        static void onBridgeDestroy();
        /// on Bridge Surface Create Callback, 設定 API後, 呼叫 m_onRendererSurfaceCreate
        static void onRendererSurfaceCreate(int width, int height, int* attribs);
        /// on Bridge Surface Size Changed, 設定 API後, 呼叫 m_onRendererSurfaceSizeChanged
        static void onRendererSurfaceSizeChanged(int width, int height);
        /// on Renderer Draw Frame, 呼叫 m_onFrameUpdate, 再 m_onDrawFrame
        static void onRendererDrawFrame();

        static void onGestureLongPress(float x, float y);
        static void onGestureShowPress(float x, float y);
        static void onGestureDoubleTap(float x, float y);
        static void onGestureSingleTap(float x, float y);
        static void onGestureScroll(float startX, float startY, float currentX, float currentY,
            float deltaX, float deltaY);
        static void onGestureFling(float startX, float startY, float currentX, float currentY,
            float velocityX, float velocityY);
        static void onGestureScale(float factor, float focusX, float focusY);
        static void onGestureScaleBegin(float spanX, float spanY, float focusX, float focusY);
        static void onGestureScaleEnd(float spanX, float spanY, float focusX, float focusY);
        static void onGestureShove(float focusX, float focusY, float deltaX, float deltaY);
        static void onGestureShoveBegin(float focusX, float focusY);
        static void onGestureShoveEnd(float focusX, float focusY);

        static void onTextEditingBegin(int index);
        static void onTextEditingChanged(int index, const std::string& text);
        static void onTextEditingEnd(int index, const std::string& text);

    public:
        /** @name call back functors
        the functors those can be installed by app
        @code
            Enigma::GraphicAPIBridge::m_onBridgeCreate = []() { a_callback_function(); };
        @endcode */
        //@{
        /// on Bridge Activity Create Callback assigned by app, default null
        static std::function<void()> m_onBridgeCreate;
        /// on Bridge Activity Destroy Callback assigned by app, default null
        static std::function<void()> m_onBridgeDestroy;
        /// after Device Created Callback assigned by app, default null
        static std::function<void()> m_afterDeviceCreated;
        /// before Device Cleaning Callback assigned by app, default null
        static std::function<void()> m_beforeDeviceCleaning;
        /// on Bridge Surface Create Callback assigned by app, default null
        static std::function<void(int, int, int*)> m_onRendererSurfaceCreate;
        /// on Bridge Surface Size Changed Callback assigned by app, default null
        static std::function<void(int, int)> m_onRendererSurfaceSizeChanged;
        /// on Bridge Renderer Frame Update Callback assigned by app, default null.
        static std::function<void()> m_onFrameUpdate;
        /// on Bridge Renderer Prepare Frame Callback assigned by app, default null
        static std::function<void()> m_onPrepareFrame;
        /// on Bridge Renderer Draw Frame Callback assigned by app, default null
        static std::function<void()> m_onDrawFrame;
        //@}
        /** @name replaceable functors
        the functors those can be replced by app. Graphic API module has its default implement
        @code
        Enigma::GraphicAPIBridge::m_initializeGraphicDevice = []() { app_assigned_function(); };
        @endcode */
        //@{
        /// init graphic device,
        static std::function<void()> m_initializeGraphicDevice;
        /// init graphic device,
        static std::function<void()> m_finalizeGraphicDevice;
        //@}

    private:
        static std::weak_ptr<InputHandlers::InputHandlerService> m_input;
    };
}
#endif
#endif