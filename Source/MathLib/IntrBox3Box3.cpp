#include "IntrBox3Box3.h"
#include "MathGlobal.h"
#include <cmath>

using namespace Enigma::MathLib;

IntrBox3Box3::IntrBox3Box3(const Box3& box0, const Box3& box1) :
    m_box0{ box0 }, m_box1{ box1 }
{
}

const Box3& IntrBox3Box3::GetBox0() const
{
    return m_box0;
}

const Box3& IntrBox3Box3::GetBox1() const
{
    return m_box1;
}

Intersector::Result IntrBox3Box3::Test(IntersectorCache* /*last_result*/)
{
    // Cutoff for cosine of angles between box axes.  This is used to catch
    // the cases when at least one pair of axes are parallel.  If this happens,
    // there is no need to test for separation along the Cross(A[i],B[j])
    // directions.
    const float cutoff = 1.0f - Math::ZERO_TOLERANCE;
    bool isExistsParallelPair = false;
    int i;

    // convenience variables
    const Vector3* axisA = m_box0.Axis();
    const Vector3* axisB = m_box1.Axis();
    const float* extentA = m_box0.Extent();
    const float* extentB = m_box1.Extent();

    // compute difference of box centers, D = C1-C0
    Vector3 vecD = m_box1.Center() - m_box0.Center();

    float mxC[3][3];     // matrix C = A^T B, c_{ij} = Dot(A_i,B_j)
    float absC[3][3];  // |c_{ij}|
    float axDotD[3];        // Dot(A_i,D)
    float radius0, radius1, radius;   // interval radii and distance between centers
    float radius01;           // = R0 + R1

    // axis C0+t*A0
    for (i = 0; i < 3; i++)
    {
        mxC[0][i] = axisA[0].Dot(axisB[i]);
        absC[0][i] = fabs(mxC[0][i]);
        if (absC[0][i] > cutoff)
            isExistsParallelPair = true;
    }
    axDotD[0] = axisA[0].Dot(vecD);
    radius = fabs(axDotD[0]);
    radius1 = extentB[0] * absC[0][0] + extentB[1] * absC[0][1] + extentB[2] * absC[0][2];
    radius01 = extentA[0] + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*A1
    for (i = 0; i < 3; i++)
    {
        mxC[1][i] = axisA[1].Dot(axisB[i]);
        absC[1][i] = fabs(mxC[1][i]);
        if (absC[1][i] > cutoff)
            isExistsParallelPair = true;
    }
    axDotD[1] = axisA[1].Dot(vecD);
    radius = fabs(axDotD[1]);
    radius1 = extentB[0] * absC[1][0] + extentB[1] * absC[1][1] + extentB[2] * absC[1][2];
    radius01 = extentA[1] + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*A2
    for (i = 0; i < 3; i++)
    {
        mxC[2][i] = axisA[2].Dot(axisB[i]);
        absC[2][i] = fabs(mxC[2][i]);
        if (absC[2][i] > cutoff)
            isExistsParallelPair = true;
    }
    axDotD[2] = axisA[2].Dot(vecD);
    radius = fabs(axDotD[2]);
    radius1 = extentB[0] * absC[2][0] + extentB[1] * absC[2][1] + extentB[2] * absC[2][2];
    radius01 = extentA[2] + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*B0
    radius = fabs(axisB[0].Dot(vecD));
    radius0 = extentA[0] * absC[0][0] + extentA[1] * absC[1][0] + extentA[2] * absC[2][0];
    radius01 = radius0 + extentB[0];
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*B1
    radius = fabs(axisB[1].Dot(vecD));
    radius0 = extentA[0] * absC[0][1] + extentA[1] * absC[1][1] + extentA[2] * absC[2][1];
    radius01 = radius0 + extentB[1];
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*B2
    radius = fabs(axisB[2].Dot(vecD));
    radius0 = extentA[0] * absC[0][2] + extentA[1] * absC[1][2] + extentA[2] * absC[2][2];
    radius01 = radius0 + extentB[2];
    if (radius > radius01)
        return { false, nullptr };

    // At least one pair of box axes was parallel, so the separation is
    // effectively in 2D where checking the "edge" normals is sufficient for
    // the separation of the boxes.
    if (isExistsParallelPair)
        return { true, nullptr };

    // axis C0+t*A0xB0
    radius = fabs(axDotD[2] * mxC[1][0] - axDotD[1] * mxC[2][0]);
    radius0 = extentA[1] * absC[2][0] + extentA[2] * absC[1][0];
    radius1 = extentB[1] * absC[0][2] + extentB[2] * absC[0][1];
    radius01 = radius0 + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*A0xB1
    radius = fabs(axDotD[2] * mxC[1][1] - axDotD[1] * mxC[2][1]);
    radius0 = extentA[1] * absC[2][1] + extentA[2] * absC[1][1];
    radius1 = extentB[0] * absC[0][2] + extentB[2] * absC[0][0];
    radius01 = radius0 + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*A0xB2
    radius = fabs(axDotD[2] * mxC[1][2] - axDotD[1] * mxC[2][2]);
    radius0 = extentA[1] * absC[2][2] + extentA[2] * absC[1][2];
    radius1 = extentB[0] * absC[0][1] + extentB[1] * absC[0][0];
    radius01 = radius0 + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*A1xB0
    radius = fabs(axDotD[0] * mxC[2][0] - axDotD[2] * mxC[0][0]);
    radius0 = extentA[0] * absC[2][0] + extentA[2] * absC[0][0];
    radius1 = extentB[1] * absC[1][2] + extentB[2] * absC[1][1];
    radius01 = radius0 + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*A1xB1
    radius = fabs(axDotD[0] * mxC[2][1] - axDotD[2] * mxC[0][1]);
    radius0 = extentA[0] * absC[2][1] + extentA[2] * absC[0][1];
    radius1 = extentB[0] * absC[1][2] + extentB[2] * absC[1][0];
    radius01 = radius0 + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*A1xB2
    radius = fabs(axDotD[0] * mxC[2][2] - axDotD[2] * mxC[0][2]);
    radius0 = extentA[0] * absC[2][2] + extentA[2] * absC[0][2];
    radius1 = extentB[0] * absC[1][1] + extentB[1] * absC[1][0];
    radius01 = radius0 + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*A2xB0
    radius = fabs(axDotD[1] * mxC[0][0] - axDotD[0] * mxC[1][0]);
    radius0 = extentA[0] * absC[1][0] + extentA[1] * absC[0][0];
    radius1 = extentB[1] * absC[2][2] + extentB[2] * absC[2][1];
    radius01 = radius0 + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*A2xB1
    radius = fabs(axDotD[1] * mxC[0][1] - axDotD[0] * mxC[1][1]);
    radius0 = extentA[0] * absC[1][1] + extentA[1] * absC[0][1];
    radius1 = extentB[0] * absC[2][2] + extentB[2] * absC[2][0];
    radius01 = radius0 + radius1;
    if (radius > radius01)
        return { false, nullptr };

    // axis C0+t*A2xB2
    radius = fabs(axDotD[1] * mxC[0][2] - axDotD[0] * mxC[1][2]);
    radius0 = extentA[0] * absC[1][2] + extentA[1] * absC[0][2];
    radius1 = extentB[0] * absC[2][1] + extentB[1] * absC[2][0];
    radius01 = radius0 + radius1;
    if (radius > radius01)
        return { false, nullptr };

    return { true, nullptr };
}
