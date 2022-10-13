/*********************************************************************
 * \file   RenderTargetClearingProperties.h
 * \brief  clearing properties, data objects
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _RENDER_TARGET_CLEARING_PROPERTIES_H
#define _RENDER_TARGET_CLEARING_PROPERTIES_H

#include "MathLib/ColorRGBA.h"
#include <optional>

namespace Enigma::Renderer
{
    enum class RenderTargetClearFlag
    {
        ColorBuffer = 0x01,
        DepthBuffer = 0x02,
        BothBuffer = 0x03,
    };
    struct RenderTargetClearingProperty
    {
        MathLib::ColorRGBA m_color;
        float m_depth;
        unsigned int m_stencil;
        RenderTargetClearFlag m_flag;
    };

    struct RenderTargetClearChangingProperty
    {
        std::optional<MathLib::ColorRGBA> m_color;
        std::optional<float> m_depth;
        std::optional<unsigned int> m_stencil;
        std::optional<RenderTargetClearFlag> m_flag;
    };
}

#endif // _RENDER_TARGET_CLEARING_PROPERTIES_H
