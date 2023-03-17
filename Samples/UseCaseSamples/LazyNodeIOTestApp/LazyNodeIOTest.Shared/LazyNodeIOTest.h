/*********************************************************************
 * \file   LazyNodeIOTest.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _LAZY_NODE_IO_TEST_H
#define _LAZY_NODE_IO_TEST_H

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

class LazyNodeIOTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    LazyNodeIOTest(const std::string app_name);
    ~LazyNodeIOTest() override;

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;

protected:
    void OnRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);

private:
    void RetrieveDtoCreatedModel();
    void InsertDtoCreatedModelToRenderer();

    void PrepareRenderScene();

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;

    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    Enigma::Renderer::ModelPrimitivePtr m_model;
    Enigma::SceneGraph::NodePtr m_sceneRoot;
    Enigma::SceneGraph::PawnPtr m_pawn;

    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
    Enigma::SceneGraph::Culler* m_culler;
};


#endif // _LAZY_NODE_IO_TEST_H
