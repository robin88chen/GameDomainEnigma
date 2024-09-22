/*********************************************************************
 * \file   ViewerAppDelegate.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef VIEWER_APP_DELEGATE_H
#define VIEWER_APP_DELEGATE_H

#include <windows.h>
#include "GraphicKernel/IGraphicAPI.h"
#include "Controllers/GraphicMain.h"
#include "InputHandlers/InputHandlerService.h"
#include "Rendering/SceneRendering.h"
#include "Frameworks/EventSubscriber.h"
#include "GameCommon/AnimatedPawn.h"
#include "Frameworks/CommandSubscriber.h"
#include "ShadowMap/ShadowMapService.h"
#include "FileStorage/GeometryDataFileStoreMapper.h"
#include "FileStorage/AnimationAssetFileStoreMapper.h"
#include "FileStorage/AnimatorFileStoreMapper.h"
#include "ViewerSceneGraphFileStoreMapper.h"
#include "AnimatedPawnPresentation.h"
#include "ViewerAvatarBaker.h"
#include "ViewerAnimationClipCommandHandler.h"
#include "Controllers/InstallingPolicyList.h"

namespace EnigmaViewer
{
    class ViewerRenderablesFileStoreMapper;
    class ViewerSceneGraphFileStoreMapper;
    class ViewerTextureFileStoreMapper;
    class ViewerAppDelegate
    {
    public:
        ViewerAppDelegate();
        ~ViewerAppDelegate();

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

        void importDaeFile(const std::string& filename);

        void saveAnimatedPawn();

        std::shared_ptr<Enigma::InputHandlers::InputHandlerService> inputHandler() const { return m_inputHandler.lock(); }

    protected:
        void installEngineService();
        void installDomainService();
        void installUseCaseService();
        void installPresentationService();
        void installGameCommonService();
        void onRenderEngineInstalled(const Enigma::Frameworks::IEventPtr& e);
        void onSceneGraphRootCreated(const Enigma::Frameworks::IEventPtr& e);

        void loadModelPrimitive(const Enigma::Frameworks::ICommandPtr& c);
        void createAnimatedPawn(const Enigma::Frameworks::ICommandPtr& c);
        void loadAnimatedPawn(const Enigma::Frameworks::ICommandPtr& c);
        void removeModelPrimitive(const Enigma::Frameworks::ICommandPtr& c);
        void removeAnimatedPawn(const Enigma::Frameworks::ICommandPtr& c);

        void refreshModelList();
        void loadModelPrimitive(const std::string& model_name);
        void removeModelPrimitive(const std::string& model_name);
        void refreshPawnList();
        void loadAnimatedPawn(const std::string& pawn_name);
        void removeAnimatedPawn(const std::string& pawn_name);

    protected:
        HWND m_hwnd;
        bool m_hasLogFile;

        std::string m_mediaRootPath;

        Enigma::Controllers::GraphicMain* m_graphicMain;

        Enigma::Controllers::InstallingPolicyList m_policyList;

        std::shared_ptr<Enigma::FileStorage::GeometryDataFileStoreMapper> m_geometryDataFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::AnimationAssetFileStoreMapper> m_animationAssetFileStoreMapper;
        std::shared_ptr<Enigma::FileStorage::AnimatorFileStoreMapper> m_animatorFileStoreMapper;
        std::shared_ptr<ViewerRenderablesFileStoreMapper> m_primitiveFileStoreMapper;
        std::shared_ptr<ViewerSceneGraphFileStoreMapper> m_sceneGraphFileStoreMapper;
        std::shared_ptr<ViewerTextureFileStoreMapper> m_textureFileStoreMapper;

        std::weak_ptr<Enigma::InputHandlers::InputHandlerService> m_inputHandler;
        std::weak_ptr<Enigma::Rendering::SceneRendering> m_sceneRendering;
        std::weak_ptr<Enigma::ShadowMap::ShadowMapService> m_shadowMapService;
        std::weak_ptr<Enigma::GameCommon::GameSceneService> m_gameSceneService;

        Enigma::Frameworks::EventSubscriberPtr m_onRenderEngineInstalled;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphRootCreated;

        Enigma::Frameworks::CommandSubscriberPtr m_loadModelPrimitive;
        Enigma::Frameworks::CommandSubscriberPtr m_createAnimatedPawn;
        Enigma::Frameworks::CommandSubscriberPtr m_loadAnimatedPawn;
        Enigma::Frameworks::CommandSubscriberPtr m_removeModelPrimitive;
        Enigma::Frameworks::CommandSubscriberPtr m_removeAnimatedPawn;

        Enigma::SceneGraph::SpatialId m_primaryCameraId;
        Enigma::SceneGraph::SpatialId m_sceneRootId;
        std::shared_ptr<Enigma::SceneGraph::Node> m_sceneRoot;
        AnimatedPawnPresentation m_viewingPawnPresentation;
        ViewerAvatarBaker m_avatarBaker;
        ViewerAnimationClipCommandHandler m_animationClipCommandHandler;
        Enigma::SceneGraph::SpatialId m_viewingPawnId;
        Enigma::SceneGraph::SpatialId m_creatingPawnId;
    };
}

#endif // VIEWER_APP_DELEGATE_H
