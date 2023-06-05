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
#include "Frameworks/ResponseSubscriber.h"

namespace Enigma::SceneGraph
{
    class SpatialRenderState
    {
    public:
        SpatialRenderState();
        SpatialRenderState(const SpatialRenderState&) = default;
        SpatialRenderState(SpatialRenderState&&) = default;
        SpatialRenderState(const Engine::RenderLightingState& lightingState);
        ~SpatialRenderState();
        SpatialRenderState& operator=(const SpatialRenderState&) = default;
        SpatialRenderState& operator=(SpatialRenderState&&) = default;

        [[nodiscard]] Engine::RenderLightingState ToLightingState() const { return m_lightingState; };  // 物件複製取用，避免 race condition

        void QueryLightingState(const MathLib::Vector3& spatialPos);

    protected:
        void OnLightingStateResponse(const Frameworks::IResponsePtr& r);

    protected:
        Engine::RenderLightingState m_lightingState;
        Frameworks::ResponseSubscriberPtr m_onLightingStateResponse;
        Frameworks::Ruid m_requestRuid;
    };
}

#endif // SPATIAL_RENDER_STATE_H
