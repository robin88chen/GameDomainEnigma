#include "IntrRay3Triangle3.h"
#include "MathGlobal.h"

using namespace Enigma::MathLib;

IntrRay3Triangle3::IntrRay3Triangle3(const Ray3& ray, const Triangle3& triangle) :
    m_ray{ ray }, m_triangle{ triangle }
{
    m_intersectionType = IntersectionType::POINT;
    m_quantity = 0;
}

const Ray3& IntrRay3Triangle3::ray() const
{
    return m_ray;
}

const Triangle3& IntrRay3Triangle3::triangle() const
{
    return m_triangle;
}

Intersector::Result IntrRay3Triangle3::test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    Vector3 e1 = m_triangle[1] - m_triangle[0];
    Vector3 e2 = m_triangle[2] - m_triangle[0];
    Vector3 q = m_ray.direction().cross(e2);
    float a = e1.dot(q);
    if ((a > -Math::Epsilon()) && (a < Math::Epsilon())) return { false, nullptr };
    float f = 1.0f / a;
    Vector3 s = m_ray.origin() - m_triangle[0];
    float u = f * (s.dot(q));
    if (u < 0.0f || u > 1.0f) return { false, nullptr };
    Vector3 r = s.cross(e1);
    float v = f * (m_ray.direction().dot(r));
    if ((v < 0.0f) || (u + v > 1.0f)) return { false, nullptr };

    // if we got here, we found intersection
    float t = f * (e2.dot(r));
    m_quantity = 1;
    m_tParam = t;
    m_point = t * m_ray.direction() + m_ray.origin();
    return { true, nullptr };
}

Intersector::Result IntrRay3Triangle3::find(std::unique_ptr<IntersectorCache> last_result)
{
    // 因為沒有多少運算上的差異，所以直接呼叫Test
    return test(std::move(last_result));
}

int IntrRay3Triangle3::getQuantity() const
{
    return m_quantity;
}

const Vector3& IntrRay3Triangle3::getPoint() const
{
    return m_point;
}

float IntrRay3Triangle3::getRayT() const
{
    return m_tParam;
}
