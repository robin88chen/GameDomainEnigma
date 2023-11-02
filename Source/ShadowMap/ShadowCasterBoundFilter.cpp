#include "ShadowCasterBoundFilter.h"
#include "SpatialShadowFlags.h"

using namespace Enigma::ShadowMap;
using namespace Enigma::SceneGraph;

ShadowCasterBoundFilter::ShadowCasterBoundFilter() : RenderableBoundFilter()
{
}

bool ShadowCasterBoundFilter::FilterOutSpatial(const std::shared_ptr<SceneGraph::Spatial>& spatial)
{
    if (RenderableBoundFilter::FilterOutSpatial(spatial)) return true;
    // 保留可render 的 caster
    if (spatial->testSpatialFlag(SpatialShadowFlags::SpatialBit::Spatial_ShadowCaster)) return false;
    return true;
}
