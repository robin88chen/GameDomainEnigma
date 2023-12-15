/*********************************************************************
 * \file   MeshPrimitiveTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_TEST_H
#define _MESH_PRIMITIVE_TEST_H

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

class MeshPrimitiveTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    MeshPrimitiveTest(const std::string app_name);
    ~MeshPrimitiveTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

protected:
    void makeCamera();
    void makeCube();

    void onCameraConstituted(const Enigma::Frameworks::IEventPtr& e);
    void onGeometryConstituted(const Enigma::Frameworks::IEventPtr& e);
    void onRenderablePrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);
    void onBuildRenderablePrimitiveFailed(const Enigma::Frameworks::IEventPtr& e);
    void onRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onCameraConstituted;
    Enigma::Frameworks::EventSubscriberPtr m_onGeometryConstituted;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderablePrimitiveBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onBuildRenderablePrimitiveFailed;
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;

    Enigma::Geometries::GeometryId m_cubeId;
    Enigma::Engine::PrimitiveId m_meshId;
    Enigma::SceneGraph::SpatialId m_cameraId;
    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    Enigma::Renderer::MeshPrimitivePtr m_mesh;
    bool m_isPrefabBuilt;
    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
};

#endif // _MESH_PRIMITIVE_TEST_H
