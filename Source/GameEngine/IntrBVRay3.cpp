#include "IntrBVRay3.h"
#include "BoundingVolume.h"
#include "MathLib/IntrRay3Box3.h"
#include "MathLib/IntrRay3Sphere3.h"
#include <cassert>

using namespace Enigma::Engine;
using namespace Enigma::MathLib;

IntrBVRay3::IntrBVRay3(const BoundingVolume& bv, const Ray3& ray) :
    Intersector(), m_bv(bv), m_ray(ray)
{
    m_intersectionType = IntersectionType::POINT;
}

IntrBVRay3::~IntrBVRay3()
{
}

const Ray3& IntrBVRay3::GetRay() const
{
    return m_ray;
}

const BoundingVolume& IntrBVRay3::GetBV() const
{
    return m_bv;
}

Intersector::Result IntrBVRay3::test(std::unique_ptr<IntersectorCache>)
{
    if (auto box = m_bv.BoundingBox3())
    {
        IntrRay3Box3 intr_box(m_ray, *box);
        auto res = intr_box.test(nullptr);
        return res;
    }
    if (auto sphere = m_bv.BoundingSphere3())
    {
        IntrRay3Sphere3 intr_sphere(m_ray, *sphere);
        auto res = intr_sphere.test(nullptr);
        return res;
    }
    return { false, nullptr };
}

Intersector::Result IntrBVRay3::find(std::unique_ptr<IntersectorCache>)
{
    m_points.clear();
    m_tParams.clear();

    if (auto box = m_bv.BoundingBox3())
    {
        IntrRay3Box3 intr_box(m_ray, *box);
        auto res = intr_box.find(nullptr);
        if (res.m_hasIntersect)
        {
            for (int i = 0; i < intr_box.GetQuantity(); i++)
            {
                m_tParams.emplace_back(intr_box.GetRayT(i));
                m_points.emplace_back(intr_box.GetPoint(i));
            }
        }
        return res;
    }
    if (auto sphere = m_bv.BoundingSphere3())
    {
        IntrRay3Sphere3 intr_sphere(m_ray, *sphere);
        auto res = intr_sphere.find(0);
        if (res.m_hasIntersect)
        {
            for (int i = 0; i < intr_sphere.GetQuantity(); i++)
            {
                m_tParams.emplace_back(intr_sphere.GetRayT(i));
                m_points.emplace_back(intr_sphere.GetPoint(i));
            }
        }
        return res;
    }
    return { false, nullptr };
}

size_t IntrBVRay3::GetQuantity() const
{
    return m_points.size();
}

const Vector3& IntrBVRay3::GetPoint(unsigned int i) const
{
    assert(i < m_points.size());
    return m_points[i];
}

float IntrBVRay3::GetRayT(unsigned int i) const
{
    assert(i < m_tParams.size());
    return m_tParams[i];
}
