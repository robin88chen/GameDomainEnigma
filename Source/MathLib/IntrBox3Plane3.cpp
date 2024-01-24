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

Intersector::Result IntrBox3Plane3::test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    if (PlaneSideTest() == Plane3::SideOfPlane::Overlap) return { true, nullptr };
    return { false, nullptr };
}

Plane3::SideOfPlane IntrBox3Plane3::PlaneSideTest()
{
    if (m_isAligned)
    {
        Vector3 vecMax = m_box.Center() + Vector3((float*)m_box.Extent());
        Vector3 vecMin = m_box.Center() - Vector3((float*)m_box.Extent());
        float t;
        if (m_plane.Normal().x() < 0.0f)
        {
            t = vecMax.x(); vecMax.x() = vecMin.x(); vecMin.x() = t;
        }
        if (m_plane.Normal().y() < 0.0f)
        {
            t = vecMax.y(); vecMax.y() = vecMin.y(); vecMin.y() = t;
        }
        if (m_plane.Normal().z() < 0.0f)
        {
            t = vecMax.z(); vecMax.z() = vecMin.z(); vecMin.z() = t;
        }

        if (m_plane.Normal().dot(vecMin) > m_plane.Constant()) return Plane3::SideOfPlane::Positive;
        if (m_plane.Normal().dot(vecMax) < m_plane.Constant()) return Plane3::SideOfPlane::Negative;
        return Plane3::SideOfPlane::Overlap;
    }
    else
    {
        float tmp[3] =
        {
            m_box.Extent(0) * (m_plane.Normal().dot(m_box.Axis(0))),
            m_box.Extent(1) * (m_plane.Normal().dot(m_box.Axis(1))),
            m_box.Extent(2) * (m_plane.Normal().dot(m_box.Axis(2))),
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
