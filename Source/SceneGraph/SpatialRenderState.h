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
        SpatialRenderState() = default;
        SpatialRenderState(const SpatialRenderState&) = default;
        SpatialRenderState(SpatialRenderState&&) = default;
        SpatialRenderState(const Engine::RenderLightingState& lightingState) : m_lightingState(lightingState) {}
        ~SpatialRenderState() = default;
        SpatialRenderState& operator=(const SpatialRenderState&) = default;
        SpatialRenderState& operator=(SpatialRenderState&&) = default;

        [[nodiscard]] Engine::RenderLightingState lightingState() const { return m_lightingState; };  // 物件複製取用，避免 race condition

    protected:
        Engine::RenderLightingState m_lightingState;
    };
}

#endif // SPATIAL_RENDER_STATE_H
