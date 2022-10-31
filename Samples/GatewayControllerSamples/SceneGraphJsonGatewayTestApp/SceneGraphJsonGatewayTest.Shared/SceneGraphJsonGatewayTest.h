/*********************************************************************
 * \file   SceneGraphJsonGatewayTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _SCENEGRAPH_JSON_GATEWAY_TEST_H
#define _SCENEGRAPH_JSON_GATEWAY_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include "Frameworks/EventSubscriber.h"
#include "MathLib/Matrix4.h"
#include "MathLib/Box3.h"

class BufferBuilder;
class TextureSamplerBuilder;

class SceneGraphJsonGatewayTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    SceneGraphJsonGatewayTest(const std::string app_name);
    ~SceneGraphJsonGatewayTest();

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

private:
    void OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);

private:
    Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
    Enigma::MathLib::Matrix4 m_mxChild1Local;
    Enigma::MathLib::Matrix4 m_mxChild2Local;
    Enigma::MathLib::Box3 m_child1WorldBox;
};

#endif // _SCENEGRAPH_JSON_GATEWAY_TEST_H
