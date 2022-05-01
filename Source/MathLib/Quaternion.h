/*********************************************************************
 * \file   Quaternion.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_QUATERNION_H
#define _MATH_QUATERNION_H

#include "Vector3.h"
#include "Matrix3.h"

namespace Enigma::MathLib
{
    class Quaternion;
    struct QuaternionDecompose;
    /** Math Lib Quaternion
    @remarks
    A quaternion is q = w + x*i + y*j + z*k where (w,x,y,z) is not
     necessarily a unit length vector in 4D. */
    class Quaternion
    {
    public:
        // construction
        Quaternion();  ///< w=x=y=z=0.0
        Quaternion(float w, float x, float y, float z);
        Quaternion(const Quaternion& quat) = default;
        Quaternion(Quaternion&&) = default;

        /// quaternion for the input rotation matrix
        Quaternion(const Matrix3& rot);

        /// quaternion for the rotation of the axis-angle pair
        Quaternion(const Vector3& axis, float angle);

        /** @name member access
        // member access:  0 = w, 1 = x, 2 = y, 3 = z */
        //@{
        operator const float* () const;
        operator float* ();
        float operator[] (int i) const;
        float& operator[] (int i);
        float W() const;
        float& W();
        float X() const;
        float& X();
        float Y() const;
        float& Y();
        float Z() const;
        float& Z();
        //@}

        /** @name assignment */
        //@{
        Quaternion& operator= (const Quaternion& quat) = default;
        Quaternion& operator= (Quaternion&&) = default;
        //@}

        /** @name comparison */
        //@{
        bool operator== (const Quaternion& quat) const; ///< 浮點數值比較
        bool operator!= (const Quaternion& quat) const; ///< 浮點數值比較
        bool operator<  (const Quaternion& quat) const; ///< 比較陣列記憶體內容
        bool operator<= (const Quaternion& quat) const;  ///< 比較陣列記憶體內容
        bool operator>  (const Quaternion& quat) const; ///< 比較陣列記憶體內容
        bool operator>= (const Quaternion& quat) const; ///< 比較陣列記憶體內容
        //@}

        /** @name arithmetic operations */
        //@{
        Quaternion operator+ (const Quaternion& quat) const;
        Quaternion operator- (const Quaternion& quat) const;
        Quaternion operator* (const Quaternion& quat) const;
        Quaternion operator* (float scalar) const;
        Quaternion operator/ (float scalar) const;
        Quaternion operator- () const;
        Vector3 operator* (const Vector3& vec) const;
        //@}

        /** @name arithmetic updates */
        //@{
        Quaternion& operator+= (const Quaternion& quat);
        Quaternion& operator-= (const Quaternion& quat);
        Quaternion& operator*= (float scalar);
        Quaternion& operator/= (float scalar);
        //@}

        /** @name conversion between quaternions, matrices, and axis-angle */
        //@{
        static Quaternion FromRotationMatrix(const Matrix3& rot);
        Matrix3 ToRotationMatrix() const;
        /** @remarks
         assert:  axis[] is unit length \n
         The quaternion representing the rotation is \n
           q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)  */
        static Quaternion FromAxisAngle(const Vector3& axis, float angle);
        std::tuple<Vector3, float> ToAxisAngle() const;
        //@}

        /**  @name functions of a quaternion */
        //@{
        float Length() const;  ///< length of 4-tuple
        float SquaredLength() const;  ///< squared length of 4-tuple
        float Dot(const Quaternion& quat) const;  ///< dot product of 4-tuples
        Quaternion Normalize() const;  ///< make the 4-tuple unit length
        Quaternion Inverse() const;  ///< apply to non-zero quaternion
        Quaternion Conjugate() const;
        Quaternion Exp() const;  ///< apply to quaternion with w = 0
        Quaternion Log() const;  ///< apply to unit-length quaternion

        /** rotation of a vector by a quaternion */
        Vector3 Rotate(const Vector3& vec) const;

        /// spherical linear interpolation, t=0 --> P, t=1 --> Q
        static Quaternion Slerp(float t, const Quaternion& p, const Quaternion& q, bool shortestPath = false);

        /// spherical linear interpolation, t=0 --> P, t=1 --> Q
        static Quaternion SlerpExtraSpins(float t, const Quaternion& p,
            const Quaternion& q, int extraSpins);

        /// intermediate terms for spherical quadratic interpolation
        static Quaternion Intermediate(const Quaternion& q0,
            const Quaternion& q1, const Quaternion& q2);

        /// spherical quadratic interpolation
        static Quaternion Squad(float t, const Quaternion& q0,
            const Quaternion& a0, const Quaternion& a1,
            const Quaternion& q1, bool shortestPath = false);

        /** Compute a quaternion that rotates unit-length vector V1 to unit-length
         vector V2.  The rotation is about the axis perpendicular to both V1 and
         V2, with angle of that between V1 and V2.  If V1 and V2 are parallel,
         any axis of rotation will do, such as the permutation (z2,x2,y2), where
         V2 = (x2,y2,z2). */
        static Quaternion Align(const Vector3& vec1, const Vector3& vec2);

        /** Decompose a quaternion into q = q_twist * q_swing, where q is 'this'
         quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
         q, q_swing represents the rotation about the axis perpendicular to
         V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1. */
        QuaternionDecompose DecomposeTwistTimesSwing(const Vector3& vec1) const;

        /** Decompose a quaternion into q = q_swing * q_twist, where q is 'this'
         quaternion.  If V1 is the input axis and V2 is the rotation of V1 by
         q, q_swing represents the rotation about the axis perpendicular to
         V1 and V2 (see Quaternion::Align), and q_twist is a rotation about V1. */
        QuaternionDecompose DecomposeSwingTimesTwist(const Vector3 & vec1) const;
        //@}

        /** @name special values */
        //@{
        static const Quaternion IDENTITY;  // the identity rotation
        static const Quaternion ZERO;
        //@}

    private:

        void ImplicitFromRotationMatrix(const Matrix3& rot);
        void ImplicitFromAxisAngle(const Vector3& axis, float angle);

        // support for comparisons
        int CompareArrays(const Quaternion& quat) const;

        union
        {
            struct
            {
                float m_w, m_x, m_y, m_z;
            };
            float m_tuple[4];
        };
    };

    struct QuaternionDecompose
    {
        Quaternion m_swing;
        Quaternion m_twist;
    };

    Quaternion operator* (float scalar, const Quaternion& quat);
};

#endif // !_MATH_QUATERNION_H
