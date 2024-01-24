#include "Matrix2.h"
#include "MathGlobal.h"
#include <cstring>
#include <cmath>
#include <cassert>

using namespace Enigma::MathLib;

const Matrix2 Matrix2::ZERO(
    0.0f, 0.0f,
    0.0f, 0.0f);
const Matrix2 Matrix2::IDENTITY(
    1.0f, 0.0f,
    0.0f, 1.0f);

Matrix2::Matrix2(bool isZero)
{
    if (isZero)
    {
        memset(m_entry, 0, 4 * sizeof(float));
    }
    else
    {
        m_11 = 1.0f;
        m_12 = 0.0f;
        m_21 = 0.0f;
        m_22 = 1.0f;
    }
}

Matrix2::Matrix2(float m00, float m01, float m10, float m11)
{
    m_11 = m00;
    m_12 = m01;
    m_21 = m10;
    m_22 = m11;
}

Matrix2::Matrix2(const float entry[4])
{
    m_11 = entry[0];
    m_12 = entry[1];
    m_21 = entry[2];
    m_22 = entry[3];
}

Matrix2::Matrix2(float m00, float m11)
{
    m_11 = m00;
    m_12 = 0.0f;
    m_21 = 0.0f;
    m_22 = m11;
}

Matrix2::Matrix2(float angle)
{
    m_entry[0][0] = cos(angle);
    m_entry[0][1] = -sin(angle);
    m_entry[1][0] = -m_entry[0][1];
    m_entry[1][1] = m_entry[0][0];
}

Matrix2 Matrix2::MakeZero()
{
    return Matrix2(true);
}

Matrix2 Matrix2::MakeIdentity()
{
    return Matrix2(false);
}

Matrix2 Matrix2::MakeDiagonal(float m00, float m11)
{
    return Matrix2(m00, m11);
}

Matrix2 Matrix2::FromAngle(float angle)
{
    return Matrix2(angle);
}

Matrix2::operator const float*() const
{
    return &m_entry[0][0];
}

Matrix2::operator float*()
{
    return &m_entry[0][0];
}

const float* Matrix2::operator[](int row) const
{
    return &m_entry[row][0];
}

float* Matrix2::operator[](int row)
{
    return &m_entry[row][0];
}

float Matrix2::operator()(int row, int col) const
{
    return m_entry[row][col];
}

float& Matrix2::operator()(int row, int col)
{
    return m_entry[row][col];
}

void Matrix2::SetRow(int row, const Vector2& v)
{
    assert((row < 2) && (row >= 0));
    m_entry[row][0] = v[0];
    m_entry[row][1] = v[1];
}

Vector2 Matrix2::GetRow(int row) const
{
    assert((row < 2) && (row >= 0));
    Vector2 v = Vector2(m_entry[row][0], m_entry[row][1]);
    return v;
}

void Matrix2::SetColumn(int col, const Vector2& v)
{
    assert((col < 2) && (col >= 0));
    m_entry[0][col] = v[0];
    m_entry[1][col] = v[1];
}

Vector2 Matrix2::GetColumn(int col) const
{
    assert((col < 2) && (col >= 0));
    Vector2 v = Vector2(m_entry[0][col], m_entry[1][col]);
    return v;
}

void Matrix2::GetColumnMajor(float* col_major) const
{
    for (int row = 0, i = 0; row < 2; row++)
    {
        for (int col = 0; col < 2; col++)
            col_major[i++] = m_entry[col][row]; // 轉置過來取數值
    }
}

bool Matrix2::operator ==(const Matrix2& mx) const
{
    if ((Math::IsEqual(m_11, mx.m_11)) && (Math::IsEqual(m_12, mx.m_12))
        && (Math::IsEqual(m_21, mx.m_21)) && (Math::IsEqual(m_22, mx.m_22)))
        return true;
    return false;
}

bool Matrix2::operator !=(const Matrix2& mx) const
{
    if (!((Math::IsEqual(m_11, mx.m_11)) && (Math::IsEqual(m_12, mx.m_12))
        && (Math::IsEqual(m_21, mx.m_21)) && (Math::IsEqual(m_22, mx.m_22))))
        return true;
    return false;
}

int Matrix2::compareArrays(const Matrix2& mx) const
{
    return memcmp(m_entry, mx.m_entry, 4 * sizeof(float));
}

bool Matrix2::operator<(const Matrix2& mx) const
{
    return compareArrays(mx) < 0;
}

bool Matrix2::operator<=(const Matrix2& mx) const
{
    return compareArrays(mx) <= 0;
}

bool Matrix2::operator>(const Matrix2& mx) const
{
    return compareArrays(mx) > 0;
}

bool Matrix2::operator>=(const Matrix2& mx) const
{
    return compareArrays(mx) >= 0;
}

Matrix2 Matrix2::operator+(const Matrix2& mx) const
{
    Matrix2 sum;
    sum.m_11 = m_11 + mx.m_11;
    sum.m_12 = m_12 + mx.m_12;
    sum.m_21 = m_21 + mx.m_21;
    sum.m_22 = m_22 + mx.m_22;
    return sum;
}

Matrix2 Matrix2::operator-(const Matrix2& mx) const
{
    Matrix2 diff;
    diff.m_11 = m_11 - mx.m_11;
    diff.m_12 = m_12 - mx.m_12;
    diff.m_21 = m_21 - mx.m_21;
    diff.m_22 = m_22 - mx.m_22;
    return diff;
}

