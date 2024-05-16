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
#include "GameCommon/SceneRendererService.h"
#include "InputHandlers/InputHandlerService.h"
#include "Frameworks/EventSubscriber.h"
#include "ShadowMap/ShadowMapService.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "FileStorage/AnimatorFileStoreMapper.h"
#include "FileStorage/SceneGraphFileStoreMapper.h"
#include "FileStorage/WorldMapFileStoreMapper.h"
#include "FileStorage/TextureFileStoreMapper.h"

namespace LevelEditor
{
    class AppConfiguration;

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

        const std::unique_ptr<AppConfiguration>& appConfig() const { return m_appConfig; };
        std::shared_ptr<Enigma::InputHandlers::InputHandlerService> inputHandler() const { return m_inputHandler.lock(); };

    protected:
        void onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e);
        void onSceneRootCreated(const Enigma::Frameworks::IEventPtr& e);
        void onSceneGraphChanged(const Enigma::Frameworks::IEventPtr& e);
        //void onWorldMapCreated(const Enigma::Frameworks::IEventPtr& e);
        //void onCreateWorldFailed(const Enigma::Frameworks::IEventPtr& e);
        //void onWorldMapDeserialized(const Enigma::Frameworks::IEventPtr& e);
        //void onDeserializeWorldMapFailed(const Enigma::Frameworks::IEventPtr& e);

    protected:
        HWND m_hwnd;
        bool m_hasLogFile;

        std::string m_mediaRootPath;

        std::unique_ptr<AppConfiguration> m_appConfig;

        Enigma::Controllers::GraphicMain* m_graphicMain;
        std::shared_ptr<Enigma::FileStorage::GeometryDataFileStoreMapper> m_geometryDataFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::PrimitiveFileStoreMapper> m_primitiveFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::AnimationAssetFileStoreMapper> m_animationAssetFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::AnimatorFileStoreMapper> m_animatorFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::TextureFileStoreMapper> m_textureFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::SceneGraphFileStoreMapper> m_sceneGraphFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::WorldMapFileStoreMapper> m_worldMapFileStoreMapper;
        std::weak_ptr<Enigma::InputHandlers::InputHandlerService> m_inputHandler;
        std::weak_ptr<Enigma::GameCommon::SceneRendererService> m_sceneRenderer;
        std::weak_ptr<Enigma::ShadowMap::ShadowMapService> m_shadowMapService;

        std::weak_ptr<Enigma::SceneGraph::Node> m_sceneRoot;

        Enigma::Frameworks::EventSubscriberPtr m_onRenderEngineInstalled;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneRootCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphChanged;
        //Enigma::Frameworks::EventSubscriberPtr m_onWorldMapCreated;
        //Enigma::Frameworks::EventSubscriberPtr m_onCreateWorldFailed;
        //Enigma::Frameworks::EventSubscriberPtr m_onWorldMapDeserialized;
        //Enigma::Frameworks::EventSubscriberPtr m_onDeserializeWorldMapFailed;
    };
}


#endif // _LEVEL_EDITOR_APP_DELEGATE_H
