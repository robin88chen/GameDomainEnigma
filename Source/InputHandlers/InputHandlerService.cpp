#include "InputHandlerService.h"
#include "Frameworks/EventPublisher.h"
#include "MouseInputEvents.h"
#include "WinKeyboardInputEvents.h"
#include "GestureInputEvents.h"
#include "PadTextEditingEvents.h"
#include "Platforms/TextConverter.h"
#include <memory>
#if TARGET_PLATFORM == PLATFORM_WIN32
#include <mbctype.h>
#include <Windows.h>
#endif

using namespace Enigma::InputHandlers;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::Platforms;

DEFINE_RTTI(InputHandlers, InputHandlerService, ISystemService);

InputHandlerService::InputHandlerService(Frameworks::ServiceManager* mngr) : ISystemService(mngr)
{
    m_needTick = true;
    memset(&m_dragMouseParam, 0, sizeof(m_dragMouseParam));
    m_hasLastAsyncPressed = false;
}

InputHandlerService::~InputHandlerService()
{
}

ServiceResult InputHandlerService::OnInit()
{
    return ServiceResult::Complete;
}

ServiceResult InputHandlerService::OnTick()
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    DetectAsyncKeyPress();
#endif

    return ServiceResult::Pendding;
}

ServiceResult InputHandlerService::OnTerm()
{
    m_asyncKeyArray.clear();
    return ServiceResult::Complete;
}

void InputHandlerService::RegisterKeyboardAsyncKey(unsigned vk)
{
    m_asyncKeyArray.push_back(vk);
}

#if TARGET_PLATFORM == PLATFORM_WIN32
void InputHandlerService::ProcessWinMouseRightButtonDown(bool isDoubleClick, int x, int y, const MouseKeyFlags& keyFlags)
{
    m_dragMouseParam.m_isDraging = false;
    m_dragMouseParam.m_isDragRightBtn = true;
    m_dragMouseParam.m_dragBeginX = x;
    m_dragMouseParam.m_dragBeginY = y;
    EventPublisher::Send(std::make_shared<MouseRightButtonDown>(
        MouseInputEvent::MouseInputParameters(x, y, 0, 0, 0, keyFlags, isDoubleClick)));
}

void InputHandlerService::ProcessWinMouseRightButtonUp(int x, int y, const MouseKeyFlags& keyFlags)
{
    m_dragMouseParam.m_isDraging = false;
    m_dragMouseParam.m_isDragRightBtn = false;
    EventPublisher::Send(std::make_shared<MouseRightButtonUp>(
        MouseInputEvent::MouseInputParameters(x, y, 0, 0, 0, keyFlags, false)));
}

void InputHandlerService::ProcessWinMouseLeftButtonDown(bool isDoubleClick, int x, int y, const MouseKeyFlags& keyFlags)
{
    m_dragMouseParam.m_isDraging = false;
    m_dragMouseParam.m_isDragLeftBtn = true;
    m_dragMouseParam.m_dragBeginX = x;
    m_dragMouseParam.m_dragBeginY = y;
    EventPublisher::Send(std::make_shared<MouseLeftButtonDown>(
        MouseInputEvent::MouseInputParameters(x, y, 0, 0, 0, keyFlags, isDoubleClick)));
}

void InputHandlerService::ProcessWinMouseLeftButtonUp(int x, int y, const MouseKeyFlags& keyFlags)
{
    m_dragMouseParam.m_isDraging = false;
    m_dragMouseParam.m_isDragLeftBtn = false;
    EventPublisher::Send(std::make_shared<MouseLeftButtonUp>(
        MouseInputEvent::MouseInputParameters(x, y, 0, 0, 0, keyFlags, false)));
}

void InputHandlerService::ProcessWinMouseMiddleButtonDown(bool isDoubleClick, int x, int y, const MouseKeyFlags& keyFlags)
{
    m_dragMouseParam.m_isDraging = false;
    m_dragMouseParam.m_isDragMiddleBtn = true;
    m_dragMouseParam.m_dragBeginX = x;
    m_dragMouseParam.m_dragBeginY = y;
    EventPublisher::Send(std::make_shared<MouseMiddleButtonDown>(
        MouseInputEvent::MouseInputParameters(x, y, 0, 0, 0, keyFlags, isDoubleClick)));
}

void InputHandlerService::ProcessWinMouseMiddleButtonUp(int x, int y, const MouseKeyFlags& keyFlags)
{
    m_dragMouseParam.m_isDraging = false;
    m_dragMouseParam.m_isDragMiddleBtn = false;
    EventPublisher::Send(std::make_shared<MouseMiddleButtonUp>(
        MouseInputEvent::MouseInputParameters(x, y, 0, 0, 0, keyFlags, false)));
}

