#include "ApplicationBridge.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/PlatformConfig.h"
#include "FileSystem/FileSystem.h"
#include "InputHandlers/InputHandlerService.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "AppDelegateAndroid.h"
#include "GraphicAPIEgl/GraphicAPIEgl.h"

using namespace Enigma::Platforms;
using namespace Enigma::FileSystem;
using namespace Enigma::Devices;
using namespace Enigma::Application;
using namespace Enigma::InputHandlers;

std::function<void()> ApplicationBridge::m_onBridgeCreate = nullptr;
std::function<void()> ApplicationBridge::m_onBridgeDestroy = nullptr;
std::function<void()> ApplicationBridge::m_afterDeviceCreated = nullptr;
std::function<void()> ApplicationBridge::m_beforeDeviceCleaning = nullptr;
std::function<void(int, int, int*)> ApplicationBridge::m_onRendererSurfaceCreate = nullptr;
std::function<void(int, int)> ApplicationBridge::m_onRendererSurfaceSizeChanged = nullptr;
std::function<void()> ApplicationBridge::m_onFrameUpdate = nullptr;
std::function<void()> ApplicationBridge::m_onDrawFrame = nullptr;
std::function<void()> ApplicationBridge::m_onPrepareFrame = nullptr;

std::function<void()> ApplicationBridge::m_initializeGraphicDevice = nullptr;
std::function<void()> ApplicationBridge::m_finalizeGraphicDevice = nullptr;

std::weak_ptr<InputHandlerService> ApplicationBridge::m_input;

void ApplicationBridge::InitInputHandler(const std::shared_ptr<InputHandlers::InputHandlerService>& input_handler)
{
    m_input = input_handler;
}

void ApplicationBridge::OnBridgeCreate()
{
    if (m_onBridgeCreate)
    {
        m_onBridgeCreate();
    }
    m_initializeGraphicDevice();
    if (m_afterDeviceCreated)
    {
        m_afterDeviceCreated();
    }
}

void ApplicationBridge::OnBridgeDestroy()
{
    if (m_beforeDeviceCleaning)
    {
        m_beforeDeviceCleaning();
    }
    m_finalizeGraphicDevice();
    if (m_onBridgeDestroy)
    {
        m_onBridgeDestroy();
    }
}

void ApplicationBridge::OnRendererSurfaceCreate(int width, int height, int* attribs)
{
    GraphicAPIEgl* graphic = dynamic_cast<GraphicAPIEgl*>(Graphics::IGraphicAPI::Instance());
    if (!graphic)
    {
        Debug::ErrorPrintf("GraphicAPI EGL cast fail");
        return;
    }
    graphic->SetFormat(attribs);
    graphic->SetDimension(MathLib::Dimension<unsigned>{ (unsigned int)width, (unsigned int)height });
    if (m_onRendererSurfaceCreate)
    {
        m_onRendererSurfaceCreate(width, height, attribs);
    }
}

void ApplicationBridge::OnRendererSurfaceSizeChanged(int width, int height)
{
    GraphicAPIEgl* graphic = dynamic_cast<GraphicAPIEgl*>(Graphics::IGraphicAPI::Instance());
    if (!graphic)
    {
        Debug::ErrorPrintf("GraphicAPI EGL cast fail");
        return;
    }
    graphic->SetDimension(MathLib::Dimension<unsigned>{ (unsigned int)width, (unsigned int)height });
    if (m_onRendererSurfaceSizeChanged)
    {
        m_onRendererSurfaceSizeChanged(width, height);
    }
}

void ApplicationBridge::OnRendererDrawFrame()
{
    if (AppDelegate::Instance())
    {
        AppDelegate::Instance()->FrameUpdate();
    }
    if (m_onFrameUpdate)
    {
        m_onFrameUpdate();
    }
    if (m_onPrepareFrame)
    {
        m_onPrepareFrame();
    }
    if (m_onDrawFrame)
    {
        m_onDrawFrame();
    }
}

void ApplicationBridge::OnGestureLongPress(float x, float y)
{
    Debug::Printf("Long Press @ (%f, %f)", x, y);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureLongPress(x, y);
    }
}

