/********************************************************************
 * \file   IntrBox3Sphere3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTR_BOX3_SPHERE3_H
#define _MATH_INTR_BOX3_SPHERE3_H

#include "Intersector.h"
#include "Box3.h"
#include "Sphere3.h"

namespace Enigma::MathLib
{
    /** Intersector of Box3 & Sphere3 */
    class IntrBox3Sphere3 : public Intersector
    {
    public:
        IntrBox3Sphere3(const Box3& box, const Sphere3& sphere);

        // object access
        const Box3& GetBox() const;
        const Sphere3& GetSphere() const;

        // static test-intersection query
        virtual bool Test(IntersectorCache* last_result) override;

    private:
        // the objects to intersect
        const Box3& m_box;
        const Sphere3& m_sphere;
    };
};

#endif // !_MATH_INTR_BOX3_SPHERE3_H
