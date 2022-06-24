/*********************************************************************
 * \file   RendererCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RENDERER_COMMANDS_H
#define RENDERER_COMMANDS_H

#include "Frameworks/Command.h"
#include "GraphicKernel/TargetViewPort.h"
#include "RenderTarget.h"
#include <optional>

namespace Enigma::Engine
{
    class ChangeTargetViewPort : public Frameworks::ICommand
    {
    public:
        ChangeTargetViewPort(const std::string& name, const Graphics::TargetViewPort& vp) :
            m_name(name), m_view_port(vp) {}
        const std::string& GetRenderTargetName() { return m_name; }
        const Graphics::TargetViewPort& GetViewPort() { return m_view_port; }

    private:
        std::string m_name;
        Graphics::TargetViewPort m_view_port;
    };
    class ChangeTargetClearingProperty : public Frameworks::ICommand
    {
    public:
        ChangeTargetClearingProperty(const std::string& name, const std::optional<MathLib::ColorRGBA>& color,
            const std::optional<float>& depth, const std::optional<unsigned int>& stencil,
            const std::optional<RenderTarget::BufferClearFlag>& flag) :
            m_name(name), m_color(color), m_depth(depth), m_stencil(stencil), m_flag(flag) {}
        const std::string& GetRenderTargetName() { return m_name; }
        const std::optional<MathLib::ColorRGBA>& GetClearingColor() const { return m_color; }
        const std::optional<float>& GetClearingDepth() const { return m_depth; }
        const std::optional<unsigned int>& GetClearingStencil() const { return m_stencil; }
        const std::optional<RenderTarget::BufferClearFlag>& GetClearingFlag() const { return m_flag; }

    private:
        std::string m_name;
        std::optional<MathLib::ColorRGBA> m_color;
        std::optional<float> m_depth;
        std::optional<unsigned int> m_stencil;
        std::optional<RenderTarget::BufferClearFlag> m_flag;
    };
}

#endif // RENDERER_COMMANDS_H
