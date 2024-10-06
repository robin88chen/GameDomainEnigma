/*********************************************************************
 * \file   RenderTargetClearingProperties.h
 * \brief  clearing properties, data objects
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _RENDER_TARGET_CLEARING_PROPERTIES_H
#define _RENDER_TARGET_CLEARING_PROPERTIES_H

#include <bitset>
#include "MathLib/ColorRGBA.h"
#include <optional>

namespace Enigma::Renderer
{
    enum RenderTargetClear
    {
        ColorBuffer = 0x01,
        DepthBuffer = 0x02,
        BothBuffer = 0x03,
    };
    using RenderTargetClearingBits = std::bitset<static_cast<size_t>(2)>;
    struct RenderTargetClearingProperty
    {
        MathLib::ColorRGBA m_color;
        float m_depth;
        unsigned int m_stencil;
        RenderTargetClearingBits m_clearingBits;
        RenderTargetClearingProperty() : m_color(MathLib::ColorRGBA::BLACK), m_depth(1.0f), m_stencil(0), m_clearingBits(RenderTargetClear::BothBuffer) {}
        RenderTargetClearingProperty(const MathLib::ColorRGBA& color, float depth, unsigned int stencil, RenderTargetClearingBits clearingBits) : m_color(color), m_depth(depth), m_stencil(stencil), m_clearingBits(clearingBits) {}
    };

    struct RenderTargetClearChangingProperty
    {
        std::optional<MathLib::ColorRGBA> m_color;
        std::optional<float> m_depth;
        std::optional<unsigned int> m_stencil;
        std::optional<RenderTargetClearingBits> m_clearingBits;
    };
}

#endif // _RENDER_TARGET_CLEARING_PROPERTIES_H