void InputHandlerService::ProcessWinMouseMove(int x, int y, const MouseKeyFlags& keyFlags)
{
    bool drag_flag = false;
    if ((x - m_dragMouseParam.m_dragBeginX > 10) || (m_dragMouseParam.m_dragBeginX - x > 10)
        || (y - m_dragMouseParam.m_dragBeginY > 10) || (m_dragMouseParam.m_dragBeginY - y > 10))
    {
        drag_flag = true;
    }
    else if (m_dragMouseParam.m_isDraging)
    {
        drag_flag = true;
    }

    if ((m_dragMouseParam.m_isDragLeftBtn) && (drag_flag) && (keyFlags.m_buttons.m_left))
    {
        EventPublisher::Send(std::make_shared<MouseLeftButtonDrag>(
            MouseInputEvent::MouseInputParameters(x, y, 0,
                x - m_dragMouseParam.m_dragBeginX, y - m_dragMouseParam.m_dragBeginY, keyFlags, false)));
        m_dragMouseParam.m_dragBeginX = x;
        m_dragMouseParam.m_dragBeginY = y;
        m_dragMouseParam.m_isDraging = true;
        return;
    }
    if ((m_dragMouseParam.m_isDragRightBtn) && (drag_flag) && (keyFlags.m_buttons.m_right))
    {
        EventPublisher::Send(std::make_shared<MouseRightButtonDrag>(
            MouseInputEvent::MouseInputParameters(x, y, 0,
                x - m_dragMouseParam.m_dragBeginX, y - m_dragMouseParam.m_dragBeginY, keyFlags, false)));
        m_dragMouseParam.m_dragBeginX = x;
        m_dragMouseParam.m_dragBeginY = y;
        m_dragMouseParam.m_isDraging = true;
        return;
    }
    if ((m_dragMouseParam.m_isDragMiddleBtn) && (drag_flag) && (keyFlags.m_buttons.m_middle))
    {
        EventPublisher::Send(std::make_shared<MouseMiddleButtonDrag>(
            MouseInputEvent::MouseInputParameters(x, y, 0,
                x - m_dragMouseParam.m_dragBeginX, y - m_dragMouseParam.m_dragBeginY, keyFlags, false)));
        m_dragMouseParam.m_dragBeginX = x;
        m_dragMouseParam.m_dragBeginY = y;
        m_dragMouseParam.m_isDraging = true;
        return;
    }

    EventPublisher::Send(std::make_shared<MouseMoved>(MouseInputEvent::MouseInputParameters(x, y, 0, 0, 0, keyFlags, false)));
}

void InputHandlerService::ProcessWinMouseWheel(int x, int y, const MouseKeyFlags& keyFlags)
{
    EventPublisher::Send(std::make_shared<MouseWheeled>(MouseInputEvent::MouseInputParameters(x, y, keyFlags.m_buttons.wheel_delta,
        0, 0, keyFlags, false)));
}

void InputHandlerService::ProcessWinKeyChar(char ch, int /*repeat*/)
{
    static unsigned char cch[4] = { 0,0,0,0 };
    static bool wait_tail = false;
    if ((_ismbblead(ch)) && (!wait_tail))
    {
        cch[0] = ch;
        wait_tail = true;
    }
    else if (wait_tail)
    {
        cch[1] = ch;
        wait_tail = false;
        cch[2] = 0;
        unsigned char utf8ch[4] = { 0, 0, 0, 0 };
        TextConverter::LocalStringToUtf8((char*)cch, 2, (char*)utf8ch, 4);
        EventPublisher::Send(std::make_shared<WinKeyboardCharInput>(
            WinKeyboardInputEvent::WinKeyboardInputParameters(utf8ch, 0)));
    }
    else
    {
        cch[0] = ch;
        cch[1] = 0;
        EventPublisher::Send(std::make_shared<WinKeyboardCharInput>(
            WinKeyboardInputEvent::WinKeyboardInputParameters(cch, 0)));
    }
}

void InputHandlerService::ProcessWinKeyPressDown(unsigned int vk, unsigned int keyFlags)
{
    EventPublisher::Send(std::make_shared<WinKeyboardPressingDown>(
        WinKeyboardInputEvent::WinKeyboardInputParameters(
            (WinKeyboardInputEvent::WinKeyboardInputParameters::VirtualKey)vk, keyFlags)));
}

void InputHandlerService::ProcessWinKeyPressUp(unsigned int vk, unsigned int keyFlags)
{
    EventPublisher::Send(std::make_shared<WinKeyboardPressingUp>(
        WinKeyboardInputEvent::WinKeyboardInputParameters(
            (WinKeyboardInputEvent::WinKeyboardInputParameters::VirtualKey)vk, keyFlags)));
}

