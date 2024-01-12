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

private:
    void onAllServicesInitialized(const Enigma::Frameworks::IEventPtr& e);

    void createNewGeometry();
    void queryGeometry();

private:
    Enigma::Frameworks::EventSubscriberPtr m_onAllServicesInitialized;

    std::vector<Enigma::MathLib::Vector2> m_uvs;
    std::vector<Enigma::MathLib::Vector3> m_positions;
    std::vector<Enigma::MathLib::Vector3> m_normals;
    std::vector<Enigma::MathLib::Vector4> m_diffuses;
    std::vector<unsigned> m_indices = { 0, 1, 2, 3 };
};

#endif // GEOMETRY_REPOSITORY_TEST_H
