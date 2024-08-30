#include "Degree.h"
#include "Radian.h"
#include "MathGlobal.h"

using namespace Enigma::MathLib;

const float DEG_TO_RAD = Math::PI / 180.0f;

Degree& Degree::operator=(const Degree& other)
{
    m_degree = other.m_degree;
    return *this;
}

Degree& Degree::operator=(Degree&& other) noexcept
{
    m_degree = other.m_degree;
    return *this;
}

Degree Degree::operator+(const Degree& other) const
{
    return Degree(m_degree + other.m_degree);
}

Degree Degree::operator-(const Degree& other) const
{
    return Degree(m_degree - other.m_degree);
}

Degree Degree::operator*(float scalar) const
{
    return Degree(m_degree * scalar);
}

Degree Degree::operator/(float scalar) const
{
    return Degree(m_degree / scalar);
}

Degree& Degree::operator+=(const Degree& other)
{
    m_degree += other.m_degree;
    return *this;
}

Degree& Degree::operator-=(const Degree& other)
{
    m_degree -= other.m_degree;
    return *this;
}

Degree& Degree::operator*=(float scalar)
{
    m_degree *= scalar;
    return *this;
}

Degree& Degree::operator/=(float scalar)
{
    m_degree /= scalar;
    return *this;
}

bool Degree::operator==(const Degree& other) const
{
    return m_degree == other.m_degree;
}

bool Degree::operator!=(const Degree& other) const
{
    return m_degree != other.m_degree;
}

bool Degree::operator<(const Degree& other) const
{
    return m_degree < other.m_degree;
}

bool Degree::operator>(const Degree& other) const
{
    return m_degree > other.m_degree;
}

bool Degree::operator<=(const Degree& other) const
{
    return m_degree <= other.m_degree;
}

bool Degree::operator>=(const Degree& other) const
{
    return m_degree >= other.m_degree;
}

Radian Degree::radian() const
{
    return Radian(m_degree * DEG_TO_RAD);
}
