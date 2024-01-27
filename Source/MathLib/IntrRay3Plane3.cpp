#include "IntrRay3Plane3.h"
#include "MathGlobal.h"

using namespace Enigma::MathLib;

IntrRay3Plane3::IntrRay3Plane3(const Ray3& ray, const Plane3& plane) :
    m_ray{ ray }, m_plane{ plane }
{
    m_intersectionType = IntersectionType::POINT;
    m_quantity = 0;
}

const Ray3& IntrRay3Plane3::GetRay() const
{
    return m_ray;
}

const Plane3& IntrRay3Plane3::GetPlane() const
{
    return m_plane;
}

Intersector::Result IntrRay3Plane3::test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    float e = m_plane.Normal().dot(m_ray.direction());
    if ((e <= Math::Epsilon()) && (e >= -Math::Epsilon())) return { false, nullptr };
    float t = (m_plane.Constant() - m_plane.Normal().dot(m_ray.origin())) / e;
    if (t < 0.0f) return { false, nullptr };
    m_quantity = 1;
    m_tParam = t;
    m_point = t * m_ray.direction() + m_ray.origin();
    return { true, nullptr };
}

Intersector::Result IntrRay3Plane3::find(std::unique_ptr<IntersectorCache> last_result)
{
    // 因為沒有多少運算上的差異，所以直接呼叫Test
    return test(std::move(last_result));
}

int IntrRay3Plane3::GetQuantity() const
{
    return m_quantity;
}

const Vector3& IntrRay3Plane3::GetPoint() const
{
    return m_point;
}

float IntrRay3Plane3::GetRayT() const
{
    return m_tParam;
}
