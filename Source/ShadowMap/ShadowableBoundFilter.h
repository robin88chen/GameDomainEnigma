/*********************************************************************
 * \file   ShadowableBoundFilter.h
 * \brief  過濾掉不是 Shadow Caster 或 Shadow Receiver
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef _SHADOWABLE_BOUND_FILTER_H
#define _SHADOWABLE_BOUND_FILTER_H

#include "Renderer/RenderableBoundFilter.h"

namespace Enigma::ShadowMap
{
    /** Shadowable Bound Filter */
    class ShadowableBoundFilter : public Renderer::RenderableBoundFilter
    {
    public:
        ShadowableBoundFilter();
        virtual ~ShadowableBoundFilter();

        virtual bool FilterOutSpatial(const std::shared_ptr<SceneGraph::Spatial>& spatial) override;
    };
};


#endif // _SHADOWABLE_BOUND_FILTER_H
