#include "IntersectionFinderFactories.h"
#include <algorithm>

using namespace Enigma::Engine;

std::unordered_map<std::string, PrimitiveRay3IntersectionFinderCreator> PrimitiveRay3IntersectionFinderFactory::m_creators;

PrimitiveRay3IntersectionFinder* PrimitiveRay3IntersectionFinderFactory::CreatePrimitiveRay3IntersectionFinder(const Frameworks::Rtti& primitive_rtti)
{
    if (auto it = std::find_if(m_creators.begin(), m_creators.end(), [&primitive_rtti](const auto& pair) { return Frameworks::Rtti::isExactlyOrDerivedFrom(primitive_rtti.getName(), pair.first); }); it != m_creators.end())
    {
        return it->second();
    }
    return nullptr;
}

void PrimitiveRay3IntersectionFinderFactory::RegisterCreator(const std::string& primitive_rtti_name, PrimitiveRay3IntersectionFinderCreator creator)
{
    m_creators.insert_or_assign(primitive_rtti_name, creator);
}

