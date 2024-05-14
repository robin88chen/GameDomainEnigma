/*********************************************************************
 * \file   FindQuadVolumeTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _FIND_QUAD_VOLUME_TEST_H
#define _FIND_QUAD_VOLUME_TEST_H

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
#include "FileStorage/SceneGraphFileStoreMapper.h"

class FindQuadVolumeTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    FindQuadVolumeTest(const std::string app_name);
    ~FindQuadVolumeTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

protected:
    void onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e);

private:
    void makeOneLevelQuad();
    void testOneLevelQuad();
    void makeThreeLevelTree();
    void testThreeLevelTree();

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onRenderEngineInstalled;
    std::shared_ptr<Enigma::FileStorage::SceneGraphFileStoreMapper> m_sceneGraphFileStoreMapper;
    Enigma::SceneGraph::SpatialId m_rootQuadId;
    Enigma::SceneGraph::SpatialId m_level1QuadId1;
    Enigma::SceneGraph::SpatialId m_level1QuadId2;
    Enigma::SceneGraph::SpatialId m_level2QuadId1;
};


#endif // _FIND_QUAD_VOLUME_TEST_H
