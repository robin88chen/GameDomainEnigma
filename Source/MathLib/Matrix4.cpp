#include "Matrix4.h"
#include "Matrix4.h"
#include "Matrix4.h"
#include "Matrix4.h"
#include "Matrix3.h"
#include "MathGlobal.h"
#include "Quaternion.h"
#include <cstring>
#include <cassert>
#include <cmath>

using namespace Enigma::MathLib;

const Matrix4 Matrix4::ZERO(
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f);
const Matrix4 Matrix4::IDENTITY(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f);

Matrix4::Matrix4(bool isZero)
{
    if (isZero)
    {
        memset(m_entry, 0, sizeof(m_entry));
    }
    else
    {
        m_11 = (float)1.0;
        m_12 = (float)0.0;
        m_13 = (float)0.0;
        m_14 = (float)0.0;
        m_21 = (float)0.0;
        m_22 = (float)1.0;
        m_23 = (float)0.0;
        m_24 = (float)0.0;
        m_31 = (float)0.0;
        m_32 = (float)0.0;
        m_33 = (float)1.0;
        m_34 = (float)0.0;
        m_41 = (float)0.0;
        m_42 = (float)0.0;
        m_43 = (float)0.0;
        m_44 = (float)1.0;
    }
}

Matrix4::Matrix4(float m00, float m01, float m02, float m03,
                 float m10, float m11, float m12, float m13,
                 float m20, float m21, float m22, float m23,
                 float m30, float m31, float m32, float m33)
{
    m_11 = m00;
    m_12 = m01;
    m_13 = m02;
    m_14 = m03;
    m_21 = m10;
    m_22 = m11;
    m_23 = m12;
    m_24 = m13;
    m_31 = m20;
    m_32 = m21;
    m_33 = m22;
    m_34 = m23;
    m_41 = m30;
    m_42 = m31;
    m_43 = m32;
    m_44 = m33;
}

Matrix4::Matrix4(const float entry[16])
{
    m_11 = entry[0];
    m_12 = entry[1];
    m_13 = entry[2];
    m_14 = entry[3];
    m_21 = entry[4];
    m_22 = entry[5];
    m_23 = entry[6];
    m_24 = entry[7];
    m_31 = entry[8];
    m_32 = entry[9];
    m_33 = entry[10];
    m_34 = entry[11];
    m_41 = entry[12];
    m_42 = entry[13];
    m_43 = entry[14];
    m_44 = entry[15];
}

Matrix4::Matrix4(const Vector3& u, const Vector3& v,
                 const Vector3& w, const Vector3& pos, bool isColumns)
{
    if (isColumns)
    {
        m_11 = u[0];
        m_12 = v[0];
        m_13 = w[0];
        m_14 = pos[0];
        m_21 = u[1];
        m_22 = v[1];
        m_23 = w[1];
        m_24 = pos[1];
        m_31 = u[2];
        m_32 = v[2];
        m_33 = w[2];
        m_34 = pos[2];
        m_41 = (float)0.0;
        m_42 = (float)0.0;
        m_43 = (float)0.0;
        m_44 = (float)1.0;
    }
    else
    {
        m_11 = u[0];
        m_12 = u[1];
        m_13 = u[2];
        m_14 = pos[0];
        m_21 = v[0];
        m_22 = v[1];
        m_23 = v[2];
        m_24 = pos[1];
        m_31 = w[0];
        m_32 = w[1];
        m_33 = w[2];
        m_34 = pos[2];
        m_41 = (float)0.0;
        m_42 = (float)0.0;
        m_43 = (float)0.0;
        m_44 = (float)1.0;
    }
}

Matrix4::Matrix4(const Vector3* vec, bool isColumns)
{
    if (isColumns)
    {
        m_11 = vec[0][0];
        m_12 = vec[1][0];
        m_13 = vec[2][0];
        m_14 = vec[3][0];
        m_21 = vec[0][1];
        m_22 = vec[1][1];
        m_23 = vec[2][1];
        m_24 = vec[3][1];
        m_31 = vec[0][2];
        m_32 = vec[1][2];
        m_33 = vec[2][2];
        m_34 = vec[3][2];
        m_41 = (float)0.0;
        m_42 = (float)0.0;
        m_43 = (float)0.0;
        m_44 = (float)1.0;
    }
    else
    {
        m_11 = vec[0][0];
        m_12 = vec[0][1];
        m_13 = vec[0][2];
        m_14 = vec[3][0];
        m_21 = vec[1][0];
        m_22 = vec[1][1];
        m_23 = vec[1][2];
        m_24 = vec[3][1];
        m_31 = vec[2][0];
        m_32 = vec[2][1];
        m_33 = vec[2][2];
        m_34 = vec[3][2];
        m_41 = (float)0.0;
        m_42 = (float)0.0;
        m_43 = (float)0.0;
        m_44 = (float)1.0;
    }
}