Matrix2 Matrix2::operator*(const Matrix2& mx) const
{
    Matrix2 prod;
    prod.m_11 = m_11 * mx.m_11 + m_12 * mx.m_21;
    prod.m_12 = m_11 * mx.m_12 + m_12 * mx.m_22;
    prod.m_21 = m_21 * mx.m_11 + m_22 * mx.m_21;
    prod.m_22 = m_21 * mx.m_12 + m_22 * mx.m_22;
    return prod;
}

Matrix2 Matrix2::operator*(float scalar) const
{
    Matrix2 prod;
    prod.m_11 = scalar * m_11;
    prod.m_12 = scalar * m_12;
    prod.m_21 = scalar * m_21;
    prod.m_22 = scalar * m_22;
    return prod;
}

Matrix2 Matrix2::operator/(float scalar) const
{
    Matrix2 quot;
    if (scalar != 0.0f)
    {
        //float invScalar = 1.0f / scalar;
        quot.m_11 = m_11 / scalar;
        quot.m_12 = m_12 / scalar;
        quot.m_21 = m_21 / scalar;
        quot.m_22 = m_22 / scalar;
    }
    else
    {
        quot.m_11 = Math::MAX_FLOAT;
        quot.m_12 = Math::MAX_FLOAT;
        quot.m_21 = Math::MAX_FLOAT;
        quot.m_22 = Math::MAX_FLOAT;
    }
    return quot;
}

Matrix2 Matrix2::operator-() const
{
    Matrix2 neg;
    neg.m_11 = -m_11;
    neg.m_12 = -m_12;
    neg.m_21 = -m_21;
    neg.m_22 = -m_22;
    return neg;
}

Matrix2& Matrix2::operator+=(const Matrix2& mx)
{
    m_11 += mx.m_11;
    m_12 += mx.m_12;
    m_21 += mx.m_21;
    m_22 += mx.m_22;
    return *this;
}

Matrix2& Matrix2::operator-=(const Matrix2& mx)
{
    m_11 -= mx.m_11;
    m_12 -= mx.m_12;
    m_21 -= mx.m_21;
    m_22 -= mx.m_22;
    return *this;
}

Matrix2& Matrix2::operator*=(float scalar)
{
    m_11 *= scalar;
    m_12 *= scalar;
    m_21 *= scalar;
    m_22 *= scalar;
    return *this;
}

Matrix2& Matrix2::operator/=(float scalar)
{
    if (scalar != (float)0.0)
    {
        //float invScalar = (float)1.0 / scalar;
        m_11 /= scalar;
        m_12 /= scalar;
        m_21 /= scalar;
        m_22 /= scalar;
    }
    else
    {
        m_11 = Math::MAX_FLOAT;
        m_12 = Math::MAX_FLOAT;
        m_21 = Math::MAX_FLOAT;
        m_22 = Math::MAX_FLOAT;
    }
    return *this;
}

Vector2 Matrix2::operator*(const Vector2& v) const
{
    Vector2 prod;
    prod.x() = m_11 * v.x() + m_12 * v.y();
    prod.y() = m_21 * v.x() + m_22 * v.y();
    return prod;
}

Matrix2 Matrix2::Transpose() const
{
    Matrix2 transpose;
    transpose.m_11 = m_11;
    transpose.m_12 = m_21;
    transpose.m_21 = m_12;
    transpose.m_22 = m_22;
    return transpose;
}

Matrix2 Matrix2::Inverse() const
{
    Matrix2 inverse;

    float det = m_11 * m_22 - m_12 * m_21;
    if (fabs(det) > Math::ZERO_TOLERANCE)
    {
        //float invDet = (float)1.0 / det;
        inverse.m_11 = m_22 / det;
        inverse.m_12 = -m_12 / det;
        inverse.m_21 = -m_21 / det;
        inverse.m_22 = m_11 / det;
    }
    else
    {
        inverse.MakeZero();
    }
    return inverse;
}

Matrix2 Matrix2::Adjoint() const
{
    return Matrix2(m_22, -m_12, -m_21, m_11);
}

float Matrix2::Determinant() const
{
    return m_11 * m_22 - m_21 * m_12;
}

float Matrix2::ToAngle() const
{
    // assert:  matrix is a rotation
    return atan2(m_21, m_11);
}

namespace Enigma::MathLib
{
    Matrix2 operator*(float scalar, const Matrix2& mx)
    {
        return mx * scalar;
    }
}
EigenDecompose<Matrix2> Matrix2::EigenDecomposition() const
{
    Matrix2 rot;
    Matrix2 diag;
    float trace = m_11 + m_22;
    float diff = m_11 - m_22;
    float discr = sqrt(diff * diff + ((float)4.0) * m_12 * m_12);
    float eigVal0 = ((float)0.5) * (trace - discr);
    float eigVal1 = ((float)0.5) * (trace + discr);
    diag = MakeDiagonal(eigVal0, eigVal1);

    float cos_value, sin_value;
    if (diff >= (float)0.0)
    {
        cos_value = m_12;
        sin_value = eigVal0 - m_11;
    }
    else
    {
        cos_value = eigVal0 - m_22;
        sin_value = m_12;
    }
    float tmp = sqrt(cos_value * cos_value + sin_value * sin_value);
    cos_value /= tmp;
    sin_value /= tmp;

    rot.m_11 = cos_value;
    rot.m_12 = -sin_value;
    rot.m_21 = sin_value;
    rot.m_22 = cos_value;

    return { rot, diag };
}
