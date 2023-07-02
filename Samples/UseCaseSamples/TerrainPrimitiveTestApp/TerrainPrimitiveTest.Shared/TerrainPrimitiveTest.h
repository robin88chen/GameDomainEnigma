/*********************************************************************
 * \file   TerrainPrimitiveTest.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef TERRAIN_PRIMITIVE_TEST_H
#define TERRAIN_PRIMITIVE_TEST_H

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
#include "Renderer/MeshPrimitive.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/RenderElement.h"
#include "Renderer/Renderer.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectMaterialManager.h"
#include "Frameworks/ResponseSubscriber.h"
#include "Terrain/TerrainPrimitive.h"
#include "Frameworks/ruid.h"

class TerrainPrimitiveTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    TerrainPrimitiveTest(const std::string app_name);
    ~TerrainPrimitiveTest() override;

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;

protected:
    void OnRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnBuildRenderablePrimitiveResponse(const Enigma::Frameworks::IResponsePtr& r);

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::ResponseSubscriberPtr m_onBuildRenderablePrimitiveResponse;

    Enigma::Frameworks::Ruid m_ruidBuildRequester;
    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    std::shared_ptr<Enigma::Terrain::TerrainPrimitive> m_terrain;
    //bool m_isPrefabBuilt;
    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
};


#endif // TERRAIN_PRIMITIVE_TEST_H
