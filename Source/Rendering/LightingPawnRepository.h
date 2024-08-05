/*********************************************************************
 * \file   LightingPawnRepository.h
 * \brief  雖然是類似 Repository 的功能，但與Repository的模式邏輯還是不太一樣，想不到好的名稱，先這樣
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 *********************************************************************/
#ifndef LIGHTING_PAWN_REPOSITORY_H
#define LIGHTING_PAWN_REPOSITORY_H

#include "SceneGraph/SpatialId.h"
#include "SceneGraph/Light.h"
#include "Frameworks/EventSubscriber.h"

namespace Enigma::Rendering
{
    class LightingPawn;

    class LightingPawnRepository
    {
    public:
        LightingPawnRepository();
        LightingPawnRepository(const LightingPawnRepository&) = delete;
        LightingPawnRepository(LightingPawnRepository&&) = delete;
        virtual ~LightingPawnRepository();
        LightingPawnRepository& operator=(const LightingPawnRepository&) = delete;
        LightingPawnRepository& operator=(LightingPawnRepository&&) = delete;

        void registerHandlers();
        void unregisterHandlers();

        void createAmbientLightPawn(const std::shared_ptr<SceneGraph::Light>& lit);
        void createSunLightPawn(const std::shared_ptr<SceneGraph::Light>& lit);
        void createPointLightPawn(const std::shared_ptr<SceneGraph::Light>& lit, const SceneGraph::SpatialId& present_camera_id);
        void removeLightingPawn(const SceneGraph::SpatialId& lit_id);

    protected:
        std::shared_ptr<LightingPawn> findLightingPawn(const SceneGraph::SpatialId& lit_id);

        void onLightAttached(const Frameworks::IEventPtr& e);
        void onLightAttachmentFailed(const Frameworks::IEventPtr& e);

        void tryCompleteLightPawnAttachment(const std::shared_ptr<SceneGraph::Light>& lit, const std::shared_ptr<LightingPawn>& lit_pawn);
        void resolvePendingLightPawnAttachment(const std::shared_ptr<SceneGraph::Light>& lit);

    protected:
        using LightingPawnMap = std::unordered_map<SceneGraph::SpatialId, std::weak_ptr<LightingPawn>, SceneGraph::SpatialId::hash>;
        LightingPawnMap m_lightingPawns;

        struct PendingLightPawn
        {
            std::shared_ptr<SceneGraph::Light> light;
            std::shared_ptr<LightingPawn> pawn;
        };
        std::unordered_map<SceneGraph::SpatialId, PendingLightPawn, SceneGraph::SpatialId::hash> m_pendingLightPawnsOfAttachment;

        Frameworks::EventSubscriberPtr m_onLightAttached;
        Frameworks::EventSubscriberPtr m_onLightAttachmentFailed;
    };
}

#endif // LIGHTING_PAWN_REPOSITORY_H
