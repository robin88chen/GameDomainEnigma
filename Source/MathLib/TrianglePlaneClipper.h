/*********************************************************************
 * \file   TrianglePlaneClipper.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _TRIANGLE_PLANE_CLIPPER_H
#define _TRIANGLE_PLANE_CLIPPER_H

#include "Plane3.h"
#include "Triangle3.h"
#include <vector>

namespace Enigma::MathLib
{
    /** Triangle Plane Clipper */
    class TrianglePlaneClipper
    {
    public:
        TrianglePlaneClipper();
        ~TrianglePlaneClipper();

        void SetPlanes(const std::vector<Plane3>& clipPlanes);
        void SetTriangles(const std::vector<Triangle3>& triangles);

        void Clip();
        unsigned int ClippedTriangleCount() { return (unsigned int)m_resultTriangles.size(); };
        std::vector<Triangle3>& GetResultTriangles() { return m_resultTriangles; };

    private:
        unsigned int ClipTriangle(const Plane3& clipPlane, const std::vector<Triangle3>& srcTriangles, std::vector<Triangle3>& destTriangles);
        bool SplitOneTriangleByPlane(const Triangle3& triangle, const Plane3& plane, bool isNegative[3],
            Triangle3& destTriangle);
        bool SplitTwoTriangleByPlane(const Triangle3& triangle, const Plane3& plane, bool isNegative[3],
            Triangle3& destTriangle1, Triangle3& destTriangle2);

    private:
        std::vector<Plane3> m_clipPlanes;
        std::vector<Triangle3> m_triangles;

        std::vector<Triangle3> m_resultTriangles;
    };
};

#endif // !_TRIANGLE_PLANE_CLIPPER_H
