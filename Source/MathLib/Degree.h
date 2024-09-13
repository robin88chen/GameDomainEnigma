/*********************************************************************
 * \file   Degree.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef MATH_DEGREE_H
#define MATH_DEGREE_H

namespace Enigma::MathLib
{
    class Radian;

    class Degree
    {
    public:
        Degree() : m_degree(0.0f) {}
        explicit Degree(float degree) : m_degree(degree) {}
        Degree(const Degree& other) : m_degree(other.m_degree) {}
        Degree(Degree&& other) noexcept : m_degree(other.m_degree) {}
        ~Degree() = default;

        Degree& operator=(const Degree& other);
        Degree& operator=(Degree&& other) noexcept;

        Degree operator+(const Degree& other) const;
        Degree operator-(const Degree& other) const;
        Degree operator*(float scalar) const;
        Degree operator/(float scalar) const;

        Degree& operator+=(const Degree& other);
        Degree& operator-=(const Degree& other);
        Degree& operator*=(float scalar);
        Degree& operator/=(float scalar);

        bool operator==(const Degree& other) const;
        bool operator!=(const Degree& other) const;
        bool operator<(const Degree& other) const;
        bool operator>(const Degree& other) const;
        bool operator<=(const Degree& other) const;
        bool operator>=(const Degree& other) const;

        [[nodiscard]] float value() const { return m_degree; }
        Radian radian() const;

    private:
        float m_degree;
    };
}

#endif // MATH_DEGREE_H
