/*********************************************************************
 * \file   IntrRay3Triangle3.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef INTR_RAY3_TRIANGLE3_H
#define INTR_RAY3_TRIANGLE3_H

#include "Intersector.h"
#include "Triangle3.h"
#include "Ray3.h"

namespace Enigma::MathLib
{
    /** Intersector of Ray3 & Plane3 */
    class IntrRay3Triangle3 : public Intersector
    {
    public:
        IntrRay3Triangle3(const Ray3& ray, const Triangle3& triangle);

        // object access
        const Ray3& ray() const;
        const Triangle3& triangle() const;

        // static test-intersection query
        virtual Result test(std::unique_ptr<IntersectorCache> last_result) override;
        virtual Result find(std::unique_ptr<IntersectorCache> last_result) override;

        // the intersection set
        int getQuantity() const;
        const Vector3& getPoint() const;
        float getRayT() const;

    private:
        // the objects to intersect
        const Ray3& m_ray;
        const Triangle3& m_triangle;

        // information about the intersection set
        int m_quantity;
        Vector3 m_point;
        float m_tParam;
    };
};


#endif // INTR_RAY3_TRIANGLE3_H
