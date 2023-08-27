/*********************************************************************
 * \file   IntrGeometryRay3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef INTR_GEOMETRY_RAY3_H
#define INTR_GEOMETRY_RAY3_H

#include "MathLib/Ray3.h"
#include "MathLib/Intersector.h"
#include "GeometryData.h"
#include <vector>

namespace Enigma::Engine
{
    class IntrGeometryCache;

    /** Intersect of Geometry & Ray3 */
    class IntrGeometryRay3 : public MathLib::Intersector
    {
    public:
        IntrGeometryRay3(const GeometryDataPtr& geo, const MathLib::Ray3& ray);
        virtual ~IntrGeometryRay3();

        // object access
        const MathLib::Ray3& GetRay() const;
        const GeometryDataPtr& GetGeometry();

        // static test-intersection query
        virtual bool Test(MathLib::IntersectorCache* last_result) override;
        virtual bool Find(MathLib::IntersectorCache* last_result) override;

        // the intersection set
        size_t GetQuantity() const;
        const MathLib::Vector3& GetPoint(unsigned int i) const;
        float GetRayT(unsigned int i) const;

        const std::vector<MathLib::Vector3>& GetPointArray() const;
        const std::vector<float>& GetParamArray() const;

    private:
        bool TestForTriangleList(IntrGeometryCache* geo_cache);
        bool FindForTriangleList(IntrGeometryCache* geo_cache);

    private:
        // the objects to intersect
        const MathLib::Ray3& m_ray;
        GeometryDataPtr m_geometry;

        // information about the intersection set
        std::vector<MathLib::Vector3> m_points;
        std::vector<float> m_tParams;
    };
};


#endif // INTR_GEOMETRY_RAY3_H
