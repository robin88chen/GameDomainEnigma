/*********************************************************************
 * \file   RenderLightingState.h
 * \brief  Render Lighting State, value object
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef RENDER_LIGHTING_STATE_H
#define RENDER_LIGHTING_STATE_H

#include "MathLib/Vector4.h"
#include "MathLib/ColorRGBA.h"
#include "MathLib/Vector3.h"
#include <vector>

namespace Enigma::Engine
{
    class RenderLightingState
    {
    public:
        RenderLightingState();
        RenderLightingState(const RenderLightingState&) = default;
        RenderLightingState(RenderLightingState&&) = default;
        ~RenderLightingState();
        RenderLightingState& operator=(const RenderLightingState&) = default;
        RenderLightingState& operator=(RenderLightingState&&) = default;

        void SetAmbientLightColor(const MathLib::ColorRGBA& color);
        void SetSunLight(const MathLib::Vector3& dir, const MathLib::ColorRGBA& color);
        void SetPointLightArray(const std::vector<MathLib::Vector4>& positions,
            std::vector<MathLib::ColorRGBA>& colors, std::vector<MathLib::Vector4>& attenuations);

    protected:
        MathLib::ColorRGBA m_colorAmbient;
        MathLib::ColorRGBA m_colorSun;
        MathLib::Vector3 m_vecSunDir;

        std::vector<MathLib::Vector4> m_lightPositions;
        std::vector<MathLib::ColorRGBA> m_lightColors;
        std::vector<MathLib::Vector4> m_lightAttenuations;
    };
};
#endif // RENDER_LIGHTING_STATE_H
