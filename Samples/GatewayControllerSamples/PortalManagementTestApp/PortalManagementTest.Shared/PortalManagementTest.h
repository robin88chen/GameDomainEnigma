/*****************************************************************
 * \file   PortalManagementTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2024
 ******************************************************************/
#ifndef PORTAL_MANAGEMENT_TEST_H
#define PORTAL_MANAGEMENT_TEST_H

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
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "GameCommon/SceneRendererService.h"
#include <memory>
#include <string>

class PortalManagementTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    PortalManagementTest(const std::string app_name);
    ~PortalManagementTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

private:
    void makeSceneGraph();
    void prepareRenderScene();

    void onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e);

protected:
    std::weak_ptr<Enigma::GameCommon::SceneRendererService> m_sceneRendererService;

    Enigma::Frameworks::EventSubscriberPtr m_onRenderEngineInstalled;

    std::shared_ptr<Enigma::FileStorage::GeometryDataFileStoreMapper> m_geometryDataFileStoreMapper;
    std::shared_ptr<Enigma::FileStorage::SceneGraphFileStoreMapper> m_sceneGraphFileStoreMapper;
    std::shared_ptr<Enigma::FileStorage::PrimitiveFileStoreMapper> m_primitiveFileStoreMapper;
    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    std::shared_ptr<Enigma::Renderables::ModelPrimitive> m_model;
    Enigma::SceneGraph::SpatialId m_rootId;
    Enigma::SceneGraph::SpatialId m_portalManagementId;
    Enigma::SceneGraph::NodePtr m_sceneRoot;
    Enigma::SceneGraph::PawnPtr m_pawn;

    Enigma::SceneGraph::SpatialId m_cameraId;
    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
    Enigma::SceneGraph::Culler* m_culler;
};


#endif // PORTAL_MANAGEMENT_TEST_H
