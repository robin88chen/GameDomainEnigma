#include "Vector4.h"
#include "MathGlobal.h"
#include <cassert>
#include <cstring>
#include <cmath>

using namespace Enigma;
using namespace MathLib;

const Vector4 Vector4::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::UNIT_X(1.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::UNIT_Y(0.0f, 1.0f, 0.0f, 0.0f);
const Vector4 Vector4::UNIT_Z(0.0f, 0.0f, 1.0f, 0.0f);
const Vector4 Vector4::UNIT_W(0.0f, 0.0f, 0.0f, 1.0f);

Vector4::Vector4()
{
    m_x = m_y = m_z = m_w = (float)0.0;
}

Vector4::Vector4(float x, float y, float z, float w)
{
    m_tuple[0] = x;
    m_tuple[1] = y;
    m_tuple[2] = z;
    m_tuple[3] = w;
}
Vector4::Vector4(const Vector3& vec, float w)
{
    m_tuple[0] = vec.X();
    m_tuple[1] = vec.Y();
    m_tuple[2] = vec.Z();
    m_tuple[3] = w;
}

Vector4::Vector4(float* f)
{
    assert(f);
    memcpy(m_tuple, f, 4 * sizeof(float));
}

Vector4::operator const float* () const
{
    return m_tuple;
}

Vector4::operator float* ()
{
    return m_tuple;
}

float Vector4::operator[] (int i) const
{
    assert(0 <= i && i <= 3);
    return m_tuple[i];
}

float& Vector4::operator[] (int i)
{
    assert(0 <= i && i <= 3);
    return m_tuple[i];
}

float Vector4::X() const
{
    return m_tuple[0];
}

float& Vector4::X()
{
    return m_tuple[0];
}

float Vector4::Y() const
{
    return m_tuple[1];
}

float& Vector4::Y()
{
    return m_tuple[1];
}

float Vector4::Z() const
{
    return m_tuple[2];
}

float& Vector4::Z()
{
    return m_tuple[2];
}

float Vector4::W() const
{
    return m_tuple[3];
}

float& Vector4::W()
{
    return m_tuple[3];
}

int Vector4::CompareArrays(const Vector4& v) const
{
    return memcmp(m_tuple, v.m_tuple, 4 * sizeof(float));
}

bool Vector4::operator== (const Vector4& v) const
{
    return (Math::IsEqual(m_x, v.m_x) && Math::IsEqual(m_y, v.m_y) &&
        Math::IsEqual(m_z, v.m_z) && Math::IsEqual(m_w, v.m_w));
}

bool Vector4::operator!= (const Vector4& v) const
{
    return (!(Math::IsEqual(m_x, v.m_x) && Math::IsEqual(m_y, v.m_y)
        && Math::IsEqual(m_z, v.m_z) && Math::IsEqual(m_w, v.m_w)));
}

bool Vector4::operator< (const Vector4& v) const
{
    return CompareArrays(v) < 0;
}

bool Vector4::operator<= (const Vector4& v) const
{
    return CompareArrays(v) <= 0;
}

bool Vector4::operator> (const Vector4& v) const
{
    return CompareArrays(v) > 0;
}

bool Vector4::operator>= (const Vector4& v) const
{
    return CompareArrays(v) >= 0;
}

Vector4 Vector4::operator+ (const Vector4& v) const
{
    return Vector4(
        m_tuple[0] + v.m_tuple[0],
        m_tuple[1] + v.m_tuple[1],
        m_tuple[2] + v.m_tuple[2],
        m_tuple[3] + v.m_tuple[3]);
}

Vector4 Vector4::operator- (const Vector4& v) const
{
    return Vector4(
        m_tuple[0] - v.m_tuple[0],
        m_tuple[1] - v.m_tuple[1],
        m_tuple[2] - v.m_tuple[2],
        m_tuple[3] - v.m_tuple[3]);
}

Vector4 Vector4::operator* (float scalar) const
{
    return Vector4(
        scalar * m_tuple[0],
        scalar * m_tuple[1],
        scalar * m_tuple[2],
        scalar * m_tuple[3]);
}

Vector4 Vector4::operator/ (float scalar) const
{
    Vector4 quot;

    if (scalar != (float)0.0)
    {
        //float invScalar = ((float)1.0) / scalar;
        quot.m_tuple[0] = m_tuple[0] / scalar;
        quot.m_tuple[1] = m_tuple[1] / scalar;
        quot.m_tuple[2] = m_tuple[2] / scalar;
        quot.m_tuple[3] = m_tuple[3] / scalar;
    }
    else
    {
        quot.m_tuple[0] = Math::MAX_FLOAT;
        quot.m_tuple[1] = Math::MAX_FLOAT;
        quot.m_tuple[2] = Math::MAX_FLOAT;
        quot.m_tuple[3] = Math::MAX_FLOAT;
    }

    return quot;
}

Vector4 Vector4::operator- () const
{
    return Vector4(
        -m_tuple[0],
        -m_tuple[1],
        -m_tuple[2],
        -m_tuple[3]);
}

Vector4& Vector4::operator+= (const Vector4& v)
{
    m_tuple[0] += v.m_tuple[0];
    m_tuple[1] += v.m_tuple[1];
    m_tuple[2] += v.m_tuple[2];
    m_tuple[3] += v.m_tuple[3];
    return *this;
}

Vector4& Vector4::operator-= (const Vector4& v)
{
    m_tuple[0] -= v.m_tuple[0];
    m_tuple[1] -= v.m_tuple[1];
    m_tuple[2] -= v.m_tuple[2];
    m_tuple[3] -= v.m_tuple[3];
    return *this;
}

Vector4& Vector4::operator*= (float scalar)
{
    m_tuple[0] *= scalar;
    m_tuple[1] *= scalar;
    m_tuple[2] *= scalar;
    m_tuple[3] *= scalar;
    return *this;
}

Vector4& Vector4::operator/= (float scalar)
{
    if (scalar != (float)0.0)
    {
        //float invScalar = ((float)1.0) / scalar;
        m_tuple[0] /= scalar;
        m_tuple[1] /= scalar;
        m_tuple[2] /= scalar;
        m_tuple[3] /= scalar;
    }
    else
    {
        m_tuple[0] = Math::MAX_FLOAT;
        m_tuple[1] = Math::MAX_FLOAT;
        m_tuple[2] = Math::MAX_FLOAT;
        m_tuple[3] = Math::MAX_FLOAT;
    }

    return *this;
}

float Vector4::Length() const
{
    return sqrt(
        m_tuple[0] * m_tuple[0] +
        m_tuple[1] * m_tuple[1] +
        m_tuple[2] * m_tuple[2] +
        m_tuple[3] * m_tuple[3]);
}

float Vector4::SquaredLength() const
{
    return
        m_tuple[0] * m_tuple[0] +
        m_tuple[1] * m_tuple[1] +
        m_tuple[2] * m_tuple[2] +
        m_tuple[3] * m_tuple[3];
}
float Vector4::Dot(const Vector4& v) const
{
    return
        m_tuple[0] * v.m_tuple[0] +
        m_tuple[1] * v.m_tuple[1] +
        m_tuple[2] * v.m_tuple[2] +
        m_tuple[3] * v.m_tuple[3];
}

Vector4 Vector4::Normalize() const
{
    float length = Length();

    Vector4 v;
    if (length > Math::ZERO_TOLERANCE)
    {
        //float invLength = ((float)1.0) / length;
        v.m_tuple[0] = m_tuple[0] / length;
        v.m_tuple[1] = m_tuple[1] / length;
        v.m_tuple[2] = m_tuple[2] / length;
        v.m_tuple[3] = m_tuple[3] / length;
    }
    else
    {
        v.m_tuple[0] = (float)0.0;
        v.m_tuple[1] = (float)0.0;
        v.m_tuple[2] = (float)0.0;
        v.m_tuple[3] = (float)0.0;
    }

    return v;
}

namespace Enigma::MathLib
{
    Vector4 operator* (float scalar, const Vector4& v)
    {
        return Vector4(
            scalar * v[0],
            scalar * v[1],
            scalar * v[2],
            scalar * v[3]);
    }
}