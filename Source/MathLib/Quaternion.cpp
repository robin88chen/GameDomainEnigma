#include "Quaternion.h"
#include "MathGlobal.h"
#include <cstring>
#include <cmath>
#include <cassert>

using namespace Enigma::MathLib;

const Quaternion Quaternion::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
const Quaternion Quaternion::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f);

Quaternion::Quaternion()
{
    memset(m_tuple, 0, sizeof(m_tuple));
}

Quaternion::Quaternion(float w, float x, float y, float z)
{
    m_w = w;
    m_x = x;
    m_y = y;
    m_z = z;
}

Quaternion::Quaternion(const Matrix3& rot)
{
    ImplicitFromRotationMatrix(rot);
}

Quaternion::Quaternion(const Vector3& axis, float angle)
{
    ImplicitFromAxisAngle(axis, angle);
}

Quaternion::operator const float* () const
{
    return m_tuple;
}

Quaternion::operator float* ()
{
    return m_tuple;
}

float Quaternion::operator[](int i) const
{
    assert(0 <= i && i <= 3);
    return m_tuple[i];
}

float& Quaternion::operator[](int i)
{
    assert(0 <= i && i <= 3);
    return m_tuple[i];
}

float Quaternion::w() const
{
    return m_w;
}

float& Quaternion::w()
{
    return m_w;
}

float Quaternion::x() const
{
    return m_x;
}

float& Quaternion::x()
{
    return m_x;
}

float Quaternion::y() const
{
    return m_y;
}

float& Quaternion::y()
{
    return m_y;
}

float Quaternion::z() const
{
    return m_z;
}

float& Quaternion::z()
{
    return m_z;
}


int Quaternion::compareArrays(const Quaternion& quat) const
{
    return memcmp(m_tuple, quat.m_tuple, sizeof(m_tuple));
}

bool Quaternion::operator==(const Quaternion& quat) const
{
    return (Math::IsEqual(m_x, quat.m_x) && Math::IsEqual(m_y, quat.m_y)
        && Math::IsEqual(m_z, quat.m_z) && Math::IsEqual(m_w, quat.m_w));
}

bool Quaternion::operator!=(const Quaternion& quat) const
{
    return (!(Math::IsEqual(m_x, quat.m_x) && Math::IsEqual(m_y, quat.m_y)
        && Math::IsEqual(m_z, quat.m_z) && Math::IsEqual(m_w, quat.m_w)));
}

bool Quaternion::operator<(const Quaternion& quat) const
{
    return compareArrays(quat) < 0;
}

bool Quaternion::operator<=(const Quaternion& quat) const
{
    return compareArrays(quat) <= 0;
}

bool Quaternion::operator>(const Quaternion& quat) const
{
    return compareArrays(quat) > 0;
}

bool Quaternion::operator>=(const Quaternion& quat) const
{
    return compareArrays(quat) >= 0;
}

Quaternion Quaternion::operator+(const Quaternion& quat) const
{
    return Quaternion(m_w + quat.m_w, m_x + quat.m_x, m_y + quat.m_y, m_z + quat.m_z);
}

Quaternion Quaternion::operator-(const Quaternion& quat) const
{
    return Quaternion(m_w - quat.m_w, m_x - quat.m_x, m_y - quat.m_y, m_z - quat.m_z);
}

Quaternion Quaternion::operator*(const Quaternion& quat) const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.
    return Quaternion
    (
        m_w * quat.m_w - m_x * quat.m_x - m_y * quat.m_y - m_z * quat.m_z,
        m_w * quat.m_x + m_x * quat.m_w + m_y * quat.m_z - m_z * quat.m_y,
        m_w * quat.m_y + m_y * quat.m_w + m_z * quat.m_x - m_x * quat.m_z,
        m_w * quat.m_z + m_z * quat.m_w + m_x * quat.m_y - m_y * quat.m_x
    );
}

