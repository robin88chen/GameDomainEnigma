/********************************************************************
 * \file   ContainmentSphere2.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_CONTAINMENT_SPHERE2_H
#define _MATH_CONTAINMENT_SPHERE2_H

#include "Sphere2.h"

namespace Enigma::MathLib
{
    class ContainmentSphere2
    {
    public:
        static Sphere2 MergeSpheres(const Sphere2& sphere0, const Sphere2& sphere1);
    };
};

#endif
