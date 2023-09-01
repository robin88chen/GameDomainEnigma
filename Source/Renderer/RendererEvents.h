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
#include "GameEngine/IRenderer.h"
#include "RenderTargetClearingProperties.h"
#include "GraphicKernel/TargetViewPort.h"

namespace Enigma::Renderer
{
    class RenderElement;
    class RenderTarget;

    //------------ Renderer Events ------------
    class RendererCreated : public Frameworks::IEvent
    {
    public:
        RendererCreated(const std::string& name, const std::shared_ptr<Engine::IRenderer>& renderer) :
            m_name(name), m_renderer(renderer) {};
        const std::string& GetRendererName() { return m_name; }
        const std::shared_ptr<Engine::IRenderer>& GetRenderer() const { return m_renderer; }

    private:
        std::string m_name;
        std::shared_ptr<Engine::IRenderer> m_renderer;
    };

    class RendererDestroyed : public Frameworks::IEvent
    {
    public:
        RendererDestroyed(const std::string& name) : m_name(name) {};
        const std::string& GetRendererName() { return m_name; }

    private:
        std::string m_name;
    };

    //------------ Render Target Events ----------
    class RenderTargetResized : public Frameworks::IEvent
    {
    public:
        RenderTargetResized(const std::shared_ptr<RenderTarget>& target, const MathLib::Dimension<unsigned>& dimension) :
            m_target(target), m_dimension(dimension) {};
        [[nodiscard]] std::shared_ptr<RenderTarget> GetRenderTarget() const { return m_target.lock(); }
        [[nodiscard]] const MathLib::Dimension<unsigned>& GetDimension() const { return m_dimension; }

    private:
        std::weak_ptr<RenderTarget> m_target;
        MathLib::Dimension<unsigned> m_dimension;
    };
    class PrimaryRenderTargetCreated : public Frameworks::IEvent
    {
    public:
        PrimaryRenderTargetCreated(const std::string& name, const std::shared_ptr<RenderTarget>& target) :
            m_name(name), m_target(target) {};
        const std::string& GetRenderTargetName() { return m_name; }
        const std::shared_ptr<RenderTarget>& GetRenderTarget() const { return m_target; }

    private:
        std::string m_name;
        std::shared_ptr<RenderTarget> m_target;
    };
    class RenderTargetDestroyed : public Frameworks::IEvent
    {
    public:
        RenderTargetDestroyed(const std::string& name) : m_name(name) {};
        const std::string& GetRenderTargetName() { return m_name; }

    private:
        std::string m_name;
    };
    class TargetViewPortInitialized : public Frameworks::IEvent
    {
    public:
        TargetViewPortInitialized(const std::shared_ptr<RenderTarget>& target, const Graphics::TargetViewPort& vp) :
            m_target{ target }, m_view_port(vp) {}
        [[nodiscard]] std::shared_ptr<RenderTarget> GetRenderTarget() const { return m_target.lock(); }
        [[nodiscard]] const Graphics::TargetViewPort& GetViewPort() { return m_view_port; }

    private:
        std::weak_ptr<RenderTarget> m_target;
        Graphics::TargetViewPort m_view_port;
    };
    class TargetViewPortChanged : public Frameworks::IEvent
    {
    public:
        TargetViewPortChanged(const std::shared_ptr<RenderTarget>& target, const Graphics::TargetViewPort& vp) :
            m_target{ target }, m_view_port(vp) {}
        [[nodiscard]] std::shared_ptr<RenderTarget> GetRenderTarget() const { return m_target.lock(); }
        [[nodiscard]] const Graphics::TargetViewPort& GetViewPort() { return m_view_port; }

    private:
        std::weak_ptr<RenderTarget> m_target;
        Graphics::TargetViewPort m_view_port;
    };
    class TargetClearingPropertyChanged : public Frameworks::IEvent
    {
    public:
        TargetClearingPropertyChanged(const std::shared_ptr<RenderTarget>& target, const RenderTargetClearingProperty& prop) :
            m_target{ target }, m_clearing(prop) {}
        [[nodiscard]] std::shared_ptr<RenderTarget> GetRenderTarget() const { return m_target.lock(); }
        [[nodiscard]] const RenderTargetClearingProperty& GetClearingProperty() const { return m_clearing; }

    private:
        std::weak_ptr<RenderTarget> m_target;
        RenderTargetClearingProperty m_clearing;
    };
    class RenderTargetTextureCreated : public Frameworks::IEvent
    {
    public:
        RenderTargetTextureCreated(const std::shared_ptr<RenderTarget>& target, const std::string texture_name) :
            m_target{ target }, m_textureName(texture_name) {}
        [[nodiscard]] std::shared_ptr<RenderTarget> GetRenderTarget() const { return m_target.lock(); }
        [[nodiscard]] const std::string& GetTextureName() const { return m_textureName; }

    private:
        std::weak_ptr<RenderTarget> m_target;
        std::string m_textureName;
    };
}

#endif // RENDERER_EVENTS_H
