/*********************************************************************
 * \file   GeometryRepositoryTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef GEOMETRY_REPOSITORY_TEST_H
#define GEOMETRY_REPOSITORY_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include "Frameworks/EventSubscriber.h"

class GeometryRepositoryTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    GeometryRepositoryTest(const std::string app_name);
    ~GeometryRepositoryTest() override;

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;
};

#endif // GEOMETRY_REPOSITORY_TEST_H
