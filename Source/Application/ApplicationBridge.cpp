#include "ApplicationBridge.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/PlatformConfig.h"
#include "FileSystem/FileSystem.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "AppDelegateAndroid.h"
#include "GraphicAPIEgl/GraphicAPIEgl.h"

using namespace Enigma::Platforms;
using namespace Enigma::FileSystem;
using namespace Enigma::Devices;
using namespace Enigma::Application;

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
    graphic->SetDimension(MathLib::Dimension{ (unsigned int)width, (unsigned int)height });
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
    graphic->SetDimension(MathLib::Dimension{ (unsigned int)width, (unsigned int)height });
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
}

void ApplicationBridge::OnGestureShowPress(float x, float y)
{
}

void ApplicationBridge::OnGestureDoubleTap(float x, float y)
{
}

void ApplicationBridge::OnGestureSingleTap(float x, float y)
{
}

void ApplicationBridge::OnGestureScroll(float startX, float startY, float currentX, float currentY, float deltaX, float deltaY)
{
}

void ApplicationBridge::OnGestureFling(float startX, float startY, float currentX, float currentY, float velocityX, float velocityY)
{
}

void ApplicationBridge::OnGestureScale(float factor, float focusX, float focusY)
{
}

void ApplicationBridge::OnGestureScaleBegin(float spanX, float spanY, float focusX, float focusY)
{
}

void ApplicationBridge::OnGestureScaleEnd(float spanX, float spanY, float focusX, float focusY)
{
}

void ApplicationBridge::OnGestureShove(float focusX, float focusY, float deltaX, float deltaY)
{
}

void ApplicationBridge::OnGestureShoveBegin(float focusX, float focusY)
{
}

void ApplicationBridge::OnGestureShoveEnd(float focusX, float focusY)
{
}

void ApplicationBridge::OnTextEditingBegin(int index)
{
}

void ApplicationBridge::OnTextEditingChanged(int index, const std::string& text)
{
}

void ApplicationBridge::OnTextEditingEnd(int index, const std::string& text)
{
}

#endif

#include "ApplicationBridgeJni.inl"
