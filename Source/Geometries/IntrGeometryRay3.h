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
#include "IntrGeometryCache.h"
#include <vector>

namespace Enigma::Geometries
{
    class IntrGeometryCache;

    /** Intersect of Geometry & Ray3 */
    class IntrGeometryRay3 : public MathLib::Intersector
    {
    public:
        IntrGeometryRay3(const GeometryDataPtr& geo, const MathLib::Ray3& ray);
        virtual ~IntrGeometryRay3();

        // object access
        const MathLib::Ray3& ray() const;
        const GeometryDataPtr& geometry();

        // static test-intersection query
        virtual Result test(std::unique_ptr<MathLib::IntersectorCache> last_result) override;
        virtual Result find(std::unique_ptr<MathLib::IntersectorCache> last_result) override;

        // the intersection set
        size_t getQuantity() const;
        const MathLib::Vector3& getPoint(unsigned int i) const;
        float getRayT(unsigned int i) const;

        const std::vector<MathLib::Vector3>& getPointArray() const;
        const std::vector<float>& getParamArray() const;

    private:
        Result testForTriangleList(std::unique_ptr<IntrGeometryCache> geo_cache);
        Result findForTriangleList(std::unique_ptr<IntrGeometryCache> geo_cache);

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
