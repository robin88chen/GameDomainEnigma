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
#include "Renderables/ModelPrimitive.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Renderer.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/Pawn.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "FileStorage/PrimitiveFileStoreMapper.h"
#include <memory>
#include <string>

class PortalSystemTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    PortalSystemTest(const std::string app_name);
    ~PortalSystemTest() override;

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
    void makeSceneGraph();

    void prepareRenderScene();

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;

    std::shared_ptr<Enigma::FileStorage::GeometryDataFileStoreMapper> m_geometryDataFileStoreMapper;
    std::shared_ptr<Enigma::FileStorage::SceneGraphFileStoreMapper> m_sceneGraphFileStoreMapper;
    std::shared_ptr<Enigma::FileStorage::PrimitiveFileStoreMapper> m_primitiveFileStoreMapper;
    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    std::shared_ptr<Enigma::Renderables::ModelPrimitive> m_model;
    Enigma::SceneGraph::SpatialId m_rootId;
    Enigma::SceneGraph::NodePtr m_sceneRoot;
    Enigma::SceneGraph::PawnPtr m_pawn;

    Enigma::SceneGraph::SpatialId m_cameraId;
    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
    Enigma::SceneGraph::Culler* m_culler;
};

#endif // _PORTAL_SYSTEM_TEST_H
