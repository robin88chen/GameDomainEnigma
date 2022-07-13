/*********************************************************************
 * \file   DrawPrimitiveTestApp.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef DRAW_PRIMITIVE_TEST_APP_H
#define DRAW_PRIMITIVE_TEST_APP_H

#include "Application/AppDelegateWin32.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/RenderTarget.h"
#include "GameEngine/RendererManager.h"

class ShaderBuilder;

class DrawPrimitiveTestApp : public Enigma::Application::AppDelegate
{
public:
    DrawPrimitiveTestApp(const std::string app_name);
    ~DrawPrimitiveTestApp();

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;
private:
    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnShaderProgramCreated(const Enigma::Frameworks::IEventPtr& e);

private:
    Enigma::Engine::RendererManager* m_rendererManager;
    Enigma::Engine::RenderTargetPtr m_renderTarget;
    Enigma::Graphics::IShaderProgramPtr m_program;

    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onShaderProgramCreated;

    ShaderBuilder* m_shaderBuilder;
};

#endif // DRAW_PRIMITIVE_TEST_APP_H
