#include "ContainmentBox3.h"
#include "Quaternion.h"
#include "MathGlobal.h"
#include <cassert>
#include <cmath>
#include <vector>

using namespace Enigma::MathLib;

//----------------------------------------------------------------------------
// Merge Boxes
// 中心點為兩個Box中心點的中心，再用Quaternion取得兩組軸向的旋轉值，取出slerp平均值
// 最後再以兩個Box的頂點取出最大最小值，得到邊長
// Box要將三個軸取出Major x,y,z，這樣子在做Quaternion時，才會取得比較小的平均旋轉值，
// 否則會因為角度差異太大，造成算出來的Box比實際需要大很多
//----------------------------------------------------------------------------
Box3 ContainmentBox3::MergeBoxes(const Box3& box0, const Box3& box1)
{
    // construct a box that contains the input boxes
    Box3 boxMerge;
    Box3 boxMajor0, boxMajor1;
    boxMajor0 = box0.SwapToMajorAxis();
    boxMajor1 = box1.SwapToMajorAxis();

    // The first guess at the box center.  This value will be updated later
    // after the input box vertices are projected onto axes determined by an
    // average of box axes.
    boxMerge.Center() = 0.5f * (boxMajor0.Center() + boxMajor1.Center());

    // A box's axes, when viewed as the columns of a matrix, form a rotation
    // matrix.  The input box axes are converted to quaternions.  The average
    // quaternion is computed, then normalized to unit length.  The result is
    // the slerp of the two input quaternions with t-value of 1/2.  The result
    // is converted back to a rotation matrix and its columns are selected as
    // the merged box axes.
    Quaternion q0, q1;
    q0 = Quaternion::FromRotationMatrix(Matrix3(boxMajor0.Axis(), true));
    q1 = Quaternion::FromRotationMatrix(Matrix3(boxMajor1.Axis(), true));
    if (q0.dot(q1) < 0.0f)
    {
        q1 = -q1;
    }

    Quaternion q = q0 + q1;
    float invLength = 1.0f / std::sqrt(q.dot(q));
    q = invLength * q;
    Matrix3 rotMerge = q.ToRotationMatrix();
    boxMerge.Axis(0) = rotMerge.GetColumn(0);
    boxMerge.Axis(1) = rotMerge.GetColumn(1);
    boxMerge.Axis(2) = rotMerge.GetColumn(2);

    // Project the input box vertices onto the merged-box axes.  Each axis
    // D[i] containing the current center C has a minimum projected value
    // pmin[i] and a maximum projected value pmax[i].  The corresponding end
    // points on the axes are C+pmin[i]*D[i] and C+pmax[i]*D[i].  The point C
    // is not necessarily the midpoint for any of the intervals.  The actual
    // box center will be adjusted from C to a point C' that is the midpoint
    // of each interval,
    //   C' = C + sum_{i=0}^2 0.5*(pmin[i]+pmax[i])*D[i]
    // The box extents are
    //   e[i] = 0.5*(pmax[i]-pmin[i])

    int i, j;
    float dot;
    Vector3 diff;
    Vector3 vecMin = Vector3::ZERO;
    Vector3 vecMax = Vector3::ZERO;

    std::array<Vector3, 8> vertex = boxMajor0.ComputeVertices();
    for (i = 0; i < 8; i++)
    {
        diff = vertex[i] - boxMerge.Center();
        for (j = 0; j < 3; j++)
        {
            dot = diff.dot(boxMerge.Axis(j));
            if (dot > vecMax[j])
            {
                vecMax[j] = dot;
            }
            else if (dot < vecMin[j])
            {
                vecMin[j] = dot;
            }
        }
    }

    vertex = boxMajor1.ComputeVertices();
    for (i = 0; i < 8; i++)
    {
        diff = vertex[i] - boxMerge.Center();
        for (j = 0; j < 3; j++)
        {
            dot = diff.dot(boxMerge.Axis(j));
            if (dot > vecMax[j])
            {
                vecMax[j] = dot;
            }
            else if (dot < vecMin[j])
            {
                vecMin[j] = dot;
            }
        }
    }

    // [kMin,kMax] is the axis-aligned box in the coordinate system of the
    // merged box axes.  Update the current box center to be the center of
    // the new box.  Compute the extents based on the new center.
    for (j = 0; j < 3; j++)
    {
        boxMerge.Center() += (0.5f * (vecMax[j] + vecMin[j])) * boxMerge.Axis(j);
        boxMerge.Extent(j) = 0.5f * (vecMax[j] - vecMin[j]);
    }

    return boxMerge;
}

