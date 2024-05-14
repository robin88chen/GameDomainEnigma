#include "SphereBV.h"
#include "Platforms/PlatformLayer.h"
#include "MathLib/ContainmentSphere3.h"
#include <cassert>

using namespace Enigma::Engine;

SphereBV::SphereBV(const MathLib::Sphere3& sphere) : GenericBV()
{
    m_sphere = sphere;
}

SphereBV::~SphereBV()
{
}

void SphereBV::CreateFromTransform(const MathLib::Matrix4& mx, const std::unique_ptr<GenericBV>& source)
{
    if (FATAL_LOG_EXPR(!source)) return;
    SphereBV* sphere_bv = dynamic_cast<SphereBV*>(source.get());
    if (FATAL_LOG_EXPR(!sphere_bv)) return;

    m_sphere.Center() = mx.TransformCoord(sphere_bv->m_sphere.Center());
    float max_scale = mx.GetMaxScale();
    m_sphere.Radius() = max_scale * sphere_bv->m_sphere.Radius();
}

void SphereBV::Copy(const std::unique_ptr<GenericBV>& source)
{
    if (FATAL_LOG_EXPR(!source)) return;
    SphereBV* sphere_bv = dynamic_cast<SphereBV*>(source.get());
    if (FATAL_LOG_EXPR(!sphere_bv)) return;

    m_sphere = sphere_bv->m_sphere;
}

void SphereBV::ZeroReset()
{
    m_sphere = MathLib::Sphere3();
}

void SphereBV::MergeBoundingVolume(const MathLib::Matrix4& mx, const std::unique_ptr<GenericBV>& source)
{
    assert(!m_sphere.isEmpty());

    SphereBV merge_bv{ MathLib::Sphere3::UNIT_SPHERE };
    merge_bv.CreateFromTransform(mx, source);
    MathLib::ContainmentSphere3::MergeSpheres(merge_bv.m_sphere, m_sphere);
}

Enigma::MathLib::Plane3::SideOfPlane SphereBV::WhichSide(const MathLib::Plane3& plane) const
{
    float distance = plane.DistanceTo(m_sphere.Center());

    if (distance <= -m_sphere.Radius())
    {
        return MathLib::Plane3::SideOfPlane::Negative;
    }

    if (distance >= m_sphere.Radius())
    {
        return MathLib::Plane3::SideOfPlane::Positive;
    }

    return MathLib::Plane3::SideOfPlane::Overlap;
}

bool SphereBV::PointInside(const MathLib::Vector3& vecPos)
{
    MathLib::Vector3 vecDiff = vecPos - m_sphere.Center();
    float r = vecDiff.length();
    if (r > m_sphere.Radius()) return false;
    return true;
}

GenericBV::FlagBits SphereBV::PointInsideFlags(const MathLib::Vector3& pos)
{
    if (PointInside(pos)) return TestedAxis::XYZ;
    MathLib::Vector3 vecDiff = pos - m_sphere.Center();
    if (vecDiff.x() * vecDiff.x() + vecDiff.y() * vecDiff.y() < m_sphere.Radius() * m_sphere.Radius())
    {
        return TestedAxis::XY;
    }
    if (vecDiff.x() * vecDiff.x() + vecDiff.z() * vecDiff.z() < m_sphere.Radius() * m_sphere.Radius())
    {
        return TestedAxis::XZ;
    }
    if (vecDiff.y() * vecDiff.y() + vecDiff.z() * vecDiff.z() < m_sphere.Radius() * m_sphere.Radius())
    {
        return TestedAxis::YZ;
    }
    return TestedAxis::None;
}

void SphereBV::ComputeFromData(const MathLib::Vector3* pos, unsigned quantity, [[maybe_unused]] bool axis_align)
{
    assert(pos);
    assert(quantity > 0);
    m_sphere = MathLib::ContainmentSphere3::ComputeAverageSphere(pos, quantity);
}

void SphereBV::ComputeFromData(const MathLib::Vector4* pos, unsigned quantity, [[maybe_unused]] bool axis_align)
{
    assert(pos);
    assert(quantity > 0);
    m_sphere = MathLib::ContainmentSphere3::ComputeAverageSphere(pos, quantity);
}

void SphereBV::ComputeFromData(const float* vert, unsigned pitch, unsigned quantity, [[maybe_unused]] bool axis_align)
{
    assert(vert);
    assert(quantity > 0);
    m_sphere = MathLib::ContainmentSphere3::ComputeAverageSphere(vert, pitch, quantity);
}
