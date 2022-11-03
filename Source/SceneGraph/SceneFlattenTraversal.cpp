#include "SceneFlattenTraversal.h"

using namespace Enigma::SceneGraph;

SceneFlattenTraversal::SceneFlattenTraversal() : SceneTraveler()
{

}

SceneFlattenTraversal::~SceneFlattenTraversal()
{
    m_spatials.clear();
}

SceneTraveler::TravelResult SceneFlattenTraversal::TravelTo(const SpatialPtr& spatial)
{
    if (!spatial) return TravelResult::InterruptError;
    if (spatial) m_spatials.emplace_back(spatial);
    return TravelResult::Continue;
}
