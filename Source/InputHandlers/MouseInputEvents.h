/*********************************************************************
 * \file   MouseInputEvents.h
 * \brief mouse input events
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _MOUSE_INPUT_EVENTS_H
#define _MOUSE_INPUT_EVENTS_H

#include "Frameworks/Event.h"
#include "MouseKeyButtons.h"

namespace Enigma::InputHandlers
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    class MouseInputEvent : public Frameworks::IEvent
    {
    public:
        /** mouse input parameters */
        struct MouseInputParameters
        {
            int m_x;
            int m_y;
            int m_deltaWheel;  // data for mouse wheel
            int m_deltaX; // data for drag
            int m_deltaY; // data for drag
            bool m_isDblClick;
            MouseKeyFlags m_flags;
            MouseInputParameters() : m_x(0), m_y(0), m_deltaWheel(0), m_deltaX(0), m_deltaY(0),
                m_isDblClick(false), m_flags{ 0 } {}
            MouseInputParameters(int x, int y, int deltaW, int deltaX, int deltaY, const MouseKeyFlags& flag, bool dblClick)
                : m_x(x), m_y(y), m_deltaWheel(deltaW), m_deltaX(deltaX), m_deltaY(deltaY),
                m_isDblClick(dblClick), m_flags(flag) {}
        };
    public:
        MouseInputEvent(const MouseInputParameters& param) : m_param(param) {}

        MouseInputParameters m_param;
    };
    /** mouse move message */
    class MouseMoved : public MouseInputEvent
    {
    public:
        MouseMoved(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };
    /** mouse wheel message */
    class MouseWheeled : public MouseInputEvent
    {
    public:
        MouseWheeled(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };
    /** mouse r button down message */
    class MouseRightButtonDown : public MouseInputEvent
    {
    public:
        MouseRightButtonDown(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };
    /** mouse r button up message */
    class MouseRightButtonUp : public MouseInputEvent
    {
    public:
        MouseRightButtonUp(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };
    /** mouse r button drag message */
    class MouseRightButtonDrag : public MouseInputEvent
    {
    public:
        MouseRightButtonDrag(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };

    /** mouse l button down message */
    class MouseLeftButtonDown : public MouseInputEvent
    {
    public:
        MouseLeftButtonDown(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };
    /** mouse l button up message */
    class MouseLeftButtonUp : public MouseInputEvent
    {
    public:
        MouseLeftButtonUp(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };
    /** mouse l button drag message */
    class MouseLeftButtonDrag : public MouseInputEvent
    {
    public:
        MouseLeftButtonDrag(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };

    /** mouse m button down message */
    class MouseMiddleButtonDown : public MouseInputEvent
    {
    public:
        MouseMiddleButtonDown(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };
    /** mouse m button up message */
    class MouseMiddleButtonUp : public MouseInputEvent
    {
    public:
        MouseMiddleButtonUp(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };
    /** mouse m button drag message */
    class MouseMiddleButtonDrag : public MouseInputEvent
    {
    public:
        MouseMiddleButtonDrag(const MouseInputParameters& param) : MouseInputEvent(param) {};
    };
#endif
}

#endif // _MOUSE_INPUT_EVENTS_H
