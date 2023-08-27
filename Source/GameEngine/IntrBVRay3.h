/*********************************************************************
 * \file   IntrBVRay3.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef INTRBVRAY3_H
#define INTRBVRAY3_H

#include "MathLib/Intersector.h"
#include "MathLib/Ray3.h"
#include <vector>

namespace Enigma::Engine
{
    class BoundingVolume;
    /** Intersector of Bounding Volume & Ray3 */
    class IntrBVRay3 : public MathLib::Intersector
    {
    public:
        IntrBVRay3(const BoundingVolume& bv, const MathLib::Ray3& ray);
        virtual ~IntrBVRay3();

        // object access
        const MathLib::Ray3& GetRay() const;
        const BoundingVolume& GetBV() const;

        // static test-intersection query
        virtual bool Test(MathLib::IntersectorCache* last_result) override;
        virtual bool Find(MathLib::IntersectorCache* last_result) override;

        // the intersection set
        size_t GetQuantity() const;
        const MathLib::Vector3& GetPoint(unsigned int i) const;
        float GetRayT(unsigned int i) const;

    private:
        // the objects to intersect
        const MathLib::Ray3& m_ray;
        const BoundingVolume& m_bv;

        // information about the intersection set
        std::vector<MathLib::Vector3> m_points;
        std::vector<float> m_tParams;
    };
};

#endif // INTRBVRAY3_H