Matrix4::Matrix4(const Matrix3& mx)
{
    operator=(mx);
}

Matrix4 Matrix4::MakeZero()
{
    return Matrix4(true);
}

Matrix4 Matrix4::MakeIdentity()
{
    return Matrix4(false);
}

Matrix4::operator const float*() const
{
    return &m_entry[0][0];
}

Matrix4::operator float*()
{
    return &m_entry[0][0];
}

const float* Matrix4::operator[](int row) const
{
    return &m_entry[row][0];
}

float* Matrix4::operator[](int row)
{
    return &m_entry[row][0];
}

float Matrix4::operator()(int row, int col) const
{
    return m_entry[row][col];
}

float& Matrix4::operator()(int row, int col)
{
    return m_entry[row][col];
}

void Matrix4::SetRow(int row, const Vector4& vec)
{
    assert((row < 4) && (row >= 0));
    m_entry[row][0] = vec[0];
    m_entry[row][1] = vec[1];
    m_entry[row][2] = vec[2];
    m_entry[row][3] = vec[3];
}

Vector4 Matrix4::GetRow(int row) const
{
    assert((row < 4) && (row >= 0));
    Vector4 v = Vector4(m_entry[row][0], m_entry[row][1], m_entry[row][2], m_entry[row][3]);
    return v;
}

void Matrix4::SetColumn(int col, const Vector4& vec)
{
    assert((col < 4) && (col >= 0));
    m_entry[0][col] = vec[0];
    m_entry[1][col] = vec[1];
    m_entry[2][col] = vec[2];
    m_entry[3][col] = vec[3];
}

Vector4 Matrix4::GetColumn(int col) const
{
    assert((col < 4) && (col >= 0));
    Vector4 v = Vector4(m_entry[0][col], m_entry[1][col], m_entry[2][col], m_entry[3][col]);
    return v;
}

void Matrix4::GetColumnMajor(float* colMajor) const
{
    for (int row = 0, i = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            colMajor[i++] = m_entry[col][row]; // 轉置過來取數值
    }
}

Matrix4& Matrix4::operator =(const Matrix3& mx)
{
    m_11 = mx[0][0];
    m_12 = mx[0][1];
    m_13 = mx[0][2];
    m_14 = (float)0.0;
    m_21 = mx[1][0];
    m_22 = mx[1][1];
    m_23 = mx[1][2];
    m_24 = (float)0.0;
    m_31 = mx[2][0];
    m_32 = mx[2][1];
    m_33 = mx[2][2];
    m_34 = (float)0.0;
    m_41 = (float)0.0;
    m_42 = (float)0.0;
    m_43 = (float)0.0;
    m_44 = (float)1.0;
    return *this;
}

bool Matrix4::operator ==(const Matrix4& mx) const
{
    if ((Math::IsEqual(m_11, mx.m_11)) && (Math::IsEqual(m_12, mx.m_12))
        && (Math::IsEqual(m_13, mx.m_13)) && (Math::IsEqual(m_14, mx.m_14))
        && (Math::IsEqual(m_21, mx.m_21)) && (Math::IsEqual(m_22, mx.m_22))
        && (Math::IsEqual(m_23, mx.m_23)) && (Math::IsEqual(m_24, mx.m_24))
        && (Math::IsEqual(m_31, mx.m_31)) && (Math::IsEqual(m_32, mx.m_32))
        && (Math::IsEqual(m_33, mx.m_33)) && (Math::IsEqual(m_34, mx.m_34))
        && (Math::IsEqual(m_41, mx.m_41)) && (Math::IsEqual(m_42, mx.m_42))
        && (Math::IsEqual(m_43, mx.m_43)) && (Math::IsEqual(m_44, mx.m_44)))
        return true;
    return false;
}

bool Matrix4::operator !=(const Matrix4& mx) const
{
    if (!((Math::IsEqual(m_11, mx.m_11)) && (Math::IsEqual(m_12, mx.m_12))
        && (Math::IsEqual(m_13, mx.m_13)) && (Math::IsEqual(m_14, mx.m_14))
        && (Math::IsEqual(m_21, mx.m_21)) && (Math::IsEqual(m_22, mx.m_22))
        && (Math::IsEqual(m_23, mx.m_23)) && (Math::IsEqual(m_24, mx.m_24))
        && (Math::IsEqual(m_31, mx.m_31)) && (Math::IsEqual(m_32, mx.m_32))
        && (Math::IsEqual(m_33, mx.m_33)) && (Math::IsEqual(m_34, mx.m_34))
        && (Math::IsEqual(m_41, mx.m_41)) && (Math::IsEqual(m_42, mx.m_42))
        && (Math::IsEqual(m_43, mx.m_43)) && (Math::IsEqual(m_44, mx.m_44))))
        return true;
    return false;
}