void InputHandlerService::DetectAsyncKeyPress()
{
    if (!m_asyncKeyArray.empty())
    {
        bool hasGotKey = false;
        for (unsigned int i = 0; i < m_asyncKeyArray.size(); i++)
        {
            if (GetAsyncKeyState(m_asyncKeyArray[i]) < 0)
            {
                hasGotKey = true;
                EventPublisher::Send(std::make_shared<WinKeyboardAsyncPressed>(
                    WinKeyboardInputEvent::WinKeyboardInputParameters(
                        (WinKeyboardInputEvent::WinKeyboardInputParameters::VirtualKey)m_asyncKeyArray[i], 0)));
            }
        }
        if (hasGotKey)
        {
            m_hasLastAsyncPressed = true;
        }
        else
        {
            if (m_hasLastAsyncPressed)
            {
                EventPublisher::Send(std::make_shared<WinKeyboardAsyncReleased>());
                m_hasLastAsyncPressed = false;
            }
        }
    }
}
#endif

#if TARGET_PLATFORM == PLATFORM_ANDROID
void InputHandlerService::ProcessGestureSingleTap(float x, float y)
{
    EventPublisher::Send(std::make_shared<GestureSingleTap>(Vector2(x, y)));
}
void InputHandlerService::ProcessGestureDoubleTap(float x, float y)
{
    EventPublisher::Send(std::make_shared<GestureDoubleTap>(Vector2(x, y)));
}
void InputHandlerService::ProcessGestureShowPress(float x, float y)
{
    EventPublisher::Send(std::make_shared<GestureShowPress>(Vector2(x, y)));
}
void InputHandlerService::ProcessGestureLongPress(float x, float y)
{
    EventPublisher::Send(std::make_shared<GestureLongPress>(Vector2(x, y)));
}
void InputHandlerService::ProcessGestureScroll(float startX, float startY,
    float currentX, float currentY, float deltaX, float deltaY)
{
    if ((std::fabs(deltaX) < m_gestureThreshold.m_deltaScroll)
        && (std::fabs(deltaY) < m_gestureThreshold.m_deltaScroll)) return;
    EventPublisher::Send(std::make_shared<GestureScroll>(Vector2(startX, startY),
        Vector2(currentX, currentY), Vector2(deltaX, deltaY)));
}
void InputHandlerService::ProcessGestureFling(float startX, float startY,
    float currentX, float currentY, float velocityX, float velocityY)
{
    if ((std::fabs(velocityX) < m_gestureThreshold.m_velocityFling)
        && (std::fabs(velocityY) < m_gestureThreshold.m_velocityFling)) return;
    EventPublisher::Send(std::make_shared<GestureFling>(Vector2(startX, startY),
        Vector2(currentX, currentY), Vector2(velocityX, velocityY)));
}
void InputHandlerService::ProcessGestureScale(float factor, float focusX, float focusY)
{
    if (std::fabs(1.0f - factor) < m_gestureThreshold.m_deltaScale) return;
    EventPublisher::Send(std::make_shared<GestureScale>(factor, Vector2(focusX, focusY)));
}
void InputHandlerService::ProcessGestureScaleBegin(float spanX, float spanY, float focusX, float focusY)
{
    EventPublisher::Send(std::make_shared<GestureScaleBegin>(Vector2(spanX, spanY), Vector2(focusX, focusY)));
}
void InputHandlerService::ProcessGestureScaleEnd(float spanX, float spanY, float focusX, float focusY)
{
    EventPublisher::Send(std::make_shared<GestureScaleEnd>(Vector2(spanX, spanY), Vector2(focusX, focusY)));
}
void InputHandlerService::ProcessGestureShove(float focusX, float focusY, float deltaX, float deltaY)
{
    if ((std::fabs(deltaX) < m_gestureThreshold.m_deltaShove)
        && (std::fabs(deltaY) < m_gestureThreshold.m_deltaShove)) return;
    EventPublisher::Send(std::make_shared<GestureShove>(Vector2(focusX, focusY), Vector2(deltaX, deltaY)));
}
void InputHandlerService::ProcessGestureShoveBegin(float focusX, float focusY)
{
    EventPublisher::Send(std::make_shared<GestureShoveBegin>(Vector2(focusX, focusY)));
}
void InputHandlerService::ProcessGestureShoveEnd(float focusX, float focusY)
{
    EventPublisher::Send(std::make_shared<GestureShoveEnd>(Vector2(focusX, focusY)));
}

void InputHandlerService::ProcessTextEditingBegin(int index)
{
    EventPublisher::Send(std::make_shared<PadTextEditingBegin>(index));
}
void InputHandlerService::ProcessTextEditingChanged(int index, const std::string& text)
{
    EventPublisher::Send(std::make_shared<PadTextEditingChanged>(index, text));
}
void InputHandlerService::ProcessTextEditingEnd(int index, const std::string& text)
{
    EventPublisher::Send(std::make_shared<PadTextEditingEnd>(index, text));
}
#endif

