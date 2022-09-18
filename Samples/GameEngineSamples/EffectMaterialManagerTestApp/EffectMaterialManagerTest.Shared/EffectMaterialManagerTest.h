/*********************************************************************
 * \file   EffectMaterialManagerTest.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_MATERIAL_MANAGER_TEST_H
#define EFFECT_MATERIAL_MANAGER_TEST_H

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
#include "GameEngine/EffectCompiler.h"
#include "GameEngine/EffectCompilingPolicies.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectMaterialManager.h"

class BufferBuilder;
class TextureSamplerBuilder;

class EffectMaterialManagerTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    EffectMaterialManagerTest(const std::string app_name);
    ~EffectMaterialManagerTest();

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;
private:
    Enigma::Engine::EffectCompilingPolicy MakeEffectCompilingPolicy();

    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnEffectMaterialCompiled(const Enigma::Frameworks::IEventPtr& e);
    void OnEffectCompileFailed(const Enigma::Frameworks::IEventPtr& e);
    void OnVertexBufferBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnIndexBufferBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnTextureLoaded(const Enigma::Frameworks::IEventPtr& e);

    void BuildVariables();

private:
    Enigma::Engine::RendererManager* m_rendererManager;
    Enigma::Engine::RenderTargetPtr m_renderTarget;
    Enigma::Engine::EffectMaterialManager* m_materialManager;
    Enigma::Graphics::IVertexBufferPtr m_vtxBuffer;
    Enigma::Graphics::IIndexBufferPtr m_idxBuffer;
    Enigma::Graphics::ITexturePtr m_texture;
    Enigma::Engine::EffectMaterialPtr m_material;

    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onEffectMaterialCompiled;
    Enigma::Frameworks::EventSubscriberPtr m_onEffectCompileFailed;
    Enigma::Frameworks::EventSubscriberPtr m_onVertexBufferBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onIndexBufferBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onTextureLoaded;
    Enigma::Frameworks::EventSubscriberPtr m_onSamplerBuilt;

    Enigma::Frameworks::Timer* m_timer;
    float m_tick;
    BufferBuilder* m_bufferBuilder;
    TextureSamplerBuilder* m_textureBuilder;
};


#endif // EFFECT_MATERIAL_MANAGER_TEST_H
