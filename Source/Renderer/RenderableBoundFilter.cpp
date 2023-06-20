#include "RenderableBoundFilter.h"

using namespace Enigma::Renderer;
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
    if (visSet.GetCount() == 0) return;
    for (auto spatial : visSet.GetObjectSet())
    {
        if (FilterOutSpatial(spatial)) continue;
        if (m_mergedBound.IsEmpty())
        {
            m_mergedBound = spatial->GetWorldBound();
        }
        else
        {
            m_mergedBound.Merge(MathLib::Matrix4::IDENTITY, spatial->GetWorldBound());
        }
    }
}

bool RenderableBoundFilter::FilterOutSpatial(const std::shared_ptr<Spatial>& spatial)
{
    if ((!spatial) || (!(spatial->IsRenderable()))) return true;
    return false;
}
