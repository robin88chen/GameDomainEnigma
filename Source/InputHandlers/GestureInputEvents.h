/*********************************************************************
 * \file   GestureInputEvents.h
 * \brief mobile gesture events
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _GESTURE_INPUT_EVENTS_H
#define _GESTURE_INPUT_EVENTS_H

#include "Frameworks/Event.h"
#include "MathLib/Vector2.h"

namespace Enigma::InputHandlers
{
    /** gesture single tap message */
    class GestureSingleTap : public Frameworks::IEvent
    {
    public:
        GestureSingleTap(MathLib::Vector2 pt) : m_pt(pt) {}
        MathLib::Vector2 m_pt;
    };
    /** gesture double tap message */
    class GestureDoubleTap : public Frameworks::IEvent
    {
    public:
        GestureDoubleTap(MathLib::Vector2 pt) : m_pt(pt) {}
        MathLib::Vector2 m_pt;
    };
    /** gesture show press message */
    class GestureShowPress : public Frameworks::IEvent
    {
    public:
        GestureShowPress(MathLib::Vector2 pt) : m_pt(pt) {}
        MathLib::Vector2 m_pt;
    };
    /** gesture long press message */
    class GestureLongPress : public Frameworks::IEvent
    {
    public:
        GestureLongPress(MathLib::Vector2 pt) : m_pt(pt) {}
        MathLib::Vector2 m_pt;
    };

    /** gesture scroll message */
    class GestureScroll : public Frameworks::IEvent
    {
    public:
        GestureScroll(MathLib::Vector2 start, MathLib::Vector2 current, MathLib::Vector2 delta) : m_start(start), m_current(current), m_delta(delta) {}
        MathLib::Vector2 m_start;
        MathLib::Vector2 m_current;
        MathLib::Vector2 m_delta;
    };
    /** gesture fling message */
    class GestureFling : public Frameworks::IEvent
    {
    public:
        GestureFling(MathLib::Vector2 start, MathLib::Vector2 current, MathLib::Vector2 velocity) : m_start(start), m_current(current), m_velocity(velocity) {}
        MathLib::Vector2 m_start;
        MathLib::Vector2 m_current;
        MathLib::Vector2 m_velocity;
    };

    /** gesture scale message */
    class GestureScale : public Frameworks::IEvent
    {
    public:
        GestureScale(float factor, MathLib::Vector2 focus) : m_factor(factor), m_focus(focus) {}
        float m_factor;
        MathLib::Vector2 m_focus;
    };
    /** gesture scale begin message */
    class GestureScaleBegin : public Frameworks::IEvent
    {
    public:
        GestureScaleBegin(MathLib::Vector2 span, MathLib::Vector2 focus) : m_span(span), m_focus(focus) {}
        MathLib::Vector2 m_span;
        MathLib::Vector2 m_focus;
    };
    /** gesture scale end message */
    class GestureScaleEnd : public Frameworks::IEvent
    {
    public:
        GestureScaleEnd(MathLib::Vector2 span, MathLib::Vector2 focus) : m_span(span), m_focus(focus) {}
        MathLib::Vector2 m_span;
        MathLib::Vector2 m_focus;
    };

    /** gesture shove message */
    class GestureShove : public Frameworks::IEvent
    {
    public:
        GestureShove(MathLib::Vector2 focus, MathLib::Vector2 delta) : m_focus(focus), m_delta(delta) {}
        MathLib::Vector2 m_focus;
        MathLib::Vector2 m_delta;
    };
    /** gesture shove begin message */
    class GestureShoveBegin : public Frameworks::IEvent
    {
    public:
        GestureShoveBegin(MathLib::Vector2 focus) : m_focus(focus) {}
        MathLib::Vector2 m_focus;
    };
    /** gesture shove end message */
    class GestureShoveEnd : public Frameworks::IEvent
    {
    public:
        GestureShoveEnd(MathLib::Vector2 focus) : m_focus(focus) {}
        MathLib::Vector2 m_focus;
    };

}
#endif // _GESTURE_INPUT_EVENTS_H