Quaternion Quaternion::operator*(float scalar) const
{
    return Quaternion(scalar * m_w, scalar * m_x, scalar * m_y, scalar * m_z);
}

Quaternion Quaternion::operator/(float scalar) const
{
    Quaternion quot;
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

Quaternion Quaternion::operator-() const
{
    return Quaternion(-m_w, -m_x, -m_y, -m_z);
}

Vector3 Quaternion::operator*(const Vector3& v) const
{
    // nVidia SDK implementation
    Vector3 uv, uuv;
    Vector3 qvec(m_x, m_y, m_z);
    uv = qvec.cross(v);
    uuv = qvec.cross(uv);
    uv *= ((float)2.0 * m_w);
    uuv *= (float)2.0;

    return v + uv + uuv;
}

namespace Enigma::MathLib
{
    Quaternion operator*(float scalar, const Quaternion& quat)
    {
        return Quaternion(
            scalar * quat.w(), scalar * quat.x(),
            scalar * quat.y(), scalar * quat.z());
    }
}

Quaternion& Quaternion::operator+=(const Quaternion& quat)
{
    m_w += quat.m_w;
    m_x += quat.m_x;
    m_y += quat.m_y;
    m_z += quat.m_z;
    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& quat)
{
    m_w -= quat.m_w;
    m_x -= quat.m_x;
    m_y -= quat.m_y;
    m_z -= quat.m_z;
    return *this;
}

Quaternion& Quaternion::operator*=(float scalar)
{
    m_w *= scalar;
    m_x *= scalar;
    m_y *= scalar;
    m_z *= scalar;
    return *this;
}

Quaternion& Quaternion::operator/=(float scalar)
{
    if (scalar != (float)0.0)
    {
        //float invScalar = ((float)1.0) / scalar;
        m_w /= scalar;
        m_x /= scalar;
        m_y /= scalar;
        m_z /= scalar;
    }
    else
    {
        m_w = Math::MAX_FLOAT;
        m_x = Math::MAX_FLOAT;
        m_y = Math::MAX_FLOAT;
        m_z = Math::MAX_FLOAT;
    }

    return *this;
}

Quaternion Quaternion::FromRotationMatrix(const Matrix3& rot)
{
    return Quaternion(rot);
}

void Quaternion::ImplicitFromRotationMatrix(const Matrix3& rot)
{
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".
    float trace = rot(0, 0) + rot(1, 1) + rot(2, 2);
    float root;

    if (trace > (float)0.0)
    {
        // |w| > 1/2, may as well choose w > 1/2
        root = sqrt(trace + (float)1.0); // 2w
        m_w = (float)0.5 * root;
        root = (float)0.5 / root; // 1/(4w)
        m_x = (rot(2, 1) - rot(1, 2)) * root;
        m_y = (rot(0, 2) - rot(2, 0)) * root;
        m_z = (rot(1, 0) - rot(0, 1)) * root;
    }
    else
    {
        // |w| <= 1/2
        static int s_iNext[3] = { 1, 2, 0 };
        int i = 0;
        if (rot(1, 1) > rot(0, 0))
            i = 1;
        if (rot(2, 2) > rot(i, i))
            i = 2;
        int j = s_iNext[i];
        int k = s_iNext[j];

        root = sqrt(rot(i, i) - rot(j, j) - rot(k, k) + (float)1.0);
        float* quat[3] = { &m_x, &m_y, &m_z };
        *quat[i] = (float)0.5 * root;
        root = (float)0.5 / root;
        m_w = (rot(k, j) - rot(j, k)) * root;
        *quat[j] = (rot(j, i) + rot(i, j)) * root;
        *quat[k] = (rot(k, i) + rot(i, k)) * root;
    }
}

Matrix3 Quaternion::ToRotationMatrix() const
{
    Matrix3 rot;
    float twoX = ((float)2) * m_x;
    float twoY = ((float)2) * m_y;
    float twoZ = ((float)2) * m_z;
    float twoWX = twoX * m_w;
    float twoWY = twoY * m_w;
    float twoWZ = twoZ * m_w;
    float twoXX = twoX * m_x;
    float twoXY = twoY * m_x;
    float twoXZ = twoZ * m_x;
    float twoYY = twoY * m_y;
    float twoYZ = twoZ * m_y;
    float twoZZ = twoZ * m_z;

    rot(0, 0) = (float)1 - (twoYY + twoZZ);
    rot(0, 1) = twoXY - twoWZ;
    rot(0, 2) = twoXZ + twoWY;
    rot(1, 0) = twoXY + twoWZ;
    rot(1, 1) = (float)1 - (twoXX + twoZZ);
    rot(1, 2) = twoYZ - twoWX;
    rot(2, 0) = twoXZ - twoWY;
    rot(2, 1) = twoYZ + twoWX;
    rot(2, 2) = (float)1 - (twoXX + twoYY);
    return rot;
}

Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle)
{
    return Quaternion(axis, angle);
}

