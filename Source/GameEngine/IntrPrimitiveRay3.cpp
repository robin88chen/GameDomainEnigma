#include "IntrPrimitiveRay3.h"
#include "Primitive.h"
#include "PrimitiveRay3IntersectionFinder.h"
#include "Platforms/MemoryMacro.h"
#include "IntersectionFinderFactories.h"
#include <cassert>
#include <algorithm>

using namespace Enigma::Engine;
using namespace Enigma::MathLib;

bool compare_result(const IntrPrimitiveRay3::ResultRecord& rec1,
    const IntrPrimitiveRay3::ResultRecord& rec2)
{
    return rec1.m_tParam < rec2.m_tParam;
}

IntrPrimitiveRay3::IntrPrimitiveRay3(const std::shared_ptr<Primitive>& primitive, const Ray3& ray) :
    Intersector(), m_ray(ray), m_primitive(primitive)
{
    assert(m_primitive);
    m_intersectionType = IntersectionType::POINT;
    m_intersectionFinder = PrimitiveRay3IntersectionFinderFactory::CreatePrimitiveRay3IntersectionFinder(m_primitive->TypeInfo().GetName());
}

IntrPrimitiveRay3::~IntrPrimitiveRay3()
{
    m_primitive = nullptr;
    SAFE_DELETE(m_intersectionFinder);
}

const Ray3& IntrPrimitiveRay3::GetRay() const
{
    return m_ray;
}

const std::shared_ptr<Primitive>& IntrPrimitiveRay3::GetPrimitive()
{
    return m_primitive;
}

Intersector::Result IntrPrimitiveRay3::Test(IntersectorCache* last_result)
{
    if (!m_primitive) return { false, last_result };
    if (!m_intersectionFinder) return { false, last_result };

    return m_intersectionFinder->Test(m_primitive, m_ray, last_result);
}

Intersector::Result IntrPrimitiveRay3::Find(IntersectorCache* last_result)
{
    if (!m_primitive) return { false, last_result };
    if (!m_intersectionFinder) return { false, last_result };

    Intersector::Result result;
    std::tie(m_resultRecords, result) = m_intersectionFinder->Find(m_primitive, m_ray, last_result);
    sort(m_resultRecords.begin(), m_resultRecords.end(), compare_result);
    return result;
}
