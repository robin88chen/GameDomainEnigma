/*********************************************************************
 * \file   CreateFittingNodeTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _CREATE_FITTING_NODE_TEST_H
#define _CREATE_FITTING_NODE_TEST_H

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
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "SceneGraph/SceneGraphRepository.h"

class CreateFittingNodeTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    CreateFittingNodeTest(const std::string app_name);
    ~CreateFittingNodeTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

protected:
    void onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e);

private:
    void makeOneLevelQuad();
    void testFittingNodeCreation();

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onRenderEngineInstalled;
    std::shared_ptr<Enigma::FileStorage::SceneGraphFileStoreMapper> m_sceneGraphFileStoreMapper;
    std::shared_ptr<Enigma::SceneGraph::SceneGraphRepository> m_sceneGraphRepository;
    Enigma::SceneGraph::SpatialId m_rootQuadId;
    Enigma::SceneGraph::SpatialId m_level1QuadId3;
    Enigma::SceneGraph::SpatialId m_level4QuadId2;
    Enigma::SceneGraph::SpatialId m_level3QuadId0;
};


#endif // _CREATE_FITTING_NODE_TEST_H
