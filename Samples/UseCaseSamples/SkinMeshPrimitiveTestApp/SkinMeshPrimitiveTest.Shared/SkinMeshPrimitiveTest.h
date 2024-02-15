/*********************************************************************
 * \file   SkinMeshPrimitiveTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _SKIN_MESH_PRIMITIVE_TEST_H
#define _SKIN_MESH_PRIMITIVE_TEST_H

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
#include "Geometries/GeometryId.h"
#include "Animators/AnimationAssetId.h"

class SkinMeshPrimitiveTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    SkinMeshPrimitiveTest(const std::string app_name);
    ~SkinMeshPrimitiveTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

protected:
    void makeCamera();
    void makeModel();

    void onCameraConstituted(const Enigma::Frameworks::IEventPtr& e);

    void onRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onCameraConstituted;

    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;

    std::vector<std::string> m_meshNodeNames;
    Enigma::SceneGraph::SpatialId m_cameraId;
    Enigma::Geometries::GeometryId m_cubeId;
    Enigma::Animators::AnimationAssetId m_animationId;
    Enigma::Animators::AnimatorId m_animatorId;
    Enigma::Primitives::PrimitiveId m_meshId;
    Enigma::Primitives::PrimitiveId m_modelId;
    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    std::shared_ptr<Enigma::Renderables::ModelPrimitive> m_model;
    bool m_isPrefabBuilt;
    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
};


#endif // _SKIN_MESH_PRIMITIVE_TEST_H
