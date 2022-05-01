/********************************************************************
 * \file   IntrLine3Box3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_INTR_LINE3_BOX3_H
#define _MATH_INTR_LINE3_BOX3_H

#include "Intersector.h"
#include "Line3.h"
#include "Box3.h"

namespace Enigma::MathLib
{
    class IntrLine3Box3 : public Intersector
    {
    public:
        IntrLine3Box3(const Line3& line, const Box3& box);

        // object access
        const Line3& GetLine() const;
        const Box3& GetBox() const;

        // test-intersection query
        virtual bool Test(IntersectorCache* last_result) override;

        // find-intersection query
        virtual bool Find(IntersectorCache* last_result) override;
        int GetQuantity() const;
        const Vector3& GetPoint(int i) const;
        float GetLineT(int i) const;

    private:
        bool Clip(float denom, float numer, float& t0, float& t1);

    private:
        // the objects to intersect
        const Line3& m_line;
        const Box3& m_box;

        // information about the intersection set
        int m_quantity;
        Vector3 m_point[2];
        float m_lineT[2];
    };

};

#endif // !_MATH_INTR_LINE3_BOX3_H
