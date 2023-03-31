/*********************************************************************
 * \file   MouseKeyButtons.h
 * \brief  define mouse buttons & key flags
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _MOUSE_KEY_BUTTONS_H
#define _MOUSE_KEY_BUTTONS_H

#include "Platforms/PlatformConfig.h"

namespace Enigma::InputHandlers
{
#if TARGET_PLATFORM == PLATFORM_WIN32
    struct MouseButtons
    {
        bool m_left : 1;
        bool m_right : 1;
        bool m_shift : 1;
        bool m_ctrl : 1;
        bool m_middle : 1;
        bool place_holder : 3;
        char place_holder_c[1];
        short wheel_delta;
    };
    union MouseKeyFlags
    {
        unsigned int m_keyFlags;
        MouseButtons m_buttons;
    };
#endif
}

#endif // _MOUSE_KEY_BUTTONS_H
