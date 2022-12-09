/*********************************************************************
 * \file   MeshPrimitiveTest.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_TEST_H
#define _MESH_PRIMITIVE_TEST_H

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
#include "Renderer/RenderTarget.h"
#include "Renderer/RenderElement.h"
#include "Renderer/Renderer.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectMaterialManager.h"

class MeshPrimitiveTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    MeshPrimitiveTest(const std::string app_name);
    ~MeshPrimitiveTest();

    virtual void InitializeMountPaths() override;

    virtual void InstallEngine() override final;
    virtual void ShutdownEngine() override final;

    virtual void FrameUpdate() override;
    virtual void RenderFrame() override;
};


#endif // _MESH_PRIMITIVE_TEST_H
