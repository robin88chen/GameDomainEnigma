#include "IntrBox3Sphere3.h"
#include <cmath>

using namespace Enigma::MathLib;

IntrBox3Sphere3::IntrBox3Sphere3(const Box3& box, const Sphere3& sphere) :
    m_box{ box }, m_sphere{ sphere }
{
}

const Box3& IntrBox3Sphere3::GetBox() const
{
    return m_box;
}

const Sphere3& IntrBox3Sphere3::GetSphere() const
{
    return m_sphere;
}

Intersector::Result IntrBox3Sphere3::Test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    // Test for intersection in the coordinate system of the box by
    // transforming the sphere into that coordinate system.
    Vector3 centerDiff = m_sphere.Center() - m_box.Center();

    float ax = fabs(centerDiff.Dot(m_box.Axis(0)));
    float ay = fabs(centerDiff.Dot(m_box.Axis(1)));
    float az = fabs(centerDiff.Dot(m_box.Axis(2)));
    float dx = ax - m_box.Extent(0);
    float dy = ay - m_box.Extent(1);
    float dz = az - m_box.Extent(2);

    if (ax <= m_box.Extent(0))
    {
        if (ay <= m_box.Extent(1))
        {
            if (az <= m_box.Extent(2))
            {
                // sphere center inside box
                return { true, nullptr };
            }
            else
            {
                // potential sphere-face intersection with face z
                return { dz <= m_sphere.Radius(), nullptr };
            }
        }
        else
        {
            if (az <= m_box.Extent(2))
            {
                // potential sphere-face intersection with face y
                return { dy <= m_sphere.Radius(), nullptr };
            }
            else
            {
                // potential sphere-edge intersection with edge formed
                // by faces y and z
                float sqrRadius = m_sphere.Radius() * m_sphere.Radius();
                return { dy * dy + dz * dz <= sqrRadius, nullptr };
            }
        }
    }
    else
    {
        if (ay <= m_box.Extent(1))
        {
            if (az <= m_box.Extent(2))
            {
                // potential sphere-face intersection with face x
                return { dx <= m_sphere.Radius(), nullptr };
            }
            else
            {
                // potential sphere-edge intersection with edge formed
                // by faces x and z
                float sqrRadius = m_sphere.Radius() * m_sphere.Radius();
                return { dx * dx + dz * dz <= sqrRadius, nullptr };
            }
        }
        else
        {
            if (az <= m_box.Extent(2))
            {
                // potential sphere-edge intersection with edge formed
                // by faces x and y
                float sqrRadius = m_sphere.Radius() * m_sphere.Radius();
                return { dx * dx + dy * dy <= sqrRadius, nullptr };
            }
            else
            {
                // potential sphere-vertex intersection at corner formed
                // by faces x,y,z
                float sqrRadius = m_sphere.Radius() * m_sphere.Radius();
                return { dx * dx + dy * dy + dz * dz <= sqrRadius, nullptr };
            }
        }
    }
}
