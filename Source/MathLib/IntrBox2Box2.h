/********************************************************************
 * \file   IntrBox2Box2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTR_BOX2_BOX2_H
#define _MATH_INTR_BOX2_BOX2_H

#include "Intersector.h"
#include "Box2.h"

namespace Enigma::MathLib
{
    /** Intersector of Box2 & Box2 */
    class IntrBox2Box2 : public Intersector
    {
    public:
        IntrBox2Box2(const Box2& box0, const Box2& box1);

        // object access
        const Box2& GetBox0() const;
        const Box2& GetBox1() const;

        // static test-intersection query
        virtual bool Test(IntersectorCache* last_result) override;

    private:
        // the objects to intersect
        const Box2& m_box0;
        const Box2& m_box1;
    };

};

#endif // !_MATH_INTR_BOX2_BOX2_H
