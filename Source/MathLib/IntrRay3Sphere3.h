/********************************************************************
 * \file   IntrRay3Sphere3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTR_RAY3_SPHERE3_H
#define _MATH_INTR_RAY3_SPHERE3_H

#include "Intersector.h"
#include "Sphere3.h"
#include "Ray3.h"

/** @file EnIntrRay3Sphere3.h */

namespace Enigma::MathLib
{
    /** Intersector of Ray3 & Sphere3 */
    class IntrRay3Sphere3 : public Intersector
    {
    public:
        IntrRay3Sphere3(const Ray3& ray, const Sphere3& sphere);

        // object access
        const Ray3& GetRay() const;
        const Sphere3& GetSphere() const;

        // static test-intersection query
        virtual Result Test(std::unique_ptr<IntersectorCache> last_result) override;
        virtual Result Find(std::unique_ptr<IntersectorCache> last_result) override;

        // the intersection set
        int GetQuantity() const;
        const Vector3& GetPoint(int i) const;
        float GetRayT(int i) const;

    private:
        // the objects to intersect
        const Ray3& m_ray;
        const Sphere3& m_sphere;

        // information about the intersection set
        int m_quantity;
        Vector3 m_point[2];
        float m_tParam[2];
    };

};

#endif // !_MATH_INTR_RAY3_SPHERE3_H
