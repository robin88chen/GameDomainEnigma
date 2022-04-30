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
    if (q0.Dot(q1) < 0.0f)
    {
        q1 = -q1;
    }

    Quaternion q = q0 + q1;
    float invLength = 1.0f / std::sqrt(q.Dot(q));
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
            dot = diff.Dot(boxMerge.Axis(j));
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
            dot = diff.Dot(boxMerge.Axis(j));
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
    Vector3 vecMax = box0.Center() + Vector3((float*)box0.Extent());
    Vector3 vecMin = box0.Center() - Vector3((float*)box0.Extent());

    Vector3 vecMax1 = box1.Center() + Vector3((float*)box1.Extent());
    Vector3 vecMin1 = box1.Center() - Vector3((float*)box1.Extent());

    if (vecMax.X() < vecMax1.X())
    {
        vecMax.X() = vecMax1.X();
    }
    if (vecMax.Y() < vecMax1.Y())
    {
        vecMax.Y() = vecMax1.Y();
    }
    if (vecMax.Z() < vecMax1.Z())
    {
        vecMax.Z() = vecMax1.Z();
    }
    if (vecMin.X() > vecMin1.X())
    {
        vecMin.X() = vecMin1.X();
    }
    if (vecMin.Y() > vecMin1.Y())
    {
        vecMin.Y() = vecMin1.Y();
    }
    if (vecMin.Z() > vecMin1.Z())
    {
        vecMin.Z() = vecMin1.Z();
    }

    Box3 box;
    box.Center() = 0.5f * (vecMax + vecMin);
    Vector3 vecDiff = vecMax - box.Center();
    box.Extent(0) = vecDiff.X();
    box.Extent(1) = vecDiff.Y();
    box.Extent(2) = vecDiff.Z();

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
        if (pos[i].X() > vecMax.X()) vecMax.X() = pos[i].X();
        if (pos[i].Y() > vecMax.Y()) vecMax.Y() = pos[i].Y();
        if (pos[i].Z() > vecMax.Z()) vecMax.Z() = pos[i].Z();
        if (pos[i].X() < vecMin.X()) vecMin.X() = pos[i].X();
        if (pos[i].Y() < vecMin.Y()) vecMin.Y() = pos[i].Y();
        if (pos[i].Z() < vecMin.Z()) vecMin.Z() = pos[i].Z();
    }
    Vector3 center = 0.5f * (vecMax + vecMin);
    Vector3 extend = vecMax - center;
    // 加一個基本大小
    if (extend.X() <= Math::EPSILON) extend.X() = Math::EPSILON * 10.0f;
    if (extend.Y() <= Math::EPSILON) extend.Y() = Math::EPSILON * 10.0f;
    if (extend.Z() <= Math::EPSILON) extend.Z() = Math::EPSILON * 10.0f;
    return Box3(
        center, Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z,
        extend.X(), extend.Y(), extend.Z());
}

Box3 ContainmentBox3::ComputeAlignedBox(const Vector4* pos, unsigned int quantity)
{
    assert(pos != 0);
    assert(quantity != 0);

    Vector3 vecMin = Vector3(pos[0].X(), pos[0].Y(), pos[0].Z());
    Vector3 vecMax = vecMin;
    for (unsigned int i = 1; i < quantity; i++)
    {
        if (pos[i].X() > vecMax.X()) vecMax.X() = pos[i].X();
        if (pos[i].Y() > vecMax.Y()) vecMax.Y() = pos[i].Y();
        if (pos[i].Z() > vecMax.Z()) vecMax.Z() = pos[i].Z();
        if (pos[i].X() < vecMin.X()) vecMin.X() = pos[i].X();
        if (pos[i].Y() < vecMin.Y()) vecMin.Y() = pos[i].Y();
        if (pos[i].Z() < vecMin.Z()) vecMin.Z() = pos[i].Z();
    }
    Vector3 center = 0.5f * (vecMax + vecMin);
    Vector3 extend = vecMax - center;
    // 加一個基本大小
    if (extend.X() <= Math::EPSILON) extend.X() = Math::EPSILON * 10.0f;
    if (extend.Y() <= Math::EPSILON) extend.Y() = Math::EPSILON * 10.0f;
    if (extend.Z() <= Math::EPSILON) extend.Z() = Math::EPSILON * 10.0f;
    return Box3(
        center, Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z,
        extend.X(), extend.Y(), extend.Z());
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
        if (vert[index] > vecMax.X()) vecMax.X() = vert[index];
        if (vert[index + 1] > vecMax.Y()) vecMax.Y() = vert[index + 1];
        if (vert[index + 2] > vecMax.Z()) vecMax.Z() = vert[index + 2];
        if (vert[index] < vecMin.X()) vecMin.X() = vert[index];
        if (vert[index + 1] < vecMin.Y()) vecMin.Y() = vert[index + 1];
        if (vert[index + 2] < vecMin.Z()) vecMin.Z() = vert[index + 2];
        index += pitch;
    }
    Vector3 center = 0.5f * (vecMax + vecMin);
    Vector3 extend = vecMax - center;
    // 加一個基本大小
    if (extend.X() <= Math::EPSILON) extend.X() = Math::EPSILON * 10.0f;
    if (extend.Y() <= Math::EPSILON) extend.Y() = Math::EPSILON * 10.0f;
    if (extend.Z() <= Math::EPSILON) extend.Z() = Math::EPSILON * 10.0f;
    return Box3(
        center, Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z,
        extend.X(), extend.Y(), extend.Z());
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
        sum_xx += diff.X() * diff.X();
        sum_xy += diff.X() * diff.Y();
        sum_xz += diff.X() * diff.Z();
        sum_yy += diff.Y() * diff.Y();
        sum_yz += diff.Y() * diff.Z();
        sum_zz += diff.Z() * diff.Z();
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
    Vector3 vecMin = Vector3(vecTrans.Dot(box.Axis(0)), vecTrans.Dot(box.Axis(1)), vecTrans.Dot(box.Axis(2)));
    Vector3 vecMax = vecMin;
    for (unsigned int i = 1; i < quantity; i++)
    {
        vecTrans = pos[i] - box.Center();
        float dot = vecTrans.Dot(box.Axis(0));
        if (dot < vecMin.X()) vecMin.X() = dot;
        if (dot > vecMax.X()) vecMax.X() = dot;
        dot = vecTrans.Dot(box.Axis(1));
        if (dot < vecMin.Y()) vecMin.Y() = dot;
        if (dot > vecMax.Y()) vecMax.Y() = dot;
        dot = vecTrans.Dot(box.Axis(2));
        if (dot < vecMin.Z()) vecMin.Z() = dot;
        if (dot > vecMax.Z()) vecMax.Z() = dot;
    }

    box.Center() += (0.5f * (vecMin.X() + vecMax.X()) * box.Axis(0)
        + 0.5f * (vecMin.Y() + vecMax.Y()) * box.Axis(1)
        + 0.5f * (vecMin.Z() + vecMax.Z()) * box.Axis(2));
    box.Extent(0) = 0.5f * (vecMax.X() - vecMin.X());
    box.Extent(1) = 0.5f * (vecMax.Y() - vecMin.Y());
    box.Extent(2) = 0.5f * (vecMax.Z() - vecMin.Z());

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
        pos[i] = Vector3(pos4[i].X(), pos4[i].Y(), pos4[i].Z());
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
