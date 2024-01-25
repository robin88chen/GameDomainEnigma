/*********************************************************************
 * \file   AnimatedModelTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATED_MODEL_TEST_H
#define _ANIMATED_MODEL_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include <Geometries/GeometryId.h>

#include "GameEngine/IRenderer.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/Timer.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Renderer.h"
#include "Animators/AnimationAssetId.h"

class AnimatedModelTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    AnimatedModelTest(const std::string app_name);
    ~AnimatedModelTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

protected:
    void makeCamera();
    void makeCube();
    void makeMesh();
    void makeModel();
    void makeAnimation();

    void onCameraConstituted(const Enigma::Frameworks::IEventPtr& e);
    void onGeometryConstituted(const Enigma::Frameworks::IEventPtr& e);
    void onPrimitiveConstituted(const Enigma::Frameworks::IEventPtr& e);
    void onRenderablePrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);
    void onBuildRenderablePrimitiveFailed(const Enigma::Frameworks::IEventPtr& e);
    void onAnimationAssetConstituted(const Enigma::Frameworks::IEventPtr& e);
    void onConstituteAnimationAssetFailed(const Enigma::Frameworks::IEventPtr& e);
    void onRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onCameraConstituted;
    Enigma::Frameworks::EventSubscriberPtr m_onGeometryConstituted;
    Enigma::Frameworks::EventSubscriberPtr m_onPrimitiveConstituted;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderablePrimitiveBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onBuildRenderablePrimitiveFailed;
    Enigma::Frameworks::EventSubscriberPtr m_onAnimationAssetConstituted;
    Enigma::Frameworks::EventSubscriberPtr m_onConstituteAnimationAssetFailed;
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;

    std::vector<std::string> m_meshNodeNames;
    Enigma::SceneGraph::SpatialId m_cameraId;
    Enigma::Geometries::GeometryId m_cubeId;
    Enigma::Animators::AnimationAssetId m_animationId;
    Enigma::Primitives::PrimitiveId m_meshId;
    Enigma::Primitives::PrimitiveId m_modelId;
    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    std::shared_ptr<Enigma::Renderables::ModelPrimitive> m_model;
    bool m_isPrefabBuilt;
    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
};


#endif // _ANIMATED_MODEL_TEST_H
