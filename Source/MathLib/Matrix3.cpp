#include "Matrix3.h"
#include "Matrix4.h"
#include "MathGlobal.h"
#include <cstring>
#include <cassert>
#include <cmath>

using namespace Enigma::MathLib;

const Matrix3 Matrix3::ZERO(
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f);
const Matrix3 Matrix3::IDENTITY(
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f);

Matrix3::Matrix3(bool isZero)
{
    if (isZero)
    {
        memset(m_entry, 0, 9 * sizeof(float));
    }
    else
    {
        m_11 = (float)1.0;
        m_12 = (float)0.0;
        m_13 = (float)0.0;
        m_21 = (float)0.0;
        m_22 = (float)1.0;
        m_23 = (float)0.0;
        m_31 = (float)0.0;
        m_32 = (float)0.0;
        m_33 = (float)1.0;
    }
}

Matrix3::Matrix3(float m00, float m01, float m02,
                 float m10, float m11, float m12,
                 float m20, float m21, float m22)
{
    m_11 = m00;
    m_12 = m01;
    m_13 = m02;
    m_21 = m10;
    m_22 = m11;
    m_23 = m12;
    m_31 = m20;
    m_32 = m21;
    m_33 = m22;
}

Matrix3::Matrix3(const float entry[9])
{
    m_11 = entry[0];
    m_12 = entry[1];
    m_13 = entry[2];
    m_21 = entry[3];
    m_22 = entry[4];
    m_23 = entry[5];
    m_31 = entry[6];
    m_32 = entry[7];
    m_33 = entry[8];
}

Matrix3::Matrix3(float m00, float m11, float m22)
{
    m_11 = m00;
    m_12 = (float)0.0;
    m_13 = (float)0.0;
    m_21 = (float)0.0;
    m_22 = m11;
    m_23 = (float)0.0;
    m_31 = (float)0.0;
    m_32 = (float)0.0;
    m_33 = m22;
}

Matrix3::Matrix3(const Vector3& axis, float angle)
{
    float cos_value = cos(angle);
    float sin_value = sin(angle);
    float one_minus_cos = 1.0f - cos_value;
    float x2 = axis[0] * axis[0];
    float y2 = axis[1] * axis[1];
    float z2 = axis[2] * axis[2];
    float xym = axis[0] * axis[1] * one_minus_cos;
    float xzm = axis[0] * axis[2] * one_minus_cos;
    float yzm = axis[1] * axis[2] * one_minus_cos;
    float x_sin = axis[0] * sin_value;
    float y_sin = axis[1] * sin_value;
    float z_sin = axis[2] * sin_value;

    m_11 = x2 * one_minus_cos + cos_value;
    m_12 = xym - z_sin;
    m_13 = xzm + y_sin;
    m_21 = xym + z_sin;
    m_22 = y2 * one_minus_cos + cos_value;
    m_23 = yzm - x_sin;
    m_31 = xzm - y_sin;
    m_32 = yzm + x_sin;
    m_33 = z2 * one_minus_cos + cos_value;
}

Matrix3::Matrix3(const Vector3& u, const Vector3& v,
                 const Vector3& w, bool isColumns)
{
    if (isColumns)
    {
        m_11 = u[0];
        m_12 = v[0];
        m_13 = w[0];
        m_21 = u[1];
        m_22 = v[1];
        m_23 = w[1];
        m_31 = u[2];
        m_32 = v[2];
        m_33 = w[2];
    }
    else
    {
        m_11 = u[0];
        m_12 = u[1];
        m_13 = u[2];
        m_21 = v[0];
        m_22 = v[1];
        m_23 = v[2];
        m_31 = w[0];
        m_32 = w[1];
        m_33 = w[2];
    }
}

Matrix3::Matrix3(const Vector3* v, bool isColumns)
{
    if (isColumns)
    {
        m_11 = v[0][0];
        m_12 = v[1][0];
        m_13 = v[2][0];
        m_21 = v[0][1];
        m_22 = v[1][1];
        m_23 = v[2][1];
        m_31 = v[0][2];
        m_32 = v[1][2];
        m_33 = v[2][2];
    }
    else
    {
        m_11 = v[0][0];
        m_12 = v[0][1];
        m_13 = v[0][2];
        m_21 = v[1][0];
        m_22 = v[1][1];
        m_23 = v[1][2];
        m_31 = v[2][0];
        m_32 = v[2][1];
        m_33 = v[2][2];
    }
}

Matrix3::Matrix3(const Matrix4& mx)
{
    operator=(mx);
}

Matrix3 Matrix3::MakeZero()
{
    return Matrix3(true);
}

Matrix3 Matrix3::MakeIdentity()
{
    return Matrix3(false);
}

Matrix3 Matrix3::MakeDiagonal(float m00, float m11, float m22)
{
    return Matrix3(m00, m11, m22);
}

Matrix3 Matrix3::FromAxisAngle(const Vector3& axis, float angle)
{
    return Matrix3(axis, angle);
}

Matrix3 Matrix3::FromColumnVectors(Vector3 col[3])
{
    return Matrix3(col[0], col[1], col[2], true);
}

Matrix3::operator const float*() const
{
    return &m_entry[0][0];
}

Matrix3::operator float*()
{
    return &m_entry[0][0];
}

const float* Matrix3::operator[](int row) const
{
    return &m_entry[row][0];
}

float* Matrix3::operator[](int row)
{
    return &m_entry[row][0];
}

float Matrix3::operator()(int row, int col) const
{
    return m_entry[row][col];
}

float& Matrix3::operator()(int row, int col)
{
    return m_entry[row][col];
}

void Matrix3::SetRow(int row, const Vector3& v)
{
    assert((row < 3) && (row >= 0));
    m_entry[row][0] = v[0];
    m_entry[row][1] = v[1];
    m_entry[row][2] = v[2];
}

Vector3 Matrix3::GetRow(int row) const
{
    assert((row < 3) && (row >= 0));
    Vector3 v = Vector3(m_entry[row][0], m_entry[row][1], m_entry[row][2]);
    return v;
}

void Matrix3::SetColumn(int col, const Vector3& v)
{
    assert((col < 3) && (col >= 0));
    m_entry[0][col] = v[0];
    m_entry[1][col] = v[1];
    m_entry[2][col] = v[2];
}

Vector3 Matrix3::GetColumn(int col) const
{
    assert((col < 3) && (col >= 0));
    Vector3 v = Vector3(m_entry[0][col], m_entry[1][col], m_entry[2][col]);
    return v;
}

