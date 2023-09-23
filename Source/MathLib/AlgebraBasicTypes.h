/*********************************************************************
 * \file   AlgebraBasicTypes.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/

#ifndef _ALGEBRA_BASIC_TYPES_H
#define _ALGEBRA_BASIC_TYPES_H

#include <type_traits>

namespace Enigma::MathLib
{
    template<class T> struct EigenDecompose
    {
        T m_rot;
        T m_diag;
    };

    struct EulerAngles
    {
        float m_x;
        float m_y;
        float m_z;
    };
    enum class UniqueEulerAngle
    {
        False,
        True
    };
    template<typename T, typename = typename std::enable_if<std::is_scalar<T>::value, T>::type> struct Dimension
    {
        T m_width;
        T m_height;
        bool operator==(const Dimension& rhs) const
        {
            return m_width == rhs.m_width && m_height == rhs.m_height;
        }
        bool operator!=(const Dimension& rhs) const
        {
            return !(*this == rhs);
        }
    };
}
#endif // !_ALGEBRA_BASIC_TYPES_H
