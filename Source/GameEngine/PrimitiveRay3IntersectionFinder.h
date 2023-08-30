/*********************************************************************
 * \file   PrimitiveRay3IntersectionFinder.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef PRIMITIVE_RAY3_INTERSECTION_FINDER_H
#define PRIMITIVE_RAY3_INTERSECTION_FINDER_H

#include "MathLib/Intersector.h"
#include "MathLib/IntersectorCache.h"
#include "MathLib/Ray3.h"
#include "IntrPrimitiveRay3.h"

namespace Enigma::Engine
{
    class Primitive;

    class PrimitiveRay3IntersectionFinder
    {
    public:
        PrimitiveRay3IntersectionFinder() {};
        virtual ~PrimitiveRay3IntersectionFinder() {};

        virtual MathLib::Intersector::Result Test(const std::shared_ptr<Primitive>& primitive, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const = 0;
        virtual std::tuple<std::vector<IntrPrimitiveRay3::ResultRecord>, MathLib::Intersector::Result>
            Find(const std::shared_ptr<Primitive>& primitive, const MathLib::Ray3& ray, std::unique_ptr<MathLib::IntersectorCache> cache) const = 0;
    };
}

#endif // PRIMITIVE_RAY3_INTERSECTION_FINDER_H
