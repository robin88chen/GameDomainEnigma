#include "IntrBox2Box2.h"
#include <cmath>

using namespace Enigma::MathLib;

IntrBox2Box2::IntrBox2Box2(const Box2& box0, const Box2& box1) :
    m_box0{ box0 }, m_box1{ box1 }
{
}

const Box2& IntrBox2Box2::GetBox0() const
{
    return m_box0;
}

const Box2& IntrBox2Box2::GetBox1() const
{
    return m_box1;
}

Intersector::Result IntrBox2Box2::test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    // convenience variables
    const Vector2* axisA = m_box0.Axis();
    const Vector2* axisB = m_box1.Axis();
    const float* extentA = m_box0.Extent();
    const float* extentB = m_box1.Extent();

    // compute difference of box centers, D = C1-C0
    Vector2 vecD = m_box1.Center() - m_box0.Center();

    float absADotB[2][2], absADotD, sum;

    // axis C0+t*A0
    absADotB[0][0] = fabs(axisA[0].Dot(axisB[0]));
    absADotB[0][1] = fabs(axisA[0].Dot(axisB[1]));
    absADotD = fabs(axisA[0].Dot(vecD));
    sum = extentA[0] + extentB[0] * absADotB[0][0] + extentB[1] * absADotB[0][1];
    if (absADotD > sum)
        return { false, nullptr };

    // axis C0+t*A1
    absADotB[1][0] = fabs(axisA[1].Dot(axisB[0]));
    absADotB[1][1] = fabs(axisA[1].Dot(axisB[1]));
    absADotD = fabs(axisA[1].Dot(vecD));
    sum = extentA[1] + extentB[0] * absADotB[1][0] + extentB[1] * absADotB[1][1];
    if (absADotD > sum)
        return { false, nullptr };

    // axis C0+t*B0
    absADotD = fabs(axisB[0].Dot(vecD));
    sum = extentB[0] + extentA[0] * absADotB[0][0] + extentA[1] * absADotB[1][0];
    if (absADotD > sum)
        return { false, nullptr };

    // axis C0+t*B1
    absADotD = fabs(axisB[1].Dot(vecD));
    sum = extentB[1] + extentA[0] * absADotB[0][1] + extentA[1] * absADotB[1][1];
    if (absADotD > sum)
        return { false, nullptr };

    return { true, nullptr };
}
