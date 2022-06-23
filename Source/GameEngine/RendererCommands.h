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
        ChangeTargetClearingProperty(const std::string& name, const RenderTarget::ClearingProperty& prop) :
            m_name(name), m_clearing(prop) {}
        const std::string& GetRenderTargetName() { return m_name; }
        const RenderTarget::ClearingProperty& GetClearingProperty() { return m_clearing; }

    private:
        std::string m_name;
        RenderTarget::ClearingProperty m_clearing;
    };
}

#endif // RENDERER_COMMANDS_H
