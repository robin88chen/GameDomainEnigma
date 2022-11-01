#include "BoundingVolume.h"
#include "BoxBV.h"
#include "SphereBV.h"

using namespace Enigma::Engine;

BoundingVolume::BoundingVolume()
{
}

BoundingVolume::BoundingVolume(const BoundingVolumeContract& contract)
{
    if (contract.Box())
    {
        m_bv = std::make_unique<BoxBV>(contract.Box().value());
    }
    else if (contract.Sphere())
    {
        m_bv = std::make_unique<SphereBV>(contract.Sphere().value());
    }
}

BoundingVolume::BoundingVolume(const MathLib::Box3& box)
{
    m_bv = std::make_unique<BoxBV>(box);
}

BoundingVolume::BoundingVolume(const MathLib::Sphere3& sphere)
{
    m_bv = std::make_unique<SphereBV>(sphere);
}

BoundingVolume::BoundingVolume(const BoundingVolume& bv)
{
    if (bv.BoundingBox3())
    {
        m_bv = std::make_unique<BoxBV>(bv.BoundingBox3().value());
    }
    else if (bv.BoundingSphere3())
    {
        m_bv = std::make_unique<SphereBV>(bv.BoundingSphere3().value());
    }
}

BoundingVolume::BoundingVolume(BoundingVolume&& bv)
{
    if (bv.m_bv)
    {
        m_bv = std::move(bv.m_bv);
    }
}

BoundingVolume::~BoundingVolume()
{
}

BoundingVolume& BoundingVolume::operator=(const BoundingVolume& bv)
{
    if (bv.BoundingBox3())
    {
        m_bv = std::make_unique<BoxBV>(bv.BoundingBox3().value());
    }
    else if (bv.BoundingSphere3())
    {
        m_bv = std::make_unique<SphereBV>(bv.BoundingSphere3().value());
    }

    return *this;
}

BoundingVolume& BoundingVolume::operator=(BoundingVolume&& bv)
{
    if (bv.m_bv)
    {
        m_bv = std::move(bv.m_bv);
    }

    return *this;
}

BoundingVolumeContract BoundingVolume::SerializeContract() const
{
    return BoundingVolumeContract{ BoundingBox3(), BoundingSphere3() };
}

BoundingVolume BoundingVolume::CreateFromTransform(const BoundingVolume& source_bv, const MathLib::Matrix4& mx)
{
    BoundingVolume bv(source_bv); // will make bv & source_bv have same implement BV type
    if (bv.m_bv)
    {
        bv.m_bv->CreateFromTransform(mx, source_bv.m_bv);
    }
    return bv;
}

void BoundingVolume::Merge(const MathLib::Matrix4& to_mx, const BoundingVolume& to_bv)
{
    if ((m_bv) && (to_bv.m_bv))
    {
        m_bv->MergeBoundingVolume(to_mx, to_bv.m_bv);
    }
}

bool BoundingVolume::IsEmpty() const
{
    if (!m_bv) return true;
    return m_bv->IsEmpty();
}

std::optional<Enigma::MathLib::Box3> BoundingVolume::BoundingBox3() const
{
    if (!m_bv) return std::nullopt;
    if (const auto box_bv = dynamic_cast<BoxBV*>(m_bv.get()))
    {
        return box_bv->GetBox();
    }
    return std::nullopt;
}

std::optional<Enigma::MathLib::Sphere3> BoundingVolume::BoundingSphere3() const
{
    if (!m_bv) return std::nullopt;
    if (const auto sphere_bv = dynamic_cast<SphereBV*>(m_bv.get()))
    {
        return sphere_bv->GetSphere();
    }
    return std::nullopt;
}

bool BoundingVolume::PointInside(const MathLib::Vector3& pos) const
{
    if (m_bv) return m_bv->PointInside(pos);
    return false;
}

BoundingVolume::Side BoundingVolume::SideOfPlane(const MathLib::Plane3& plane) const
{
    if (!m_bv) return Side::Failed;
    MathLib::Plane3::SideOfPlane s = m_bv->WhichSide(plane);
    if (s == MathLib::Plane3::SideOfPlane::Positive) return Side::Positive;
    if (s == MathLib::Plane3::SideOfPlane::Negative) return Side::Negative;
    return Side::Overlap;
}
