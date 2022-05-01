#include "Triangle2.h"
#include <cmath>

using namespace Enigma::MathLib;

Triangle2::Triangle2()
{
}

Triangle2::Triangle2(const Vector2& vec0, const Vector2& vec1, const Vector2& vec2)
{
    m_vec[0] = vec0;
    m_vec[1] = vec1;
    m_vec[2] = vec2;
}

Triangle2::Triangle2(const Vector2 vec[3])
{
    m_vec[0] = vec[0];
    m_vec[1] = vec[1];
    m_vec[2] = vec[2];
}

bool Triangle2::operator ==(const Triangle2& tri) const
{
    return ((m_vec[0] == tri.m_vec[0]) && (m_vec[1] == tri.m_vec[1]) && (m_vec[2] == tri.m_vec[2]));
}

bool Triangle2::operator !=(const Triangle2& tri) const
{
    return ((m_vec[0] != tri.m_vec[0]) || (m_vec[1] != tri.m_vec[1]) || (m_vec[2] != tri.m_vec[2]));
}

float Triangle2::DistanceTo(const Vector2& vec) const
{
    Vector2 diff = m_vec[0] - vec;
    Vector2 edge0 = m_vec[1] - m_vec[0], edge1 = m_vec[2] - m_vec[0];
    float a00 = edge0.SquaredLength();
    float a01 = edge0.Dot(edge1);
    float a11 = edge1.SquaredLength();
    float b0 = diff.Dot(edge0);
    float b1 = diff.Dot(edge1);
    float c = diff.SquaredLength();
    float det = std::fabs(a00 * a11 - a01 * a01);
    float s = a01 * b1 - a11 * b0;
    float t = a01 * b0 - a00 * b1;
    float sqrDist;

    if (s + t <= det)
    {
        if (s < 0.0f)
        {
            if (t < 0.0f) // region 4
            {
                if (b0 < 0.0f)
                {
                    if (-b0 >= a00)
                        sqrDist = a00 + 2.0f * b0 + c;
                    else
                        sqrDist = c - b0 * b0 / a00;
                }
                else
                {
                    if (b1 >= 0.0f)
                        sqrDist = c;
                    else if (-b1 >= a11)
                        sqrDist = a11 + 2.0f * b1 + c;
                    else
                        sqrDist = c - b1 * b1 / a11;
                }
            }
            else // region 3
            {
                if (b1 >= 0.0f)
                    sqrDist = c;
                else if (-b1 >= a11)
                    sqrDist = a11 + 2.0f * b1 + c;
                else
                    sqrDist = c - b1 * b1 / a11;
            }
        }
        else if (t < 0.0f) // region 5
        {
            if (b0 >= 0.0f)
                sqrDist = c;
            else if (-b0 >= a00)
                sqrDist = a00 + 2.0f * b0 + c;
            else
                sqrDist = b0 * s + c - b0 * b0 / a00;
        }
        else // region 0
        {
            // minimum at interior point
            float invDet = 1.0f / det;
            s *= invDet;
            t *= invDet;
            sqrDist = s * (a00 * s + a01 * t + 2.0f * b0) +
                t * (a01 * s + a11 * t + 2.0f * b1) + c;
        }
    }
    else
    {
        float tmp0, tmp1, numer, denom;

        if (s < 0.0f) // region 2
        {
            tmp0 = a01 + b0;
            tmp1 = a11 + b1;
            if (tmp1 > tmp0)
            {
                numer = tmp1 - tmp0;
                denom = a00 - 2.0f * a01 + a11;
                if (numer >= denom)
                {
                    sqrDist = a00 + 2.0f * b0 + c;
                }
                else
                {
                    s = numer / denom;
                    t = 1.0f - s;
                    sqrDist = s * (a00 * s + a01 * t + 2.0f * b0) +
                        t * (a01 * s + a11 * t + 2.0f * b1) + c;
                }
            }
            else
            {
                if (tmp1 <= 0.0f)
                    sqrDist = a11 + 2.0f * b1 + c;
                else if (b1 >= 0.0f)
                    sqrDist = c;
                else
                    sqrDist = c - b1 * b1 / a11;
            }
        }
        else if (t < 0.0f) // region 6
        {
            tmp0 = a01 + b1;
            tmp1 = a00 + b0;
            if (tmp1 > tmp0)
            {
                numer = tmp1 - tmp0;
                denom = a00 - 2.0f * a01 + a11;
                if (numer >= denom)
                {
                    t = 1.0f;
                    s = 0.0f;
                    sqrDist = a11 + 2.0f * b1 + c;
                }
                else
                {
                    t = numer / denom;
                    s = 1.0f - t;
                    sqrDist = s * (a00 * s + a01 * t + 2.0f * b0) +
                        t * (a01 * s + a11 * t + 2.0f * b1) + c;
                }
            }
            else
            {
                if (tmp1 <= 0.0f)
                    sqrDist = a00 + 2.0f * b0 + c;
                else if (b0 >= 0.0f)
                    sqrDist = c;
                else
                    sqrDist = c - b0 * b0 / a00;
            }
        }
        else // region 1
        {
            numer = a11 + b1 - a01 - b0;
            if (numer <= 0.0f)
            {
                sqrDist = a11 + 2.0f * b1 + c;
            }
            else
            {
                denom = a00 - 2.0f * a01 + a11;
                if (numer >= denom)
                {
                    sqrDist = a00 + 2.0f * b0 + c;
                }
                else
                {
                    s = numer / denom;
                    t = 1.0f - s;
                    sqrDist = s * (a00 * s + a01 * t + 2.0f * b0) +
                        t * (a01 * s + a11 * t + 2.0f * b1) + c;
                }
            }
        }
    }

    return std::sqrt(std::fabs(sqrDist));
}
