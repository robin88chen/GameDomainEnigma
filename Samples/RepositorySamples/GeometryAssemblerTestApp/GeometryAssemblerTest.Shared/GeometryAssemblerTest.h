/*********************************************************************
 * \file   GeometryAssemblerTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef GEOMETRY_ASSEMBLER_TEST_H
#define GEOMETRY_ASSEMBLER_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include "Frameworks/EventSubscriber.h"

class GeometryAssemblerTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    GeometryAssemblerTest(const std::string app_name);
    ~GeometryAssemblerTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

private:
    void onAllServicesInitialized(const Enigma::Frameworks::IEventPtr& e);

    void createNewGeometry();
    void queryGeometry();

    void assembleStandardGeometry();

private:
    Enigma::Frameworks::EventSubscriberPtr m_onAllServicesInitialized;

    std::vector<Enigma::MathLib::Vector2> m_uvs;
    std::vector<Enigma::MathLib::Vector3> m_positions;
    std::vector<Enigma::MathLib::Vector3> m_normals;
    std::vector<Enigma::MathLib::Vector4> m_diffuses;
    std::vector<unsigned> m_indices = { 0, 1, 2, 3 };
};

#endif // GEOMETRY_ASSEMBLER_TEST_H
