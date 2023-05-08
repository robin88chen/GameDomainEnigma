/********************************************************************
 * \file   ControllerEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef CONTROLLER_EVENTS_H
#define CONTROLLER_EVENTS_H

#include "Frameworks/Event.h"
#include "Renderer/RendererManager.h"

namespace Enigma::Controllers
{
    class FrameworksInstalled : public Frameworks::IEvent
    {
    };
    class InstallingRenderer : public Frameworks::IEvent
    {
    public:
        InstallingRenderer(const std::shared_ptr<Renderer::RendererManager>& manager) : m_manager(manager) {}

        std::shared_ptr<Renderer::RendererManager> GetManager() const { return m_manager.lock(); }
    protected:
        std::weak_ptr<Renderer::RendererManager> m_manager;
    };
    class ShutdownRenderer : public Frameworks::IEvent
    {
    public:
        ShutdownRenderer(const std::shared_ptr<Renderer::RendererManager>& manager) : m_manager(manager) {}

        std::shared_ptr<Renderer::RendererManager> GetManager() const { return m_manager.lock(); }
    protected:
        std::weak_ptr<Renderer::RendererManager> m_manager;
    };
}


#endif // CONTROLLER_EVENTS_H
