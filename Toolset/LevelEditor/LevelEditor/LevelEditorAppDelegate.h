/*********************************************************************
 * \file   LevelEditorAppDelegate.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _LEVEL_EDITOR_APP_DELEGATE_H
#define _LEVEL_EDITOR_APP_DELEGATE_H

#include <windows.h>
#include "GraphicKernel/IGraphicAPI.h"
#include "Controllers/GraphicMain.h"
#include "Rendering/SceneRendering.h"
#include "InputHandlers/InputHandlerService.h"
#include "Frameworks/EventSubscriber.h"
#include "ShadowMap/ShadowMapService.h"
#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "FileStorage/AnimatorFileStoreMapper.h"

namespace LevelEditor
{
    class AppConfiguration;
    class GeometryDataFileStoreMapper;
    class WorldMapFileStoreMapper;
    class TextureFileStoreMapper;
    class SceneGraphFileStoreMapper;

    class EditorAppDelegate
    {
    public:
        EditorAppDelegate();
        ~EditorAppDelegate();

        void initialize(Enigma::Graphics::IGraphicAPI::APIVersion api_ver, Enigma::Graphics::IGraphicAPI::AsyncType useAsyncDevice,
            const std::string& log_filename, HWND hwnd);
        virtual void initializeMountPaths();
        void installEngine();
        void registerMediaMountPaths(const std::string& media_path);
        void shutdownEngine();
        void finalize();

        void frameUpdate();
        void prepareRender();
        void renderFrame();

        void onTimerElapsed();

        const std::unique_ptr<AppConfiguration>& appConfig() const { return m_appConfig; }
        std::shared_ptr<Enigma::InputHandlers::InputHandlerService> inputHandler() const { return m_inputHandler.lock(); }
        const std::shared_ptr<WorldMapFileStoreMapper>& worldMapFileStoreMapper() const { return m_worldMapFileStoreMapper; }
        const std::shared_ptr<GeometryDataFileStoreMapper>& geometryDataFileStoreMapper() const { return m_geometryDataFileStoreMapper; }
        const std::shared_ptr<SceneGraphFileStoreMapper>& sceneGraphFileStoreMapper() const { return m_sceneGraphFileStoreMapper; }

    protected:
        void onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e);
        void onSceneRootCreated(const Enigma::Frameworks::IEventPtr& e);
        void onNodeChildAttached(const Enigma::Frameworks::IEventPtr& e);
        void onOutsideRegionAttached(const Enigma::Frameworks::IEventPtr& e);
        void onLazyNodeHydrated(const Enigma::Frameworks::IEventPtr& e);
        //void onWorldMapCreated(const Enigma::Frameworks::IEventPtr& e);
        //void onCreateWorldFailed(const Enigma::Frameworks::IEventPtr& e);
        //void onWorldMapDeserialized(const Enigma::Frameworks::IEventPtr& e);
        //void onDeserializeWorldMapFailed(const Enigma::Frameworks::IEventPtr& e);

        void refreshSceneGraphPanel();

    protected:
        HWND m_hwnd;
        bool m_hasLogFile;

        std::string m_mediaRootPath;

        std::unique_ptr<AppConfiguration> m_appConfig;

        Enigma::Controllers::GraphicMain* m_graphicMain;
        std::shared_ptr<GeometryDataFileStoreMapper> m_geometryDataFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::PrimitiveFileStoreMapper> m_primitiveFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::AnimationAssetFileStoreMapper> m_animationAssetFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::AnimatorFileStoreMapper> m_animatorFileStoreMapper;
        std::shared_ptr<TextureFileStoreMapper> m_textureFileStoreMapper;
        std::shared_ptr<SceneGraphFileStoreMapper> m_sceneGraphFileStoreMapper;
        std::shared_ptr<WorldMapFileStoreMapper> m_worldMapFileStoreMapper;
        std::weak_ptr<Enigma::InputHandlers::InputHandlerService> m_inputHandler;
        std::weak_ptr<Enigma::GameCommon::GameSceneService> m_gameSceneService;
        std::weak_ptr<Enigma::Rendering::SceneRendering> m_sceneRendering;
        std::weak_ptr<Enigma::ShadowMap::ShadowMapService> m_shadowMapService;

        std::weak_ptr<Enigma::SceneGraph::Node> m_sceneRoot;

        Enigma::Frameworks::EventSubscriberPtr m_onRenderEngineInstalled;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneRootCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onNodeChildAttached;
        Enigma::Frameworks::EventSubscriberPtr m_onOutsideRegionAttached;
        Enigma::Frameworks::EventSubscriberPtr m_onLazyNodeHydrated;
        //Enigma::Frameworks::EventSubscriberPtr m_onWorldMapCreated;
        //Enigma::Frameworks::EventSubscriberPtr m_onCreateWorldFailed;
        //Enigma::Frameworks::EventSubscriberPtr m_onWorldMapDeserialized;
        //Enigma::Frameworks::EventSubscriberPtr m_onDeserializeWorldMapFailed;
    };
}


#endif // _LEVEL_EDITOR_APP_DELEGATE_H
