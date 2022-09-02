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
#include "Frameworks/Timer.h"
#include "GameEngine/RenderTarget.h"
#include "GameEngine/RendererManager.h"
#include "GameEngine/EffectPass.h"

class BufferBuilder;
class TextureSamplerBuilder;

class EffectPassTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    EffectPassTest(const std::string app_name);
    ~EffectPassTest();

    virtual void InitializeMountPaths() override;

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
    void OnTextureLoaded(const Enigma::Frameworks::IEventPtr& e);
    void OnSamplerBuilt(const Enigma::Frameworks::IEventPtr& e);

	void BuildVariables();

private:
    Enigma::Engine::RendererManager* m_rendererManager;
    Enigma::Engine::RenderTargetPtr m_renderTarget;
    Enigma::Graphics::IShaderProgramPtr m_program;
    Enigma::Graphics::IVertexBufferPtr m_vtxBuffer;
    Enigma::Graphics::IIndexBufferPtr m_idxBuffer;
    Enigma::Graphics::IVertexDeclarationPtr m_vtxDecl;
    Enigma::Graphics::ITexturePtr m_texture;
    Enigma::Graphics::IDeviceSamplerStatePtr m_sampler;

    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onShaderProgramBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onShaderProgramBuildFailed;
    Enigma::Frameworks::EventSubscriberPtr m_onVertexBufferBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onIndexBufferBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onTextureLoaded;
    Enigma::Frameworks::EventSubscriberPtr m_onSamplerBuilt;

    Enigma::Frameworks::Timer* m_timer;
    float m_tick;
    BufferBuilder* m_bufferBuilder;
    TextureSamplerBuilder* m_textureBuilder;

    Enigma::Engine::EffectPass* m_effectPass;
    Enigma::Engine::EffectPass* m_effectPass2;
};


#endif // EFFECT_PASS_TEST_H
