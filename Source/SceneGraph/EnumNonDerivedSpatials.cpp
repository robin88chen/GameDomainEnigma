#include "EnumNonDerivedSpatials.h"
#include "SceneGraph/Spatial.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;

EnumNonDerivedSpatials::EnumNonDerivedSpatials(const Frameworks::Rtti& baseRtti)
{
    m_baseRtti = const_cast<Rtti*>(&baseRtti);
    m_spatials.clear();
}

EnumNonDerivedSpatials::~EnumNonDerivedSpatials()
{
    m_spatials.clear();
}

SceneTraveler::TravelResult EnumNonDerivedSpatials::TravelTo(const SpatialPtr& spatial)
{
    if (!spatial) return SceneTraveler::TravelResult::InterruptError;
    if (spatial->typeInfo().isDerived(*m_baseRtti)) return SceneTraveler::TravelResult::Skip;

    m_spatials.emplace_back(spatial);
    return SceneTraveler::TravelResult::Continue;
}
