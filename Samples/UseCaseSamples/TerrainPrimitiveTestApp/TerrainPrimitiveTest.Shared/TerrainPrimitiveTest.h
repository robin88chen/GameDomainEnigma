/*********************************************************************
 * \file   TerrainPrimitiveTest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef TERRAIN_PRIMITIVE_TEST_H
#define TERRAIN_PRIMITIVE_TEST_H

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
#include "Renderer/Renderer.h"
#include "Terrain/TerrainPrimitive.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "FileStorage/TextureFileStoreMapper.h"

class TerrainPrimitiveTest : public Enigma::Application::AppDelegate
{
public:
    static inline std::string MediaPathName = "APK_PATH";
public:
    TerrainPrimitiveTest(const std::string& app_name);
    ~TerrainPrimitiveTest() override;

    virtual void initializeMountPaths() override;

    virtual void installEngine() override final;
    virtual void shutdownEngine() override final;

    virtual void frameUpdate() override;
    virtual void renderFrame() override;

protected:
    void onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e);
    void onRendererCreated(const Enigma::Frameworks::IEventPtr& e);
    void onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);

    void makeCamera();
    void makeTerrain();
    void makeSplatTextureDto();

protected:
    Enigma::Frameworks::EventSubscriberPtr m_onRenderEngineInstalled;
    Enigma::Frameworks::EventSubscriberPtr m_onRendererCreated;
    Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;

    std::shared_ptr<Enigma::FileStorage::GeometryDataFileStoreMapper> m_geometryDataFileStoreMapper;
    std::shared_ptr<Enigma::FileStorage::SceneGraphFileStoreMapper> m_sceneGraphFileStoreMapper;
    std::shared_ptr<Enigma::FileStorage::PrimitiveFileStoreMapper> m_primitiveFileStoreMapper;
    std::shared_ptr<Enigma::FileStorage::TextureFileStoreMapper> m_textureFileStoreMapper;

    Enigma::Renderer::RendererPtr m_renderer;
    Enigma::Renderer::RenderTargetPtr m_renderTarget;
    Enigma::Engine::TextureId m_splatTextureId;
    std::shared_ptr<Enigma::Terrain::TerrainPrimitive> m_terrain;
    Enigma::SceneGraph::SpatialId m_cameraId;
    std::shared_ptr<Enigma::SceneGraph::Camera> m_camera;
};


#endif // TERRAIN_PRIMITIVE_TEST_H
