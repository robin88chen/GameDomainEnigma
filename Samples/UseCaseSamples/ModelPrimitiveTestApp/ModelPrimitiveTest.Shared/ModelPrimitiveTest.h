/*********************************************************************
 * \file   ModelPrimitiveTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MODEL_PRIMITIVE_TEST_H
#define _MODEL_PRIMITIVE_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include "Geometries/GeometryId.h"
#include "GameEngine/IRenderer.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/Timer.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderer/RenderTarget.h"
#include "Renderer/Renderer.h"

class ModelPrimitiveTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    ModelPrimitiveTest(const std::string app_name);
    ~ModelPrimitiveTest() override;

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

    void onCameraConstituted(const Enigma::Frameworks::IEventPtr& e);
    void onRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onCameraConstituted;
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;

    Enigma::SceneGraph::SpatialId m_cameraId;
    Enigma::Geometries::GeometryId m_cubeId;
    Enigma::Primitives::PrimitiveId m_meshId;
    Enigma::Primitives::PrimitiveId m_modelId;
    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    std::shared_ptr<Enigma::Renderables::ModelPrimitive> m_model;
    bool m_isPrefabBuilt;
    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
};


#endif // _MODEL_PRIMITIVE_TEST_H
