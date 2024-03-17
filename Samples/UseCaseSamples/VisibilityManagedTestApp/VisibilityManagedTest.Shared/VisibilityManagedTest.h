/*********************************************************************
 * \file   VisibilityManagedTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _VISIBILITY_MANAGED_TEST_H
#define _VISIBILITY_MANAGED_TEST_H

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
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include <memory>
#include <string>

class VisibilityManagedTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    VisibilityManagedTest(const std::string app_name);
    ~VisibilityManagedTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

protected:
    void onRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);
    //void OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);
    //void OnPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);
    void onLazyNodeHydrated(const Enigma::Frameworks::IEventPtr& e);
    void onHydrateLazyNodeFailed(const Enigma::Frameworks::IEventPtr& e);

private:
    void makeCamera();
    void makeModel();
    //void RetrieveDtoCreatedModel();
    //void InsertDtoCreatedModelToRenderer();

    void prepareRenderScene();

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
    //Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
    //Enigma::Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onLazyNodeHydrated;
    Enigma::Frameworks::EventSubscriberPtr m_onHydrateLazyNodeFailed;

    std::shared_ptr<Enigma::FileStorage::SceneGraphFileStoreMapper> m_sceneGraphFileStoreMapper;
    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    std::shared_ptr<Enigma::Renderables::ModelPrimitive> m_model;
    Enigma::SceneGraph::SpatialId m_rootId;
    Enigma::SceneGraph::NodePtr m_sceneRoot;
    Enigma::SceneGraph::SpatialId m_pawnId;
    Enigma::SceneGraph::PawnPtr m_pawn;

    Enigma::SceneGraph::SpatialId m_cameraId;
    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
    Enigma::SceneGraph::Culler* m_culler;
};


#endif // _VISIBILITY_MANAGED_TEST_H
