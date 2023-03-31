#include "FindSpatialByName.h"
#include "Spatial.h"

using namespace Enigma::SceneGraph;

FindSpatialByName::FindSpatialByName(const std::string& spatial_name) : SceneTraveler()
{
    m_targetSpatialName = spatial_name;
    m_foundSpatial = nullptr;
}

FindSpatialByName::~FindSpatialByName()
{
    m_foundSpatial = nullptr;
}

SceneTraveler::TravelResult FindSpatialByName::TravelTo(const SpatialPtr& spatial)
{
    if (!spatial) return SceneTraveler::TravelResult::InterruptError;
    if (m_targetSpatialName == "") return SceneTraveler::TravelResult::InterruptError;
    if (spatial->GetSpatialName() == m_targetSpatialName)
    {
        m_foundSpatial = spatial;
        return TravelResult::InterruptTargetFound;
    }

    return SceneTraveler::TravelResult::Continue;
}
