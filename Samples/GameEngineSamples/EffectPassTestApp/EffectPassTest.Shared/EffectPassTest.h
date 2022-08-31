/*********************************************************************
 * \file   EffectPassTest.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef EFFECT_PASS_TEST_H
#define EFFECT_PASS_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/RenderTarget.h"
#include "GameEngine/RendererManager.h"

class BufferBuilder;

class EffectPassTest : public Enigma::Application::AppDelegate
{
public:
    EffectPassTest(const std::string app_name);
    ~EffectPassTest();

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;
private:
    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnShaderProgramBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnShaderProgramBuildFailed(const Enigma::Frameworks::IEventPtr& e);
    void OnVertexBufferBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnIndexBufferBuilt(const Enigma::Frameworks::IEventPtr& e);

private:
    Enigma::Engine::RendererManager* m_rendererManager;
    Enigma::Engine::RenderTargetPtr m_renderTarget;
    Enigma::Graphics::IShaderProgramPtr m_program;
    Enigma::Graphics::IVertexBufferPtr m_vtxBuffer;
    Enigma::Graphics::IIndexBufferPtr m_idxBuffer;
    Enigma::Graphics::IVertexDeclarationPtr m_vtxDecl;

    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onShaderProgramBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onShaderProgramBuildFailed;
    Enigma::Frameworks::EventSubscriberPtr m_onVertexBufferBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onIndexBufferBuilt;

    BufferBuilder* m_bufferBuilder;
};


#endif // EFFECT_PASS_TEST_H
