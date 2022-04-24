#include "Vector2.h"
#include "MathGlobal.h"
#include <cassert>
#include <cstring>
#include <cmath>

using namespace Enigma;
using namespace MathLib;

const Vector2 Vector2::ZERO(0.0f, 0.0f);
const Vector2 Vector2::UNIT_X(1.0f, 0.0f);
const Vector2 Vector2::UNIT_Y(0.0f, 1.0f);

Vector2::Vector2()
{
    m_x = m_y = (float)0.0;
}

Vector2::Vector2(float x, float y)
{
    m_tuple[0] = x;
    m_tuple[1] = y;
}

Vector2::Vector2(float* f)
{
    assert(f);
    memcpy(m_tuple, f, 2 * sizeof(float));
}

Vector2::operator const float* () const
{
    return m_tuple;
}

Vector2::operator float* ()
{
    return m_tuple;
}

float Vector2::operator[] (int i) const
{
    assert(0 <= i && i <= 1);
    return m_tuple[i];
}

float& Vector2::operator[] (int i)
{
    assert(0 <= i && i <= 1);
    return m_tuple[i];
}

float Vector2::X() const
{
    return m_tuple[0];
}

float& Vector2::X()
{
    return m_tuple[0];
}

float Vector2::Y() const
{
    return m_tuple[1];
}

float& Vector2::Y()
{
    return m_tuple[1];
}

int Vector2::CompareArrays(const Vector2& v) const
{
    return memcmp(m_tuple, v.m_tuple, 2 * sizeof(float));
}

bool Vector2::operator== (const Vector2& v) const
{
    return (Math::IsEqual(m_x, v.m_x) && Math::IsEqual(m_y, v.m_y));
}

bool Vector2::operator!= (const Vector2& v) const
{
    return (!(Math::IsEqual(m_x, v.m_x) && Math::IsEqual(m_y, v.m_y)));
}

bool Vector2::operator< (const Vector2& v) const
{
    return CompareArrays(v) < 0;
}

bool Vector2::operator<= (const Vector2& v) const
{
    return CompareArrays(v) <= 0;
}

bool Vector2::operator> (const Vector2& v) const
{
    return CompareArrays(v) > 0;
}

bool Vector2::operator>= (const Vector2& v) const
{
    return CompareArrays(v) >= 0;
}

Vector2 Vector2::operator+ (const Vector2& v) const
{
    return Vector2(
        m_tuple[0] + v.m_tuple[0],
        m_tuple[1] + v.m_tuple[1]);
}

Vector2 Vector2::operator- (const Vector2& v) const
{
    return Vector2(
        m_tuple[0] - v.m_tuple[0],
        m_tuple[1] - v.m_tuple[1]);
}

Vector2 Vector2::operator* (float scalar) const
{
    return Vector2(
        scalar * m_tuple[0],
        scalar * m_tuple[1]);
}

Vector2 Vector2::operator/ (float scalar) const
{
    Vector2 quot;

    if (scalar != (float)0.0)
    {
        //float invScalar = ((float)1.0) / scalar;
        quot.m_tuple[0] = m_tuple[0] / scalar;
        quot.m_tuple[1] = m_tuple[1] / scalar;
    }
    else
    {
        quot.m_tuple[0] = Math::MAX_FLOAT;
        quot.m_tuple[1] = Math::MAX_FLOAT;
    }

    return quot;
}

Vector2 Vector2::operator- () const
{
    return Vector2(
        -m_tuple[0],
        -m_tuple[1]);
}

Vector2& Vector2::operator+= (const Vector2& v)
{
    m_tuple[0] += v.m_tuple[0];
    m_tuple[1] += v.m_tuple[1];
    return *this;
}

Vector2& Vector2::operator-= (const Vector2& v)
{
    m_tuple[0] -= v.m_tuple[0];
    m_tuple[1] -= v.m_tuple[1];
    return *this;
}

Vector2& Vector2::operator*= (float scalar)
{
    m_tuple[0] *= scalar;
    m_tuple[1] *= scalar;
    return *this;
}

Vector2& Vector2::operator/= (float scalar)
{
    if (scalar != (float)0.0)
    {
        //float invScalar = ((float)1.0) / scalar;
        m_tuple[0] /= scalar;
        m_tuple[1] /= scalar;
    }
    else
    {
        m_tuple[0] = Math::MAX_FLOAT;
        m_tuple[1] = Math::MAX_FLOAT;
    }

    return *this;
}

float Vector2::Length() const
{
    return sqrt(
        m_tuple[0] * m_tuple[0] +
        m_tuple[1] * m_tuple[1]);
}

float Vector2::SquaredLength() const
{
    return
        m_tuple[0] * m_tuple[0] +
        m_tuple[1] * m_tuple[1];
}

float Vector2::Dot(const Vector2& v) const
{
    return
        m_tuple[0] * v.m_tuple[0] +
        m_tuple[1] * v.m_tuple[1];
}

Vector2 Vector2::Normalize() const
{
    float length = Length();
    Vector2 v;
    if (length > Math::ZERO_TOLERANCE)
    {
        //float invLength = ((float)1.0) / length;
        v.m_tuple[0] = m_tuple[0] / length;
        v.m_tuple[1] = m_tuple[1] / length;
    }
    else
    {
        v.m_tuple[0] = (float)0.0;
        v.m_tuple[1] = (float)0.0;
    }

    return v;
}

Vector2 Vector2::Perp() const
{
    return Vector2(m_tuple[1], -m_tuple[0]);
}

Vector2 Vector2::UnitPerp() const
{
    Vector2 kPerp(m_tuple[1], -m_tuple[0]);
    return kPerp.Normalize();
}

float Vector2::DotPerp(const Vector2& v) const
{
    return m_tuple[0] * v.m_tuple[1] - m_tuple[1] * v.m_tuple[0];
}

namespace Enigma::MathLib
{
    Vector2 operator* (float scalar, const Vector2& v)
    {
        return Vector2(
            scalar * v[0],
            scalar * v[1]);
    }
}