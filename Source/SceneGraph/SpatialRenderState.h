/*********************************************************************
 * \file   SpatialRenderState.h
 * \brief  Spatial Render State, value object
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef SPATIAL_RENDER_STATE_H
#define SPATIAL_RENDER_STATE_H

#include "GameEngine/RenderLightingState.h"
namespace Enigma::SceneGraph
{
    class SpatialRenderState
    {
    public:
        SpatialRenderState();
        SpatialRenderState(const SpatialRenderState&) = default;
        SpatialRenderState(SpatialRenderState&&) = default;
        ~SpatialRenderState();
        SpatialRenderState& operator=(const SpatialRenderState&) = default;
        SpatialRenderState& operator=(SpatialRenderState&&) = default;

        const Engine::RenderLightingState& ToLightingState() const { return m_lightingState; };

    protected:
        Engine::RenderLightingState m_lightingState;
    };
}

#endif // SPATIAL_RENDER_STATE_H
