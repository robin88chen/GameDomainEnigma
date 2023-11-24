#include "SceneNonLazyFlattenTraversal.h"
#include "Spatial.h"
#include "LazyNode.h"

using namespace Enigma::SceneGraph;

SceneNonLazyFlattenTraversal::SceneNonLazyFlattenTraversal() : SceneTraveler()
{

}

SceneNonLazyFlattenTraversal::~SceneNonLazyFlattenTraversal()
{
    m_spatials.clear();
}

SceneTraveler::TravelResult SceneNonLazyFlattenTraversal::TravelTo(const SpatialPtr& spatial)
{
    if (!spatial) return TravelResult::InterruptError;
    if (spatial) m_spatials.emplace_back(spatial);
    if (spatial->typeInfo().isDerived(LazyNode::TYPE_RTTI)) return SceneTraveler::TravelResult::Skip;
    return TravelResult::Continue;
}

