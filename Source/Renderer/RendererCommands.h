/*********************************************************************
 * \file   RendererCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RENDERER_COMMANDS_H
#define RENDERER_COMMANDS_H

#include "RenderTargetClearingProperties.h"
#include "Frameworks/Command.h"
#include "GraphicKernel/TargetViewPort.h"
#include "RenderTarget.h"
#include <optional>

namespace Enigma::Renderer
{
    //--------- renderer commands --------------
    class CreateRenderer : public Frameworks::ICommand
    {
    public:
        CreateRenderer(const std::string& name) : m_name(name) {}
        const std::string& GetRendererName() { return m_name; }

    private:
        std::string m_name;
    };
    class DestroyRenderer : public Frameworks::ICommand
    {
    public:
        DestroyRenderer(const std::string& name) : m_name(name) {}
        const std::string& GetRendererName() { return m_name; }

    private:
        std::string m_name;
    };

    //--------- render target commands -----------
    class CreateRenderTarget : public Frameworks::ICommand
    {
    public:
        CreateRenderTarget(const std::string& name, RenderTarget::PrimaryType primary)
            : m_name(name), m_primary(primary) {}
        const std::string& GetRenderTargetName() { return m_name; }
        RenderTarget::PrimaryType GetPrimaryType() const { return m_primary; }
    private:
        std::string m_name;
        RenderTarget::PrimaryType m_primary;
    };
    class DestroyRenderTarget : public Frameworks::ICommand
    {
    public:
        DestroyRenderTarget(const std::string& name) : m_name(name) {}
        const std::string& GetRenderTargetName() { return m_name; }

    private:
        std::string m_name;
    };
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
        ChangeTargetClearingProperty(const std::string& name, const RenderTargetClearChangingProperty& prop) :
            m_name(name), m_prop(prop) {}
        const std::string& GetRenderTargetName() { return m_name; }
        const RenderTargetClearChangingProperty& GetProperty() const { return m_prop; }
        const std::optional<MathLib::ColorRGBA>& GetClearingColor() const { return m_prop.m_color; }
        const std::optional<float>& GetClearingDepth() const { return m_prop.m_depth; }
        const std::optional<unsigned int>& GetClearingStencil() const { return m_prop.m_stencil; }
        const std::optional<RenderTargetClearFlag>& GetClearingFlag() const { return m_prop.m_flag; }

    private:
        std::string m_name;
        RenderTargetClearChangingProperty m_prop;
    };
    class ResizePrimaryRenderTarget : public Frameworks::ICommand
    {
    public:
        ResizePrimaryRenderTarget(const MathLib::Dimension& dimension) : m_dimension(dimension) {}
        const MathLib::Dimension& GetDimension() { return m_dimension; }

    private:
        MathLib::Dimension m_dimension;
    };
}

#endif // RENDERER_COMMANDS_H