void Matrix3::GetColumnMajor(float* col_major) const
{
    for (int row = 0, i = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
            col_major[i++] = m_entry[col][row]; // 轉置過來取數值
    }
}

Matrix3& Matrix3::operator =(const Matrix4& mx)
{
    m_11 = mx.m_11;
    m_12 = mx.m_12;
    m_13 = mx.m_13;
    m_21 = mx.m_21;
    m_22 = mx.m_22;
    m_23 = mx.m_23;
    m_31 = mx.m_31;
    m_32 = mx.m_32;
    m_33 = mx.m_33;
    return *this;
}

bool Matrix3::operator ==(const Matrix3& mx) const
{
    if ((Math::IsEqual(m_11, mx.m_11)) && (Math::IsEqual(m_12, mx.m_12)) && (Math::IsEqual(m_13, mx.m_13))
        && (Math::IsEqual(m_21, mx.m_21)) && (Math::IsEqual(m_22, mx.m_22)) && (Math::IsEqual(m_23, mx.m_23))
        && (Math::IsEqual(m_31, mx.m_31)) && (Math::IsEqual(m_32, mx.m_32)) && (Math::IsEqual(m_33, mx.m_33)))
        return true;
    return false;
}

bool Matrix3::operator !=(const Matrix3& mx) const
{
    if (!((Math::IsEqual(m_11, mx.m_11)) && (Math::IsEqual(m_12, mx.m_12)) && (Math::IsEqual(m_13, mx.m_13))
        && (Math::IsEqual(m_21, mx.m_21)) && (Math::IsEqual(m_22, mx.m_22)) && (Math::IsEqual(m_23, mx.m_23))
        && (Math::IsEqual(m_31, mx.m_31)) && (Math::IsEqual(m_32, mx.m_32)) && (Math::IsEqual(m_33, mx.m_33))))
        return true;
    return false;
}

int Matrix3::compareArrays(const Matrix3& mx) const
{
    return memcmp(m_entry, mx.m_entry, 9 * sizeof(float));
}

bool Matrix3::operator<(const Matrix3& mx) const
{
    return compareArrays(mx) < 0;
}

bool Matrix3::operator<=(const Matrix3& mx) const
{
    return compareArrays(mx) <= 0;
}

bool Matrix3::operator>(const Matrix3& mx) const
{
    return compareArrays(mx) > 0;
}

bool Matrix3::operator>=(const Matrix3& mx) const
{
    return compareArrays(mx) >= 0;
}

Matrix3 Matrix3::operator+(const Matrix3& mx) const
{
    Matrix3 sum;
    sum.m_11 = m_11 + mx.m_11;
    sum.m_12 = m_12 + mx.m_12;
    sum.m_13 = m_13 + mx.m_13;
    sum.m_21 = m_21 + mx.m_21;
    sum.m_22 = m_22 + mx.m_22;
    sum.m_23 = m_23 + mx.m_23;
    sum.m_31 = m_31 + mx.m_31;
    sum.m_32 = m_32 + mx.m_32;
    sum.m_33 = m_33 + mx.m_33;
    return sum;
}

Matrix3 Matrix3::operator-(const Matrix3& mx) const
{
    Matrix3 diff;
    diff.m_11 = m_11 - mx.m_11;
    diff.m_12 = m_12 - mx.m_12;
    diff.m_13 = m_13 - mx.m_13;
    diff.m_21 = m_21 - mx.m_21;
    diff.m_22 = m_22 - mx.m_22;
    diff.m_23 = m_23 - mx.m_23;
    diff.m_31 = m_31 - mx.m_31;
    diff.m_32 = m_32 - mx.m_32;
    diff.m_33 = m_33 - mx.m_33;
    return diff;
}

Matrix3 Matrix3::operator*(const Matrix3& mx) const
{
    Matrix3 prod;
    prod.m_11 = m_11 * mx.m_11 + m_12 * mx.m_21 + m_13 * mx.m_31;
    prod.m_12 = m_11 * mx.m_12 + m_12 * mx.m_22 + m_13 * mx.m_32;
    prod.m_13 = m_11 * mx.m_13 + m_12 * mx.m_23 + m_13 * mx.m_33;
    prod.m_21 = m_21 * mx.m_11 + m_22 * mx.m_21 + m_23 * mx.m_31;
    prod.m_22 = m_21 * mx.m_12 + m_22 * mx.m_22 + m_23 * mx.m_32;
    prod.m_23 = m_21 * mx.m_13 + m_22 * mx.m_23 + m_23 * mx.m_33;
    prod.m_31 = m_31 * mx.m_11 + m_32 * mx.m_21 + m_33 * mx.m_31;
    prod.m_32 = m_31 * mx.m_12 + m_32 * mx.m_22 + m_33 * mx.m_32;
    prod.m_33 = m_31 * mx.m_13 + m_32 * mx.m_23 + m_33 * mx.m_33;
    return prod;
}

Matrix3 Matrix3::operator*(float scalar) const
{
    Matrix3 prod;
    prod.m_11 = scalar * m_11;
    prod.m_12 = scalar * m_12;
    prod.m_13 = scalar * m_13;
    prod.m_21 = scalar * m_21;
    prod.m_22 = scalar * m_22;
    prod.m_23 = scalar * m_23;
    prod.m_31 = scalar * m_31;
    prod.m_32 = scalar * m_32;
    prod.m_33 = scalar * m_33;
    return prod;
}

Matrix3 Matrix3::operator/(float scalar) const
{
    Matrix3 quot;
    if (scalar != (float)0.0)
    {
        //float invScalar = (float)1.0 / scalar;
        quot.m_11 = m_11 / scalar;
        quot.m_12 = m_12 / scalar;
        quot.m_13 = m_13 / scalar;
        quot.m_21 = m_21 / scalar;
        quot.m_22 = m_22 / scalar;
        quot.m_23 = m_23 / scalar;
        quot.m_31 = m_31 / scalar;
        quot.m_32 = m_32 / scalar;
        quot.m_33 = m_33 / scalar;
    }
    else
    {
        quot.m_11 = Math::MAX_FLOAT;
        quot.m_12 = Math::MAX_FLOAT;
        quot.m_13 = Math::MAX_FLOAT;
        quot.m_21 = Math::MAX_FLOAT;
        quot.m_22 = Math::MAX_FLOAT;
        quot.m_23 = Math::MAX_FLOAT;
        quot.m_31 = Math::MAX_FLOAT;
        quot.m_32 = Math::MAX_FLOAT;
        quot.m_33 = Math::MAX_FLOAT;
    }
    return quot;
}

