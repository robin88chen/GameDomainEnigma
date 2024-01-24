#include "TrianglePlaneClipper.h"
#include "MathGlobal.h"
#include <cmath>

using namespace Enigma::MathLib;

TrianglePlaneClipper::TrianglePlaneClipper()
{
}

TrianglePlaneClipper::~TrianglePlaneClipper()
{
}

void TrianglePlaneClipper::SetPlanes(const std::vector<Plane3>& clipPlanes)
{
    m_clipPlanes = clipPlanes;
}

void TrianglePlaneClipper::SetTriangles(const std::vector<Triangle3>& triangles)
{
    m_triangles = triangles;
}

void TrianglePlaneClipper::Clip()
{
    m_resultTriangles.clear();
    if ((m_clipPlanes.empty()) || (m_triangles.empty())) return;
    m_resultTriangles.reserve(m_clipPlanes.size() * m_triangles.size());
    for (auto triangle : m_triangles)
    {
        std::vector<Triangle3> tempTriangles1;
        tempTriangles1.reserve(m_clipPlanes.size() * 3);
        std::vector<Triangle3> tempTriangles2;
        tempTriangles2.reserve(m_clipPlanes.size() * 3);
        tempTriangles1.emplace_back(triangle);
        unsigned int tempResultCount = 0;
        for (unsigned int pli = 0; pli < m_clipPlanes.size(); pli++)
        {
            if (pli % 2 == 0)
            {
                tempTriangles2.clear();
                tempResultCount = ClipTriangle(m_clipPlanes[pli], tempTriangles1, tempTriangles2);
            }
            else
            {
                tempTriangles1.clear();
                tempResultCount = ClipTriangle(m_clipPlanes[pli], tempTriangles2, tempTriangles1);
            }
            if (tempResultCount == 0) break;
        }
        if (tempResultCount > 0)
        {
            std::vector<Triangle3>* tempResult = 0;
            if (m_clipPlanes.size() % 2 == 1)
            {  // result is Temp2
                tempResult = &tempTriangles2;
            }
            else
            {
                tempResult = &tempTriangles1;
            }
            for (unsigned int ri = 0; ri < tempResult->size(); ri++)
            {
                m_resultTriangles.emplace_back((*tempResult)[ri]);
            }
        }

    }
}

unsigned int TrianglePlaneClipper::ClipTriangle(const Plane3& clipPlane,
    const std::vector<Triangle3>& srcTriangles, std::vector<Triangle3>& destTriangles)
{
    if (srcTriangles.size() == 0) return 0;

    for (unsigned int ti = 0; ti < srcTriangles.size(); ti++)
    {
        int negativeCount = 0;
        bool isNegative[3];
        for (unsigned int vi = 0; vi < 3; vi++)
        {
            Plane3::SideOfPlane side = clipPlane.WhichSide(srcTriangles[ti][vi]);
            isNegative[vi] = side == Plane3::SideOfPlane::Negative;
            if (isNegative[vi]) negativeCount++;
        }
        if (negativeCount >= 3) continue;  // 整個三角形都在背面，跳到下一個三角形
        if (negativeCount == 0)  // 都在正面，三角形直接加入
        {
            destTriangles.emplace_back(srcTriangles[ti]);
        }
        else if (negativeCount == 1) // 正面有兩個點
        {
            Triangle3 trianglePart[2];
            bool is_ok = SplitTwoTriangleByPlane(srcTriangles[ti], clipPlane, isNegative, trianglePart[0], trianglePart[1]);
            if (is_ok)
            {
                destTriangles.emplace_back(trianglePart[0]);
                destTriangles.emplace_back(trianglePart[1]);
            }
        }
        else if (negativeCount == 2)  // 正面只有一個點
        {
            Triangle3 trianglePart;
            bool is_ok = SplitOneTriangleByPlane(srcTriangles[ti], clipPlane, isNegative, trianglePart);
            if (is_ok) destTriangles.emplace_back(trianglePart);
        }
        else break;  // error go here!!
    }
    return (unsigned int)destTriangles.size();
}

bool TrianglePlaneClipper::SplitOneTriangleByPlane(const Triangle3& triangle, const Plane3& plane,
    bool isNegative[3], Triangle3& destTriangle)
{
    if (!destTriangle) return false;
    unsigned int va = 4;
    for (unsigned int v = 0; v < 3; v++)
    {
        if (!isNegative[v])
        {
            va = v;
            break;
        }
    }
    if (va >= 3) return false;
    unsigned int vb = va + 1;
    vb %= 3;
    unsigned int vc = va + 2;
    vc %= 3;

    // intersect Pt = ( P1 - P0 )t + P0
    // t = ( D - N dot P0 ) / ( N dot ( P1 - P0 ) )
    float dt = plane.Constant() - plane.Normal().dot(triangle[va]);
    float ndvb = plane.Normal().dot(triangle[vb] - triangle[va]);
    if (std::fabs(ndvb) < Math::Epsilon()) return false;
    float ndvc = plane.Normal().dot(triangle[vc] - triangle[va]);
    if (std::fabs(ndvc) < Math::Epsilon()) return false;
    destTriangle[va] = triangle[va];
    destTriangle[vb] = (dt / ndvb) * (triangle[vb] - triangle[va]) + triangle[va];
    destTriangle[vc] = (dt / ndvc) * (triangle[vc] - triangle[va]) + triangle[va];

    return true;
}

bool TrianglePlaneClipper::SplitTwoTriangleByPlane(const Triangle3& triangle, const Plane3& plane,
    bool isNegative[3], Triangle3& destTriangle1, Triangle3& destTriangle2)
{
    if ((!destTriangle1) || (!destTriangle2)) return false;
    unsigned int va = 4;
    for (unsigned int v = 0; v < 3; v++)
    {
        if (isNegative[v])
        {
            va = v;
            break;
        }
    }
    if (va >= 3) return false;
    unsigned int vb = va + 1;
    vb %= 3;
    unsigned int vc = va + 2;
    vc %= 3;

    // intersect Pt = ( P1 - P0 )t + P0
    // t = ( D - N dot P0 ) / ( N dot ( P1 - P0 ) )
    float dt = plane.Constant() - plane.Normal().dot(triangle[va]);
    float ndvb = plane.Normal().dot(triangle[vb] - triangle[va]);
    if (std::fabs(ndvb) < Math::Epsilon()) return false;
    float ndvc = plane.Normal().dot(triangle[vc] - triangle[va]);
    if (std::fabs(ndvc) < Math::Epsilon()) return false;
    Vector3 vecInterAB = (dt / ndvb) * (triangle[vb] - triangle[va]) + triangle[va];
    Vector3 vecInterAC = (dt / ndvc) * (triangle[vc] - triangle[va]) + triangle[va];

    destTriangle1[va] = vecInterAB;
    destTriangle1[vb] = triangle[vb];
    destTriangle1[vc] = triangle[vc];
    destTriangle2[va] = vecInterAC;
    destTriangle2[vb] = vecInterAB;
    destTriangle2[vc] = triangle[vc];

    return true;
}

