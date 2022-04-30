/********************************************************************
 * \file   ContainmentBox3.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _MATH_CONTAINMENT_BOX3_H
#define _MATH_CONTAINMENT_BOX3_H

#include "Box3.h"
#include "Vector4.h"

namespace Enigma::MathLib
{
    class ContainmentBox3
    {
    public:
        static Box3 MergeBoxes(const Box3& box0, const Box3& box1);
        static Box3 MergeAlignedBoxes(const Box3& box0, const Box3& box1);
        static Box3 ComputeAlignedBox(const Vector3* pos, unsigned int quantity);
        static Box3 ComputeAlignedBox(const Vector4* pos, unsigned int quantity);
        static Box3 ComputeAlignedBox(const float* vert, unsigned int pitch, unsigned int quantity);

        static Box3 ComputeOrientedBox(const Vector3* pos, unsigned int quantity);
        static Box3 ComputeOrientedBox(const Vector4* pos, unsigned int quantity);
        static Box3 ComputeOrientedBox(const float* vert, unsigned int pitch, unsigned int quantity);
    };
};

#endif