Matrix3 Matrix3::operator-() const
{
    Matrix3 neg;
    neg.m_11 = -m_11;
    neg.m_12 = -m_12;
    neg.m_13 = -m_13;
    neg.m_21 = -m_21;
    neg.m_22 = -m_22;
    neg.m_23 = -m_23;
    neg.m_31 = -m_31;
    neg.m_32 = -m_32;
    neg.m_33 = -m_33;
    return neg;
}

Matrix3& Matrix3::operator+=(const Matrix3& mx)
{
    m_11 += mx.m_11;
    m_12 += mx.m_12;
    m_13 += mx.m_13;
    m_21 += mx.m_21;
    m_22 += mx.m_22;
    m_23 += mx.m_23;
    m_31 += mx.m_31;
    m_32 += mx.m_32;
    m_33 += mx.m_33;
    return *this;
}

Matrix3& Matrix3::operator-=(const Matrix3& mx)
{
    m_11 -= mx.m_11;
    m_12 -= mx.m_12;
    m_13 -= mx.m_13;
    m_21 -= mx.m_21;
    m_22 -= mx.m_22;
    m_23 -= mx.m_23;
    m_31 -= mx.m_31;
    m_32 -= mx.m_32;
    m_33 -= mx.m_33;
    return *this;
}

Matrix3& Matrix3::operator*=(float scalar)
{
    m_11 *= scalar;
    m_12 *= scalar;
    m_13 *= scalar;
    m_21 *= scalar;
    m_22 *= scalar;
    m_23 *= scalar;
    m_31 *= scalar;
    m_32 *= scalar;
    m_33 *= scalar;
    return *this;
}

Matrix3& Matrix3::operator/=(float scalar)
{
    if (scalar != (float)0.0)
    {
        //float invScalar = (float)1.0 / scalar;
        m_11 /= scalar;
        m_12 /= scalar;
        m_13 /= scalar;
        m_21 /= scalar;
        m_22 /= scalar;
        m_23 /= scalar;
        m_31 /= scalar;
        m_32 /= scalar;
        m_33 /= scalar;
    }
    else
    {
        m_11 = Math::MAX_FLOAT;
        m_12 = Math::MAX_FLOAT;
        m_13 = Math::MAX_FLOAT;
        m_21 = Math::MAX_FLOAT;
        m_22 = Math::MAX_FLOAT;
        m_23 = Math::MAX_FLOAT;
        m_31 = Math::MAX_FLOAT;
        m_32 = Math::MAX_FLOAT;
        m_33 = Math::MAX_FLOAT;
    }
    return *this;
}

Vector3 Matrix3::operator*(const Vector3& v) const
{
    Vector3 prod;
    prod.x() = m_11 * v.x() + m_12 * v.y() + m_13 * v.z();
    prod.y() = m_21 * v.x() + m_22 * v.y() + m_23 * v.z();
    prod.z() = m_31 * v.x() + m_32 * v.y() + m_33 * v.z();
    return prod;
}

Matrix3 Matrix3::Transpose() const
{
    Matrix3 transpose;
    transpose.m_11 = m_11;
    transpose.m_12 = m_21;
    transpose.m_13 = m_31;
    transpose.m_21 = m_12;
    transpose.m_22 = m_22;
    transpose.m_23 = m_32;
    transpose.m_31 = m_13;
    transpose.m_32 = m_23;
    transpose.m_33 = m_33;
    return transpose;
}

Matrix3 Matrix3::Inverse() const
{
    Matrix3 inverse;

    inverse.m_11 = m_22 * m_33 - m_23 * m_32;
    inverse.m_12 = m_13 * m_32 - m_12 * m_33;
    inverse.m_13 = m_12 * m_23 - m_13 * m_22;
    inverse.m_21 = m_23 * m_31 - m_21 * m_33;
    inverse.m_22 = m_11 * m_33 - m_13 * m_31;
    inverse.m_23 = m_13 * m_21 - m_11 * m_23;
    inverse.m_31 = m_21 * m_32 - m_22 * m_31;
    inverse.m_32 = m_12 * m_31 - m_11 * m_32;
    inverse.m_33 = m_11 * m_22 - m_12 * m_21;

    float det = m_11 * inverse.m_11 + m_12 * inverse.m_21 + m_13 * inverse.m_31;

    if (fabs(det) <= Math::ZERO_TOLERANCE)
        return ZERO;

    inverse /= det;
    return inverse;
}

Matrix3 Matrix3::Adjoint() const
{
    Matrix3 adjoint;

    adjoint.m_11 = m_22 * m_33 - m_23 * m_32;
    adjoint.m_12 = m_13 * m_32 - m_12 * m_33;
    adjoint.m_13 = m_12 * m_23 - m_13 * m_22;
    adjoint.m_21 = m_23 * m_31 - m_21 * m_33;
    adjoint.m_22 = m_11 * m_33 - m_13 * m_31;
    adjoint.m_23 = m_13 * m_21 - m_11 * m_23;
    adjoint.m_31 = m_21 * m_32 - m_22 * m_31;
    adjoint.m_32 = m_12 * m_31 - m_11 * m_32;
    adjoint.m_33 = m_11 * m_22 - m_12 * m_21;

    return adjoint;
}

float Matrix3::Determinant() const
{
    return (m_11 * (m_22 * m_33 - m_23 * m_32) - m_12 * (m_21 * m_33 - m_23 * m_31) + m_13 * (m_21 * m_32 - m_22 *
        m_31));
}