int Matrix4::CompareArrays(const Matrix4& mx) const
{
    return memcmp(m_entry, mx.m_entry, sizeof(m_entry));
}

bool Matrix4::operator<(const Matrix4& mx) const
{
    return CompareArrays(mx) < 0;
}

bool Matrix4::operator<=(const Matrix4& mx) const
{
    return CompareArrays(mx) <= 0;
}

bool Matrix4::operator>(const Matrix4& mx) const
{
    return CompareArrays(mx) > 0;
}

bool Matrix4::operator>=(const Matrix4& mx) const
{
    return CompareArrays(mx) >= 0;
}

Matrix4 Matrix4::operator+(const Matrix4& mx) const
{
    Matrix4 sum;
    sum.m_11 = m_11 + mx.m_11;
    sum.m_12 = m_12 + mx.m_12;
    sum.m_13 = m_13 + mx.m_13;
    sum.m_14 = m_14 + mx.m_14;
    sum.m_21 = m_21 + mx.m_21;
    sum.m_22 = m_22 + mx.m_22;
    sum.m_23 = m_23 + mx.m_23;
    sum.m_24 = m_24 + mx.m_24;
    sum.m_31 = m_31 + mx.m_31;
    sum.m_32 = m_32 + mx.m_32;
    sum.m_33 = m_33 + mx.m_33;
    sum.m_34 = m_34 + mx.m_34;
    sum.m_41 = m_41 + mx.m_41;
    sum.m_42 = m_42 + mx.m_42;
    sum.m_43 = m_43 + mx.m_43;
    sum.m_44 = m_44 + mx.m_44;
    return sum;
}

Matrix4 Matrix4::operator-(const Matrix4& mx) const
{
    Matrix4 diff;
    diff.m_11 = m_11 - mx.m_11;
    diff.m_12 = m_12 - mx.m_12;
    diff.m_13 = m_13 - mx.m_13;
    diff.m_14 = m_14 - mx.m_14;
    diff.m_21 = m_21 - mx.m_21;
    diff.m_22 = m_22 - mx.m_22;
    diff.m_23 = m_23 - mx.m_23;
    diff.m_24 = m_24 - mx.m_24;
    diff.m_31 = m_31 - mx.m_31;
    diff.m_32 = m_32 - mx.m_32;
    diff.m_33 = m_33 - mx.m_33;
    diff.m_34 = m_34 - mx.m_34;
    diff.m_41 = m_41 - mx.m_41;
    diff.m_42 = m_42 - mx.m_42;
    diff.m_43 = m_43 - mx.m_43;
    diff.m_44 = m_44 - mx.m_44;
    return diff;
}

Matrix4 Matrix4::operator*(const Matrix4& mx) const
{
    Matrix4 prod;
    prod.m_11 = m_11 * mx.m_11 + m_12 * mx.m_21 + m_13 * mx.m_31 + m_14 * mx.m_41;
    prod.m_12 = m_11 * mx.m_12 + m_12 * mx.m_22 + m_13 * mx.m_32 + m_14 * mx.m_42;
    prod.m_13 = m_11 * mx.m_13 + m_12 * mx.m_23 + m_13 * mx.m_33 + m_14 * mx.m_43;
    prod.m_14 = m_11 * mx.m_14 + m_12 * mx.m_24 + m_13 * mx.m_34 + m_14 * mx.m_44;
    prod.m_21 = m_21 * mx.m_11 + m_22 * mx.m_21 + m_23 * mx.m_31 + m_24 * mx.m_41;
    prod.m_22 = m_21 * mx.m_12 + m_22 * mx.m_22 + m_23 * mx.m_32 + m_24 * mx.m_42;
    prod.m_23 = m_21 * mx.m_13 + m_22 * mx.m_23 + m_23 * mx.m_33 + m_24 * mx.m_43;
    prod.m_24 = m_21 * mx.m_14 + m_22 * mx.m_24 + m_23 * mx.m_34 + m_24 * mx.m_44;
    prod.m_31 = m_31 * mx.m_11 + m_32 * mx.m_21 + m_33 * mx.m_31 + m_34 * mx.m_41;
    prod.m_32 = m_31 * mx.m_12 + m_32 * mx.m_22 + m_33 * mx.m_32 + m_34 * mx.m_42;
    prod.m_33 = m_31 * mx.m_13 + m_32 * mx.m_23 + m_33 * mx.m_33 + m_34 * mx.m_43;
    prod.m_34 = m_31 * mx.m_14 + m_32 * mx.m_24 + m_33 * mx.m_34 + m_34 * mx.m_44;
    prod.m_41 = m_41 * mx.m_11 + m_42 * mx.m_21 + m_43 * mx.m_31 + m_44 * mx.m_41;
    prod.m_42 = m_41 * mx.m_12 + m_42 * mx.m_22 + m_43 * mx.m_32 + m_44 * mx.m_42;
    prod.m_43 = m_41 * mx.m_13 + m_42 * mx.m_23 + m_43 * mx.m_33 + m_44 * mx.m_43;
    prod.m_44 = m_41 * mx.m_14 + m_42 * mx.m_24 + m_43 * mx.m_34 + m_44 * mx.m_44;
    return prod;
}

