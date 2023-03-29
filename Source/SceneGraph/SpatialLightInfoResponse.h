/*********************************************************************
 * \file   SpatialLightInfoResponse.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _SPATIAL_LIGHT_INFO_RESPONSE_H
#define _SPATIAL_LIGHT_INFO_RESPONSE_H

#include "Frameworks/Response.h"
#include "GameEngine/RenderLightingState.h"
#include <system_error>

namespace Enigma::SceneGraph
{
    class SpatialLightInfoResponse : public Frameworks::IResponse
    {
    public:
        SpatialLightInfoResponse(const Frameworks::Ruid& ruid, const Engine::RenderLightingState& lighting_state, std::error_code er)
            : Frameworks::IResponse(ruid), m_lightingState(lighting_state), m_error(er) {}
        const Engine::RenderLightingState& GetLightingState() const { return m_lightingState; }
        std::error_code GetErrorCode() const { return m_error; }

    private:
        Engine::RenderLightingState m_lightingState;
        std::error_code m_error;
    };
}
#endif // _SPATIAL_LIGHT_INFO_RESPONSE_H