std::tuple<Vector3, float> Matrix3::ToAxisAngle() const
{
    // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
    // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 -z +y |
    //       | +z  0 -x |
    //       | -y +x  0 |
    //       +-        -+
    //
    // If A > 0, R represents a counterclockwise rotation about the axis in
    // the sense of looking from the tip of the axis vector towards the
    // origin.  Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
    //
    // In the event that A = pi, R-R^t = 0 which prevents us from extracting
    // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
    // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
    // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
    // it does not matter which sign you choose on the square roots.

    float trace = m_11 + m_22 + m_33;
    float cos_value = (float)0.5 * (trace - (float)1.0);
    float angle = acos(cos_value); // in [0,PI]

    Vector3 axis;
    if (angle > (float)0.0)
    {
        if (angle < Math::PI)
        {
            axis[0] = m_32 - m_23;
            axis[1] = m_13 - m_31;
            axis[2] = m_21 - m_12;
            axis.normalizeSelf();
        }
        else
        {
            // angle is PI
            float halfInverse;
            if (m_11 >= m_22)
            {
                // r00 >= r11
                if (m_11 >= m_33)
                {
                    // r00 is maximum diagonal term
                    axis[0] = (float)0.5 * sqrt(m_11 - m_22 - m_33 + (float)1.0);
                    halfInverse = (float)0.5 / axis[0];
                    axis[1] = halfInverse * m_12;
                    axis[2] = halfInverse * m_13;
                }
                else
                {
                    // r22 is maximum diagonal term
                    axis[2] = (float)0.5 * sqrt(m_33 - m_11 - m_22 + (float)1.0);
                    halfInverse = (float)0.5 / axis[2];
                    axis[0] = halfInverse * m_13;
                    axis[1] = halfInverse * m_23;
                }
            }
            else
            {
                // r11 > r00
                if (m_22 >= m_33)
                {
                    // r11 is maximum diagonal term
                    axis[1] = (float)0.5 * sqrt(m_22 - m_11 - m_33 + (float)1.0);
                    halfInverse = (float)0.5 / axis[1];
                    axis[0] = halfInverse * m_12;
                    axis[2] = halfInverse * m_23;
                }
                else
                {
                    // r22 is maximum diagonal term
                    axis[2] = (float)0.5 * sqrt(m_33 - m_11 - m_22 + (float)1.0);
                    halfInverse = (float)0.5 / axis[2];
                    axis[0] = halfInverse * m_13;
                    axis[1] = halfInverse * m_23;
                }
            }
        }
    }
    else
    {
        // The angle is 0 and the matrix is the identity.  Any axis will
        // work, so just use the x-axis.
        axis[0] = (float)1.0;
        axis[1] = (float)0.0;
        axis[2] = (float)0.0;
    }
    return { axis, angle };
}

Matrix3 Matrix3::FromEulerAnglesXYZ(const EulerAngles& angles)
{
    float xAngle = angles.m_x;
    float yAngle = angles.m_y;
    float zAngle = angles.m_z;

    float cos_value, sin_value;

    cos_value = cos(xAngle);
    sin_value = sin(xAngle);
    Matrix3 xmat(
        (float)1.0, (float)0.0, (float)0.0,
        (float)0.0, cos_value, -sin_value,
        (float)0.0, sin_value, cos_value);

    cos_value = cos(yAngle);
    sin_value = sin(yAngle);
    Matrix3 ymat(
        cos_value, (float)0.0, sin_value,
        (float)0.0, (float)1.0, (float)0.0,
        -sin_value, (float)0.0, cos_value);

    cos_value = cos(zAngle);
    sin_value = sin(zAngle);
    Matrix3 zmat(
        cos_value, -sin_value, (float)0.0,
        sin_value, cos_value, (float)0.0,
        (float)0.0, (float)0.0, (float)1.0);

    return xmat * (ymat * zmat);
}

Matrix3 Matrix3::FromEulerAnglesXZY(const EulerAngles& angles)
{
    float xAngle = angles.m_x;
    float yAngle = angles.m_y;
    float zAngle = angles.m_z;

    float cos_value, sin_value;

    cos_value = cos(xAngle);
    sin_value = sin(xAngle);
    Matrix3 xmat(
        (float)1.0, (float)0.0, (float)0.0,
        (float)0.0, cos_value, -sin_value,
        (float)0.0, sin_value, cos_value);

    cos_value = cos(zAngle);
    sin_value = sin(zAngle);
    Matrix3 zmat(
        cos_value, -sin_value, (float)0.0,
        sin_value, cos_value, (float)0.0,
        (float)0.0, (float)0.0, (float)1.0);

    cos_value = cos(yAngle);
    sin_value = sin(yAngle);
    Matrix3 ymat(
        cos_value, (float)0.0, sin_value,
        (float)0.0, (float)1.0, (float)0.0,
        -sin_value, (float)0.0, cos_value);

    return xmat * (zmat * ymat);
}

Matrix3 Matrix3::FromEulerAnglesYXZ(const EulerAngles& angles)
{
    float xAngle = angles.m_x;
    float yAngle = angles.m_y;
    float zAngle = angles.m_z;

    float cos_value, sin_value;

    cos_value = cos(yAngle);
    sin_value = sin(yAngle);
    Matrix3 ymat(
        cos_value, (float)0.0, sin_value,
        (float)0.0, (float)1.0, (float)0.0,
        -sin_value, (float)0.0, cos_value);

    cos_value = cos(xAngle);
    sin_value = sin(xAngle);
    Matrix3 xmat(
        (float)1.0, (float)0.0, (float)0.0,
        (float)0.0, cos_value, -sin_value,
        (float)0.0, sin_value, cos_value);

    cos_value = cos(zAngle);
    sin_value = sin(zAngle);
    Matrix3 zmat(
        cos_value, -sin_value, (float)0.0,
        sin_value, cos_value, (float)0.0,
        (float)0.0, (float)0.0, (float)1.0);

    return ymat * (xmat * zmat);
}

Matrix3 Matrix3::FromEulerAnglesYZX(const EulerAngles& angles)
{
    float xAngle = angles.m_x;
    float yAngle = angles.m_y;
    float zAngle = angles.m_z;

    float cos_value, sin_value;

    cos_value = cos(yAngle);
    sin_value = sin(yAngle);
    Matrix3 ymat(
        cos_value, (float)0.0, sin_value,
        (float)0.0, (float)1.0, (float)0.0,
        -sin_value, (float)0.0, cos_value);

    cos_value = cos(zAngle);
    sin_value = sin(zAngle);
    Matrix3 zmat(
        cos_value, -sin_value, (float)0.0,
        sin_value, cos_value, (float)0.0,
        (float)0.0, (float)0.0, (float)1.0);

    cos_value = cos(xAngle);
    sin_value = sin(xAngle);
    Matrix3 xmat(
        (float)1.0, (float)0.0, (float)0.0,
        (float)0.0, cos_value, -sin_value,
        (float)0.0, sin_value, cos_value);

    return ymat * (zmat * xmat);
}

