#include "IntrBox3Plane3.h"
#include <cmath>

using namespace Enigma::MathLib;

IntrBox3Plane3::IntrBox3Plane3(const Box3& box, const Plane3& plane, bool isAligned) :
    m_box{ box }, m_plane{ plane }, m_isAligned{ isAligned }
{
}

const Box3& IntrBox3Plane3::GetBox() const
{
    return m_box;
}

const Plane3& IntrBox3Plane3::GetPlane() const
{
    return m_plane;
}

bool IntrBox3Plane3::Test(IntersectorCache* /*last_result*/)
{
    if (PlaneSideTest() == Plane3::SideOfPlane::Overlap) return true;
    return false;
}

Plane3::SideOfPlane IntrBox3Plane3::PlaneSideTest()
{
    if (m_isAligned)
    {
        Vector3 vecMax = m_box.Center() + Vector3((float*)m_box.Extent());
        Vector3 vecMin = m_box.Center() - Vector3((float*)m_box.Extent());
        float t;
        if (m_plane.Normal().X() < 0.0f)
        {
            t = vecMax.X(); vecMax.X() = vecMin.X(); vecMin.X() = t;
        }
        if (m_plane.Normal().Y() < 0.0f)
        {
            t = vecMax.Y(); vecMax.Y() = vecMin.Y(); vecMin.Y() = t;
        }
        if (m_plane.Normal().Z() < 0.0f)
        {
            t = vecMax.Z(); vecMax.Z() = vecMin.Z(); vecMin.Z() = t;
        }

        if (m_plane.Normal().Dot(vecMin) > m_plane.Constant()) return Plane3::SideOfPlane::Positive;
        if (m_plane.Normal().Dot(vecMax) < m_plane.Constant()) return Plane3::SideOfPlane::Negative;
        return Plane3::SideOfPlane::Overlap;
    }
    else
    {
        float tmp[3] =
        {
            m_box.Extent(0) * (m_plane.Normal().Dot(m_box.Axis(0))),
            m_box.Extent(1) * (m_plane.Normal().Dot(m_box.Axis(1))),
            m_box.Extent(2) * (m_plane.Normal().Dot(m_box.Axis(2))),
        };

        float radius = fabs(tmp[0]) + fabs(tmp[1]) + fabs(tmp[2]);

        float signedDistance = m_plane.DistanceTo(m_box.Center());

        if (signedDistance - radius >= 0.0f)
        {
            return Plane3::SideOfPlane::Positive;
        }

        if (signedDistance + radius <= 0.0f)
        {
            return Plane3::SideOfPlane::Negative;
        }

        return Plane3::SideOfPlane::Overlap;
    }
}
