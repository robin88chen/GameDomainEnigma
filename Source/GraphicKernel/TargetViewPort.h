/********************************************************************
 * \file   TargetViewPort.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef TARGET_VIEWPORT_H
#define TARGET_VIEWPORT_H

#include "MathLib/Vector2.h"

namespace Enigma::Graphics
{
    /** Target View Port class */
    class TargetViewPort
    {
    public:
        TargetViewPort();
        TargetViewPort(unsigned int x, unsigned int y,
            unsigned int width, unsigned int height,
            float min_z = 0.0f, float max_z = 1.0f);
        TargetViewPort(const TargetViewPort& vp) = default;
        TargetViewPort(TargetViewPort&& vp) = default;
        ~TargetViewPort();
        TargetViewPort& operator=(const TargetViewPort&) = default;
        TargetViewPort& operator=(TargetViewPort&&) = default;

        inline unsigned int x() const { return m_x; };
        inline unsigned int& x() { return m_x; };
        inline unsigned int y() const { return m_y; };
        inline unsigned int& y() { return m_y; };
        inline unsigned int Width() const { return m_width; };
        inline unsigned int& Width() { return m_width; };
        inline unsigned int Height() const { return m_height; };
        inline unsigned int& Height() { return m_height; };
        inline float MinZ() const { return m_minZ; };
        inline float& MinZ() { return m_minZ; };
        inline float MaxZ() const { return m_maxZ; };
        inline float& MaxZ() { return m_maxZ; };

        bool operator==(const TargetViewPort& vp);

        MathLib::Vector2 ViewportPositionToClippingPosition(const MathLib::Vector2& vp_pos) const;

    private:
        unsigned int m_x;
        unsigned int m_y;
        unsigned int m_width;
        unsigned int m_height;
        float m_minZ;
        float m_maxZ;
    };
};


#endif // TARGET_VIEWPORT_H