Matrix3 Matrix3::FromEulerAnglesZXY(const EulerAngles& angles)
{
    float xAngle = angles.m_x;
    float yAngle = angles.m_y;
    float zAngle = angles.m_z;

    float cos_value, sin_value;

    cos_value = cos(zAngle);
    sin_value = sin(zAngle);
    Matrix3 zmat(
        cos_value, -sin_value, (float)0.0,
        sin_value, cos_value, (float)0.0,
        (float)0.0, (float)0.0, (float)1.0);

    cos_value = cos(xAngle);
    sin_value = sin(xAngle);
    Matrix3 xmat(
        (float)1.0, (float)0.0, (float)0.0,
        (float)0.0, cos_value, -sin_value,
        (float)0.0, sin_value, cos_value);

    cos_value = cos(yAngle);
    sin_value = sin(yAngle);
    Matrix3 ymat(
        cos_value, (float)0.0, sin_value,
        (float)0.0, (float)1.0, (float)0.0,
        -sin_value, (float)0.0, cos_value);

    return zmat * (xmat * ymat);
}

Matrix3 Matrix3::FromEulerAnglesZYX(const EulerAngles& angles)
{
    float xAngle = angles.m_x;
    float yAngle = angles.m_y;
    float zAngle = angles.m_z;

    float cos_value, sin_value;

    cos_value = cos(zAngle);
    sin_value = sin(zAngle);
    Matrix3 zmat(
        cos_value, -sin_value, (float)0.0,
        sin_value, cos_value, (float)0.0,
        (float)0.0, (float)0.0, (float)1.0);

    cos_value = cos(yAngle);
    sin_value = sin(yAngle);
    Matrix3 ymat(
        cos_value, (float)0.0, sin_value,
        (float)0.0, (float)1.0, (float)0.0,
        -sin_value, (float)0.0, cos_value);

    cos_value = cos(xAngle);
    sin_value = sin(xAngle);
    Matrix3 xmat(
        (float)1.0, (float)0.0, (float)0.0,
        (float)0.0, cos_value, -sin_value,
        (float)0.0, sin_value, cos_value);

    return zmat * (ymat * xmat);
}

std::tuple<EulerAngles, UniqueEulerAngle> Matrix3::ToEulerAnglesXYZ() const
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy
    if (m_13 < (float)1.0)
    {
        if (m_13 > (float)-1.0)
        {
            float xAngle = atan2(-m_23, m_33);
            float yAngle = asin(m_13);
            float zAngle = atan2(-m_12, m_11);
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::True };
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            float xAngle = -atan2(m_21, m_22);
            float yAngle = -Math::HALF_PI;
            float zAngle = (float)0.0;
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        float xAngle = atan2(m_21, m_22);
        float yAngle = Math::HALF_PI;
        float zAngle = (float)0.0;
        return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
    }
}

std::tuple<EulerAngles, UniqueEulerAngle> Matrix3::ToEulerAnglesXZY() const
{
    // rot =  cy*cz          -sz              cz*sy
    //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
    //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz
    if (m_12 < (float)1.0)
    {
        if (m_12 > (float)-1.0)
        {
            float xAngle = atan2(m_32, m_22);
            float zAngle = asin(-m_12);
            float yAngle = atan2(m_13, m_11);
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::True };
        }
        else
        {
            // WARNING.  Not unique.  XA - YA = atan2(r20,r22)
            float xAngle = atan2(m_31, m_33);
            float zAngle = Math::HALF_PI;
            float yAngle = (float)0.0;
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
        }
    }
    else
    {
        // WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
        float xAngle = atan2(-m_31, m_33);
        float zAngle = -Math::HALF_PI;
        float yAngle = (float)0.0;
        return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
    }
}

std::tuple<EulerAngles, UniqueEulerAngle> Matrix3::ToEulerAnglesYXZ() const
{
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

    if (m_23 < (float)1.0)
    {
        if (m_23 > (float)-1.0)
        {
            float yAngle = atan2(m_13, m_33);
            float xAngle = asin(-m_23);
            float zAngle = atan2(m_21, m_22);
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::True };
        }
        else
        {
            // WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
            float yAngle = atan2(m_12, m_11);
            float xAngle = Math::HALF_PI;
            float zAngle = (float)0.0;
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
        float yAngle = atan2(-m_12, m_11);
        float xAngle = -Math::HALF_PI;
        float zAngle = (float)0.0;
        return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
    }
}

std::tuple<EulerAngles, UniqueEulerAngle> Matrix3::ToEulerAnglesYZX() const
{
    // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
    //        sz              cx*cz          -cz*sx
    //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz
    if (m_21 < (float)1.0)
    {
        if (m_21 > (float)-1.0)
        {
            float yAngle = atan2(-m_31, m_11);
            float zAngle = asin(m_21);
            float xAngle = atan2(-m_23, m_22);
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::True };
        }
        else
        {
            // WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
            float yAngle = -atan2(m_32, m_33);
            float zAngle = -Math::HALF_PI;
            float xAngle = (float)0.0;
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + XA = atan2(r21,r22)
        float yAngle = (float)atan2(m_32, m_33);
        float zAngle = Math::HALF_PI;
        float xAngle = (float)0.0;
        return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
    }
}

std::tuple<EulerAngles, UniqueEulerAngle> Matrix3::ToEulerAnglesZXY() const
{
    // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
    //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
    //       -cx*sy           sx              cx*cy
    if (m_32 < (float)1.0)
    {
        if (m_32 > (float)-1.0)
        {
            float zAngle = atan2(-m_12, m_22);
            float xAngle = asin(m_32);
            float yAngle = atan2(-m_31, m_33);
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::True };
        }
        else
        {
            // WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
            float zAngle = -atan2(m_13, m_11);
            float xAngle = -Math::HALF_PI;
            float yAngle = (float)0.0;
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
        float zAngle = atan2(m_13, m_11);
        float xAngle = Math::HALF_PI;
        float yAngle = (float)0.0;
        return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
    }
}

std::tuple<EulerAngles, UniqueEulerAngle> Matrix3::ToEulerAnglesZYX() const
{
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy
    if (m_31 < (float)1.0)
    {
        if (m_31 > (float)-1.0)
        {
            float zAngle = atan2(m_21, m_11);
            float yAngle = asin(-m_31);
            float xAngle = atan2(m_32, m_33);
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::True };
        }
        else
        {
            // WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
            float zAngle = -atan2(m_12, m_13);
            float yAngle = Math::HALF_PI;
            float xAngle = (float)0.0;
            return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
        float zAngle = atan2(-m_12, -m_13);
        float yAngle = -Math::HALF_PI;
        float xAngle = (float)0.0;
        return { {xAngle, yAngle, zAngle}, UniqueEulerAngle::False };
    }
}