Matrix4 Matrix4::operator*(float scalar) const
{
    Matrix4 prod;
    prod.m_11 = scalar * m_11;
    prod.m_12 = scalar * m_12;
    prod.m_13 = scalar * m_13;
    prod.m_14 = scalar * m_14;
    prod.m_21 = scalar * m_21;
    prod.m_22 = scalar * m_22;
    prod.m_23 = scalar * m_23;
    prod.m_24 = scalar * m_24;
    prod.m_31 = scalar * m_31;
    prod.m_32 = scalar * m_32;
    prod.m_33 = scalar * m_33;
    prod.m_34 = scalar * m_34;
    prod.m_41 = scalar * m_41;
    prod.m_42 = scalar * m_42;
    prod.m_43 = scalar * m_43;
    prod.m_44 = scalar * m_44;
    return prod;
}

Matrix4 Matrix4::operator/(float scalar) const
{
    Matrix4 quot;
    if (scalar != (float)0.0)
    {
        //float invScalar = (float)1.0 / scalar;
        quot.m_11 = m_11 / scalar;
        quot.m_12 = m_12 / scalar;
        quot.m_13 = m_13 / scalar;
        quot.m_14 = m_14 / scalar;
        quot.m_21 = m_21 / scalar;
        quot.m_22 = m_22 / scalar;
        quot.m_23 = m_23 / scalar;
        quot.m_24 = m_24 / scalar;
        quot.m_31 = m_31 / scalar;
        quot.m_32 = m_32 / scalar;
        quot.m_33 = m_33 / scalar;
        quot.m_34 = m_34 / scalar;
        quot.m_41 = m_41 / scalar;
        quot.m_42 = m_42 / scalar;
        quot.m_43 = m_43 / scalar;
        quot.m_44 = m_44 / scalar;
    }
    else
    {
        quot.m_11 = Math::MAX_FLOAT;
        quot.m_12 = Math::MAX_FLOAT;
        quot.m_13 = Math::MAX_FLOAT;
        quot.m_14 = Math::MAX_FLOAT;
        quot.m_21 = Math::MAX_FLOAT;
        quot.m_22 = Math::MAX_FLOAT;
        quot.m_23 = Math::MAX_FLOAT;
        quot.m_24 = Math::MAX_FLOAT;
        quot.m_31 = Math::MAX_FLOAT;
        quot.m_32 = Math::MAX_FLOAT;
        quot.m_33 = Math::MAX_FLOAT;
        quot.m_34 = Math::MAX_FLOAT;
        quot.m_41 = Math::MAX_FLOAT;
        quot.m_42 = Math::MAX_FLOAT;
        quot.m_43 = Math::MAX_FLOAT;
        quot.m_44 = Math::MAX_FLOAT;
    }
    return quot;
}

Matrix4 Matrix4::operator-() const
{
    Matrix4 neg;
    neg.m_11 = -m_11;
    neg.m_12 = -m_12;
    neg.m_13 = -m_13;
    neg.m_14 = -m_14;
    neg.m_21 = -m_21;
    neg.m_22 = -m_22;
    neg.m_23 = -m_23;
    neg.m_24 = -m_24;
    neg.m_31 = -m_31;
    neg.m_32 = -m_32;
    neg.m_33 = -m_33;
    neg.m_34 = -m_34;
    neg.m_41 = -m_41;
    neg.m_42 = -m_42;
    neg.m_43 = -m_43;
    neg.m_44 = -m_44;
    return neg;
}

