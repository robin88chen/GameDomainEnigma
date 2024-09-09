/*********************************************************************
 * \file   SceneGraphPawnTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _SCENE_GRAPH_PAWN_TEST_H
#define _SCENE_GRAPH_PAWN_TEST_H

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
#include "SceneGraph/NodalSceneGraph.h"

class SceneGraphPawnTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    SceneGraphPawnTest(const std::string app_name);
    ~SceneGraphPawnTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

protected:
    void onRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);

private:
    void makeCamera();
    void makeModel();

    void prepareRenderScene();

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;

    std::unique_ptr<Enigma::SceneGraph::SceneGraph> m_sceneGraph;
    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    std::shared_ptr<Enigma::Renderables::ModelPrimitive> m_model;
    Enigma::SceneGraph::SpatialId m_rootId;
    //Enigma::SceneGraph::NodePtr m_sceneRoot;
    std::shared_ptr<Enigma::SceneGraph::Pawn> m_pawn;

    bool m_isPrefabBuilt;
    Enigma::SceneGraph::SpatialId m_cameraId;
    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
    Enigma::SceneGraph::Culler* m_culler;
};


#endif // _SCENE_GRAPH_PAWN_TEST_H