Matrix3 Matrix3::RotationX(float radian)
{
    Matrix3 mx;
    mx.m_11 = (float)1.0;
    mx.m_12 = (float)0.0;
    mx.m_13 = (float)0.0;
    mx.m_21 = (float)0.0;
    mx.m_22 = cos(radian);
    mx.m_23 = -sin(radian);
    mx.m_31 = (float)0.0;
    mx.m_32 = sin(radian);
    mx.m_33 = cos(radian);
    return mx;
}

Matrix3 Matrix3::RotationY(float radian)
{
    Matrix3 mx;
    mx.m_11 = cos(radian);
    mx.m_12 = (float)0.0;
    mx.m_13 = sin(radian);
    mx.m_21 = (float)0.0;
    mx.m_22 = (float)1.0;
    mx.m_23 = (float)0.0;
    mx.m_31 = -sin(radian);
    mx.m_32 = (float)0.0;
    mx.m_33 = cos(radian);
    return mx;
}

Matrix3 Matrix3::RotationZ(float radian)
{
    Matrix3 mx;
    mx.m_11 = cos(radian);
    mx.m_12 = -sin(radian);
    mx.m_13 = (float)0.0;
    mx.m_21 = sin(radian);
    mx.m_22 = cos(radian);
    mx.m_23 = (float)0.0;
    mx.m_31 = (float)0.0;
    mx.m_32 = (float)0.0;
    mx.m_33 = (float)1.0;
    return mx;
}

Matrix3 Matrix3::RotationYawPitchRoll(float yaw, float pitch, float roll)
{
    Matrix3 mtx3Rx = RotationX(pitch);
    Matrix3 mtx3Ry = RotationY(yaw);
    Matrix3 mtx3Rz = RotationZ(roll);
    return mtx3Ry * mtx3Rx * mtx3Rz;
}

Matrix3 Matrix3::Slerp(float t, const Matrix3& rot0, const Matrix3& rot1)
{
    Vector3 axis;
    float angle;
    Matrix3 prod = rot0.TransposeTimes(rot1);
    std::tie(axis, angle) = prod.ToAxisAngle();
    return rot0 * FromAxisAngle(axis, t * angle);
}

Matrix3 Matrix3::TransposeTimes(const Matrix3& mx) const
{
    // A^T*B
    return Matrix3
    (
        m_11 * mx.m_11 +
        m_21 * mx.m_21 +
        m_31 * mx.m_31,

        m_11 * mx.m_12 +
        m_21 * mx.m_22 +
        m_31 * mx.m_32,

        m_11 * mx.m_13 +
        m_21 * mx.m_23 +
        m_31 * mx.m_33,

        m_12 * mx.m_11 +
        m_22 * mx.m_21 +
        m_32 * mx.m_31,

        m_12 * mx.m_12 +
        m_22 * mx.m_22 +
        m_32 * mx.m_32,

        m_12 * mx.m_13 +
        m_22 * mx.m_23 +
        m_32 * mx.m_33,

        m_13 * mx.m_11 +
        m_23 * mx.m_21 +
        m_33 * mx.m_31,

        m_13 * mx.m_12 +
        m_23 * mx.m_22 +
        m_33 * mx.m_32,

        m_13 * mx.m_13 +
        m_23 * mx.m_23 +
        m_33 * mx.m_33
    );
}

Matrix3 Matrix3::TimesTranspose(const Matrix3& mx) const
{
    // A*B^T
    return Matrix3
    (
        m_11 * mx.m_11 +
        m_12 * mx.m_12 +
        m_13 * mx.m_13,

        m_11 * mx.m_21 +
        m_12 * mx.m_22 +
        m_13 * mx.m_23,

        m_11 * mx.m_31 +
        m_12 * mx.m_32 +
        m_13 * mx.m_33,

        m_21 * mx.m_11 +
        m_22 * mx.m_12 +
        m_23 * mx.m_13,

        m_21 * mx.m_21 +
        m_22 * mx.m_22 +
        m_23 * mx.m_23,

        m_21 * mx.m_31 +
        m_22 * mx.m_32 +
        m_23 * mx.m_33,

        m_31 * mx.m_11 +
        m_32 * mx.m_12 +
        m_33 * mx.m_13,

        m_31 * mx.m_21 +
        m_32 * mx.m_22 +
        m_33 * mx.m_23,

        m_31 * mx.m_31 +
        m_32 * mx.m_32 +
        m_33 * mx.m_33
    );
}

namespace Enigma::MathLib
{
    Matrix3 operator*(float scalar, const Matrix3& mx)
    {
        return mx * scalar;
    }
}

Vector2 Matrix3::TransformCoord(const Vector2& v) const
{
    Vector2 prod;
    float invZ = (float)1.0 / (m_31 * v.x() + m_32 * v.y() + m_33);
    prod.x() = (m_11 * v.x() + m_12 * v.y() + m_13) * invZ;
    prod.y() = (m_21 * v.x() + m_22 * v.y() + m_23) * invZ;
    return prod;
}

Vector2 Matrix3::Transform(const Vector2& v) const
{
    Vector2 prod;
    prod.x() = (m_11 * v.x() + m_12 * v.y() + m_13);
    prod.y() = (m_21 * v.x() + m_22 * v.y() + m_23);
    return prod;
}

Vector2 Matrix3::TransformVector(const Vector2& v) const
{
    Vector2 prod;
    prod.x() = (m_11 * v.x() + m_12 * v.y());
    prod.y() = (m_21 * v.x() + m_22 * v.y());
    return prod;
}

std::tuple<Vector2, float> Matrix3::TransformVectorNormalized(const Vector2& v) const
{
    Vector2 prod;
    prod.x() = (m_11 * v.x() + m_12 * v.y());
    prod.y() = (m_21 * v.x() + m_22 * v.y());
    float length = prod.length();
    prod.normalizeSelf();
    return { prod, length };
}

float Matrix3::GetMaxScale() const
{
    Vector2 v0 = Vector2(m_11, m_21);
    Vector2 v1 = Vector2(m_12, m_22);
    float max_s = v0.length();
    if (v1.length() > max_s) max_s = v1.length();
    return max_s;
}

