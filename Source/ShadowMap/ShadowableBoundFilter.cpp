#include "ShadowableBoundFilter.h"
#include "SpatialShadowFlags.h"

using namespace Enigma::ShadowMap;
using namespace Enigma::SceneGraph;

ShadowableBoundFilter::ShadowableBoundFilter() : RenderableBoundFilter()
{
}

bool ShadowableBoundFilter::filterOutSpatial(const std::shared_ptr<SceneGraph::Spatial>& spatial)
{
    if (RenderableBoundFilter::filterOutSpatial(spatial)) return true;
    if (spatial->testSpatialFlag(SpatialShadowFlags::SpatialBit::Spatial_ShadowCaster | SpatialShadowFlags::SpatialBit::Spatial_ShadowReceiver)) return false;
    return true;
}

