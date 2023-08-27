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

bool IntrBVRay3::Test(IntersectorCache*)
{
    if (auto box = m_bv.BoundingBox3())
    {
        IntrRay3Box3 intr_box(m_ray, *box);
        bool res = intr_box.Test(nullptr);
        return res;
    }
    if (auto sphere = m_bv.BoundingSphere3())
    {
        IntrRay3Sphere3 intr_sphere(m_ray, *sphere);
        bool res = intr_sphere.Test(nullptr);
        return res;
    }
    return false;
}

bool IntrBVRay3::Find(IntersectorCache*)
{
    m_points.clear();
    m_tParams.clear();

    if (auto box = m_bv.BoundingBox3())
    {
        IntrRay3Box3 intr_box(m_ray, *box);
        bool res = intr_box.Find(nullptr);
        if (res)
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
        bool res = intr_sphere.Find(0);
        if (res)
        {
            for (int i = 0; i < intr_sphere.GetQuantity(); i++)
            {
                m_tParams.emplace_back(intr_sphere.GetRayT(i));
                m_points.emplace_back(intr_sphere.GetPoint(i));
            }
        }
        return res;
    }
    return false;
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
