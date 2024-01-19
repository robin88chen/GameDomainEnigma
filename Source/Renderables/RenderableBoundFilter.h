/*********************************************************************
 * \file   RenderableBoundFilter.h
 * \brief  過濾 Spatial, 計算合併的 BV, 這支只是單純將 un-renderable 過濾掉
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef _RENDERABLE_BOUND_FILTER_H
#define _RENDERABLE_BOUND_FILTER_H

#include "GameEngine/BoundingVolume.h"
#include "SceneGraph/VisibleSet.h"
#include "SceneGraph/Spatial.h"
#include <memory>

namespace Enigma::Renderables
{
    /** Renderable Bound Filter */
    class RenderableBoundFilter
    {
    public:
        RenderableBoundFilter();
        RenderableBoundFilter(const RenderableBoundFilter&) = delete;
        RenderableBoundFilter(RenderableBoundFilter&&) = delete;
        virtual ~RenderableBoundFilter();
        RenderableBoundFilter& operator=(const RenderableBoundFilter&) = delete;
        RenderableBoundFilter& operator=(RenderableBoundFilter&&) = delete;

        virtual void computeMergedBound(const SceneGraph::VisibleSet& visSet);
        const Engine::BoundingVolume& getMergedBound() { return m_mergedBound; };

        virtual bool filterOutSpatial(const std::shared_ptr<SceneGraph::Spatial>& spatial);

    protected:
        Engine::BoundingVolume m_mergedBound;
    };
};

#endif // _RENDERABLE_BOUND_FILTER_H
