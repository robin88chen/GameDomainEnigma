/*********************************************************************
 * \file   GeometryDataGatewayTest.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_DATA_GATEWAY_TEST_H
#define _GEOMETRY_DATA_GATEWAY_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include "Frameworks/EventSubscriber.h"
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "MathLib/Vector4.h"

class GeometryDataGatewayTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    GeometryDataGatewayTest(const std::string app_name);
    ~GeometryDataGatewayTest();

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

private:
    void OnGeometryDataBuilt(const Enigma::Frameworks::IEventPtr& e);
    void OnFactoryGeometryCreated(const Enigma::Frameworks::IEventPtr& e);

    void MakeGeometrySaved();
    void LoadGeometry();
    void LoadGeometryByPolicy();

private:
    Enigma::Frameworks::EventSubscriberPtr m_onGeometryDataBuilt;
    Enigma::Frameworks::EventSubscriberPtr m_onFactoryGeometryCreated;

    std::vector<Enigma::MathLib::Vector2> m_uvs;
    std::vector<Enigma::MathLib::Vector3> m_positions;
    std::vector<Enigma::MathLib::Vector3> m_normals;
    std::vector<Enigma::MathLib::Vector4> m_diffuses;
    std::vector<unsigned> m_indices = { 0, 1, 2, 3 };
};

#endif // _GEOMETRY_DATA_GATEWAY_TEST_H
