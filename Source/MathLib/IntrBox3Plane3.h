/********************************************************************
 * \file   IntrBox3Plane3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTR_BOX3_PLANE3_H
#define _MATH_INTR_BOX3_PLANE3_H

#include "Intersector.h"
#include "Box3.h"
#include "Plane3.h"

namespace Enigma::MathLib
{
    /** Intersector of Box3 & Plane3 */
    class IntrBox3Plane3 : public Intersector
    {
    public:
        IntrBox3Plane3(const Box3& box, const Plane3& plane, bool isAligned = false);

        // object access
        const Box3& GetBox() const;
        const Plane3& GetPlane() const;

        // static test-intersection query
        virtual Result Test(IntersectorCache* last_result) override;

        /// plane side test, 1 : positive side, -1 : negative side, 0 : overlap
        Plane3::SideOfPlane PlaneSideTest();

    private:
        // the objects to intersect
        const Box3& m_box;
        const Plane3& m_plane;
        bool m_isAligned;
    };

};

#endif // !_MATH_INTR_BOX3_PLANE3_H
