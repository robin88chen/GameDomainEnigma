#include "IntrLine3Sphere3.h"
#include "MathGlobal.h"
#include <cmath>

using namespace Enigma::MathLib;

IntrLine3Sphere3::IntrLine3Sphere3(const Line3& line, const Sphere3& sphere) :
    m_line{ line }, m_sphere{ sphere }
{
    m_intersectionType = IntersectionType::POINT;
    m_quantity = 0;
}

const Line3& IntrLine3Sphere3::GetLine() const
{
    return m_line;
}

const Sphere3& IntrLine3Sphere3::GetSphere() const
{
    return m_sphere;
}

Intersector::Result IntrLine3Sphere3::test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    Vector3 diff = m_line.origin() - m_sphere.Center();
    float a0 = diff.Dot(diff) - m_sphere.Radius() * m_sphere.Radius();
    float a1 = m_line.direction().Dot(diff);
    float discr = a1 * a1 - a0;
    return { discr >= 0.0f, nullptr };
}

Intersector::Result IntrLine3Sphere3::find(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    Vector3 diff = m_line.origin() - m_sphere.Center();
    float a0 = diff.Dot(diff) - m_sphere.Radius() * m_sphere.Radius();
    float a1 = m_line.direction().Dot(diff);
    float discr = a1 * a1 - a0;

    if (discr < 0.0f)
    {
        m_quantity = 0;
    }
    else if (discr >= Math::ZERO_TOLERANCE)
    {
        float root = sqrt(discr);
        m_lineT[0] = -a1 - root;
        m_lineT[1] = -a1 + root;
        m_point[0] = m_line.origin() + m_lineT[0] * m_line.direction();
        m_point[1] = m_line.origin() + m_lineT[1] * m_line.direction();
        m_quantity = 2;
    }
    else
    {
        m_lineT[0] = -a1;
        m_point[0] = m_line.origin() + m_lineT[0] * m_line.direction();
        m_quantity = 1;
    }

    return { m_quantity > 0, nullptr };
}

int IntrLine3Sphere3::GetQuantity() const
{
    return m_quantity;
}

const Vector3& IntrLine3Sphere3::GetPoint(int i) const
{
    if (i >= 2) return Vector3::ZERO;
    if (0 > i && i >= m_quantity) return Vector3::ZERO;
    return m_point[i];
}

float IntrLine3Sphere3::GetLineT(int i) const
{
    if (i >= 2) return 0.0f;
    if (0 > i && i >= m_quantity) return 0.0f;
    return m_lineT[i];
}
