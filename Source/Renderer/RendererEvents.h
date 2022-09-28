/*********************************************************************
 * \file   RendererEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RENDERER_EVENTS_H
#define RENDERER_EVENTS_H

#include "Frameworks/Event.h"
#include "MathLib/AlgebraBasicTypes.h"

namespace Enigma::Renderer
{
    class RenderTargetResized : public Frameworks::IEvent
    {
    public:
        RenderTargetResized(const std::string& name, const MathLib::Dimension& dimension) :
            m_name{ name }, m_dimension(dimension) {};
        const std::string& GetRenderTargetName() { return m_name; }
        const MathLib::Dimension& GetDimension() const { return m_dimension; }

    private:
        std::string m_name;
        MathLib::Dimension m_dimension;
    };
    class PrimaryRenderTargetCreated : public Frameworks::IEvent
    {
    public:
        PrimaryRenderTargetCreated(const std::string& name) :
            m_name{ name } {};
        const std::string& GetRenderTargetName() { return m_name; }

    private:
        std::string m_name;
    };
    class TargetViewPortInitialized : public Frameworks::IEvent
    {
    public:
        TargetViewPortInitialized(const std::string& name, const Graphics::TargetViewPort& vp) :
            m_name{ name }, m_view_port(vp) {}
        const std::string& GetRenderTargetName() { return m_name; }
        const Graphics::TargetViewPort& GetViewPort() { return m_view_port; }

    private:
        std::string m_name;
        Graphics::TargetViewPort m_view_port;
    };
    class TargetViewPortChanged : public Frameworks::IEvent
    {
    public:
        TargetViewPortChanged(const std::string& name, const Graphics::TargetViewPort& vp) :
            m_name{ name }, m_view_port(vp) {}
        const std::string& GetRenderTargetName() { return m_name; }
        const Graphics::TargetViewPort& GetViewPort() { return m_view_port; }

    private:
        std::string m_name;
        Graphics::TargetViewPort m_view_port;
    };
    class TargetClearingPropertyChanged : public Frameworks::IEvent
    {
    public:
        TargetClearingPropertyChanged(const std::string& name, const RenderTarget::ClearingProperty& prop) :
            m_name{ name }, m_clearing(prop) {}
        const std::string& GetRenderTargetName() { return m_name; }
        const RenderTarget::ClearingProperty& GetClearingProperty() { return m_clearing; }

    private:
        std::string m_name;
        RenderTarget::ClearingProperty m_clearing;
    };
}

#endif // RENDERER_EVENTS_H
