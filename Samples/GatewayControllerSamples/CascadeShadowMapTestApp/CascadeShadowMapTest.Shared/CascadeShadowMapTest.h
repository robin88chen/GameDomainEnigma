/*********************************************************************
 * \file   CascadeShadowMapTest.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef CASCADE_SHADOWMAP_TEST_H
#define CASCADE_SHADOWMAP_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include "GameCommon/SceneRendererService.h"
#include "ShadowMap/ShadowMapService.h"
#include "Frameworks/EventSubscriber.h"
#include "SceneGraph/Pawn.h"
#include <memory>
#include <string>

class CascadeShadowMapTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    CascadeShadowMapTest(const std::string app_name);
    ~CascadeShadowMapTest() override;

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;

protected:
    void CreateFloorReceiver() const;
    void CreateCubePawn() const;

    void OnSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e);
    void OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);

protected:
    std::weak_ptr<Enigma::GameCommon::SceneRendererService> m_sceneRendererService;
    std::weak_ptr<Enigma::ShadowMap::ShadowMapService> m_shadowMapService;

    Enigma::SceneGraph::NodePtr m_sceneRoot;
    Enigma::SceneGraph::PawnPtr m_floor;
    Enigma::SceneGraph::PawnPtr m_cube;

    Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphRootCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;
};


#endif // CASCADE_SHADOWMAP_TEST_H
