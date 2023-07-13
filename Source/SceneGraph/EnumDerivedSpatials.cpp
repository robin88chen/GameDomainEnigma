#include "EnumDerivedSpatials.h"
#include "SceneGraph/Spatial.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;

EnumDerivedSpatials::EnumDerivedSpatials(const Frameworks::Rtti& baseRtti)
{
    m_baseRtti = const_cast<Rtti*>(&baseRtti);
    m_spatials.clear();
}

EnumDerivedSpatials::~EnumDerivedSpatials()
{
    m_spatials.clear();
}

SceneTraveler::TravelResult EnumDerivedSpatials::TravelTo(const SpatialPtr& spatial)
{
    if (!spatial) return SceneTraveler::TravelResult::InterruptError;
    if (spatial->TypeInfo().IsDerived(*m_baseRtti))
    {
        m_spatials.emplace_back(spatial);
    }

    return SceneTraveler::TravelResult::Continue;
}
