/*********************************************************************
 * \file   MathGlobal.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_GLOBAL_H
#define _MATH_GLOBAL_H

#include "Vector3.h"

namespace Enigma
{
    namespace MathLib
    {
        class Math
        {
        public:
            /** 浮點數相等 */
            static bool IsEqual(float l, float r);
            /** Max component of vector */
            static Vector3 MaxVectorComponent(const Vector3& vec1, const Vector3& vec2);
            /** Min component of vector */
            static Vector3 MinVectorComponent(const Vector3& vec1, const Vector3& vec2);
            /** epsilon (scaled) */
            static float Epsilon();

        public:
            static float m_epsilonUlp;  ///< 修正 epsilon 誤差的放大倍數, 預設為 1.0f
            static const float ZERO_TOLERANCE;  ///< =10e-6
            static const float MAX_FLOAT;   ///< =FLT_MAX
            static const float PI;
            static const float TWO_PI;
            static const float HALF_PI;
            static const float INV_PI;
            static const float INV_TWO_PI;
        };
    }
}
#endif // !_MATH_GLOBAL_H
