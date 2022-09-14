/*********************************************************************
 * \file   EffectJsonGatewayTest.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_JSON_GATEWAY_TEST_H
#define EFFECT_JSON_GATEWAY_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include "Gateways/EffectPolicyJsonGateway.h"
#include <string>

class EffectJsonGatewayTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    EffectJsonGatewayTest(const std::string app_name);
    ~EffectJsonGatewayTest();

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;

private:
    Enigma::Gateways::EffectPolicyJsonGateway* m_gateway;
};


#endif // EFFECT_JSON_GATEWAY_TEST_H
