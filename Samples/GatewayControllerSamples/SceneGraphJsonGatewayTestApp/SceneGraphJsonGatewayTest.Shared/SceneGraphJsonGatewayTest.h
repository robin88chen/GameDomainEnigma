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
};

#endif // _SCENEGRAPH_JSON_GATEWAY_TEST_H
