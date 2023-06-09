/*********************************************************************
 * \file   ShadowMapTest.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef _SHADOW_MAP_TEST_H
#define _SHADOW_MAP_TEST_H

#include "Platforms/PlatformConfig.h"
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "Application/AppDelegateAndroid.h"
#else
#include "Application/AppDelegateWin32.h"
#endif
#include "GameCommon/SceneRendererService.h"
#include "ShadowMap/ShadowMapService.h"
#include <memory>
#include <string>

class ShadowMapTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    ShadowMapTest(const std::string app_name);
    ~ShadowMapTest() override;

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;
protected:
    std::weak_ptr<Enigma::GameCommon::SceneRendererService> m_sceneRendererService;
    std::weak_ptr<Enigma::ShadowMap::ShadowMapService> m_shadowMapService;
};


#endif // _SHADOW_MAP_TEST_H