Matrix4& Matrix4::operator+=(const Matrix4& mx)
{
    m_11 += mx.m_11;
    m_12 += mx.m_12;
    m_13 += mx.m_13;
    m_14 += mx.m_14;
    m_21 += mx.m_21;
    m_22 += mx.m_22;
    m_23 += mx.m_23;
    m_24 += mx.m_24;
    m_31 += mx.m_31;
    m_32 += mx.m_32;
    m_33 += mx.m_33;
    m_34 += mx.m_34;
    m_41 += mx.m_41;
    m_42 += mx.m_42;
    m_43 += mx.m_43;
    m_44 += mx.m_44;
    return *this;
}

Matrix4& Matrix4::operator-=(const Matrix4& mx)
{
    m_11 -= mx.m_11;
    m_12 -= mx.m_12;
    m_13 -= mx.m_13;
    m_14 -= mx.m_14;
    m_21 -= mx.m_21;
    m_22 -= mx.m_22;
    m_23 -= mx.m_23;
    m_24 -= mx.m_24;
    m_31 -= mx.m_31;
    m_32 -= mx.m_32;
    m_33 -= mx.m_33;
    m_34 -= mx.m_34;
    m_41 -= mx.m_41;
    m_42 -= mx.m_42;
    m_43 -= mx.m_43;
    m_44 -= mx.m_44;
    return *this;
}

Matrix4& Matrix4::operator*=(float scalar)
{
    m_11 *= scalar;
    m_12 *= scalar;
    m_13 *= scalar;
    m_14 *= scalar;
    m_21 *= scalar;
    m_22 *= scalar;
    m_23 *= scalar;
    m_24 *= scalar;
    m_31 *= scalar;
    m_32 *= scalar;
    m_33 *= scalar;
    m_34 *= scalar;
    m_41 *= scalar;
    m_42 *= scalar;
    m_43 *= scalar;
    m_44 *= scalar;
    return *this;
}

Matrix4& Matrix4::operator/=(float scalar)
{
    if (scalar != (float)0.0)
    {
        //float invScalar = (float)1.0 / scalar;
        m_11 /= scalar;
        m_12 /= scalar;
        m_13 /= scalar;
        m_14 /= scalar;
        m_21 /= scalar;
        m_22 /= scalar;
        m_23 /= scalar;
        m_24 /= scalar;
        m_31 /= scalar;
        m_32 /= scalar;
        m_33 /= scalar;
        m_34 /= scalar;
        m_41 /= scalar;
        m_42 /= scalar;
        m_43 /= scalar;
        m_44 /= scalar;
    }
    else
    {
        m_11 = Math::MAX_FLOAT;
        m_12 = Math::MAX_FLOAT;
        m_13 = Math::MAX_FLOAT;
        m_14 = Math::MAX_FLOAT;
        m_21 = Math::MAX_FLOAT;
        m_22 = Math::MAX_FLOAT;
        m_23 = Math::MAX_FLOAT;
        m_24 = Math::MAX_FLOAT;
        m_31 = Math::MAX_FLOAT;
        m_32 = Math::MAX_FLOAT;
        m_33 = Math::MAX_FLOAT;
        m_34 = Math::MAX_FLOAT;
        m_41 = Math::MAX_FLOAT;
        m_42 = Math::MAX_FLOAT;
        m_43 = Math::MAX_FLOAT;
        m_44 = Math::MAX_FLOAT;
    }
    return *this;
}

Vector4 Matrix4::operator*(const Vector4& vec) const
{
    Vector4 prod;
    prod.X() = m_11 * vec.X() + m_12 * vec.Y() + m_13 * vec.Z() + m_14 * vec.W();
    prod.Y() = m_21 * vec.X() + m_22 * vec.Y() + m_23 * vec.Z() + m_24 * vec.W();
    prod.Z() = m_31 * vec.X() + m_32 * vec.Y() + m_33 * vec.Z() + m_34 * vec.W();
    prod.W() = m_41 * vec.X() + m_42 * vec.Y() + m_43 * vec.Z() + m_44 * vec.W();
    return prod;
}

Vector3 Matrix4::operator*(const Vector3& vec) const
{
    Vector3 prod;
    float invW = (float)1.0 / (m_41 * vec.X() + m_42 * vec.Y() + m_43 * vec.Z() + m_44);
    prod.X() = (m_11 * vec.X() + m_12 * vec.Y() + m_13 * vec.Z() + m_14) * invW;
    prod.Y() = (m_21 * vec.X() + m_22 * vec.Y() + m_23 * vec.Z() + m_24) * invW;
    prod.Z() = (m_31 * vec.X() + m_32 * vec.Y() + m_33 * vec.Z() + m_34) * invW;
    return prod;
}

