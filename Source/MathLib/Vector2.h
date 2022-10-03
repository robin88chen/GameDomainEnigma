/*********************************************************************
 * \file   Vector2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_VECTOR2_H
#define _MATH_VECTOR2_H

namespace Enigma::MathLib
{
    /** Math Lib Vector2 */
    class Vector2
    {
    public:
        // construction
        Vector2();  // uninitialized
        Vector2(float x, float y);
        Vector2(const float* f);

        /** @name coordinate access */
        //@{
        operator const float* () const;
        operator float* ();
        float operator[] (int i) const;
        float& operator[] (int i);
        float X() const;
        float& X();
        float Y() const;
        float& Y();
        //@}


        /** @name comparison */
        //@{
        bool operator== (const Vector2& v) const;  ///< 浮點數值比較
        bool operator!= (const Vector2& v) const;  ///< 浮點數值比較
        bool operator<  (const Vector2& v) const;  ///< 比較陣列記憶體內容
        bool operator<= (const Vector2& v) const;    ///< 比較陣列記憶體內容
        bool operator>  (const Vector2& v) const;  ///< 比較陣列記憶體內容
        bool operator>= (const Vector2& v) const;  ///< 比較陣列記憶體內容
        //@}

        /** @name arithmetic operations */
        //@{
        Vector2 operator+ (const Vector2& v) const;
        Vector2 operator- (const Vector2& v) const;
        Vector2 operator* (float scalar) const;
        Vector2 operator/ (float scalar) const;
        Vector2 operator- () const;
        //@}

        /** @name arithmetic updates */
        //@{
        Vector2& operator+= (const Vector2& v);
        Vector2& operator-= (const Vector2& v);
        Vector2& operator*= (float scalar);
        Vector2& operator/= (float scalar);
        //@}

        /** @name vector operations */
        //@{
        float Length() const;
        float SquaredLength() const;
        float Dot(const Vector2& v) const;
        Vector2 Normalize() const;
        void NormalizeSelf();

        /// returns (y,-x)
        Vector2 Perp() const;

        /// returns (y,-x)/sqrt(x*x+y*y)
        Vector2 UnitPerp() const;

        /// returns DotPerp((x,y),(V.x,V.y)) = x*V.y - y*V.x
        float DotPerp(const Vector2& v) const;
        //@}

        /** @name special vectors */
        //@{
        static const Vector2 ZERO;
        static const Vector2 UNIT_X;
        static const Vector2 UNIT_Y;
        //@}

    private:
        // support for none-equal comparisons
        int CompareArrays(const Vector2& v) const;
        /** member data */
        union
        {
            struct
            {
                float m_x;
                float m_y;
            };
            float m_tuple[2];
        };
    };

    /** scalar * vector2 */
    Vector2 operator* (float scalar, const Vector2& v);
}

#endif // !_MATH_VECTOR2_H
