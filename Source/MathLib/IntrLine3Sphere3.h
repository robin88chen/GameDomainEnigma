/********************************************************************
 * \file   IntrLine3Sphere3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _MATH_LINE3_SPHERE3_H
#define _MATH_LINE3_SPHERE3_H

#include "Intersector.h"
#include "Line3.h"
#include "Sphere3.h"

/** @file EnIntrLine3Sphere3.h */

namespace Enigma::MathLib
{
    /** Intersector of Line3 & Sphere3 */
    class IntrLine3Sphere3 : public Intersector
    {
    public:
        IntrLine3Sphere3(const Line3& line, const Sphere3& sphere);

        // object access
        const Line3& GetLine() const;
        const Sphere3& GetSphere() const;

        // test-intersection query
        virtual Result Test(IntersectorCache* last_result) override;

        // find-intersection query
        virtual Result Find(IntersectorCache* last_result) override;
        int GetQuantity() const;
        const Vector3& GetPoint(int i) const;
        float GetLineT(int i) const;

    private:
        // the objects to intersect
        const Line3& m_line;
        const Sphere3& m_sphere;

        // information about the intersection set
        int m_quantity;
        Vector3 m_point[2];
        float m_lineT[2];
    };

};

#endif // !_MATH_LINE3_SPHERE3_H
