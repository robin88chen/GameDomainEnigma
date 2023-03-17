/*********************************************************************
 * \file   PadTextEditingEvents.h
 * \brief mobile text editing events
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _PAD_TEXT_EDITING_EVENTS_H
#define _PAD_TEXT_EDITING_EVENTS_H

#include "Frameworks/Event.h"
#include <string>

namespace Enigma::InputHandlers
{
    /** text editing begin message */
    class PadTextEditingBegin : public Frameworks::IEvent
    {
    public:
        PadTextEditingBegin(int index) : m_index(index) {}
        int m_index;
    };
    /** text editing changed message */
    class PadTextEditingChanged : public Frameworks::IEvent
    {
    public:
        PadTextEditingChanged(int index, const std::string& text) : m_index(index), m_text(text) {}
        int m_index;
        std::string m_text;
    };
    /** text editing end message */
    class PadTextEditingEnd : public Frameworks::IEvent
    {
    public:
        PadTextEditingEnd(int index, const std::string& text) : m_index(index), m_text(text) {}
        int m_index;
        std::string m_text;
    };
}

#endif // _PAD_TEXT_EDITING_EVENTS_H
