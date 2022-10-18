/*********************************************************************
 * \file   RenderManagerTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _RENDER_MANAGER_TEST_H
#define _RENDER_MANAGER_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include "GameEngine/IRenderer.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/Timer.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/RenderElement.h"
#include "Renderer/Renderer.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/EffectPass.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectMaterialManager.h"
#include "Gateways/EffectPolicyJsonGateway.h"

class RenderManagerTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    RenderManagerTest(const std::string app_name);
    ~RenderManagerTest();

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;
private:
    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnEffectMaterialCompiled(const Enigma::Frameworks::IEventPtr& e);
    void OnCompileEffectFailed(const Enigma::Frameworks::IEventPtr& e);
    void OnRenderBufferBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnBuildRenderBufferFailed(const Enigma::Frameworks::IEventPtr& e);
    void OnTextureLoaded(const Enigma::Frameworks::IEventPtr& e);
    void OnLoadTextureFailed(const Enigma::Frameworks::IEventPtr& e);

    void BuildVariables();
    void BuildRenderElement();

private:
    Enigma::Gateways::EffectPolicyJsonGateway* m_gateway;
    //Enigma::Renderer::RendererManager* m_rendererManager;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Engine::EffectMaterialManager* m_materialManager;
    Enigma::Engine::RenderBufferPtr m_renderBuffer;
    Enigma::Renderer::RenderElementPtr m_renderElement;
    Enigma::Engine::TexturePtr m_texture;
    Enigma::Engine::EffectMaterialPtr m_material;

    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
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


#endif // _RENDER_MANAGER_TEST_H
