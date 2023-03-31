/*********************************************************************
 * \file   GestureThreshold.h
 * \brief define gesture threshold
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _GESTURE_THRESHOLD_H
#define _GESTURE_THRESHOLD_H

namespace Enigma::InputHandlers
{
    struct GestureThreshold
    {
        float m_deltaScroll;  ///< default 10.0f
        float m_velocityFling;  ///< default 5000.0f
        float m_deltaScale; ///< default 0.01f
        float m_deltaShove; ///< default 10.0f
        GestureThreshold()
            : m_deltaScroll(10.0f), m_velocityFling(5000.0f), m_deltaScale(0.01f), m_deltaShove(10.0f)
        {}
    };
}

#endif // _GESTURE_THRESHOLD_H
