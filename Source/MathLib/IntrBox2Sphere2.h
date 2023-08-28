/********************************************************************
 * \file   IntrBox2Sphere2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTR_BOX2_SPHERE2_H
#define _MATH_INTR_BOX2_SPHERE2_H

#include "Intersector.h"
#include "Box2.h"
#include "Sphere2.h"

namespace Enigma::MathLib
{
    /** Intersector of Box2 & Sphere2 */
    class IntrBox2Sphere2 : public Intersector
    {
    public:
        IntrBox2Sphere2(const Box2& box, const Sphere2& sphere);

        // object access
        const Box2& GetBox() const;
        const Sphere2& GetSphere() const;

        // static test-intersection query
        virtual Result Test(IntersectorCache* last_result) override;

    private:
        // the objects to intersect
        const Box2& m_box;
        const Sphere2& m_sphere;
    };
};

#endif // !_MATH_INTR_BOX2_SPHERE2_H
