/*********************************************************************
 * \file   LightQuadPawn.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef LIGHT_QUAD_PAWN_H
#define LIGHT_QUAD_PAWN_H

#include "LightingPawn.h"

namespace Enigma::GameCommon
{
    using error = std::error_code;
    class LightQuadPawn : public LightingPawn
    {
        DECLARE_EN_RTTI;
    public:
        LightQuadPawn(const SceneGraph::SpatialId& id);
        LightQuadPawn(const SceneGraph::SpatialId& id, const Engine::GenericDto& o);
        LightQuadPawn(const LightQuadPawn&) = delete;
        LightQuadPawn(LightQuadPawn&&) = delete;
        virtual ~LightQuadPawn() override;
        LightQuadPawn& operator=(const LightQuadPawn&) = delete;
        LightQuadPawn& operator=(LightQuadPawn&&) = delete;

        virtual Engine::GenericDto serializeDto() override;

        /// 影響的光源就是自己所帶的光源
        virtual error _updateSpatialRenderState() override;
    };
}

#endif // LIGHT_QUAD_PAWN_H
