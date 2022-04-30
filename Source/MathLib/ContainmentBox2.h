/********************************************************************
 * \file   ContainmentBox2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_CONTAINMENT_BOX2_H
#define _MATH_CONTAINMENT_BOX2_H

#include "Box2.h"
#include "Sphere2.h"

namespace Enigma::MathLib
{
    class ContainmentBox2
    {
    public:
        static Box2 MergeBoxes(const Box2& box0, const Box2& box1);
        static bool TestBox2EnvelopBox2(const Box2& box0, const Box2& box1);
        static bool TestBox2EnvelopSphere2(const Box2& box0, const Sphere2& sphere1);
    };
};

#endif
