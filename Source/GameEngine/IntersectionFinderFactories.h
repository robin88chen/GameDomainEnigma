/*********************************************************************
 * \file   IntersectionFinderFactories.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef INTERSECTION_FINDER_FACTORIES_H
#define INTERSECTION_FINDER_FACTORIES_H

#include "Frameworks/Rtti.h"
#include <functional>

namespace Enigma::Engine
{
    class PrimitiveRay3IntersectionFinder;

    using PrimitiveRay3IntersectionFinderCreator = std::function<PrimitiveRay3IntersectionFinder* ()>;

    class PrimitiveRay3IntersectionFinderFactory
    {
    public:
        static PrimitiveRay3IntersectionFinder* CreatePrimitiveRay3IntersectionFinder(const Frameworks::Rtti& primitive_rtti);
        static void RegisterCreator(const std::string& primitive_rtti_name, PrimitiveRay3IntersectionFinderCreator creator);

    private:
        static std::unordered_map<std::string, PrimitiveRay3IntersectionFinderCreator> m_creators;
    };
}

#endif // INTERSECTION_FINDER_FACTORIES_H