Matrix4 Matrix4::Transpose() const
{
    Matrix4 transpose;
    transpose.m_11 = m_11;
    transpose.m_12 = m_21;
    transpose.m_13 = m_31;
    transpose.m_14 = m_41;
    transpose.m_21 = m_12;
    transpose.m_22 = m_22;
    transpose.m_23 = m_32;
    transpose.m_24 = m_42;
    transpose.m_31 = m_13;
    transpose.m_32 = m_23;
    transpose.m_33 = m_33;
    transpose.m_34 = m_43;
    transpose.m_41 = m_14;
    transpose.m_42 = m_24;
    transpose.m_43 = m_34;
    transpose.m_44 = m_44;
    return transpose;
}

float Matrix4::MinorDet(const int r0, const int r1, const int r2, const int c0, const int c1, const int c2) const
{
    return m_entry[r0][c0] * (m_entry[r1][c1] * m_entry[r2][c2] - m_entry[r2][c1] * m_entry[r1][c2]) -
        m_entry[r0][c1] * (m_entry[r1][c0] * m_entry[r2][c2] - m_entry[r2][c0] * m_entry[r1][c2]) +
        m_entry[r0][c2] * (m_entry[r1][c0] * m_entry[r2][c1] - m_entry[r2][c0] * m_entry[r1][c1]);
};

Matrix4 Matrix4::Adjoint() const
{
    return Matrix4(MinorDet(1, 2, 3, 1, 2, 3),
                   -MinorDet(0, 2, 3, 1, 2, 3),
                   MinorDet(0, 1, 3, 1, 2, 3),
                   -MinorDet(0, 1, 2, 1, 2, 3),

                   -MinorDet(1, 2, 3, 0, 2, 3),
                   MinorDet(0, 2, 3, 0, 2, 3),
                   -MinorDet(0, 1, 3, 0, 2, 3),
                   MinorDet(0, 1, 2, 0, 2, 3),

                   MinorDet(1, 2, 3, 0, 1, 3),
                   -MinorDet(0, 2, 3, 0, 1, 3),
                   MinorDet(0, 1, 3, 0, 1, 3),
                   -MinorDet(0, 1, 2, 0, 1, 3),

                   -MinorDet(1, 2, 3, 0, 1, 2),
                   MinorDet(0, 2, 3, 0, 1, 2),
                   -MinorDet(0, 1, 3, 0, 1, 2),
                   MinorDet(0, 1, 2, 0, 1, 2));
}

float Matrix4::Determinant() const
{
    return m_entry[0][0] * MinorDet(1, 2, 3, 1, 2, 3) -
        m_entry[0][1] * MinorDet(1, 2, 3, 0, 2, 3) +
        m_entry[0][2] * MinorDet(1, 2, 3, 0, 1, 3) -
        m_entry[0][3] * MinorDet(1, 2, 3, 0, 1, 2);
}

Matrix4 Matrix4::Inverse() const
{
    return Adjoint() * ((float)1.0 / Determinant());
}

Vector3 Matrix4::TransformCoord(const Vector3& vec) const
{
    Vector3 prod;
    float invW = (float)1.0 / (m_41 * vec.X() + m_42 * vec.Y() + m_43 * vec.Z() + m_44);
    prod.X() = (m_11 * vec.X() + m_12 * vec.Y() + m_13 * vec.Z() + m_14) * invW;
    prod.Y() = (m_21 * vec.X() + m_22 * vec.Y() + m_23 * vec.Z() + m_24) * invW;
    prod.Z() = (m_31 * vec.X() + m_32 * vec.Y() + m_33 * vec.Z() + m_34) * invW;
    return prod;
}

Vector3 Matrix4::Transform(const Vector3& vec) const
{
    Vector3 prod;
    prod.X() = (m_11 * vec.X() + m_12 * vec.Y() + m_13 * vec.Z() + m_14);
    prod.Y() = (m_21 * vec.X() + m_22 * vec.Y() + m_23 * vec.Z() + m_24);
    prod.Z() = (m_31 * vec.X() + m_32 * vec.Y() + m_33 * vec.Z() + m_34);
    return prod;
}

Vector3 Matrix4::TransformVector(const Vector3& vec) const
{
    Vector3 prod;
    prod.X() = (m_11 * vec.X() + m_12 * vec.Y() + m_13 * vec.Z());
    prod.Y() = (m_21 * vec.X() + m_22 * vec.Y() + m_23 * vec.Z());
    prod.Z() = (m_31 * vec.X() + m_32 * vec.Y() + m_33 * vec.Z());
    return prod;
}

std::tuple<Vector3, float> Matrix4::TransformVectorNormalized(const Vector3& vec) const
{
    Vector3 prod;
    prod.X() = (m_11 * vec.X() + m_12 * vec.Y() + m_13 * vec.Z());
    prod.Y() = (m_21 * vec.X() + m_22 * vec.Y() + m_23 * vec.Z());
    prod.Z() = (m_31 * vec.X() + m_32 * vec.Y() + m_33 * vec.Z());
    float length = prod.Length();
    prod = prod.Normalize();
    return { prod, length };
}