EigenDecompose<Matrix3> Matrix3::EigenDecomposition() const
{
    Matrix3 rotmx;
    Matrix3 diagmx;
    // Factor M = R*D*R^T.  The columns of R are the eigenvectors.  The
    // diagonal entries of D are the corresponding eigenvalues.
    float diagonal[3], subdiagonal[2];
    rotmx = *this;
    bool isReflection = rotmx.Tridiagonalize(diagonal, subdiagonal);
    bool isConverged = rotmx.QLAlgorithm(diagonal, subdiagonal);
    assert(isConverged);

    // (insertion) sort eigenvalues in increasing order, d0 <= d1 <= d2
    int i;
    float save_value;

    if (diagonal[1] < diagonal[0])
    {
        // swap d0 and d1
        save_value = diagonal[0];
        diagonal[0] = diagonal[1];
        diagonal[1] = save_value;

        // swap V0 and V1
        for (i = 0; i < 3; i++)
        {
            save_value = rotmx[i][0];
            rotmx[i][0] = rotmx[i][1];
            rotmx[i][1] = save_value;
        }
        isReflection = !isReflection;
    }

    if (diagonal[2] < diagonal[1])
    {
        // swap d1 and d2
        save_value = diagonal[1];
        diagonal[1] = diagonal[2];
        diagonal[2] = save_value;

        // swap V1 and V2
        for (i = 0; i < 3; i++)
        {
            save_value = rotmx[i][1];
            rotmx[i][1] = rotmx[i][2];
            rotmx[i][2] = save_value;
        }
        isReflection = !isReflection;
    }

    if (diagonal[1] < diagonal[0])
    {
        // swap d0 and d1
        save_value = diagonal[0];
        diagonal[0] = diagonal[1];
        diagonal[1] = save_value;

        // swap V0 and V1
        for (i = 0; i < 3; i++)
        {
            save_value = rotmx[i][0];
            rotmx[i][0] = rotmx[i][1];
            rotmx[i][1] = save_value;
        }
        isReflection = !isReflection;
    }

    diagmx = MakeDiagonal(diagonal[0], diagonal[1], diagonal[2]);

    if (isReflection)
    {
        // The orthogonal transformation that diagonalizes M is a reflection.
        // Make the eigenvectors a right--handed system by changing sign on
        // the last column.
        rotmx[0][2] = -rotmx[0][2];
        rotmx[1][2] = -rotmx[1][2];
        rotmx[2][2] = -rotmx[2][2];
    }
    return { rotmx, diagmx };
}

bool Matrix3::Tridiagonalize(float diagonal[3], float subdiagonal[2])
{
    // Householder reduction T = Q^t M Q
    //   Input:
    //     mat, symmetric 3x3 matrix M
    //   Output:
    //     mat, orthogonal matrix Q (a reflection)
    //     diag, diagonal entries of T
    //     subd, subdiagonal entries of T (T is symmetric)

    float m00 = m_11;
    float m01 = m_12;
    float m02 = m_13;
    float m11 = m_22;
    float m12 = m_23;
    float m22 = m_33;

    diagonal[0] = m00;
    if (fabs(m02) >= Math::ZERO_TOLERANCE)
    {
        subdiagonal[0] = sqrt(m01 * m01 + m02 * m02);
        float invLength = ((float)1.0) / subdiagonal[0];
        m01 *= invLength;
        m02 *= invLength;
        float tmp = ((float)2.0) * m01 * m12 + m02 * (m22 - m11);
        diagonal[1] = m11 + m02 * tmp;
        diagonal[2] = m22 - m02 * tmp;
        subdiagonal[1] = m12 - m01 * tmp;

        m_11 = (float)1.0;
        m_12 = (float)0.0;
        m_13 = (float)0.0;
        m_21 = (float)0.0;
        m_22 = m01;
        m_23 = m02;
        m_31 = (float)0.0;
        m_32 = m02;
        m_33 = -m01;
        return true;
    }
    else
    {
        diagonal[1] = m11;
        diagonal[2] = m22;
        subdiagonal[0] = m01;
        subdiagonal[1] = m12;

        m_11 = (float)1.0;
        m_12 = (float)0.0;
        m_13 = (float)0.0;
        m_21 = (float)0.0;
        m_22 = (float)1.0;
        m_23 = (float)0.0;
        m_31 = (float)0.0;
        m_32 = (float)0.0;
        m_33 = (float)1.0;
        return false;
    }
}