void Quaternion::ImplicitFromAxisAngle(const Vector3& axis, float angle)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

    float halfAngle = (float)0.5 * angle;
    float sn = sin(halfAngle);
    m_w = cos(halfAngle);
    m_x = sn * axis[0];
    m_y = sn * axis[1];
    m_z = sn * axis[2];
}

std::tuple<Vector3, float> Quaternion::ToAxisAngle() const
{
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
    Vector3 axis;
    float angle;
    float sqrLength = m_x * m_x + m_y * m_y + m_z * m_z;
    if (sqrLength > Math::ZERO_TOLERANCE)
    {
        angle = (float)2.0 * acos(m_w);
        float invLength = (float)1.0 / sqrt(sqrLength);
        axis[0] = m_x * invLength;
        axis[1] = m_y * invLength;
        axis[2] = m_z * invLength;
    }
    else
    {
        // angle is 0 (mod 2*pi), so any axis will do
        angle = (float)0.0;
        axis[0] = (float)1.0;
        axis[1] = (float)0.0;
        axis[2] = (float)0.0;
    }
    return { axis, angle };
}

float Quaternion::length() const
{
    return sqrt(
        m_w * m_w + m_x * m_x +
        m_y * m_y + m_z * m_z);
}

float Quaternion::squaredLength() const
{
    return
        m_w * m_w + m_x * m_x +
        m_y * m_y + m_z * m_z;
}

float Quaternion::dot(const Quaternion& quat) const
{
    return m_w * quat.m_w + m_x * quat.m_x + m_y * quat.m_y + m_z * quat.m_z;
}

Quaternion Quaternion::normalize() const
{
    float leng = length();
    Quaternion q;
    if (leng > Math::ZERO_TOLERANCE)
    {
        float invLength = (float)1.0 / leng;
        q.m_w = m_w * invLength;
        q.m_x = m_x * invLength;
        q.m_y = m_y * invLength;
        q.m_z = m_z * invLength;
    }
    else
    {
        q.m_w = (float)0.0;
        q.m_x = (float)0.0;
        q.m_y = (float)0.0;
        q.m_z = (float)0.0;
    }

    return q;
}

Quaternion Quaternion::Inverse() const
{
    float norm = m_w * m_w + m_x * m_x + m_y * m_y + m_z * m_z;
    if (norm > (float)0.0)
    {
        float invNorm = (float)1.0 / norm;
        return Quaternion(m_w * invNorm, -m_x * invNorm, -m_y * invNorm, -m_z * invNorm);
    }
    else
    {
        // return an invalid result to flag the error
        return ZERO;
    }
}

Quaternion Quaternion::Conjugate() const
{
    return Quaternion(m_w, -m_x, -m_y, -m_z);
}