Matrix4 Matrix4::MakeTranslateTransform(const float tx, const float ty, const float tz)
{
    Matrix4 mx = MakeIdentity();
    mx.m_14 = tx;
    mx.m_24 = ty;
    mx.m_34 = tz;
    return mx;
}

Matrix4 Matrix4::MakeTranslateTransform(const Vector3& vec)
{
    Matrix4 mx = MakeIdentity();
    mx.m_14 = vec.X();
    mx.m_24 = vec.Y();
    mx.m_34 = vec.Z();
    return mx;
}

Matrix4 Matrix4::MakeScaleTransform(const float sx, const float sy, const float sz)
{
    Matrix4 mx = MakeIdentity();
    mx.m_11 = sx;
    mx.m_22 = sy;
    mx.m_33 = sz;
    return mx;
}

Matrix4 Matrix4::MakeScaleTransform(const Vector3& vec)
{
    Matrix4 mx = MakeIdentity();
    mx.m_11 = vec.X();
    mx.m_22 = vec.Y();
    mx.m_33 = vec.Z();
    return mx;
}

Matrix4 Matrix4::MakeRotationXTransform(const float radian)
{
    Matrix4 mx;
    mx.m_11 = (float)1.0;
    mx.m_12 = (float)0.0;
    mx.m_13 = (float)0.0;
    mx.m_14 = (float)0.0;
    mx.m_21 = (float)0.0;
    mx.m_22 = cos(radian);
    mx.m_23 = -sin(radian);
    mx.m_24 = (float)0.0;
    mx.m_31 = (float)0.0;
    mx.m_32 = sin(radian);
    mx.m_33 = cos(radian);
    mx.m_34 = (float)0.0;
    mx.m_41 = (float)0.0;
    mx.m_42 = (float)0.0;
    mx.m_43 = (float)0.0;
    mx.m_44 = (float)1.0;
    return mx;
}

Matrix4 Matrix4::MakeRotationYTransform(const float radian)
{
    Matrix4 mx;
    mx.m_11 = cos(radian);
    mx.m_12 = (float)0.0;
    mx.m_13 = sin(radian);
    mx.m_14 = (float)0.0;
    mx.m_21 = (float)0.0;
    mx.m_22 = (float)1.0;
    mx.m_23 = (float)0.0;
    mx.m_24 = (float)0.0;
    mx.m_31 = -sin(radian);
    mx.m_32 = (float)0.0;
    mx.m_33 = cos(radian);
    mx.m_34 = (float)0.0;
    mx.m_41 = (float)0.0;
    mx.m_42 = (float)0.0;
    mx.m_43 = (float)0.0;
    mx.m_44 = (float)1.0;
    return mx;
}

Matrix4 Matrix4::MakeRotationZTransform(const float radian)
{
    Matrix4 mx;
    mx.m_11 = cos(radian);
    mx.m_12 = -sin(radian);
    mx.m_13 = (float)0.0;
    mx.m_14 = (float)0.0;
    mx.m_21 = sin(radian);
    mx.m_22 = cos(radian);
    mx.m_23 = (float)0.0;
    mx.m_24 = (float)0.0;
    mx.m_31 = (float)0.0;
    mx.m_32 = (float)0.0;
    mx.m_33 = (float)1.0;
    mx.m_34 = (float)0.0;
    mx.m_41 = (float)0.0;
    mx.m_42 = (float)0.0;
    mx.m_43 = (float)0.0;
    mx.m_44 = (float)1.0;
    return mx;
}

Matrix4 Matrix4::MakeRotationYawPitchRoll(const float yaw, const float pitch, const float roll)
{
    Matrix4 mtx4Rx = MakeRotationXTransform(pitch);
    Matrix4 mtx4Ry = MakeRotationYTransform(yaw);
    Matrix4 mtx4Rz = MakeRotationZTransform(roll);
    return mtx4Ry * mtx4Rx * mtx4Rz;
}

Vector3 Matrix4::UnMatrixTranslate() const
{
    Vector3 v;
    v.X() = m_14;
    v.Y() = m_24;
    v.Z() = m_34;
    return v;
}