Box3 ContainmentBox3::MergeAlignedBoxes(const Box3& box0, const Box3& box1)
{
    // 取兩個box的最大最小位置
    Vector3 vecMax = box0.Center() + Vector3(box0.Extent());
    Vector3 vecMin = box0.Center() - Vector3(box0.Extent());

    Vector3 vecMax1 = box1.Center() + Vector3(box1.Extent());
    Vector3 vecMin1 = box1.Center() - Vector3(box1.Extent());

    if (vecMax.x() < vecMax1.x())
    {
        vecMax.x() = vecMax1.x();
    }
    if (vecMax.y() < vecMax1.y())
    {
        vecMax.y() = vecMax1.y();
    }
    if (vecMax.z() < vecMax1.z())
    {
        vecMax.z() = vecMax1.z();
    }
    if (vecMin.x() > vecMin1.x())
    {
        vecMin.x() = vecMin1.x();
    }
    if (vecMin.y() > vecMin1.y())
    {
        vecMin.y() = vecMin1.y();
    }
    if (vecMin.z() > vecMin1.z())
    {
        vecMin.z() = vecMin1.z();
    }

    Box3 box;
    box.Center() = 0.5f * (vecMax + vecMin);
    Vector3 vecDiff = vecMax - box.Center();
    box.Extent(0) = vecDiff.x();
    box.Extent(1) = vecDiff.y();
    box.Extent(2) = vecDiff.z();

    return box;
}

Box3 ContainmentBox3::ComputeAlignedBox(const Vector3* pos, unsigned int quantity)
{
    assert(pos != 0);
    assert(quantity != 0);

    Vector3 vecMin;
    Vector3 vecMax;
    vecMax = pos[0];
    vecMin = pos[0];
    for (unsigned int i = 1; i < quantity; i++)
    {
        if (pos[i].x() > vecMax.x()) vecMax.x() = pos[i].x();
        if (pos[i].y() > vecMax.y()) vecMax.y() = pos[i].y();
        if (pos[i].z() > vecMax.z()) vecMax.z() = pos[i].z();
        if (pos[i].x() < vecMin.x()) vecMin.x() = pos[i].x();
        if (pos[i].y() < vecMin.y()) vecMin.y() = pos[i].y();
        if (pos[i].z() < vecMin.z()) vecMin.z() = pos[i].z();
    }
    Vector3 center = 0.5f * (vecMax + vecMin);
    Vector3 extend = vecMax - center;
    // 加一個基本大小
    if (extend.x() <= Math::Epsilon()) extend.x() = Math::Epsilon() * 10.0f;
    if (extend.y() <= Math::Epsilon()) extend.y() = Math::Epsilon() * 10.0f;
    if (extend.z() <= Math::Epsilon()) extend.z() = Math::Epsilon() * 10.0f;
    return Box3(
        center, Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z,
        extend.x(), extend.y(), extend.z());
}

Box3 ContainmentBox3::ComputeAlignedBox(const Vector4* pos, unsigned int quantity)
{
    assert(pos != 0);
    assert(quantity != 0);

    Vector3 vecMin = Vector3(pos[0].x(), pos[0].y(), pos[0].z());
    Vector3 vecMax = vecMin;
    for (unsigned int i = 1; i < quantity; i++)
    {
        if (pos[i].x() > vecMax.x()) vecMax.x() = pos[i].x();
        if (pos[i].y() > vecMax.y()) vecMax.y() = pos[i].y();
        if (pos[i].z() > vecMax.z()) vecMax.z() = pos[i].z();
        if (pos[i].x() < vecMin.x()) vecMin.x() = pos[i].x();
        if (pos[i].y() < vecMin.y()) vecMin.y() = pos[i].y();
        if (pos[i].z() < vecMin.z()) vecMin.z() = pos[i].z();
    }
    Vector3 center = 0.5f * (vecMax + vecMin);
    Vector3 extend = vecMax - center;
    // 加一個基本大小
    if (extend.x() <= Math::Epsilon()) extend.x() = Math::Epsilon() * 10.0f;
    if (extend.y() <= Math::Epsilon()) extend.y() = Math::Epsilon() * 10.0f;
    if (extend.z() <= Math::Epsilon()) extend.z() = Math::Epsilon() * 10.0f;
    return Box3(
        center, Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z,
        extend.x(), extend.y(), extend.z());
}

Box3 ContainmentBox3::ComputeAlignedBox(const float* vert, unsigned int pitch, unsigned int quantity)
{
    assert(vert != 0);
    assert(quantity != 0);

    Vector3 vecMin = Vector3(vert[0], vert[1], vert[2]);
    Vector3 vecMax = vecMin;
    unsigned int index = pitch;
    for (unsigned int i = 1; i < quantity; i++)
    {
        if (vert[index] > vecMax.x()) vecMax.x() = vert[index];
        if (vert[index + 1] > vecMax.y()) vecMax.y() = vert[index + 1];
        if (vert[index + 2] > vecMax.z()) vecMax.z() = vert[index + 2];
        if (vert[index] < vecMin.x()) vecMin.x() = vert[index];
        if (vert[index + 1] < vecMin.y()) vecMin.y() = vert[index + 1];
        if (vert[index + 2] < vecMin.z()) vecMin.z() = vert[index + 2];
        index += pitch;
    }
    Vector3 center = 0.5f * (vecMax + vecMin);
    Vector3 extend = vecMax - center;
    // 加一個基本大小
    if (extend.x() <= Math::Epsilon()) extend.x() = Math::Epsilon() * 10.0f;
    if (extend.y() <= Math::Epsilon()) extend.y() = Math::Epsilon() * 10.0f;
    if (extend.z() <= Math::Epsilon()) extend.z() = Math::Epsilon() * 10.0f;
    return Box3(
        center, Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z,
        extend.x(), extend.y(), extend.z());
}

