/*********************************************************************
 * \file   DeferredRendering.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef DEFERRED_RENDERER_SERVICE_H
#define DEFERRED_RENDERER_SERVICE_H

#include "SceneRendering.h"
#include "Renderables/MeshPrimitive.h"
#include "Frameworks/EventSubscriber.h"
#include "SceneGraph/Light.h"
#include "SceneGraph/LightEvents.h"
#include "SceneGraph/SceneGraphRepository.h"
#include <memory>

namespace Enigma::GameCommon
{
    class LightingPawn;
    class LightVolumePawn;
    class LightingPawnRepository;
    class LightMeshAssembler;
};

namespace Enigma::Rendering
{
    class DeferredRenderingConfiguration;

    class DeferredRendering : public SceneRendering
    {
        DECLARE_EN_RTTI
    public:
        DeferredRendering(Frameworks::ServiceManager* mngr, const std::shared_ptr<GameCommon::GameSceneService>& scene_service,
            const std::shared_ptr<GameCommon::GameCameraService>& camera_service,
            const std::shared_ptr<Renderer::RendererManager>& renderer_manager,
            const std::shared_ptr<Renderer::IRenderingConfiguration>& configuration);
        DeferredRendering(const DeferredRendering&) = delete;
        DeferredRendering(DeferredRendering&&) = delete;
        virtual ~DeferredRendering() override;
        DeferredRendering& operator=(DeferredRendering&) = delete;
        DeferredRendering& operator=(DeferredRendering&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        virtual void createSceneRenderSystem(const std::string& renderer_name, const std::string& target_name) override;
        virtual void destroySceneRenderSystem(const std::string& renderer_name, const std::string& target_name) override;

        virtual void prepareGameScene() override;

    private:
        void createGBuffer(const Renderer::RenderTargetPtr& primary_target);

        void bindGBufferToPendingLights();

        void onPrimaryRenderTargetCreated(const Frameworks::IEventPtr& e);
        void onPrimaryRenderTargetResized(const Frameworks::IEventPtr& e);

        void onLightCreatedOrConstituted(const Frameworks::IEventPtr& e);
        void onLightInfoDeleted(const Frameworks::IEventPtr& e);

    private:
        std::shared_ptr<DeferredRenderingConfiguration> m_configuration;
        std::shared_ptr<GameCommon::LightMeshAssembler> m_lightMeshAssembler;
        std::shared_ptr<GameCommon::LightingPawnRepository> m_lightingPawns;

        std::weak_ptr<Renderer::RenderTarget> m_gBuffer;

        std::vector<std::shared_ptr<SceneGraph::Light>> m_pendingLightsOfGBufferBind;

        Frameworks::EventSubscriberPtr m_onPrimaryRenderTargetCreated;
        Frameworks::EventSubscriberPtr m_onPrimaryRenderTargetResized;

        Frameworks::EventSubscriberPtr m_onLightCreated;
        Frameworks::EventSubscriberPtr m_onLightConstituted;
        Frameworks::EventSubscriberPtr m_onLightInfoDeleted;
    };
}
#endif // DEFERRED_RENDERER_SERVICE_H
