#include "Radian.h"
#include "Degree.h"
#include "MathGlobal.h"

using namespace Enigma::MathLib;

const float RAD_TO_DEG = 180.0f / Math::PI;

Radian& Radian::operator=(const Radian& other)
{
    m_radian = other.m_radian;
    return *this;
}

Radian& Radian::operator=(Radian&& other) noexcept
{
    m_radian = other.m_radian;
    return *this;
}

Radian Radian::operator+(const Radian& other) const
{
    return Radian(m_radian + other.m_radian);
}

Radian Radian::operator-(const Radian& other) const
{
    return Radian(m_radian - other.m_radian);
}

Radian Radian::operator*(float scalar) const
{
    return Radian(m_radian * scalar);
}

Radian Radian::operator/(float scalar) const
{
    return Radian(m_radian / scalar);
}

Radian& Radian::operator+=(const Radian& other)
{
    m_radian += other.m_radian;
    return *this;
}

Radian& Radian::operator-=(const Radian& other)
{
    m_radian -= other.m_radian;
    return *this;
}

Radian& Radian::operator*=(float scalar)
{
    m_radian *= scalar;
    return *this;
}

Radian& Radian::operator/=(float scalar)
{
    m_radian /= scalar;
    return *this;
}

bool Radian::operator==(const Radian& other) const
{
    return m_radian == other.m_radian;
}

bool Radian::operator!=(const Radian& other) const
{
    return m_radian != other.m_radian;
}

bool Radian::operator<(const Radian& other) const
{
    return m_radian < other.m_radian;
}

bool Radian::operator>(const Radian& other) const
{
    return m_radian > other.m_radian;
}

bool Radian::operator<=(const Radian& other) const
{
    return m_radian <= other.m_radian;
}

bool Radian::operator>=(const Radian& other) const
{
    return m_radian >= other.m_radian;
}

Degree Radian::degree() const
{
    return Degree(m_radian * RAD_TO_DEG);
}
