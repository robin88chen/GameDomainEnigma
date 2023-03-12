/*********************************************************************
 * \file   PortalSystemTest.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _PORTAL_SYSTEM_TEST_H
#define _PORTAL_SYSTEM_TEST_H

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
#include "Renderer/ModelPrimitive.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Renderer.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/Pawn.h"
#include <memory>
#include <string>

class PortalSystemTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    PortalSystemTest(const std::string app_name);
    ~PortalSystemTest() override;

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;

protected:
    void OnRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnBuildPawnPrimitiveFailed(const Enigma::Frameworks::IEventPtr& e);

private:
    void RetrieveDtoCreatedModel();
    void InsertDtoCreatedModelToRenderer();

    void PrepareRenderScene();

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onBuildPawnPrimitiveFailed;

    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    Enigma::Renderer::ModelPrimitivePtr m_model;
    Enigma::SceneGraph::NodePtr m_sceneRoot;
    Enigma::SceneGraph::PawnPtr m_pawn;

    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
    Enigma::SceneGraph::Culler* m_culler;
};

#endif // _PORTAL_SYSTEM_TEST_H
