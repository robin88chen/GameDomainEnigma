#include "FindSpatialById.h"
#include "Spatial.h"

using namespace Enigma::SceneGraph;

FindSpatialById::FindSpatialById(const SpatialId& spatial_id) : SceneTraveler()
{
    m_targetSpatialId = spatial_id;
    m_foundSpatial = nullptr;
}

FindSpatialById::~FindSpatialById()
{
    m_foundSpatial = nullptr;
}

SceneTraveler::TravelResult FindSpatialById::travelTo(const SpatialPtr& spatial)
{
    if (!spatial) return SceneTraveler::TravelResult::InterruptError;
    if (!m_targetSpatialId.isValid()) return SceneTraveler::TravelResult::InterruptError;
    if (spatial->id() == m_targetSpatialId)
    {
        m_foundSpatial = spatial;
        return TravelResult::InterruptTargetFound;
    }

    return SceneTraveler::TravelResult::Continue;
}
