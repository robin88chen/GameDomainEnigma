/********************************************************************
 * \file   IntrRay3Plane3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTR_RAY3_PLANE3_H
#define _MATH_INTR_RAY3_PLANE3_H

#include "Intersector.h"
#include "Plane3.h"
#include "Ray3.h"

/** @file EnIntrRay3Plane3.h */

namespace Enigma::MathLib
{
    /** Intersector of Ray3 & Plane3 */
    class IntrRay3Plane3 : public Intersector
    {
    public:
        IntrRay3Plane3(const Ray3& ray, const Plane3& plane);

        // object access
        const Ray3& GetRay() const;
        const Plane3& GetPlane() const;

        // static test-intersection query
        virtual Result Test(std::unique_ptr<IntersectorCache> last_result) override;
        virtual Result Find(std::unique_ptr<IntersectorCache> last_result) override;

        // the intersection set
        int GetQuantity() const;
        const Vector3& GetPoint() const;
        float GetRayT() const;

    private:
        // the objects to intersect
        const Ray3& m_ray;
        const Plane3& m_plane;

        // information about the intersection set
        int m_quantity;
        Vector3 m_point;
        float m_tParam;
    };

};

#endif // !_MATH_INTR_RAY3_PLANE3_H
