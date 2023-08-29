#include "IntersectionFinderFactories.h"

using namespace Enigma::Engine;

std::unordered_map<std::string, PrimitiveRay3IntersectionFinderCreator> PrimitiveRay3IntersectionFinderFactory::m_creators;

PrimitiveRay3IntersectionFinder* PrimitiveRay3IntersectionFinderFactory::CreatePrimitiveRay3IntersectionFinder(const Frameworks::Rtti& primitive_rtti)
{
    if (m_creators.find(primitive_rtti.GetName()) != m_creators.end())
    {
        return m_creators[primitive_rtti.GetName()]();
    }
    return nullptr;
}

void PrimitiveRay3IntersectionFinderFactory::RegisterCreator(const std::string& primitive_rtti_name, PrimitiveRay3IntersectionFinderCreator creator)
{
    m_creators.insert_or_assign(primitive_rtti_name, creator);
}

