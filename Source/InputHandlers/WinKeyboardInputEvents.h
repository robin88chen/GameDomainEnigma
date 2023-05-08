/*********************************************************************
 * \file   WinKeyboardInputEvents.h
 * \brief windows keyboard input events
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _WIN_KEYBOARD_INPUT_EVENTS_H
#define _WIN_KEYBOARD_INPUT_EVENTS_H

#include "Frameworks/Event.h"

namespace Enigma::InputHandlers
{
    class WinKeyboardInputEvent : public Frameworks::IEvent
    {
    public:
        /** Keyboard input parameters */
        struct WinKeyboardInputParameters
        {
            enum VirtualKey
            {
                vkUnknown = 0x0,
                vkEscape = 0x1B,
                vkF1 = 0x70,
                vkF2,
                vkF3,
                vkF4,
                vkF5,
                vkF6,
                vkF7,
                vkF8,
                vkF9,
                vkF10,
                vkF11,
                vkF12,
            };
            union
            {
                VirtualKey m_virtualKey;
                unsigned char m_char[4];  // utf8 char
            };
            unsigned int m_flags;
            WinKeyboardInputParameters() : m_virtualKey(vkUnknown), m_flags{ 0 } {};
            WinKeyboardInputParameters(VirtualKey vk, unsigned int flag) : m_virtualKey(vk), m_flags(flag) {};
            WinKeyboardInputParameters(unsigned char cc[4], unsigned int flag)
            {
                m_char[0] = cc[0]; m_char[1] = cc[1]; m_char[2] = cc[2]; m_char[3] = cc[3];
                m_flags = flag;
            };
        };
    public:
        WinKeyboardInputEvent(const WinKeyboardInputParameters& param) : m_param(param) {};

        WinKeyboardInputParameters m_param;
    };
    /** keyboard char message */
    class WinKeyboardCharInput : public WinKeyboardInputEvent
    {
    public:
        WinKeyboardCharInput(const WinKeyboardInputParameters& param) : WinKeyboardInputEvent(param) {}
    };
    /** keyboard async press message */
    class WinKeyboardAsyncPressed : public WinKeyboardInputEvent
    {
    public:
        WinKeyboardAsyncPressed(const WinKeyboardInputParameters& param) : WinKeyboardInputEvent(param) {};
    };
    /** keyboard async release message */
    class WinKeyboardAsyncReleased : public WinKeyboardInputEvent
    {
    public:
        WinKeyboardAsyncReleased() : WinKeyboardInputEvent(WinKeyboardInputParameters()) {};
    };
    /** keyboard press down message */
    class WinKeyboardPressingDown : public WinKeyboardInputEvent
    {
    public:
        WinKeyboardPressingDown(const WinKeyboardInputParameters& param) : WinKeyboardInputEvent(param) {}
    };
    /** keyboard press up message */
    class WinKeyboardPressingUp : public WinKeyboardInputEvent
    {
    public:
        WinKeyboardPressingUp(const WinKeyboardInputParameters& param) : WinKeyboardInputEvent(param) {}
    };
}

#endif // _WIN_KEYBOARD_INPUT_EVENTS_H
