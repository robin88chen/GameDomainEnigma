/********************************************************************
 * \file   IntrBox3Box3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTR_BOX3_BOX3_H
#define _MATH_INTR_BOX3_BOX3_H

#include "Intersector.h"
#include "Box3.h"

namespace Enigma::MathLib
{
    /** Intersector of Box3 & Box3 */
    class IntrBox3Box3 : public Intersector
    {
    public:
        IntrBox3Box3(const Box3& box0, const Box3& box1);

        // object access
        const Box3& GetBox0() const;
        const Box3& GetBox1() const;

        // static test-intersection query
        virtual Result Test(IntersectorCache* last_result) override;

    private:
        // the objects to intersect
        const Box3& m_box0;
        const Box3& m_box1;
    };

};

#endif // !_MATH_INTR_BOX3_BOX3_H
