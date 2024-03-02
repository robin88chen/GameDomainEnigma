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

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

protected:
    void createFloorReceiver();
    void createCubePawn();

    void onSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e);
    void onSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);
    void onPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);

protected:
    std::weak_ptr<Enigma::GameCommon::SceneRendererService> m_sceneRendererService;
    std::weak_ptr<Enigma::ShadowMap::ShadowMapService> m_shadowMapService;

    Enigma::SceneGraph::NodePtr m_sceneRoot;
    Enigma::SceneGraph::PawnPtr m_floor;
    Enigma::SceneGraph::PawnPtr m_cube;
    Enigma::SceneGraph::SpatialId m_floorId;
    Enigma::SceneGraph::SpatialId m_cubeId;

    Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphRootCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;
};


#endif // CASCADE_SHADOWMAP_TEST_H
