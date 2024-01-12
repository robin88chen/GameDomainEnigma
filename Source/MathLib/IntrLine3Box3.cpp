#include "IntrLine3Box3.h"
#include "MathGlobal.h"
#include <cassert>
#include <cmath>

using namespace Enigma::MathLib;

IntrLine3Box3::IntrLine3Box3(const Line3& line, const Box3& box) :
    m_line{ line }, m_box{ box }
{
    m_intersectionType = IntersectionType::POINT;
    m_quantity = 0;
}

const Line3& IntrLine3Box3::GetLine() const
{
    return m_line;
}

const Box3& IntrLine3Box3::GetBox() const
{
    return m_box;
}

Intersector::Result IntrLine3Box3::test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    /** algorism from 3D game engine architecture p.520~p.521 & wildmagic code */
    /** 有點複雜的理論，還跟Minkowski difference有關 */
    float AWdU[3], AWxDdU[3], rhs;

    Vector3 diff = m_line.origin() - m_box.Center();
    Vector3 WxD = m_line.direction().Cross(diff);

    AWdU[1] = fabs(m_line.direction().Dot(m_box.Axis(1)));
    AWdU[2] = fabs(m_line.direction().Dot(m_box.Axis(2)));
    AWxDdU[0] = fabs(WxD.Dot(m_box.Axis(0)));
    rhs = m_box.Extent(1) * AWdU[2] + m_box.Extent(2) * AWdU[1];
    if (AWxDdU[0] > rhs)
    {
        return { false, nullptr };
    }

    AWdU[0] = fabs(m_line.direction().Dot(m_box.Axis(0)));
    AWxDdU[1] = fabs(WxD.Dot(m_box.Axis(1)));
    rhs = m_box.Extent(0) * AWdU[2] + m_box.Extent(2) * AWdU[0];
    if (AWxDdU[1] > rhs)
    {
        return { false, nullptr };
    }

    AWxDdU[2] = fabs(WxD.Dot(m_box.Axis(2)));
    rhs = m_box.Extent(0) * AWdU[1] + m_box.Extent(1) * AWdU[0];
    if (AWxDdU[2] > rhs)
    {
        return { false, nullptr };
    }

    return { true, nullptr };
}

Intersector::Result IntrLine3Box3::find(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    float t0 = -Math::MAX_FLOAT, t1 = Math::MAX_FLOAT;

    // convert linear component to box coordinates
    Vector3 diff = m_line.origin() - m_box.Center();
    Vector3 BOrigin(
        diff.Dot(m_box.Axis(0)),
        diff.Dot(m_box.Axis(1)),
        diff.Dot(m_box.Axis(2))
    );
    Vector3 BDirection(
        m_line.direction().Dot(m_box.Axis(0)),
        m_line.direction().Dot(m_box.Axis(1)),
        m_line.direction().Dot(m_box.Axis(2))
    );

    bool isNotAllClipped =
        Clip(+BDirection.X(), -BOrigin.X() - m_box.Extent(0), t0, t1) &&
        Clip(-BDirection.X(), +BOrigin.X() - m_box.Extent(0), t0, t1) &&
        Clip(+BDirection.Y(), -BOrigin.Y() - m_box.Extent(1), t0, t1) &&
        Clip(-BDirection.Y(), +BOrigin.Y() - m_box.Extent(1), t0, t1) &&
        Clip(+BDirection.Z(), -BOrigin.Z() - m_box.Extent(2), t0, t1) &&
        Clip(-BDirection.Z(), +BOrigin.Z() - m_box.Extent(2), t0, t1);

    if (isNotAllClipped)
    {
        if (t1 > t0)
        {
            m_quantity = 2;
            m_point[0] = m_line.origin() + t0 * m_line.direction();
            m_point[1] = m_line.origin() + t1 * m_line.direction();
            m_lineT[0] = t0;
            m_lineT[1] = t1;
        }
        else
        {
            m_quantity = 1;
            m_point[0] = m_line.origin() + t0 * m_line.direction();
            m_lineT[0] = t0;
        }
    }
    else
    {
        m_quantity = 0;
    }

    return { (m_quantity != 0), nullptr };
}

bool IntrLine3Box3::Clip(float denom, float numer, float& t0, float& t1)
{
    // Return value is 'true' if line segment intersects the current test
    // plane.  Otherwise 'false' is returned in which case the line segment
    // is entirely clipped.

    if (denom > 0.0f)
    {
        if (numer > denom * t1)
        {
            return false;
        }
        if (numer > denom * t0)
        {
            t0 = numer / denom;
        }
        return true;
    }
    else if (denom < 0.0f)
    {
        if (numer > denom * t0)
        {
            return false;
        }
        if (numer > denom * t1)
        {
            t1 = numer / denom;
        }
        return true;
    }
    else
    {
        return numer <= 0.0f;
    }
}

int IntrLine3Box3::GetQuantity() const
{
    return m_quantity;
}

const Vector3& IntrLine3Box3::GetPoint(int i) const
{
    assert((i < 2) && (i >= 0));
    return m_point[i];
}

float IntrLine3Box3::GetLineT(int i) const
{
    assert((i < 2) && (i >= 0));
    return m_lineT[i];
}
