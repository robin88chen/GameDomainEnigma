#include "IntrPrimitiveRay3.h"
#include "Primitive.h"
#include "PrimitiveRay3IntersectionFinder.h"
#include "Platforms/MemoryMacro.h"
#include "PrimitiveIntersectionFinderFactories.h"
#include <cassert>
#include <algorithm>

using namespace Enigma::Primitives;
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
    m_intersectionFinder = PrimitiveRay3IntersectionFinderFactory::createPrimitiveRay3IntersectionFinder(m_primitive->typeInfo());
}

IntrPrimitiveRay3::~IntrPrimitiveRay3()
{
    m_primitive = nullptr;
    SAFE_DELETE(m_intersectionFinder);
}

const Ray3& IntrPrimitiveRay3::ray() const
{
    return m_ray;
}

const std::shared_ptr<Primitive>& IntrPrimitiveRay3::primitive()
{
    return m_primitive;
}

Intersector::Result IntrPrimitiveRay3::test(std::unique_ptr<IntersectorCache> last_result)
{
    if (!m_primitive) return { false, std::move(last_result) };
    if (!m_intersectionFinder) return { false, std::move(last_result) };

    return m_intersectionFinder->test(m_primitive, m_ray, std::move(last_result));
}

Intersector::Result IntrPrimitiveRay3::find(std::unique_ptr<IntersectorCache> last_result)
{
    if (!m_primitive) return { false, std::move(last_result) };
    if (!m_intersectionFinder) return { false, std::move(last_result) };

    Intersector::Result result;
    std::tie(m_resultRecords, result) = m_intersectionFinder->find(m_primitive, m_ray, std::move(last_result));
    sort(m_resultRecords.begin(), m_resultRecords.end(), compare_result);
    return result;
}

size_t IntrPrimitiveRay3::getQuantity() const
{
    return m_resultRecords.size();
}

const Vector3& IntrPrimitiveRay3::getPoint(unsigned int i) const
{
    assert(i < m_resultRecords.size());
    return m_resultRecords[i].m_vecPoint;
}

float IntrPrimitiveRay3::getRayT(unsigned int i) const
{
    assert(i < m_resultRecords.size());
    return m_resultRecords[i].m_tParam;
}

std::shared_ptr<Primitive> IntrPrimitiveRay3::getResultPrimitive(unsigned int i) const
{
    assert(i < m_resultRecords.size());
    return m_resultRecords[i].m_primitive.lock();
}

void IntrPrimitiveRay3::setRequiredResultCount(unsigned count)
{
    if (m_intersectionFinder) m_intersectionFinder->setRequiredResultCount(count);
}

const std::vector<IntrPrimitiveRay3::ResultRecord>& IntrPrimitiveRay3::getResultRecord() const
{
    return m_resultRecords;
}
