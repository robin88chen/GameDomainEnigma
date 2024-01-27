#include "IntrBox2Sphere2.h"
#include <cmath>

using namespace Enigma::MathLib;

IntrBox2Sphere2::IntrBox2Sphere2(const Box2& box, const Sphere2& sphere) :
    m_box{ box }, m_sphere{ sphere }
{
}

const Box2& IntrBox2Sphere2::GetBox() const
{
    return m_box;
}

const Sphere2& IntrBox2Sphere2::GetSphere() const
{
    return m_sphere;
}

Intersector::Result IntrBox2Sphere2::test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    // Test for intersection in the coordinate system of the box by
    // transforming the sphere into that coordinate system.
    Vector2 centerDiff = m_sphere.Center() - m_box.Center();

    float ax = fabs(centerDiff.dot(m_box.Axis(0)));
    float ay = fabs(centerDiff.dot(m_box.Axis(1)));
    float dx = ax - m_box.Extent(0);
    float dy = ay - m_box.Extent(1);

    return { ((dx <= m_sphere.Radius()) && (dy <= m_sphere.Radius())), nullptr };
}
