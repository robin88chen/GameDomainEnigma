/*********************************************************************
 * \file   TextureManagerTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef TEXTURE_MANAGER_TEST_H
#define TEXTURE_MANAGER_TEST_H

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
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/EffectPass.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectMaterialManager.h"
#include "Gateways/EffectPolicyJsonGateway.h"

class TextureManagerTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    TextureManagerTest(const std::string app_name);
    ~TextureManagerTest();

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;
private:
    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnEffectMaterialCompiled(const Enigma::Frameworks::IEventPtr& e);
    void OnCompileEffectFailed(const Enigma::Frameworks::IEventPtr& e);
    void OnRenderBufferBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnBuildRenderBufferFailed(const Enigma::Frameworks::IEventPtr& e);
    void OnTextureLoaded(const Enigma::Frameworks::IEventPtr& e);
    void OnLoadTextureFailed(const Enigma::Frameworks::IEventPtr& e);

    void BuildVariables();

private:
    Enigma::Gateways::EffectPolicyJsonGateway* m_gateway;
    Enigma::Engine::RendererManager* m_rendererManager;
    Enigma::Engine::RenderTargetPtr m_renderTarget;
    Enigma::Engine::EffectMaterialManager* m_materialManager;
    Enigma::Engine::RenderBufferPtr m_renderBuffer;
    Enigma::Engine::TexturePtr m_texture;
    Enigma::Engine::EffectMaterialPtr m_material;

    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onEffectMaterialCompiled;
    Enigma::Frameworks::EventSubscriberPtr m_onCompileEffectFailed;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderBufferBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onBuildRenderBufferFailed;
    Enigma::Frameworks::EventSubscriberPtr m_onTextureLoaded;
    Enigma::Frameworks::EventSubscriberPtr m_onLoadTextureFailed;
    Enigma::Frameworks::EventSubscriberPtr m_onSamplerBuilt;

    Enigma::Frameworks::Timer* m_timer;
    float m_tick;
};


#endif // TEXTURE_MANAGER_TEST_H
