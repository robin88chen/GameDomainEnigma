#include "RenderableBoundFilter.h"

using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;

RenderableBoundFilter::RenderableBoundFilter()
{
}

RenderableBoundFilter::~RenderableBoundFilter()
{
}

void RenderableBoundFilter::ComputeMergedBound(const SceneGraph::VisibleSet& visSet)
{
    if (visSet.getCount() == 0) return;
    for (auto spatial : visSet.GetObjectSet())
    {
        if (FilterOutSpatial(spatial)) continue;
        if (m_mergedBound.isEmpty())
        {
            m_mergedBound = spatial->getWorldBound();
        }
        else
        {
            m_mergedBound.Merge(MathLib::Matrix4::IDENTITY, spatial->getWorldBound());
        }
    }
}

bool RenderableBoundFilter::FilterOutSpatial(const std::shared_ptr<Spatial>& spatial)
{
    if ((!spatial) || (!(spatial->isRenderable()))) return true;
    return false;
}
