/*********************************************************************
 * \file   AlgebraBasicTypes.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/

#ifndef _ALGEBRA_BASIC_TYPES_H
#define _ALGEBRA_BASIC_TYPES_H

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
    struct Dimension
    {
        unsigned int m_width;
        unsigned int m_height;
    };
}
#endif // !_ALGEBRA_BASIC_TYPES_H