Box3 ContainmentBox3::ComputeOrientedBox(const Vector3* pos, unsigned int quantity)
{
    assert(pos != 0);
    assert(quantity != 0);

    Vector3 center = pos[0];
    for (unsigned int i = 1; i < quantity; i++)
    {
        center += pos[i];
    }
    float inv_quantity = 1.0f / (float)(quantity);
    center *= inv_quantity;

    float sum_xx = 0.0f, sum_xy = 0.0f, sum_xz = 0.0f, sum_yy = 0.0f, sum_yz = 0.0f, sum_zz = 0.0f;
    for (unsigned int i = 0; i < quantity; i++)
    {
        Vector3 diff = pos[i] - center;
        sum_xx += diff.x() * diff.x();
        sum_xy += diff.x() * diff.y();
        sum_xz += diff.x() * diff.z();
        sum_yy += diff.y() * diff.y();
        sum_yz += diff.y() * diff.z();
        sum_zz += diff.z() * diff.z();
    }
    sum_xx *= inv_quantity;
    sum_xy *= inv_quantity;
    sum_xz *= inv_quantity;
    sum_yy *= inv_quantity;
    sum_yz *= inv_quantity;
    sum_zz *= inv_quantity;

    Matrix3 mxEigen;
    mxEigen[0][0] = sum_xx;
    mxEigen[0][1] = sum_xy;
    mxEigen[0][2] = sum_xz;
    mxEigen[1][0] = sum_xy;
    mxEigen[1][1] = sum_yy;
    mxEigen[1][2] = sum_yz;
    mxEigen[2][0] = sum_xz;
    mxEigen[2][1] = sum_yz;
    mxEigen[2][2] = sum_zz;

    EigenDecompose<Matrix3> eigen_decompose = mxEigen.EigenDecomposition();

    Box3 box;
    box.Center() = center;
    box.Axis(0) = eigen_decompose.m_rot.GetColumn(0);
    box.Axis(1) = eigen_decompose.m_rot.GetColumn(1);
    box.Axis(2) = eigen_decompose.m_rot.GetColumn(2);

    Vector3 vecTrans = pos[0] - box.Center();
    Vector3 vecMin = Vector3(vecTrans.dot(box.Axis(0)), vecTrans.dot(box.Axis(1)), vecTrans.dot(box.Axis(2)));
    Vector3 vecMax = vecMin;
    for (unsigned int i = 1; i < quantity; i++)
    {
        vecTrans = pos[i] - box.Center();
        float dot = vecTrans.dot(box.Axis(0));
        if (dot < vecMin.x()) vecMin.x() = dot;
        if (dot > vecMax.x()) vecMax.x() = dot;
        dot = vecTrans.dot(box.Axis(1));
        if (dot < vecMin.y()) vecMin.y() = dot;
        if (dot > vecMax.y()) vecMax.y() = dot;
        dot = vecTrans.dot(box.Axis(2));
        if (dot < vecMin.z()) vecMin.z() = dot;
        if (dot > vecMax.z()) vecMax.z() = dot;
    }

    box.Center() += (0.5f * (vecMin.x() + vecMax.x()) * box.Axis(0)
        + 0.5f * (vecMin.y() + vecMax.y()) * box.Axis(1)
        + 0.5f * (vecMin.z() + vecMax.z()) * box.Axis(2));
    box.Extent(0) = 0.5f * (vecMax.x() - vecMin.x());
    box.Extent(1) = 0.5f * (vecMax.y() - vecMin.y());
    box.Extent(2) = 0.5f * (vecMax.z() - vecMin.z());

    // 將axis 0,1,2調整到主要為x,y,z (測試一下看merge box會不會比較正確)
    return box.SwapToMajorAxis();
}

Box3 ContainmentBox3::ComputeOrientedBox(const Vector4* pos4, unsigned int quantity)
{
    assert(pos4 != 0);
    assert(quantity != 0);

    std::vector<Vector3> pos(quantity);
    for (unsigned int i = 0; i < quantity; i++)
    {
        pos[i] = Vector3(pos4[i].x(), pos4[i].y(), pos4[i].z());
    }

    Box3 box = ComputeOrientedBox(&pos[0], quantity);
    return box;
}

Box3 ContainmentBox3::ComputeOrientedBox(const float* vert, unsigned int pitch, unsigned int quantity)
{
    assert(vert != 0);
    assert(quantity != 0);

    std::vector<Vector3> pos(quantity);
    unsigned int index = 0;
    for (unsigned int i = 0; i < quantity; i++)
    {
        pos[i] = Vector3(vert[index], vert[index + 1], vert[index + 2]);
        index += pitch;
    }

    Box3 box = ComputeOrientedBox(&pos[0], quantity);

    return box;
}
