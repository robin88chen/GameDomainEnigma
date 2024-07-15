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

        void setAmbientLightColor(const MathLib::ColorRGBA& color);
        void setSunLight(const MathLib::Vector3& dir, const MathLib::ColorRGBA& color);
        void setPointLightArray(const std::vector<MathLib::Vector4>& positions,
            const std::vector<MathLib::ColorRGBA>& colors, const std::vector<MathLib::Vector4>& attenuations);

        void commitState() const;
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