bool Matrix3::QLAlgorithm(float diagonal[3], float subdiagonal[2])
{
    // This is an implementation of the symmetric QR algorithm from the book
    // "Matrix Computations" by Gene H. Golub and Charles F. Van Loan, second
    // edition.  The algorithm is 8.2.3.  The implementation has a slight
    // variation to actually make it a QL algorithm, and it traps the case
    // when either of the subdiagonal terms s0 or s1 is zero and reduces the
    // 2-by-2 subblock directly.

    const int imax = 32;
    for (int i = 0; i < imax; i++)
    {
        float sum, diff, discr, eigValue0, eigValue1, cos_value, sin_value, tmp;
        int row_index;

        sum = fabs(diagonal[0]) + fabs(diagonal[1]);
        if (fabs(subdiagonal[0]) + sum == sum)
        {
            // The matrix is effectively
            //       +-        -+
            //   M = | d0  0  0 |
            //       | 0  d1 s1 |
            //       | 0  s1 d2 |
            //       +-        -+

            // Test whether M is diagonal (within numerical round-off).
            sum = fabs(diagonal[1]) + fabs(diagonal[2]);
            if (fabs(subdiagonal[1]) + sum == sum)
            {
                return true;
            }

            // Compute the eigenvalues as roots of a quadratic equation.
            sum = diagonal[1] + diagonal[2];
            diff = diagonal[1] - diagonal[2];
            discr = sqrt(diff * diff + ((float)4.0) * subdiagonal[1] * subdiagonal[1]);
            eigValue0 = ((float)0.5) * (sum - discr);
            eigValue1 = ((float)0.5) * (sum + discr);

            // Compute the Givens rotation.
            if (diff >= (float)0.0)
            {
                cos_value = subdiagonal[1];
                sin_value = diagonal[1] - eigValue0;
            }
            else
            {
                cos_value = diagonal[2] - eigValue0;
                sin_value = subdiagonal[1];
            }
            tmp = 1.0f / sqrt(cos_value * cos_value + sin_value * sin_value);
            cos_value *= tmp;
            sin_value *= tmp;

            // Postmultiply the current orthogonal matrix with the Givens
            // rotation.
            for (row_index = 0; row_index < 3; row_index++)
            {
                tmp = m_entry[row_index][2];
                m_entry[row_index][2] = sin_value * m_entry[row_index][1] + cos_value * tmp;
                m_entry[row_index][1] = cos_value * m_entry[row_index][1] - sin_value * tmp;
            }

            // Update the tridiagonal matrix.
            diagonal[1] = eigValue0;
            diagonal[2] = eigValue1;
            subdiagonal[0] = (float)0.0;
            subdiagonal[1] = (float)0.0;
            return true;
        }

        sum = fabs(diagonal[1]) + fabs(diagonal[2]);
        if (fabs(subdiagonal[1]) + sum == sum)
        {
            // The matrix is effectively
            //       +-         -+
            //   M = | d0  s0  0 |
            //       | s0  d1  0 |
            //       | 0   0  d2 |
            //       +-         -+

            // Test whether M is diagonal (within numerical round-off).
            sum = fabs(diagonal[0]) + fabs(diagonal[1]);
            if (fabs(subdiagonal[0]) + sum == sum)
            {
                return true;
            }

            // Compute the eigenvalues as roots of a quadratic equation.
            sum = diagonal[0] + diagonal[1];
            diff = diagonal[0] - diagonal[1];
            discr = sqrt(diff * diff + ((float)4.0) * subdiagonal[0] * subdiagonal[0]);
            eigValue0 = ((float)0.5) * (sum - discr);
            eigValue1 = ((float)0.5) * (sum + discr);

            // Compute the Givens rotation.
            if (diff >= (float)0.0)
            {
                cos_value = subdiagonal[0];
                sin_value = diagonal[0] - eigValue0;
            }
            else
            {
                cos_value = diagonal[1] - eigValue0;
                sin_value = subdiagonal[0];
            }
            tmp = 1.0f / sqrt(cos_value * cos_value + sin_value * sin_value);
            cos_value *= tmp;
            sin_value *= tmp;

            // Postmultiply the current orthogonal matrix with the Givens
            // rotation.
            for (row_index = 0; row_index < 3; row_index++)
            {
                tmp = m_entry[row_index][1];
                m_entry[row_index][1] = sin_value * m_entry[row_index][0] + cos_value * tmp;
                m_entry[row_index][0] = cos_value * m_entry[row_index][0] - sin_value * tmp;
            }

            // Update the tridiagonal matrix.
            diagonal[0] = eigValue0;
            diagonal[1] = eigValue1;
            subdiagonal[0] = (float)0.0;
            subdiagonal[1] = (float)0.0;
            return true;
        }

        // The matrix is
        //       +-        -+
        //   M = | d0 s0  0 |
        //       | s0 d1 s1 |
        //       | 0  s1 d2 |
        //       +-        -+

        // Set up the parameters for the first pass of the QL step.  The
        // value for A is the difference between diagonal term D[2] and the
        // implicit shift suggested by Wilkinson.
        float ratio = (diagonal[1] - diagonal[0]) / (((float)2.0f) * subdiagonal[0]);
        float root = sqrt((float)1.0 + ratio * ratio);
        float b_value = subdiagonal[1];
        float a_value = diagonal[2] - diagonal[0];
        if (ratio >= (float)0.0)
        {
            a_value += subdiagonal[0] / (ratio + root);
        }
        else
        {
            a_value += subdiagonal[0] / (ratio - root);
        }

        // Compute the Givens rotation for the first pass.
        if (fabs(b_value) >= fabs(a_value))
        {
            ratio = a_value / b_value;
            sin_value = 1.0f / sqrt((float)1.0 + ratio * ratio);
            cos_value = ratio * sin_value;
        }
        else
        {
            ratio = b_value / a_value;
            cos_value = 1.0f / sqrt((float)1.0 + ratio * ratio);
            sin_value = ratio * cos_value;
        }

        // Postmultiply the current orthogonal matrix with the Givens
        // rotation.
        for (row_index = 0; row_index < 3; row_index++)
        {
            tmp = m_entry[row_index][2];
            m_entry[row_index][2] = sin_value * m_entry[row_index][1] + cos_value * tmp;
            m_entry[row_index][1] = cos_value * m_entry[row_index][1] - sin_value * tmp;
        }

        // Set up the parameters for the second pass of the QL step.  The
        // values tmp0 and tmp1 are required to fully update the tridiagonal
        // matrix at the end of the second pass.
        float tmp0 = (diagonal[1] - diagonal[2]) * sin_value +
            ((float)2.0) * subdiagonal[1] * cos_value;
        float tmp1 = cos_value * subdiagonal[0];
        b_value = sin_value * subdiagonal[0];
        a_value = cos_value * tmp0 - subdiagonal[1];
        tmp0 *= sin_value;

        // Compute the Givens rotation for the second pass.  The subdiagonal
        // term S[1] in the tridiagonal matrix is updated at this time.
        if (fabs(b_value) >= fabs(a_value))
        {
            ratio = a_value / b_value;
            root = sqrt((float)1.0 + ratio * ratio);
            subdiagonal[1] = b_value * root;
            sin_value = ((float)1.0) / root;
            cos_value = ratio * sin_value;
        }
        else
        {
            ratio = b_value / a_value;
            root = sqrt((float)1.0 + ratio * ratio);
            subdiagonal[1] = a_value * root;
            cos_value = ((float)1.0) / root;
            sin_value = ratio * cos_value;
        }

        // Postmultiply the current orthogonal matrix with the Givens
        // rotation.
        for (row_index = 0; row_index < 3; row_index++)
        {
            tmp = m_entry[row_index][1];
            m_entry[row_index][1] = sin_value * m_entry[row_index][0] + cos_value * tmp;
            m_entry[row_index][0] = cos_value * m_entry[row_index][0] - sin_value * tmp;
        }

        // Complete the update of the tridiagonal matrix.
        float tmp2 = diagonal[1] - tmp0;
        diagonal[2] += tmp0;
        tmp0 = (diagonal[0] - tmp2) * sin_value + ((float)2.0) * tmp1 * cos_value;
        subdiagonal[0] = cos_value * tmp0 - tmp1;
        tmp0 *= sin_value;
        diagonal[1] = tmp2 + tmp0;
        diagonal[0] -= tmp0;
    }
    return false;
}
