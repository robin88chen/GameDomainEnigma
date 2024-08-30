/*********************************************************************
 * \file   Radian.h
 * \brief  Radian angle representation, value object
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef MATH_RADIAN_H
#define MATH_RADIAN_H

namespace Enigma::MathLib
{
    class Degree;

    class Radian
    {
    public:
        Radian() : m_radian(0.0f) {}
        explicit Radian(float radian) : m_radian(radian) {}
        Radian(const Radian& other) : m_radian(other.m_radian) {}
        Radian(Radian&& other) noexcept : m_radian(other.m_radian) {}
        ~Radian() = default;

        Radian& operator=(const Radian& other);
        Radian& operator=(Radian&& other) noexcept;

        Radian operator+(const Radian& other) const;
        Radian operator-(const Radian& other) const;
        Radian operator*(float scalar) const;
        Radian operator/(float scalar) const;

        Radian& operator+=(const Radian& other);
        Radian& operator-=(const Radian& other);
        Radian& operator*=(float scalar);
        Radian& operator/=(float scalar);

        bool operator==(const Radian& other) const;
        bool operator!=(const Radian& other) const;
        bool operator<(const Radian& other) const;
        bool operator>(const Radian& other) const;
        bool operator<=(const Radian& other) const;
        bool operator>=(const Radian& other) const;

        [[nodiscard]] float value() const { return m_radian; }
        Degree degree() const;

    private:
        float m_radian;
    };
}

#endif // MATH_RADIAN_H
