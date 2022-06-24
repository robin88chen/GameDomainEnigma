/*********************************************************************
 * \file   RenderTargetFlipApp.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RENDER_TARGET_FLIP_APP_H
#define RENDER_TARGET_FLIP_APP_H

#include "Application/AppDelegateWin32.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/RenderTarget.h"
#include "GameEngine/RendererManager.h"

class RenderTargetFlipApp : public Enigma::Application::AppDelegate
{
public:
    RenderTargetFlipApp(const std::string app_name);
    ~RenderTargetFlipApp();

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;
private:
    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);

private:
    Enigma::Engine::RendererManager* m_rendererManager;
    std::weak_ptr<Enigma::Engine::RenderTarget> m_renderTarget;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
};

#endif // RENDER_TARGET_FLIP_APP_H


