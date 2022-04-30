/********************************************************************
 * \file   ContainmentSphere3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_CONTAINMENT_SPHERE3_H
#define _MATH_CONTAINMENT_SPHERE3_H

#include "Sphere3.h"
#include "Vector4.h"

namespace Enigma::MathLib
{
    class ContainmentSphere3
    {
    public:
        static Sphere3 MergeSpheres(const Sphere3& sphere0, const Sphere3& sphere1);
        static Sphere3 ComputeAverageSphere(const Vector3* pos, unsigned int quantity);
        static Sphere3 ComputeAverageSphere(const Vector4* pos, unsigned int quantity);
        static Sphere3 ComputeAverageSphere(const float* vert, unsigned int pitch, unsigned int quantity);
    };
};

#endif