Quaternion Quaternion::Exp() const
{
    // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
    // use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

    float angle = sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
    float sn = sin(angle);

    Quaternion result;
    result.m_w = cos(angle);

    if (fabs(sn) >= Math::ZERO_TOLERANCE)
    {
        float coeff = sn / angle;
        result.m_x = coeff * m_x;
        result.m_y = coeff * m_y;
        result.m_z = coeff * m_z;
    }
    else
    {
        result.m_x = m_x;
        result.m_y = m_y;
        result.m_z = m_z;
    }

    return result;
}

Quaternion Quaternion::Log() const
{
    // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
    // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
    // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

    Quaternion result;
    result.m_w = (float)0.0;

    if (fabs(m_w) < (float)1.0)
    {
        float angle = acos(m_w);
        float sn = sin(angle);
        if (fabs(sn) >= Math::ZERO_TOLERANCE)
        {
            float coeff = angle / sn;
            result.m_x = coeff * m_x;
            result.m_y = coeff * m_y;
            result.m_z = coeff * m_z;
            return result;
        }
    }

    result.m_x = m_x;
    result.m_y = m_y;
    result.m_z = m_z;

    return result;
}

Vector3 Quaternion::Rotate(const Vector3& vec) const
{
    // Given a vector u = (x0,y0,z0) and a unit length quaternion
    // q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
    // rotation of u by q is v = q*u*q^{-1} where * indicates quaternion
    // multiplication and where u is treated as the quaternion <0,x0,y0,z0>.
    // Note that q^{-1} = <w,-x,-y,-z>, so no real work is required to
    // invert q.  Now
    //
    //   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
    //     = q*(x0*i+y0*j+z0*k)*q^{-1}
    //     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
    //
    // As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
    // of the rotation matrix computed in Quaternion<float>::ToRotationMatrix.
    // The vector v is obtained as the product of that rotation matrix with
    // vector u.  As such, the quaternion representation of a rotation
    // matrix requires less space than the matrix and more time to compute
    // the rotated vector.  Typical space-time tradeoff...

    Matrix3 rot = ToRotationMatrix();
    return rot * vec;
}

Quaternion Quaternion::Slerp(float t, const Quaternion& p, const Quaternion& q, bool shortestPath)
{
    float cs = p.dot(q);
    float angle = acos(cs);

    if (fabs(angle) >= Math::ZERO_TOLERANCE)
    {
        float sn = sin(angle);
        float invSn = 1.0f / sn;
        float coeff0 = sin((1.0f - t) * angle) * invSn;
        float coeff1 = sin(t * angle) * invSn;
        // Do we need to invert rotation?
        if (cs < (float)0.0 && shortestPath)
        {
            coeff0 = -coeff0;
        }

        // taking the complement requires renormalisation
        Quaternion r(coeff0 * p + coeff1 * q);
        return r.normalize();
        //}
        //else
        //{
        //*this = coeff0 * p + coeff1 * q;
        //}
    }
    else
    {
        return p;
    }
}

Quaternion Quaternion::SlerpExtraSpins(float t, const Quaternion& p, const Quaternion& q, int extraSpins)
{
    float cs = p.dot(q);
    float angle = acos(cs);

    if (fabs(angle) >= Math::ZERO_TOLERANCE)
    {
        float sn = sin(angle);
        float phase = Math::PI * (float)extraSpins * t;
        float invSn = 1.0f / sn;
        float coeff0 = sin((1.0f - t) * angle - phase) * invSn;
        float coeff1 = sin(t * angle + phase) * invSn;
        return coeff0 * p + coeff1 * q;
    }
    else
    {
        return p;
    }
}

Quaternion Quaternion::Intermediate(const Quaternion& q0, const Quaternion& q1, const Quaternion& q2)
{
    // assert:  Q0, Q1, Q2 all unit-length
    Quaternion q1Inv = q1.Conjugate();
    Quaternion p0 = q1Inv * q0;
    Quaternion p2 = q1Inv * q2;
    Quaternion arg = -0.25f * (p0.Log() + p2.Log());
    return q1 * arg.Exp();
}