Vector3 Matrix4::UnMatrixScale() const
{
    Vector3 s;
    Vector3 v0 = Vector3(m_11, m_21, m_31);
    Vector3 v1 = Vector3(m_12, m_22, m_32);
    Vector3 v2 = Vector3(m_13, m_23, m_33);
    s.X() = v0.Length();
    s.Y() = v1.Length();
    s.Z() = v2.Length();
    Vector3 nor = v0.Cross(v1);
    if (nor.Dot(v2) < (float)0.0)
    {
        // 有-1的scale, 但是不知道是哪個軸, 用eigen找mirror plane
        Matrix3 eigen_solve(*this);
        EigenDecompose<Matrix3> eigen;
        eigen = eigen_solve.EigenDecomposition();
        if (Math::IsEqual(eigen.m_diag[0][0], -1.0f)) // eigen value有排序，第一個應該就是-1
        {
            // 不多做計算了，最大的那個軸就抓出來反向
            Vector3 axis = Vector3(fabs(eigen.m_rot[0][0]), fabs(eigen.m_rot[1][0]), fabs(eigen.m_rot[2][0]));
            if ((axis.X() > axis.Y()) && (axis.X() > axis.Z()))
            {
                s.X() *= (float)-1.0;
            }
            else if ((axis.Y() > axis.X()) && (axis.Y() > axis.Z()))
            {
                s.Y() *= (float)-1.0;
            }
            else
            {
                s.Z() *= (float)-1.0;
            }
        }
    }
    return s;
}

Matrix4 Matrix4::UnMatrixRotation() const
{
    Matrix4 r;
    Vector3 s = UnMatrixScale();
    r.m_11 = m_11 / s.X();
    r.m_21 = m_21 / s.X();
    r.m_31 = m_31 / s.X();
    r.m_41 = (float)0.0;
    r.m_12 = m_12 / s.Y();
    r.m_22 = m_22 / s.Y();
    r.m_32 = m_32 / s.Y();
    r.m_42 = (float)0.0;
    r.m_13 = m_13 / s.Z();
    r.m_23 = m_23 / s.Z();
    r.m_33 = m_33 / s.Z();
    r.m_43 = (float)0.0;
    r.m_14 = (float)0.0;
    r.m_24 = (float)0.0;
    r.m_34 = (float)0.0;
    r.m_44 = (float)1.0;
    return r;
}

std::tuple<Vector3, Quaternion, Vector3> Matrix4::UnMatrixSRT() const
{
    Vector3 trans;
    trans.X() = m_14;
    trans.Y() = m_24;
    trans.Z() = m_34;

    Vector3 s;
    Vector3 v0(m_11, m_21, m_31);
    Vector3 v1(m_12, m_22, m_32);
    Vector3 v2(m_13, m_23, m_33);
    s.X() = v0.Length();
    s.Y() = v1.Length();
    s.Z() = v2.Length();
    Vector3 nor = v0.Cross(v1);
    if (nor.Dot(v2) < (float)0.0)
    {
        // 有-1的scale, 但是不知道是哪個軸, 用eigen找mirror plane
        Matrix3 eigenm_solve(*this);
        EigenDecompose<Matrix3> eigen;
        eigen = eigenm_solve.EigenDecomposition();
        if (Math::IsEqual(eigen.m_diag[0][0], -1.0f)) // eigen value有排序，第一個應該就是-1
        {
            // 不多做計算了，最大的那個軸就抓出來反向
            Vector3 axis = Vector3(fabs(eigen.m_rot[0][0]), fabs(eigen.m_rot[1][0]), fabs(eigen.m_rot[2][0]));
            if ((axis.X() > axis.Y()) && (axis.X() > axis.Z()))
            {
                s.X() *= (float)-1.0;
            }
            else if ((axis.Y() > axis.X()) && (axis.Y() > axis.Z()))
            {
                s.Y() *= (float)-1.0;
            }
            else
            {
                s.Z() *= (float)-1.0;
            }
        }
    }

    Matrix3 r;
    r[0][0] = m_11 / s.X();
    r[1][0] = m_21 / s.X();
    r[2][0] = m_31 / s.X();
    r[0][1] = m_12 / s.Y();
    r[1][1] = m_22 / s.Y();
    r[2][1] = m_32 / s.Y();
    r[0][2] = m_13 / s.Z();
    r[1][2] = m_23 / s.Z();
    r[2][2] = m_33 / s.Z();
    Quaternion rot = Quaternion::FromRotationMatrix(r);
    return { trans, rot, s };
}

Matrix4 operator*(float scalar, const Matrix4& mx)
{
    return mx * scalar;
}

float Matrix4::GetMaxScale() const
{
    Vector3 scale = UnMatrixScale();
    float max_s = fabs(scale.X());
    if (fabs(scale.Y()) > max_s) max_s = fabs(scale.Y());
    if (fabs(scale.Z()) > max_s) max_s = fabs(scale.Z());
    return max_s;
}
