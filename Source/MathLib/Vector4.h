/*********************************************************************
 * \file   Vector4.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_VECTOR4_H
#define _MATH_VECTOR4_H

namespace Enigma::MathLib
{
    class Vector3;
    /** Math Lib Vector4 */
    class Vector4
    {
    public:
        // construction
        Vector4();  ///< initializ to Zero Vector
        Vector4(float x, float y, float z, float w);
        Vector4(const Vector3& vec, float w);
        Vector4(const Vector4&) = default;
        Vector4(Vector4&&) = default;
        Vector4(const float* f);

        /** @name coordinate access */
        //@{
        operator const float* () const;
        operator float* ();
        float operator[] (int i) const;
        float& operator[] (int i);
        float x() const;
        float& x();
        float y() const;
        float& y();
        float z() const;
        float& z();
        float w() const;
        float& w();
        //@}

        /// assignment
        Vector4& operator= (const Vector4& v) = default;
        Vector4& operator= (Vector4&& v) = default;

        /** @name comparison -- 每個componet比對 */
        //@{
        bool operator== (const Vector4& v) const;   ///< 浮點數值比較
        bool operator!= (const Vector4& v) const;   ///< 浮點數值比較
        bool operator<  (const Vector4& v) const;   ///< 比較陣列記憶體內容
        bool operator<= (const Vector4& v) const;    ///< 比較陣列記憶體內容
        bool operator>  (const Vector4& v) const;   ///< 比較陣列記憶體內容
        bool operator>= (const Vector4& v) const;   ///< 比較陣列記憶體內容
        //@}

        /** @name arithmetic operations */
        //@{
        Vector4 operator+ (const Vector4& v) const;
        Vector4 operator- (const Vector4& v) const;
        Vector4 operator* (float scalar) const;
        Vector4 operator/ (float scalar) const;
        Vector4 operator- () const;
        //@}

        /** @name arithmetic updates */
        //@{
        Vector4& operator+= (const Vector4& v);
        Vector4& operator-= (const Vector4& v);
        Vector4& operator*= (float scalar);
        Vector4& operator/= (float scalar);
        //@}

        /** @name vector operations */
        //@{
        float length() const;
        float squaredLength() const;
        float dot(const Vector4& v) const;
        void normalizeSelf();
        Vector4 normalize() const;
        //@}

        /** @name special vectors */
        //@{
        static const Vector4 ZERO;
        static const Vector4 UNIT_X;
        static const Vector4 UNIT_Y;
        static const Vector4 UNIT_Z;
        static const Vector4 UNIT_W;
        //@}

    private:
        // support for none-equal comparisons
        int compareArrays(const Vector4& v) const;
        /** member data */
        union
        {
            struct
            {
                float m_x;
                float m_y;
                float m_z;
                float m_w;
            };
            float m_tuple[4];
        };
    };

    /** scalar * vector */
    Vector4 operator* (float scalar, const Vector4& v);
}

#endif // !_MATH_VECTOR4_H
