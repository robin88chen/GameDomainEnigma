#include "ShadowableBoundFilter.h"

using namespace Enigma::ShadowMap;
using namespace Enigma::SceneGraph;

ShadowableBoundFilter::ShadowableBoundFilter() : RenderableBoundFilter()
{
}

bool ShadowableBoundFilter::FilterOutSpatial(const std::shared_ptr<SceneGraph::Spatial>& spatial)
{
    if (RenderableBoundFilter::FilterOutSpatial(spatial)) return true;
    if (spatial->TestSpatialFlag(Spatial::SpatialBit::Spatial_ShadowCaster | Spatial::SpatialBit::Spatial_ShadowReceiver)) return false;
    return true;
}

