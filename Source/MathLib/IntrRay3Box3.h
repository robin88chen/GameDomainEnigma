/********************************************************************
 * \file   IntrRay3Box3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTR_RAY3_BOX3_H
#define _MATH_INTR_RAY3_BOX3_H

#include "Intersector.h"
#include "Box3.h"
#include "Ray3.h"

/** @file EnIntrRay3Box3.h */

namespace Enigma::MathLib
{
    /** Intersector of Ray3 & Box3 */
    class IntrRay3Box3 : public Intersector
    {
    public:
        IntrRay3Box3(const Ray3& ray, const Box3& box);

        // object access
        const Ray3& GetRay() const;
        const Box3& GetBox() const;

        // static test-intersection query
        virtual Result test(std::unique_ptr<IntersectorCache> last_result) override;
        virtual Result find(std::unique_ptr<IntersectorCache> last_result) override;

        // the intersection set
        int GetQuantity() const;
        const Vector3& GetPoint(int i) const;
        float GetRayT(int i) const;

    private:
        // the objects to intersect
        const Ray3& m_ray;
        const Box3& m_box;

        // information about the intersection set
        int m_quantity;
        Vector3 m_point[2];
        float m_tParam[2];
    };

};

#endif // !_MATH_INTR_RAY3_BOX3_H
