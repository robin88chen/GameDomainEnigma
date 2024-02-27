/*********************************************************************
 * \file   DeferredRenderingTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef DEFERRED_RENDERING_TEST_H
#define DEFERRED_RENDERING_TEST_H

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
#include "Renderables/ModelPrimitive.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Renderer.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/Pawn.h"
#include "GameCommon/SceneRendererService.h"
#include <memory>
#include <string>

class DeferredRenderingTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    DeferredRenderingTest(const std::string app_name);
    ~DeferredRenderingTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

protected:
    void onSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e);
    void onRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    void onSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);
    void onPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);
    void onBuildPawnPrimitiveFailed(const Enigma::Frameworks::IEventPtr& e);

private:
    void createCubePawn();
    void retrieveDtoCreatedModel();
    void insertDtoCreatedModelToRenderer();

    void prepareRenderScene();

protected:
    std::weak_ptr<Enigma::GameCommon::SceneRendererService> m_sceneRendererService;

    Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphRootCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onBuildPawnPrimitiveFailed;

    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    std::shared_ptr<Enigma::Renderables::ModelPrimitive> m_model;
    Enigma::SceneGraph::NodePtr m_sceneRoot;
    Enigma::SceneGraph::SpatialId m_sceneRootId;
    Enigma::SceneGraph::PawnPtr m_pawn;
    Enigma::SceneGraph::SpatialId m_cubeId;

    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
    Enigma::SceneGraph::Culler* m_culler;
};


#endif // DEFERRED_RENDERING_TEST_H
