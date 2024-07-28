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

namespace Enigma::GameCommon
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

        void createAmbientLightQuad(const std::shared_ptr<SceneGraph::Light>& lit);
        void createSunLightQuad(const std::shared_ptr<SceneGraph::Light>& lit);
        void createPointLightVolume(const std::shared_ptr<SceneGraph::Light>& lit);
        void removeLightingPawn(const SceneGraph::SpatialId& lit_id);

    protected:
        std::shared_ptr<LightingPawn> findLightingPawn(const SceneGraph::SpatialId& lit_id);

    protected:
        using LightingPawnMap = std::unordered_map<SceneGraph::SpatialId, std::weak_ptr<LightingPawn>, SceneGraph::SpatialId::hash>;
        LightingPawnMap m_lightingPawns;

        struct PendingLightPawn
        {
            std::shared_ptr<SceneGraph::Light> light;
            std::shared_ptr<LightingPawn> pawn;
        };
        std::unordered_map<SceneGraph::SpatialId, PendingLightPawn, SceneGraph::SpatialId::hash> m_pendingLightPawnsOfAttachment;
    };
}

#endif // LIGHTING_PAWN_REPOSITORY_H
