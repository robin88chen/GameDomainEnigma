/*********************************************************************
 * \file   LightVolumePawn.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef LIGHT_VOLUME_PAWN_H
#define LIGHT_VOLUME_PAWN_H

#include "LightingPawn.h"

namespace Enigma::GameCommon
{
    using error = std::error_code;

    class LightVolumePawn : public LightingPawn
    {
        DECLARE_EN_RTTI;
    public:
        LightVolumePawn(const SceneGraph::SpatialId& id);
        LightVolumePawn(const SceneGraph::SpatialId& id, const Engine::GenericDto& o);
        LightVolumePawn(const LightVolumePawn&) = delete;
        LightVolumePawn(LightVolumePawn&&) = delete;
        virtual ~LightVolumePawn() override;
        LightVolumePawn& operator=(const LightVolumePawn&) = delete;
        LightVolumePawn& operator=(LightVolumePawn&&) = delete;

        static std::shared_ptr<LightVolumePawn> create(const SceneGraph::SpatialId& id);
        static std::shared_ptr<LightVolumePawn> constitute(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto);
        virtual Engine::GenericDto serializeDto() override;

        /// 視攝影機位置在內、外，render 內部或外部, 用 mesh select visual tech 改變
        void ToggleCameraInside(bool is_inside);

        /// 影響的光源就是自己所帶的光源
        virtual error _updateSpatialRenderState() override;

    public:
        static void setDefaultVisualTech(const std::string& token) { m_tokenDefaultVisualTech = token; }
        static void setInsideVisualTech(const std::string& token) { m_tokenInsideVisualTech = token; }
        static const std::string& getDefaultVisualTech() { return m_tokenDefaultVisualTech; }
        static const std::string& getInsideVisualTech() { return m_tokenInsideVisualTech; }

    protected:
        bool m_isCameraInside;
        static std::string m_tokenDefaultVisualTech;
        static std::string m_tokenInsideVisualTech;
    };
}

#endif // LIGHT_VOLUME_PAWN_H