Quaternion Quaternion::Squad(float t, const Quaternion& q0, const Quaternion& a0, const Quaternion& a1, const Quaternion& q1, bool shortestPath)
{
    float slerpT = 2.0f * t * (1.0f - t);
    Quaternion slerpP = Slerp(t, q0, q1, shortestPath);
    Quaternion slerpQ = Slerp(t, a0, a1);
    return Slerp(slerpT, slerpP, slerpQ);
}

Quaternion Quaternion::Align(const Vector3& vec1, const Vector3& vec2)
{
    // If V1 and V2 are not parallel, the axis of rotation is the unit-length
    // vector U = cross(V1,V2)/length(cross(V1,V2)).  The angle of rotation,
    // A, is the angle between V1 and V2.  The quaternion for the rotation is
    // q = cos(A/2) + sin(A/2)*(ux*i+uy*j+uz*k) where U = (ux,uy,uz).
    //
    // (1) Rather than extract A = acos(dot(V1,V2)), multiply by 1/2, then
    //     compute sin(A/2) and cos(A/2), we reduce the computational costs by
    //     computing the bisector B = (V1+V2)/length(V1+V2), so cos(A/2) =
    //     dot(V1,B).
    //
    // (2) The rotation axis is U = cross(V1,B)/length(cross(V1,B)), but
    //     length(cross(V1,B)) = length(V1)*length(B)*sin(A/2) = sin(A/2), in
    //     which case sin(A/2)*(ux*i+uy*j+uz*k) = (cx*i+cy*j+cz*k) where
    //     C = cross(V1,B).
    //
    // If V1 and V2 are parallel, or nearly parallel as far as the Realing
    // point calculations are concerned, the calculation of B will produce
    // the zero vector: Vector3::Normalize checks for closeness to zero and
    // returns the zero vector accordingly.  Thus, we test for parallelism
    // by checking if cos(A/2) is zero.  The test for exactly zero is usually
    // not recommend for Realing point arithmetic, but the implementation of
    // Vector3::Normalize guarantees the comparison is robust.

    Quaternion q;
    Vector3 bisector = vec1 + vec2;
    bisector.normalizeSelf();

    float cosHalfAngle = vec1.dot(bisector);
    Vector3 cross;

    q.m_w = cosHalfAngle;

    if (cosHalfAngle != (float)0)
    {
        cross = vec1.cross(bisector);
        q.m_x = cross.x();
        q.m_y = cross.y();
        q.m_z = cross.z();
    }
    else
    {
        float invLength;
        if (fabs(vec1[0]) >= fabs(vec1[1]))
        {
            // V1.x or V1.z is the largest magnitude component.
            invLength = 1.0f / sqrt(vec1[0] * vec1[0] + vec1[2] * vec1[2]);
            q.m_x = -vec1[2] * invLength;
            q.m_y = (float)0;
            q.m_z = +vec1[0] * invLength;
        }
        else
        {
            // V1.y or V1.z is the largest magnitude component.
            invLength = 1.0f / sqrt(vec1[1] * vec1[1] + vec1[2] * vec1[2]);
            q.m_x = (float)0;
            q.m_y = +vec1[2] * invLength;
            q.m_z = -vec1[1] * invLength;
        }
    }
    return q;
}

QuaternionDecompose Quaternion::DecomposeTwistTimesSwing(const Vector3& vec1) const
{
    Vector3 vec2 = Rotate(vec1);
    Quaternion swing = Align(vec1, vec2);
    Quaternion twist = (*this) * swing.Conjugate();
    return { swing, twist };
}

QuaternionDecompose Quaternion::DecomposeSwingTimesTwist(const Vector3& vec1) const
{
    Vector3 vec2 = Rotate(vec1);
    Quaternion swing = Align(vec1, vec2);
    Quaternion twist = swing.Conjugate() * (*this);
    return { swing, twist };
}