void ApplicationBridge::OnGestureShowPress(float x, float y)
{
    Debug::Printf("Show Press @ (%f, %f)", x, y);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureShowPress(x, y);
    }
}

void ApplicationBridge::OnGestureDoubleTap(float x, float y)
{
    Debug::Printf("Double Tap @ (%f, %f)", x, y);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureDoubleTap(x, y);
    }
}

void ApplicationBridge::OnGestureSingleTap(float x, float y)
{
    Debug::Printf("Single Tap @ (%f, %f)", x, y);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureSingleTap(x, y);
    }
}

void ApplicationBridge::OnGestureScroll(float startX, float startY, float currentX, float currentY, float deltaX, float deltaY)
{
    Debug::Printf("Gesture Scroll from (%f, %f) to (%f, %f) with (%f, %f)",
        startX, startY, currentX, currentY, deltaX, deltaY);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureScroll(startX, startY, currentX, currentY, deltaX, deltaY);
    }
}

void ApplicationBridge::OnGestureFling(float startX, float startY, float currentX, float currentY, float velocityX, float velocityY)
{
    Debug::Printf("Gesture Fling from (%f, %f) to (%f, %f) with (%f, %f)",
        startX, startY, currentX, currentY, velocityX, velocityY);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureFling(startX, startY, currentX, currentY, velocityX, velocityY);
    }
}

void ApplicationBridge::OnGestureScale(float factor, float focusX, float focusY)
{
    Debug::Printf("Gesture Scale %f @ (%f, %f)", factor, focusX, focusY);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureScale(factor, focusX, focusY);
    }
}

void ApplicationBridge::OnGestureScaleBegin(float spanX, float spanY, float focusX, float focusY)
{
    Debug::Printf("Gesture Scale Begin (%f, %f) @ (%f, %f)", spanX, spanY, focusX, focusY);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureScaleBegin(spanX, spanY, focusX, focusY);
    }
}

void ApplicationBridge::OnGestureScaleEnd(float spanX, float spanY, float focusX, float focusY)
{
    Debug::Printf("Gesture Scale End (%f, %f) @ (%f, %f)", spanX, spanY, focusX, focusY);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureScaleEnd(spanX, spanY, focusX, focusY);
    }
}

void ApplicationBridge::OnGestureShove(float focusX, float focusY, float deltaX, float deltaY)
{
    // shove 的 threashold 參考 scroll
    Debug::Printf("Gesture Shove @ (%f, %f) with (%f, %f)", focusX, focusY, deltaX, deltaY);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureShove(focusX, focusY, deltaX, deltaY);
    }
}

void ApplicationBridge::OnGestureShoveBegin(float focusX, float focusY)
{
    Debug::Printf("Gesture Shove Begin @ (%f, %f)", focusX, focusY);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureShoveBegin(focusX, focusY);
    }
}

void ApplicationBridge::OnGestureShoveEnd(float focusX, float focusY)
{
    Debug::Printf("Gesture Shove End @ (%f, %f)", focusX, focusY);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessGestureShoveEnd(focusX, focusY);
    }
}

void ApplicationBridge::OnTextEditingBegin(int index)
{
    Debug::Printf("Text Editing Begin %d", index);
    if (!m_input.expired())
    {
        m_input.lock()->ProcessTextEditingBegin(index);
    }
}

void ApplicationBridge::OnTextEditingChanged(int index, const std::string& text)
{
    Debug::Printf("Text Editing Changed %d, %s", index, text.c_str());
    if (!m_input.expired())
    {
        m_input.lock()->ProcessTextEditingChanged(index, text);
    }
}

void ApplicationBridge::OnTextEditingEnd(int index, const std::string& text)
{
    Debug::Printf("Text Editing End %d, %s", index, text.c_str());
    if (!m_input.expired())
    {
        m_input.lock()->ProcessTextEditingEnd(index, text);
    }
}

#endif

#include "ApplicationBridgeJni.inl"
